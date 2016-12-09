/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka console - konsola do recznego wprowadzania dokladnych wartosci parametrow
 *
 *****************************************************************************/


#include <SDL/SDL.h>
#include <sstream>
#include <vector>
#include <iomanip>
#include "libconsole/CON_console.h"

#include "headers/console.h"
#include "headers/gui.h"
#include "headers/gui_coords.h"
#include "headers/display.h"
#include "headers/graphics.h"
#include "headers/signal.h"
#include "headers/config.h"


// uniwersalne Sleep()
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif


using namespace std;


// ilosc wierszy konsoli
const int con_size = 80;

// wyswietlenie listy dostepnych komend
void list_commands(ConsoleInformation *console, char *string);
// ustawienie czulosci
void set_sensivity(ConsoleInformation* console, char* sens);
// ustawienie przesuniecia pionowego
void set_vert_offset(ConsoleInformation* console, char* vert);
// ustawienie podstawy czasowej
void set_timebase(ConsoleInformation* console, char* timebase);
// ustawienie poziomu wyzwalania
void set_trigger_level(ConsoleInformation* console, char* level);
// ustawienie wartosci poczatkowych
void reset(ConsoleInformation *console, char *string);
// ustawienie wzmocnienia amplitudy
void set_amp_gain(ConsoleInformation* console, char* gain);
// wyswietlenie parametrow wejsciowych
void param_info(ConsoleInformation *console, char *string);


// podzial ciagu znakow na skladowe wg podanego separatora
vector<string> explode(const string& str, const char delimiter);


/*
Konstruktor
*/
Console::Console() {

  this->active = false;
  this->console = NULL;

}

/*
Inicjalizacja konsoli
*/
void Console::init() {

  SDL_Rect r;
  r.w = con_w; r.h = con_h;
  r.x = con_x; r.y = con_y;

  // ConsoleFont to czcionka bitmapowa z SDL_console,
  // dodane polskie znaki - tylko male od ą=\01 do ż=\11 (octal)

  // potrzebne w przypadku pakietowania dla Debiana
  // -----------------------------------------------
  #ifdef DEBIAN_PACKAGE
  this->console = CON_Init("/usr/share/miniosc/fonts/ConsoleFont.png", screen, con_size, r);
  // -----------------------------------------------
  #else
  this->console = CON_Init("fonts/ConsoleFont.png", screen, con_size, r);
  #endif

  // dodanie komend do konsoli
  CON_AddCommand(&list_commands, "?");
  CON_AddCommand(&list_commands, "help");
  CON_AddCommand(&set_sensivity, "sens");
  CON_AddCommand(&set_vert_offset, "vert");
  CON_AddCommand(&set_timebase, "time");
  CON_AddCommand(&set_trigger_level, "trig");
  CON_AddCommand(&reset, "reset");
  CON_AddCommand(&set_amp_gain, "gain");
  CON_AddCommand(&param_info, "info");

  // ustawienie przezroczystosci
  CON_Alpha(console, 185);
  // ustawienie znaku zachety
  CON_SetPrompt(this->console, (char*)("# "));
  // wyswietlenie listy komend
  list_commands(this->console, NULL);

}


/*
Przelaczenie widocznosci konsoli
*/
void Console::toggle() {

  this->active = !this->active;

  if (this->active) {

    CON_Topmost(this->console);

  }

  else CON_Topmost(NULL);


}


/*
Narysowanie konsoli jezeli jest aktywna
*/
void Console::render() {

  if (!this->active)  return;

  CON_DrawConsole(this->console);

  draw_rectangle(screen, con_x,con_y, con_x + con_w, con_y + con_h, 0x3c3010);

  // to zeby odswiezac widok pod spodem w stanie 'stop'
  if (!uistate.running) uistate.change_made = true;

}

/*
Przekazanie obslugi zdarzen SDL do konsoli
*/
void Console::check_events(SDL_Event &event) {

  if (!this->active) return;
  // wylaczenie skrotow klawiszowych aplikacji
  if ((SDL_GetModState() & KMOD_CTRL) != 0) return;

  CON_Events(&event);

  //Sleep(2);

}

/*
Sprzatniecie konsoli
*/
void Console::stop() {

   CON_Destroy(this->console);

}


