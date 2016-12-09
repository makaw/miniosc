/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 *  Biblioteka UIState - kontrola stanu graficznego interfejsu uzytkownika
 *
 *****************************************************************************/




#ifndef UISTATE_INCLUDED
#define UISTATE_INCLUDED


#include <cstdlib>
#include <cstring>

#include "config.h"
#include "console.h"
#include "trigger.h"


// rekord opisujacy stan interfejsu uzytkownika
struct UIState {

  // wspolrzedne myszy, przycisk myszy
  int mousex, mousey, mousedown;
  // flaga dokonanej zmiany, flaga wyjscia z aplikacji, zmiany modulu
  bool change_made, quit, module_changed;
  // obecny modul, poprzedni modul
  int module, module_prev;
  // siatka wl/wyl, etykiety wl/wyl, start/stop
  bool grid_on, labels_on, running;
  // kanaly wl/wyl
  bool channel_on[CHANNELS];
  // czulosc dla kanalow
  float sensivity[CHANNELS];
  // przesuniecie pionowe dla kanalow
  float vert_offset[CHANNELS];
  // podstawa czasowa
  float timebase;
  // mnoznik podstawy czasowej *1000, flagi zmiany mnoznika podstawy
  bool timebase_mul, timebase_mul_changed, tb_xy_mul;
  // wybrane okno czasowe (spektrogram)
  int spect_window;
  // czy menu wyboru okna rozwiniete
  bool spect_window_menu_open;
  // skala wykresu XY
  float xy_x_scale, xy_y_scale;
  // tooltipy
  bool tooltip_screenshot, tooltip_console, tooltip_info, tooltip_shortcut;
  // czy zmieniono jakies wartosci na pokretlach
  bool knobs_changed;
  // czy zmieniono wartosci na suwakach
  bool sliders_changed;
  // "przeciaganie" mysza po pokretlach, suwakach
  bool knob_mouse_drag, slider_mouse_drag;

  // konsola
  Console console;

  // wyzwalanie sygnalu
  Trigger trigger;

  // konstruktor (domyslne wartosci na start)
  UIState();
  // zmiana modulu
  void change_module(int module);
  // sprawdzenie kursora
  int check_cursor();
  // czy pozycja myszy jest w danym obszarze
  bool check_mouse_pos(int x, int y, int w, int h);
  // obsluzenie suwakow X-Y
  void check_sliders();
  // obsluzenie jednego pokretla
  void check_knob(float &val, int x, int y, float min, float max, float mid);
  // obsluzenie pokretel
  void check_knobs();
  // obsluzenie checkboksow
  void check_checkboxes();
  // przelaczenie mnoznika podstawy czasowej
  void toggle_timebase_mul();
  // przelaczenie start/stop
  void toggle_running();
  // przelaczenie kanalu
  void toggle_channel(int ch);
  // obsluzenie menu modulow wyswietlacza
  void check_modules_menu();
  // sprawdzenie przycisku-ikony skrotow klawiszowych
  bool check_shortcut_button();
  // sprawdzenie przycisku-ikony screenshot
  bool check_screenshot_button();
  // sprawdzenie przycisku-ikony konsoli
  bool check_console_button();
  // sprawdzenie przycisku-ikony info
  bool check_info_button();
  // obsluzenie przycisku start/stop
  void check_running();
  // obsluga rozwijanego menu wyboru okna czasowego
  void check_spect_window_menu();
  // wyliczenie katu miedzy promieniem pokretla a osia OY
  float get_knob_angle(int knob_x, int knob_y);
  // sprawdzenie warunkow wyzwalania sygnalu
  bool check_trigger(float prev1, float prev2, float curr);

};

/*
Funkcja zamienia wartosc na kat miedzy promieniem pokretla a osia OY
Parametry wejsciowe:
val - wartosc
min - minimalna wartosc na pokretle
max - maksymalna wartosc na pokretle
mid - srodkowa wartosc na pokretle
Zwracana wartosc:
Miara kata miedzy promieniem pokretla a osia OY (w radianach)
*/
float knob_value_to_angle(float val, float min, float max, float mid);

/*
Funkcja zamienia kat miedzy promieniem pokretla a osia OY na wartosc
Parametry wejsciowe:
angle - miara kata (w stopniach)
min - minimalna wartosc na pokretle
max - maksymalna wartosc na pokretle
mid - srodkowa wartosc na pokretle
*/
float knob_angle_to_value(float val, float min, float max, float mid);

/*
Przeliczenie wartosci parametru na wspolrzedna x na suwaku
Parametry wejsciowe:
val - wartosc parametru skali
x1, x2 - poczatkowa i koncowa wspolrzedna obszaru suwaka
Zwracana wartosc:
Wspolrzedna x przesuwnego elementu na suwaku
*/
int slider_value_to_x(float val, int x1, int x2);

/*
Przeliczenie wspolrzednej x na suwaku n wartosc parametru
Parametry wejsciowe:
x - wspolrzedna x przesuwnego elementu na suwaku
x1, x2 - poczatkowa i koncowa wspolrzedna obszaru suwaka
Zwracana wartosc:
Wartosc parametru skali
*/
float slider_x_to_value(int x, int x1, int x2);


/*
Okreslenie jednostki wartosci do wyswietlenia
Parametr wejsciowy:
value - wartosc ktora ma byc wyswietlona
precision - ilosc miejsc po przecinku (opcjonalnie, domyslnie 2)
Zwracana wartosc
Przeskalowana do wyswietlenia wartosc wraz z jednostka
*/
std::string get_value_unit(float value, int precision = 2);


#endif

