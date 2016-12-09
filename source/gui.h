/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka GUI - inicjalizacja i renderowanie graficznego interfejsu uzytkownika
 *
 *****************************************************************************/




#ifndef GUI_INCLUDED
#define GUI_INCLUDED

using namespace std;

#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include "config.h"
#include "uistate.h"


// nr modulow - funkcji wyswietlacza
#define SCOPE         1
#define GRAPH_XY      2
#define SPECTRUM      3


// definicje kolorow (hex)
const SDL_Color cl_white = { 0xFF, 0xFF, 0xFF };
const SDL_Color cl_gray = { 0x99, 0x99, 0x99 };
const SDL_Color cl_gray2 = { 0x20, 0x20, 0x20 };
const SDL_Color cl_olive   = { 0x57, 0x63, 0x28 };
const SDL_Color cl_yellow   = { 0xF5, 0xc0, 0x00 };
const SDL_Color cl_yellow2 = { 0x7F, 0x6B, 0x28 };
const SDL_Color cl_red = { 0xA3, 0x01, 0x01 };


// wymiary okna aplikacji
const int app_window_x = 800;
const int app_window_y = 500;


// caly ekran aplikacji
extern SDL_Surface* screen;
// warstwa uzywana do screenshotow wyswietlacza
extern SDL_Surface* screen_display_tmp;
// warstwa potrzebna do usuniecia konsoli i tooltipow ze zrzutu ekranu
extern SDL_Surface* screen_console_tmp;
// wskaznik do warstwy z zapamietanym obszarem dolnego panelu
extern SDL_Surface* screen_bottom_panel;
// zaladowany obrazek dolnego panelu spektrogramu
extern SDL_Surface* screen_bottom_spect_panel;
// zaladowany obrazek dolnego panelu wykresu XY
extern SDL_Surface* screen_bottom_xy_panel;


// czcionki uzywane w aplikacji
extern TTF_Font *font_big;
extern TTF_Font *font_normal;
extern TTF_Font *font_small;
extern TTF_Font *font_xsmall;
extern TTF_Font *font_lcd;


// stan interfejsu uzytkownika
extern UIState uistate;


/*
Funkcja przygotowuje interfejs graficzny aplikacji
*/
void gui_init();

/*
Zwolnienie pamieci i zakonczenie trybu graficznego
*/
void gui_stop();

/*
Funkcja wyswietla lub ukrywa menu wyboru okna czasowego
*/
void render_spect_window_menu();

/*
Wyswietlenie wszystkich checkboksow
*/
void render_checkboxes();


/*
Wyswietlenie wszystkich przyciskow radio
*/
void render_radios();



/*
Funkcja wyswietla odpowiednie wnetrze danego checkboksa
Parametry wejsciowe:
x,y - wspolrzedne (px)
active - true jezeli ma byc wlaczone
*/
void render_checkbox(int x, int y, bool active);

/*
Funkcja wyswietla odpowiedni przycisk start/stop
Parametry wejsciowe:
active - true jezeli ma byc wlaczone
*/
void render_running_button(bool active);

/*
Funkcja wyswietla pojedynczy przycisk glownego menu
Parametry wejsciowe:
module - nr modulu/przycisku [1-5]
active - jezeli true, to przycisk bedzie wyrozniony
*/
void render_module_button(int module, bool active=false);


/*
Zapamietanie fragmentu ekranu pod konsola i tooltipami
*/
void copy_console_area();


/*
Wyswietlanie tooltipow
*/
void render_tooltips();

/*
Wyswietlenie wskaznika i tekstu na pokretlach
*/
void render_knobs();

/*
Wyswietlenie wskaznika i tekstu na suwakach XY
*/
void render_xy_sliders();

/*
Wyswietlenie wspolrzednych kursora (oscylogram)
*/
void render_cursor_coords();


#endif
