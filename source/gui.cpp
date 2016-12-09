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



#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cstring>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "headers/config.h"
#include "headers/gui_coords.h"
#include "headers/graphics.h"
#include "headers/display.h"
#include "headers/gui.h"
#include "headers/info.h"
#include "headers/spectrum.h"


using namespace std;


// wskaznik do podstawowej warstwy - ekran
SDL_Surface* screen;
// wskaznik do warstwy z zapamietanym obszarem dolnego panelu
SDL_Surface* screen_bottom_panel;
// zaladowany obrazek dolnego panelu spektrogramu
SDL_Surface* screen_bottom_spect_panel;
// zaladowany obrazek dolnego panelu wykresu XY
SDL_Surface* screen_bottom_xy_panel;
// warstwa potrzebna do ew. usuniecia konsoli i tooltipow ze zrzutu ekranu
SDL_Surface* screen_console_tmp;
// warstwa pod wspolrzednymi kursora
SDL_Surface* screen_coords_tmp;

// zaladowane obrazki kontrolek
SDL_Surface* btn_start;
SDL_Surface* btn_stop;
SDL_Surface* img_checked;
SDL_Surface* img_unchecked;
SDL_Surface* img_checked_small;
SDL_Surface* img_radio_checked;
SDL_Surface* img_radio_unchecked;
SDL_Surface* knob_inner;
SDL_Surface* knob_inner_txt;
SDL_Surface* knob_dot;
SDL_Surface* slider_btn;

// zaladowane czcionki
TTF_Font *font_big = NULL;
TTF_Font *font_normal = NULL;
TTF_Font *font_small = NULL;
TTF_Font *font_xsmall = NULL;
TTF_Font *font_lcd = NULL;

// stan interfejsu uzytkownika
UIState uistate;

// wyswietla pojedynczy tooltip
void render_tooltip(string text);


