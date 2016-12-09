/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka info - ekrany informacyjne
 *
 *****************************************************************************/



#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <iomanip>

#include "headers/gui.h"
#include "headers/graphics.h"
#include "headers/info.h"
#include "headers/signal.h"



using namespace std;


// wymiary obrazka 'splash'
const int splash_x = 450;
const int splash_y = 283;
const int splash_x1 = (app_window_x - splash_x)/2;
const int splash_y1 = (app_window_y - splash_y)/2;
// wymiary obrazka 'info'
const int info_x = 400;
const int info_y = 200;
const int info_x1 = (app_window_x - info_x)/2;
const int info_y1 = (app_window_y - info_y)/2;

// skroty klawiszowe wylaczone z oczekiwania na dowolny klawisz
const SDLKey shortcut1 = SDLK_F1;
const SDLKey shortcut2 = SDLK_F3;


// przygotowanie ekranu
void prepare_screen(string bg_img, void (*content)(int, int, vector<string>),
                    int x, int y, vector<string> txt, bool about = false);
void prepare_screen(string bg_img);



/*
Funkcja wyswietla glowna zawartosc ekranu informacyjnego
Parametry wejsciowe:
x,y - pozycja pierwszej linii tekstu w oknie
string - lista linijek tekstu do wyswietlenia
*/
void info_text_content(int x, int y, vector<string> text) {

  vector<string>::iterator it = text.begin();
  // podkreslenie pod pierwsza linijka
  if (text[0].size() > 0)
  text.insert(++it, "―――――――――――――――――――――――――――");

  for(vector<string>::size_type i = 0; i != text.size(); i++)
    out_text(screen, app_window_x/2 - info_x/2 + x, app_window_y/2 - info_y/2 + y + (i-1)*18 + 5,
             text[i], cl_gray, font_big);


}





/*
Funkcja wyswietla ekran informacyjny "o programie"
*/
void info_about() {

  vector<string> s = vector<string>();


  stringstream oss;
  s.push_back("");
  oss << "  Autor: M.Kawecki 2015/16,  wersja: " << VERSION << ", licencja MIT";
  s.push_back(oss.str().c_str());
  s.push_back("");
  s.push_back(" • libconsole_SDL 1 (© G.Banuk)");
  s.push_back(" • FFTW3, RtAudio, SDL");
  prepare_screen("images/splash.png", info_text_content, 30, 100, s, true);


}



/*
Funkcja wyswietla ekran informacyjny z komunikatem
Parametry wejsciowe:
x,y - pozycja pierwszej linii tekstu w oknie
string - lista linijek tekstu do wyswietlenia
*/
void info_text(int x, int y, vector<string> text) {

  prepare_screen("images/info.png", info_text_content, x, y, text);

}


/*
Funkcja wyswietla informacje o dostepnych skrotach klawiszowych
*/
void info_shortcut() {


  vector<string> s = vector<string>();

  s.push_back("Skróty klawiszowe (F1)");

  s.push_back("          F2        - włączenie/wyłączenie konsoli");
  s.push_back("          F3        - wykonanie zrzutu ekranu \"wyświetlacza\"");
  s.push_back("          F5        - sygnał start / sygnał stop");
  s.push_back("          F10      - zakończenie pracy programu");

  s.push_back("Ctrl + 1,2       - włączenie/wyłączenie kanału 1 lub 2");
  s.push_back("Ctrl + P          - włączenie/wyłączenie ×100 podst.czasowej");
  s.push_back("Ctrl + O,X,W  - Oscyloskop / Wykres XY / Widmo");

  info_text(10, 10, s);



}


/*
Funkcja wyswietla informacje o bledzie urzadzenia audio
*/
void info_signal_error() {

  vector<string> s = vector<string>();

  stringstream oss;
  oss << TITLE << ": błąd urządzenia audio";
  s.push_back(oss.str().c_str());

  s.push_back("Nie można pobrać sygnału, brak urządzenia audio, ");
  s.push_back("lub urządzenie jest zajęte.");
  s.push_back("Zamknij wszystkie programy korzystające z systemu audio");
  s.push_back("i spróbuj uruchomić program ponownie.");
  s.push_back("");
  s.push_back("Program teraz zakończy pracę.");

  info_text(10, 10, s);

}



/*
Oczekiwanie na dowolny klawisz lub przycisk myszy
Zwracana wartosc - true jezeli uzytkownik zamknal okno calej aplikacji
*/
bool any_key_delay() {

  SDL_Event event;

  change_cursor(DEFAULT_CURSOR);

  shortcut:

  while (SDL_WaitEvent(&event) && event.type!=SDL_KEYUP  && event.type!=SDL_QUIT
          && (event.type!=SDL_MOUSEBUTTONDOWN || event.button.button != 1));

   // wylaczenie skrotow klawiszowych
   if (event.key.keysym.sym == shortcut1 || event.key.keysym.sym == shortcut2)  goto shortcut;


  // ew. aktualizacja wspolrzednych kursora myszy
  uistate.mousex = event.motion.x;
  uistate.mousey = event.motion.y;

  return (event.type==SDL_QUIT);

}




/*
Funkcja przygotowuje ekran do wyswietlenia informacji
Parametry wejsciowe:
bg_img - nazwa pliku z obrazkiem tla
content - wskaznik do funkcji void wyswietlajacej zawartosc
x,y - poczatkowe wspolrzedne tekstu
txt - kolejne linijki tekstu do wyswietlenia
*/
void prepare_screen(string bg_img, void (*content)(int, int, vector<string>),
                    int x, int y, vector<string> txt, bool about) {

  SDL_Rect r;

  SDL_Surface *temp = create_surface(app_window_x, app_window_y, screen);
  apply_surface(0, 0, screen, temp);

  r.w = app_window_x;
  r.h = app_window_y;

  SDL_Surface *splash_img = img_load(bg_img.c_str());
  // zaciemnienie calego ekranu
  SDL_Surface *splash = create_surface(app_window_x, app_window_y, screen);
  SDL_FillRect(splash, &r, 0x000);
  SDL_SetAlpha(splash, SDL_SRCALPHA, 220);
  apply_surface(0, 0, splash, screen);
  apply_surface(app_window_x/2 - splash_img->w/2, app_window_y/2 - splash_img->h/2, splash_img, screen);

  if (content != NULL)  content(x, y, txt);

  out_text(screen, app_window_x/2 - 80, app_window_y/2 + info_y/2 - (about ? 10 : 25), "Naciśnij dowolny klawisz ......", cl_yellow, font_big);


  SDL_Flip(screen);

  // oczekiwanie na dowolny klawisz
  uistate.quit = any_key_delay();

  apply_surface(0, 0, temp, screen);

  SDL_Flip(screen);

  // zwolnienie pamieci
  SDL_FreeSurface(temp);
  SDL_FreeSurface(splash);
  SDL_FreeSurface(splash_img);


}


/*
Funkcja przygotowuje ekran bez tekstu
Parametry wejsciowe:
bg_img - nazwa pliku z obrazkiem tla
*/
void prepare_screen(string bg_img) {

  vector<string> dumb;
  prepare_screen(bg_img, NULL, 0, 0, dumb);

}