/*
Funkcja wyswietla na konsoli liste dostepnych komend
*/
void list_commands(ConsoleInformation *console, char *string) {

  { stringstream oss;
    CON_NewLineConsole(console);
    oss << "Konsola " << TITLE;
    CON_Out(console, oss.str().c_str());  }
  CON_Out(console, "----------------------------------------------------");
  CON_Out(console, "Lista komend:");
  CON_NewLineConsole(console);
  CON_Out(console, "sens [c] [n]");
  CON_Out(console, "- ustawia czu\04o\07\02 [n] dla kana\04u [c]");
  CON_Out(console, "vert [c] [n]");
  CON_Out(console, "- ustawia przesuni\03cie pionowe [n] dla kana\04u [c]");
  CON_Out(console, "time [n]");
  CON_Out(console, "- ustawia podstaw\03 czasow\01 [n] ms");
  CON_Out(console, "trig [n]");
  CON_Out(console, "- ustawia poziom wyzwalania [n]");
  CON_Out(console, "reset");
  CON_Out(console, " - ustawia warto\07ci pocz\01tkowe ww.parametr\06w");
  CON_Out(console, "gain [n]");
  { stringstream oss;
    oss << " - ustawia wzmocnienie amplitudy [n] (" << fixed << setprecision(1) << AMP_GAIN_MIN
        << " - " << AMP_GAIN_MAX << ")";
    CON_Out(console, oss.str().c_str());  }
  CON_NewLineConsole(console);
  CON_Out(console, "info - wy\07wietla parametry wej\07cia");
  CON_Out(console, "help lub ? - wy\07wietla t\03 list\x03 komend");
  CON_Out(console, "----------------------------------------------------");
  CON_NewLineConsole(console);

}

/*
Funkcja ustawia parametr czulosci dla danego kanalu
*/
void set_sensivity(ConsoleInformation* console, char* sens) {

  vector <string> params = explode(sens, ' ');
  if (params.size()<2) {
    CON_Out(console, "sens [c] [n]");
    CON_Out(console, "Komenda \"sens\" wymaga 2 parametr\06w.");
    return;
  }

  int fi = params[0][0]-'1';
  if (fi<0 || fi>=CHANNELS) {
    CON_Out(console, "sens [c] [n]");
    stringstream oss;
    oss << "Wymagany nr kana" << char(4) << "u z przedzia\04u 1-" << CHANNELS << ".";
    CON_Out(console, oss.str().c_str());
    return;
  }

  string fs(params[1]);
  float sensivity = atof(fs.c_str());
  if (sensivity<SENSIVITY_MIN || sensivity>SENSIVITY_MAX) {
    CON_Out(console, "sens [c] [n]");
    stringstream oss;
    oss << "Wymagana czu\04o\07\02 z przedzia\04u " << setprecision(2) << fixed
        << SENSIVITY_MIN<< " - " << SENSIVITY_MAX << ".";
    CON_Out(console, oss.str().c_str());
    return;
  }

  uistate.sensivity[(int)(params[0][0]-'1')] = sensivity;
  uistate.knobs_changed = true;
  uistate.change_made = true;
  CON_Out(console, "OK");

}



/*
Funkcja ustawia parametr przesuniecia pionowego dla danego kanalu
*/
void set_vert_offset(ConsoleInformation* console, char* vert) {

  vector <string> params = explode(vert, ' ');
  if (params.size()<2) {
    CON_Out(console, "vert [c] [n]");
    CON_Out(console, "Komenda \"vert\" wymaga 2 parametr\06w.");
    return;
  }

  int fi = params[0][0]-'1';
  if (fi<0 || fi>=CHANNELS) {
    CON_Out(console, "vert [c] [n]");
    stringstream oss;
    oss << "Wymagany nr kana\04u z przedzia\04u 1-" << CHANNELS << ".";
    CON_Out(console, oss.str().c_str());
    return;
  }

  string fs(params[1]);
  float vert_offset = atof(fs.c_str());
  if (vert_offset<VERT_OFFSET_MIN || vert_offset>VERT_OFFSET_MAX) {
    CON_Out(console, "vert [c] [n]");
    stringstream oss;
    oss << "Wymagane przesuni\03cie z przedzia\04u " << setprecision(2) << fixed
        << VERT_OFFSET_MIN << " - " << VERT_OFFSET_MAX << ".";
    CON_Out(console, oss.str().c_str());
    return;
  }

  uistate.vert_offset[(int)(params[0][0]-'1')] = vert_offset;
  uistate.knobs_changed = true;
  uistate.change_made = true;
  CON_Out(console, "OK");

}