/*
Funkcja przygotowuje interfejs graficzny aplikacji
*/
void gui_init() {

  putenv((char*)"SDL_VIDEO_CENTERED=1");
  screen = SDL_SetVideoMode(app_window_x, app_window_y, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_EnableUNICODE(1);

  // domyslny kursor systemowy
  default_cursor = SDL_GetCursor();

  // etykieta i ikona okna
  SDL_WM_SetCaption(TITLE, "");
  SDL_Surface *icon = img_load("images/icon.png");
  Uint32 color_key = SDL_MapRGB(icon->format, 255, 0, 255);
  SDL_SetColorKey(icon, SDL_SRCCOLORKEY, color_key);
  SDL_WM_SetIcon(icon, NULL );
  SDL_FreeSurface(icon);

  // zaladowanie czcionek
  if( TTF_Init() == -1 ) {
     cerr << "Nie mozna zainicjalizowac obslugi czcionek TTF: " << TTF_GetError() << endl;
     exit(EXIT_FAILURE);
  }



  // potrzebne w przypadku pakietowania dla Debiana
  // -----------------------------------------------
  #ifdef DEBIAN_PACKAGE
  font_big = TTF_OpenFont("/usr/share/miniosc/fonts/LiberationSans-Regular.ttf", 14);
  font_normal = TTF_OpenFont("/usr/share/miniosc/fonts/LiberationSans-Regular.ttf", 12);
  font_small = TTF_OpenFont("/usr/share/miniosc/fonts/LiberationSans-Regular.ttf", 11);
  font_xsmall = TTF_OpenFont("/usr/share/miniosc/fonts/LiberationSans-Regular.ttf", 9);
  font_lcd = TTF_OpenFont("/usr/share/miniosc/fonts/lcd-solid.ttf", 11);
  // -----------------------------------------------
  #else
  font_big = TTF_OpenFont("fonts/LiberationSans-Regular.ttf", 14);
  font_normal = TTF_OpenFont("fonts/LiberationSans-Regular.ttf", 12);
  font_small = TTF_OpenFont("fonts/LiberationSans-Regular.ttf", 11);
  font_xsmall = TTF_OpenFont("fonts/LiberationSans-Regular.ttf", 9);
  font_lcd = TTF_OpenFont("fonts/lcd-solid.ttf", 11);
  #endif

  if (font_big==NULL || font_normal==NULL || font_lcd==NULL)  {
     cerr << "Nie mozna wczytac czcionek TTF: " << TTF_GetError() << endl;
     exit(EXIT_FAILURE);
  }

  // wczytanie "globalnych" obrazkow
  screen_bottom_spect_panel = img_load("images/spect_panel.png");
  screen_bottom_xy_panel = img_load("images/xy_panel.png");
  btn_start = img_load("images/button_start.png");
  btn_stop = img_load("images/button_stop.png");
  img_checked = img_load("images/checked.png");
  img_unchecked = img_load("images/unchecked.png");
  img_checked_small = img_load("images/checked_small.png");
  img_radio_checked = img_load("images/checked_radio.png");
  img_radio_unchecked = img_load("images/unchecked_radio.png");
  knob_inner = img_load("images/knob_inner.png");
  knob_inner_txt = img_load("images/knob_inner_txt.png");
  knob_dot = img_load("images/knob_dot.png");
  slider_btn = img_load("images/slider_btn.png");

  // nalozenie layoutu
  SDL_Surface *temp = img_load("images/layout.png");
  apply_surface(0, 0, temp, screen);

  // zapamietanie pola wyswietlacza
  screen_display_bg = create_surface(display_w+1, display_h+1, screen);
  SDL_Rect offset;
  offset.x = display_x1; offset.y = display_y1;
  offset.w = display_w;  offset.h = display_h;
  SDL_BlitSurface(screen, &offset, screen_display_bg, NULL);

  // zapamietanie pola pod dolnym panelem
  screen_bottom_panel = create_surface(bottom_panel_w, bottom_panel_h, screen);
  offset.x = bottom_panel_x; offset.y = bottom_panel_y;
  offset.w = bottom_panel_w; offset.h = bottom_panel_h;
  SDL_BlitSurface(screen, &offset, screen_bottom_panel, NULL);

  // zapamietanie pola pod wspolrzednymi kursora
  screen_coords_tmp = create_surface(cursor_coords_w, cursor_coords_h, screen);
  offset.x = cursor_coords_x;  offset.y = cursor_coords_y;
  offset.w = cursor_coords_w; offset.h = cursor_coords_h;
  SDL_BlitSurface(screen, &offset, screen_coords_tmp, NULL);

  // inicjalizacja warstwy pomocniczej (pod konsola)
  screen_console_tmp = create_surface(con_w, con_h+1, screen);

  // na koniec wyswietlenie przyciskow i checkboksow
  for (int i=1;i<=MODULES;i++) render_module_button(i, (i==uistate.module));

  render_running_button(uistate.running);
  render_checkboxes();
  render_radios();

  // i inicjalizacja konsoli
  uistate.console.init();

  SDL_FreeSurface(temp);


}




/*
Funkcja wyswietla lub ukrywa menu wyboru okna czasowego
*/
void render_spect_window_menu() {


  if (uistate.spect_window_menu_open) {

     for (int i=sizeof(*window_types);i>=0;i--) {

        bool mp =  uistate.check_mouse_pos(window_menu_list_x,
                   window_menu_list_y-(i+1)*window_menu_list_h+1, window_menu_list_w, window_menu_list_h-1);

        draw_rectangle(screen, window_menu_list_x, window_menu_list_y-i*window_menu_list_h+1,
                       window_menu_list_x+window_menu_list_w,
                       window_menu_list_y-(i+1)*window_menu_list_h+1, 0x808080);

        SDL_Rect r;
        r.w =window_menu_list_w-1;  r.h = window_menu_list_h-1;
        r.x = window_menu_list_x+1; r.y = window_menu_list_y-(i+1)*window_menu_list_h+2;
        SDL_FillRect(screen, &r, mp ? 0x505050 : 0x101010);
        out_text(screen, window_menu_list_x+7, window_menu_list_y-(i+1)*window_menu_list_h+4,
                 window_types[i], mp ? cl_white : cl_gray, font_normal);

        // maly checkbox przy wybranej pozycji
        if (i==uistate.spect_window-1)
          apply_surface(window_menu_list_x+window_menu_list_w-12,
                        window_menu_list_y-(i+1)*window_menu_list_h+7, img_checked_small, screen);

        if (mp && uistate.mousedown == 1) {
          uistate.spect_window = i;
          uistate.spect_window_menu_open = false;
          change_cursor(DEFAULT_CURSOR);
        }

     }


  }

  else {

      apply_surface(bottom_panel_x, bottom_panel_y, screen_bottom_spect_panel, screen);
      out_text(screen, window_menu_tsel_x, window_menu_tsel_y,
               window_types[uistate.spect_window-1], cl_gray2, font_normal);

      uistate.change_made = true;

  }

}


/*
Zapamietanie fragmentu ekranu pod konsola i tooltipami
*/
void copy_console_area() {

  if (!uistate.tooltip_screenshot && !uistate.tooltip_console && !uistate.tooltip_shortcut &&
      !uistate.tooltip_info && !uistate.console.active) return;

  SDL_Rect offset;
  offset.x = con_x; offset.y = con_y;
  offset.w = con_w; offset.h = con_h+1;
  SDL_BlitSurface(screen, &offset, screen_console_tmp, NULL);

}



/*
Wyswietlanie tooltipow
*/
void render_tooltips() {

  if (uistate.tooltip_screenshot) render_tooltip(" Zrzut ekranu");
  else if (uistate.tooltip_shortcut) render_tooltip("Skróty klawisz.");
  else if (uistate.tooltip_console)  render_tooltip(" Konsola tekst.");
  else if (uistate.tooltip_info)  render_tooltip("  O programie");

}


/*
Funkcja wyswietla pojedynczy tooltip
Parametry wejsciowe:
text - tresc podpowiedzi
*/
void render_tooltip(string text) {


  const int tx = uistate.mousex-tooltip_w;
  const int ty = uistate.mousey+tooltip_h+10;

  // na wszelki wypadek, po wyjsciu z okna inform.
  if (tx+tooltip_w>display_x2 || tx<tooltip_w) return;

  SDL_Rect r;
  r.w = tooltip_w; r.h = tooltip_h;  r.x = 1; r.y = 1;

  SDL_Surface *tooltip = create_surface(tooltip_w, tooltip_h, screen);
  SDL_FillRect(tooltip, &r, 0xffffcc);
  SDL_SetAlpha(tooltip, SDL_SRCALPHA, 120);
  apply_surface(tx, ty, tooltip, screen);
  SDL_FreeSurface(tooltip);

  draw_rectangle(screen, tx, ty, tx+tooltip_w, ty+tooltip_h, 0xffffcc);
  out_text(screen, tx+5, ty+2, text, cl_gray2, font_small);

}




/*
Funkcja wyswietla odpowiednie wnetrze danego checkboksa
Parametry wejsciowe:
x,y - wspolrzedne (px)
active - true jezeli ma byc wlaczone
*/
void render_checkbox(int x, int y, bool active) {

   apply_surface(x, y, active ? img_checked : img_unchecked, screen);

}



/*
Wyswietlenie wszystkich checkboksow
*/
void render_checkboxes() {

  render_checkbox(channel_cbox_x[0], channel_cbox_y, uistate.channel_on[0]);
  render_checkbox(channel_cbox_x[1], channel_cbox_y, uistate.channel_on[1]);
  render_checkbox(labels_cbox_x, labels_cbox_y, uistate.labels_on);
  render_checkbox(grid_cbox_x, grid_cbox_y, uistate.grid_on);
  if (uistate.module==SCOPE)
    render_checkbox(timebase_mul_cbox_x, timebase_mul_cbox_y, uistate.timebase_mul);

  uistate.timebase_mul_changed = false;

}


/*
Funkcja wyswietla odpowiednie wnetrze danego przycisku radio
Parametry wejsciowe:
x,y - wspolrzedne (px)
active - true jezeli ma byc wlaczone
*/
void render_radio(int x, int y, bool active) {

   apply_surface(x+2, y+2, active ? img_radio_checked : img_radio_unchecked, screen);

}


/*
Wyswietlenie wszystkich przyciskow radio
*/
void render_radios() {

  for (int i=0;i<3;i++)
    render_radio(trigger_slope_radio_x, trigger_slope_radio_y[i], (uistate.trigger.slope == i));

  for (int i=0;i<2;i++)
    render_radio(trigger_channel_radio_x[i], trigger_channel_radio_y, (uistate.trigger.channel == i));

}




/*
Funkcja wyswietla odpowiedni przycisk start/stop
Parametry wejsciowe:
active - true jezeli ma byc wlaczone
*/
void render_running_button(bool active) {

   apply_surface(running_btn_x, running_btn_y, active ? btn_stop : btn_start, screen);
   if (uistate.module == SCOPE) {
     apply_surface(bottom_panel_x, bottom_panel_y, screen_bottom_panel, screen);
     render_checkbox(timebase_mul_cbox_x, timebase_mul_cbox_y, uistate.timebase_mul);
     uistate.timebase_mul_changed = false;
   }

}


/*
Funkcja wyswietla pojedynczy przycisk glownego menu
Parametry wejsciowe:
module - nr modulu/przycisku [1-5]
active - jezeli true, to przycisk bedzie wyrozniony
*/
void render_module_button(int module, bool active) {

   stringstream oss;
   string s;

   oss << "images/button" << module << (active ? "a.png" : ".png");
   s = oss.str();
   SDL_Surface *temp = img_load(s.c_str());

   apply_surface(module_btn_x[module-1], module_btn_y, temp, screen);

   SDL_FreeSurface(temp);

}



/*
Wyswietlenie wskaznika i tekstu na pojedynczym pokretle
Parametry wejsciowe:
x, y - wspolrzedne srodka pokretla
value - wartosc przypisana do danego pokretla
min, max, mid - wartosc minimum, maksimum i srednia
blocked - true, jezeli pokretlo zablokowane
*/
void render_knob(int x, int y, float value, float min, float max, float mid) {

  // wyczyszczenie wnetrza pokretla
  apply_surface(x - knob_inner->w/2 +1,  y - knob_inner->h/2, knob_inner, screen);
  // wyliczenie wspolrzednych nalozenia kropki-markera
  float fi = knob_value_to_angle(value, min, max, mid);
  float dot_x = x + (knob_r-11) * cos(fi);
  float dot_y = y + (knob_r-11) * sin(fi);
  // nalozenie kropki - markera
  apply_surface(dot_x - floor(knob_dot->w/2.0) +1, dot_y - knob_dot->h/2, knob_dot, screen);
  // wyczyszczenie obszaru na tekst
  apply_surface(x - floor(knob_inner_txt->w/2.0),  y - knob_inner_txt->h/2, knob_inner_txt, screen);

  string unit =  get_value_unit(value, 3);
  out_text(screen, x-(4 * unit.length()/2), y-8, unit, cl_gray, font_xsmall);

}


/*
Wyswietlenie wskaznika i tekstu na pokretlach
*/
void render_knobs() {

  if (!uistate.knobs_changed) return;

  render_knob(sens_knob_x[0], sens_knob_y, uistate.sensivity[0], SENSIVITY_MIN, SENSIVITY_MAX, SENSIVITY_MID);
  render_knob(sens_knob_x[1], sens_knob_y, uistate.sensivity[1], SENSIVITY_MIN, SENSIVITY_MAX, SENSIVITY_MID);

  render_knob(vert_knob_x[0], vert_knob_y, uistate.vert_offset[0], VERT_OFFSET_MIN, VERT_OFFSET_MAX, VERT_OFFSET_MID);
  render_knob(vert_knob_x[1], vert_knob_y, uistate.vert_offset[1], VERT_OFFSET_MIN, VERT_OFFSET_MAX, VERT_OFFSET_MID);

  float tb_mul = uistate.timebase_mul ? TIMEBASE_MUL : 1.0;
  render_knob(timebase_knob_x, timebase_knob_y, uistate.timebase, TIMEBASE_MIN * tb_mul,
              TIMEBASE_MAX * tb_mul, TIMEBASE_MID * tb_mul);

  render_knob(trigger_knob_x, trigger_knob_y, uistate.trigger.level, TRIGGER_LEVEL_MIN, TRIGGER_LEVEL_MAX,
              TRIGGER_LEVEL_MID);

  uistate.knobs_changed = false;


}



/*
Wyswietlenie wskaznika i tekstu na pojedynczym suwaku
Parametry wejsciowe:
x, y, x2 - wspolrzedne obszaru suwaka
value - wartosc powiazanego parametru
*/
void render_xy_slider(int x, int y, int x2, float value) {

  apply_surface(slider_value_to_x(value, x, x2), y - slider_btn->h/2, slider_btn, screen);

  stringstream oss;
  oss << "× " << setprecision(2) << fixed << value;
  out_text(screen, x2 + xy_slider_mx_offset + 8, y - 5, oss.str(), cl_gray, font_small);

}


/*
Wyswietlenie wskaznika i tekstu na suwakach XY
*/
void render_xy_sliders() {

  if (!uistate.sliders_changed) return;

  apply_surface(bottom_panel_x, bottom_panel_y, screen_bottom_xy_panel, screen);

  render_xy_slider(xy_sliderx_x1,  xy_sliderx_y, xy_sliderx_x2, uistate.xy_x_scale);
  render_xy_slider(xy_slidery_x1,  xy_slidery_y, xy_slidery_x2, uistate.xy_y_scale);

  uistate.sliders_changed = false;

}


/*
Wyswietlenie wspolrzednych kursora (oscylogram)
*/
void render_cursor_coords() {

  static int old_mousex = 0, old_mousey = 0;

  if (uistate.module != SCOPE || uistate.running || (old_mousex == uistate.mousex
      && old_mousey == uistate.mousey))  return;

  apply_surface(cursor_coords_x, cursor_coords_y, screen_coords_tmp, screen);

  if (!uistate.check_mouse_pos(display_x1-6, display_y1-5, display_w - 1, display_h - 1)) return;


  string sx = get_value_unit(((uistate.mousex+7-display_x1)*uistate.timebase/BUFFER_SIZE)/base_x_scale, 3);

  stringstream oss;
  oss << "( " << fixed << setprecision(3) << sx << "s ; "
      << (grid_axis_y-uistate.mousey-6)/static_cast<float>(GRID_DIV_H) << " )";
  out_text(screen, cursor_coords_x, cursor_coords_y, "+", cl_gray, font_big);
  out_text(screen, cursor_coords_x+14, cursor_coords_y+2, oss.str(), cl_gray, font_normal);

  old_mousex = uistate.mousex;
  old_mousey = uistate.mousey;

}


/*
Zwolnienie pamieci i zakonczenie trybu graficznego
*/
void gui_stop() {

  // zwolnienie czcionek
  TTF_CloseFont(font_big);
  TTF_CloseFont(font_normal);
  TTF_CloseFont(font_small);
  TTF_CloseFont(font_xsmall);
  TTF_CloseFont(font_lcd);
  TTF_Quit();

  // zatrzymanie watku konsoli
  uistate.console.stop();

  // zwolnienie warstw obrazu
  SDL_FreeSurface(screen_display_bg);
  SDL_FreeSurface(screen_bottom_panel);
  SDL_FreeSurface(screen_bottom_spect_panel);
  SDL_FreeSurface(screen_bottom_xy_panel);
  SDL_FreeSurface(screen_console_tmp);
  SDL_FreeSurface(screen_coords_tmp);
  SDL_FreeSurface(btn_start);
  SDL_FreeSurface(btn_stop);
  SDL_FreeSurface(img_checked);
  SDL_FreeSurface(img_unchecked);
  SDL_FreeSurface(img_checked_small);
  SDL_FreeSurface(img_radio_checked);
  SDL_FreeSurface(img_radio_unchecked);
  SDL_FreeSurface(knob_inner);
  SDL_FreeSurface(knob_inner_txt);
  SDL_FreeSurface(knob_dot);
  SDL_FreeSurface(slider_btn);

  SDL_Quit();

}