/*
Funkcja ustawia parametr poziomu wyzwalania
*/
void set_trigger_level(ConsoleInformation* console, char* level) {

  float _level = atof(level);

  if (_level<TRIGGER_LEVEL_MIN || _level>TRIGGER_LEVEL_MAX) {
    CON_Out(console, "trig [n]");
    stringstream oss;
    oss << "Wymagana warto\07\02 z przedzia\04u " << setprecision(2) << fixed
        << TRIGGER_LEVEL_MIN << " - " << TRIGGER_LEVEL_MAX << ".";
    CON_Out(console, oss.str().c_str());
    return;
  }

  uistate.trigger.level = _level;
  uistate.knobs_changed = true;
  uistate.change_made = true;
  CON_Out(console, "OK");

}


/*
Funkcja ustawia parametr podstawy czasowej
*/
void set_timebase(ConsoleInformation* console, char* timebase) {

  const float _timebase_min = 1000.0 * TIMEBASE_MIN;
  const float _timebase_max = 1000.0 * TIMEBASE_MUL * TIMEBASE_MAX;

  float _timebase = atof(timebase);

  if (_timebase<_timebase_min || _timebase>_timebase_max) {
    CON_Out(console, "time [n]");
    stringstream oss;
    oss << "Wymagana warto\07\02 z przedzia\04u " << fixed << setprecision(3)
        << _timebase_min << " - " << _timebase_max << " [ms].";
    CON_Out(console, oss.str().c_str());
    return;
  }

  uistate.timebase = _timebase / 1000.0;

  uistate.timebase_mul = (uistate.timebase > TIMEBASE_MAX);

  uistate.knobs_changed = true;
  uistate.change_made = true;
  CON_Out(console, "OK");
  render_checkboxes();

}


/*
Funkcja ustawia parametr wzmocnienia amplitudy sygnalu wejsciowego
*/
void set_amp_gain(ConsoleInformation* console, char* gain) {

  float _gain = atof(gain);

  if (_gain<AMP_GAIN_MIN || _gain>AMP_GAIN_MAX) {
    CON_Out(console, "gain [n]");
    stringstream oss;
    oss << "Wymagana warto\07\02 z przedzia\04u " << fixed << setprecision(2)
        << AMP_GAIN_MIN << " - " << AMP_GAIN_MAX << " .";
    CON_Out(console, oss.str().c_str());
    return;
  }

  _AMP_GAIN = _gain;

  uistate.change_made = true;
  CON_Out(console, "OK");

}


/*
Funkcja wyswietla informacje o parametrach wejsciowych
*/
void param_info(ConsoleInformation *console, char *string) {

  CON_Out(console, "Urz\01dzenie wej\07ciowe audio: ");
  CON_Out(console, device_info.c_str());

  {  stringstream oss;
      oss << "Cz\03stotliwo\07\02 pr\06bkowania: "
          << fixed << setprecision(2) << (SAMP_RATE/1000.0) << " kHz";
     CON_Out(console, oss.str().c_str());  }

  {  stringstream oss;
      oss << "Wzmocnienie amplitudy: x "
          << fixed << setprecision(2) << _AMP_GAIN;
     CON_Out(console, oss.str().c_str());  }

}


/*
Funkcja ustawia poczatkowe wartosci parametrow
*/
void reset(ConsoleInformation *console, char *string) {

  uistate.sensivity[0] = SENSIVITY_DEF;
  uistate.sensivity[1] = SENSIVITY_DEF;
  uistate.vert_offset[0] = VERT_OFFSET_DEF;
  uistate.vert_offset[1] = VERT_OFFSET_DEF;
  uistate.trigger.level = TRIGGER_LEVEL_DEF;
  uistate.trigger.slope = TRIGGER_SLOPE_DEF;
  uistate.trigger.channel = TRIGGER_CHANNEL_DEF;
  uistate.timebase = TIMEBASE_DEF;
  uistate.timebase_mul = (TIMEBASE_DEF>TIMEBASE_MAX);

  render_radios();
  if (uistate.module == SCOPE)
    render_checkbox(timebase_mul_cbox_x, timebase_mul_cbox_y, uistate.timebase_mul);

  uistate.knobs_changed = true;
  uistate.change_made = true;
  CON_Out(console, "OK");

}


/*
Funkcja dzieli ciag znakow na skladowe wg podanego separatora
Parametry wejsciowe:
str - ciag znakow
delimiter - separator
Zwracana wartosc:
Wektor zadanych parametrow
*/
vector<string> explode(const string& str, const char delimiter) {

  vector<string> elements;
  stringstream stream(str);
  string item;
  while (getline(stream, item, delimiter))
    elements.push_back(item);

  return elements;

}



