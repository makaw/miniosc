/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka UIState - kontrola stanu graficznego interfejsu uzytkownika
 *
 *****************************************************************************/




#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "libconsole/CON_console.h"
#include <iostream>
#include <iomanip>

#include "headers/uistate.h"
#include "headers/gui.h"
#include "headers/gui_coords.h"
#include "headers/spectrum.h"
#include "headers/graphics.h"
#include "headers/display.h"



using namespace std;


/*
Konstruktor stanu interfejsu, inicjalizacja atrybutow
*/
UIState::UIState() {

  mousex = mousey = mousedown = 0;
  module = module_prev = SCOPE;
  change_made = true;
  quit = false;
  grid_on = true;
  labels_on = true;
  for (int i=0;i<CHANNELS;i++) {
    channel_on[i] = true;
    sensivity[i] = SENSIVITY_DEF;
    vert_offset[i] = VERT_OFFSET_DEF;
  }
  timebase = TIMEBASE_DEF;
  timebase_mul = (TIMEBASE_DEF>TIMEBASE_MAX);
  timebase_mul_changed = false;
  tb_xy_mul = false;
  running = true;
  spect_window = DEFAULT_WINDOW;
  spect_window_menu_open = false;
  knobs_changed = true;
  sliders_changed = true;
  knob_mouse_drag = false;
  slider_mouse_drag = false;

  xy_x_scale = xy_y_scale = XY_SCALE_DEF;

}


/*
Sprawdzenie jaki powinien byc kursor (+tooltipy) w zaleznosci od pozycji myszy
Zwracana wartosc: rodzaj kursora
*/
int UIState::check_cursor() {

  // zachowanie stanu gornych ikon-przyciskow
  bool tt_screenshot = this->tooltip_screenshot;
  this->tooltip_screenshot = false;
  bool tt_console = this->tooltip_console;
  this->tooltip_console = false;
  bool tt_info = this->tooltip_info;
  this->tooltip_info = false;
  bool tt_shortcut = this->tooltip_shortcut;
  this->tooltip_shortcut = false;

  if (this->module == SCOPE && !this->running &&
      check_mouse_pos(display_x1-6, display_y1-6, display_w-1, display_h-1)) return CROSS_CURSOR;

  // przyciski nad obszarem wyswietlacza
  for (int i=0;i<MODULES;i++)
    if (check_mouse_pos(module_btn_x[i], module_btn_y, module_btn_w, module_btn_h))
      return HAND_CURSOR;

  // przycisk skrotow klaw.
  if (check_mouse_pos(shortcut_btn_x, shortcut_btn_y, shortcut_btn_w, shortcut_btn_h)) {
    this->tooltip_shortcut = true;
    if (!this->running) this->change_made = true;
    return HAND_CURSOR;
  }

  // przycisk screenshot
  if (check_mouse_pos(screenshot_btn_x, screenshot_btn_y, screenshot_btn_w, screenshot_btn_h)) {
    this->tooltip_screenshot = true;
    if (!this->running) this->change_made = true;
    return HAND_CURSOR;
  }
  // przycisk console
  if (check_mouse_pos(console_btn_x, console_btn_y, console_btn_w, console_btn_h)) {
    this->tooltip_console = true;
    if (!this->running) this->change_made = true;
    return HAND_CURSOR;
  }
  // przycisk info
  if (check_mouse_pos(info_btn_x, info_btn_y, info_btn_w, info_btn_h)) {
    this->tooltip_info = true;
    if (!this->running) this->change_made = true;
    return HAND_CURSOR;
  }

  // przycisk start/stop
  if (check_mouse_pos(running_btn_x, running_btn_y,
                      running_btn_w, running_btn_h)) return HAND_CURSOR;

  // checkboksy
  if (check_mouse_pos(channel_cbox_x[0], channel_cbox_y , cbox_w, cbox_h)) return HAND_CURSOR;
  if (check_mouse_pos(channel_cbox_x[1], channel_cbox_y , cbox_w, cbox_h)) return HAND_CURSOR;
  if (check_mouse_pos(labels_cbox_x, labels_cbox_y, cbox_w, cbox_h)) return HAND_CURSOR;
  if (check_mouse_pos(grid_cbox_x, grid_cbox_y, cbox_w, cbox_h)) return HAND_CURSOR;

  if ((this->module==SCOPE) &&
      (check_mouse_pos(timebase_mul_cbox_x, timebase_mul_cbox_y, cbox_w, cbox_h))) return HAND_CURSOR;

  // przyciski radio do wyzwalania
  for (int i=0;i<3;i++)
    if (check_mouse_pos(trigger_slope_radio_x, trigger_slope_radio_y[i], radio_w, radio_h))
      return HAND_CURSOR;

  for (int i=0;i<2;i++)
    if (check_mouse_pos(trigger_channel_radio_x[i], trigger_channel_radio_y, radio_w, radio_h))
      return HAND_CURSOR;

  // wybor okna dla spektrogramu
  if (this->module==SPECTRUM &&
      check_mouse_pos(window_menu_open_x, window_menu_open_y,
                      window_menu_open_w, window_menu_open_h)) return HAND_CURSOR;

  // menu wyboru okna dla spektrogramu
  if (this->module==SPECTRUM && this->spect_window_menu_open)
   for (int i=sizeof(*window_types);i>=0;i--)
    if (check_mouse_pos(window_menu_list_x, window_menu_list_y-(i+1)*window_menu_list_h+1,
                        window_menu_list_w, window_menu_list_h)) return HAND_CURSOR;

  // suwaki dla wykresu XY
  if (this->module==GRAPH_XY) {

    if (check_mouse_pos(xy_sliderx_x1, xy_sliderx_y - xy_slider_h/2, xy_sliderx_x2 - xy_sliderx_x1, xy_slider_h))
      return HAND_CURSOR;

    if (check_mouse_pos(xy_slidery_x1, xy_slidery_y - xy_slider_h/2, xy_slidery_x2 - xy_slidery_x1, xy_slider_h))
      return HAND_CURSOR;

  }


  float fi;

  // pokretla czulosci i przes. pionowego kanal 1-2
  for (int i=0;i<2;i++) {

     fi = this->get_knob_angle(sens_knob_x[i], sens_knob_y);
     if (fi>knob_min_angle && fi<knob_max_angle) return HAND_CURSOR;
     fi = this->get_knob_angle(vert_knob_x[i], vert_knob_y);
     if (fi>knob_min_angle && fi<knob_max_angle) return HAND_CURSOR;

  }

  // pokretlo podstawy czasowej
  fi = this->get_knob_angle(timebase_knob_x, timebase_knob_y);
  if (fi>knob_min_angle && fi<knob_max_angle) return HAND_CURSOR;

  // pokretlo poziomu wyzwalania
  fi = this->get_knob_angle(trigger_knob_x, trigger_knob_y);
  if (fi>knob_min_angle && fi<knob_max_angle) return HAND_CURSOR;


  // odswiezenie ekranu  po wyswietleniu tooltipa przy zatrzymanym przebiegu
  if (!this->running && (tt_screenshot || tt_console || tt_info || tt_shortcut)) this->change_made = true;

  return DEFAULT_CURSOR;

}


/*
Obsluzenie suwakow pod wykresem XY (wcisniety klawisz myszy)
*/
void UIState::check_sliders() {

  this->slider_mouse_drag = false;

  if (this->module!=GRAPH_XY || this->mousedown!=1) return;

  // suwak dla osi X
  if (check_mouse_pos(xy_sliderx_x1, xy_sliderx_y - xy_slider_h/2, xy_sliderx_x2 - xy_sliderx_x1, xy_slider_h)) {

    this->xy_x_scale = slider_x_to_value(this->mousex, xy_sliderx_x1, xy_sliderx_x2);
    this->sliders_changed = true;
    this->slider_mouse_drag = true;
    this->change_made = true;

  }

  // suwak dla osi Y
  if (check_mouse_pos(xy_slidery_x1, xy_slidery_y - xy_slider_h/2, xy_slidery_x2 - xy_slidery_x1, xy_slider_h)) {

    this->xy_y_scale = slider_x_to_value(this->mousex, xy_slidery_x1, xy_slidery_x2);
    this->sliders_changed = true;
    this->slider_mouse_drag = true;
    this->change_made = true;

  }



}



/*
Obsluzenie jednego pokretla (wcisniety klawisz myszy)
Parametry wejsciowe:
val - referencja do zmienianej wartosci
x, y - wspolrzedne pokretla
min, max, mid - minimalna, maksymalna i srodkowa wartosc pokretla
*/
void UIState::check_knob(float &val, int x, int y, float min, float max, float mid) {

    float fi = this->get_knob_angle(x, y);
    if (fi>knob_min_angle && fi<knob_max_angle) {

       val = knob_angle_to_value(fi, min, max, mid);
       this->knobs_changed = true;
       this->knob_mouse_drag = true;
       this->change_made = true;

    }

}



/*
Obsluzenie pokretel (wcisniety klawisz myszy)
*/
void UIState::check_knobs() {

  this->knob_mouse_drag = false;

  if (this->mousedown!=1) return;

   // pokretla czulosci i przesuniecia pionowego kanal 1-2
  for (int i=0; i<2; i++) {
    this->check_knob(this->sensivity[i], sens_knob_x[i], sens_knob_y,
                      SENSIVITY_MIN, SENSIVITY_MAX, SENSIVITY_MID);
    this->check_knob(this->vert_offset[i], vert_knob_x[i], vert_knob_y,
                      VERT_OFFSET_MIN, VERT_OFFSET_MAX, VERT_OFFSET_MID);
  }

  // pokretlo podstawy czasowej
  float tb_mul = this->timebase_mul ? TIMEBASE_MUL : 1.0;
  this->check_knob(this->timebase, timebase_knob_x, timebase_knob_y,
                      TIMEBASE_MIN*tb_mul, TIMEBASE_MAX*tb_mul, TIMEBASE_MID*tb_mul);

  // pokretlo poziomu wyzwalania
  this->check_knob(this->trigger.level, trigger_knob_x, trigger_knob_y,
                      TRIGGER_LEVEL_MIN, TRIGGER_LEVEL_MAX, TRIGGER_LEVEL_MID);


}


/*
Przelaczenie mnoznika podstawy czasowej
*/
void UIState::toggle_timebase_mul() {

  this->timebase_mul = !this->timebase_mul;

  if (this->module == SCOPE)
    render_checkbox(timebase_mul_cbox_x, timebase_mul_cbox_y, this->timebase_mul);

  if (this->module == GRAPH_XY)  this->tb_xy_mul = true;

  if (this->timebase_mul) this->timebase*=TIMEBASE_MUL;
  else this->timebase/=TIMEBASE_MUL;

  this->change_made = true;
  this->knobs_changed = true;
  this->timebase_mul_changed = true;

}


/*
Przelaczenie kanalu
Parametr wejsciowy:
ch - nr kanalu do przelaczenia (0-1)
*/
void UIState::toggle_channel(int ch) {

  this->channel_on[ch] = !this->channel_on[ch];
  render_checkbox(channel_cbox_x[ch], channel_cbox_y, this->channel_on[ch]);
  this->change_made = true;

}


/*
Obsluzenie checkboksow (wcisniety klawisz myszy)
*/
void UIState::check_checkboxes() {

  if (this->mousedown!=1) return;

  // kanal nr 1
  if (check_mouse_pos(channel_cbox_x[0], channel_cbox_y, cbox_w, cbox_h))
    this->toggle_channel(0);

  // kanal nr 2
  else if (check_mouse_pos(channel_cbox_x[1], channel_cbox_y, cbox_w, cbox_h))
    this->toggle_channel(1);

  // etykiety wl/wyl
  else if (check_mouse_pos(labels_cbox_x, labels_cbox_y, cbox_w, cbox_h)) {
     this->labels_on = !this->labels_on;
     render_checkbox(labels_cbox_x, labels_cbox_y, this->labels_on);
     this->change_made = true;
  }

  // siatka wl/wyl
  else if (check_mouse_pos(grid_cbox_x, grid_cbox_y, cbox_w, cbox_h)) {
     this->grid_on = !this->grid_on;
     render_checkbox(grid_cbox_x, grid_cbox_y, this->grid_on);
     this->change_made = true;
  }

  // podstawa czasowa x1000 (tylko oscylogram)
  else  if ((this->module==SCOPE) &&
      (check_mouse_pos(timebase_mul_cbox_x, timebase_mul_cbox_y, cbox_w, cbox_h))) this->toggle_timebase_mul();

  // wyzwalanie - kanaly
  for (int i=0;i<CHANNELS;i++)
   if (check_mouse_pos(trigger_channel_radio_x[i], trigger_channel_radio_y, radio_w, radio_h)) {
     this->trigger.channel = i;
     render_radios();
     this->change_made = true;
  }

  // wyzwalanie - tryb
  for (int i=0;i<3;i++)
   if (check_mouse_pos(trigger_slope_radio_x, trigger_slope_radio_y[i], radio_w, radio_h)) {
     this->trigger.slope = i;
     render_radios();
     this->change_made = true;
  }


}


/*
Obsluzenie menu wyboru modulow wyswietlacza (wcisniety klawisz myszy)
*/
void UIState::check_modules_menu() {

  this->module_changed = false;

  if (this->mousedown==1)
   for (int i=0;i<MODULES;i++)
    if (check_mouse_pos(module_btn_x[i], module_btn_y, module_btn_w, module_btn_h)) {
      change_module(i+1);
      this->change_made = true;
      break;
    }

}


void UIState::toggle_running() {

  this->running = !this->running;
  render_running_button(this->running);

  if (!this->running) this->change_made = true;

}


/*
Obsluzenie przycisku start/stop (wcisniety klawisz myszy)
*/
void UIState::check_running() {

  if ((this->mousedown!=1 ||
      !check_mouse_pos(running_btn_x, running_btn_y, running_btn_w, running_btn_h))) return;

  this->toggle_running();

}


/*
Sprawdzenie ikony skrotow klaw. (wcisniety klawisz myszy)
Zwracana wartosc: true jezeli przycisnieto
*/
bool UIState::check_shortcut_button() {

  return (this->mousedown==1 &&
          check_mouse_pos(shortcut_btn_x, shortcut_btn_y, shortcut_btn_w, shortcut_btn_h));

}




/*
Sprawdzenie ikony screenshot (wcisniety klawisz myszy)
Zwracana wartosc: true jezeli przycisnieto
*/
bool UIState::check_screenshot_button() {

  return (this->mousedown==1 &&
          check_mouse_pos(screenshot_btn_x, screenshot_btn_y, screenshot_btn_w, screenshot_btn_h));

}


/*
Sprawdzenie ikony konsoli (wcisniety klawisz myszy)
Zwracana wartosc: true jezeli przycisnieto
*/
bool UIState::check_console_button() {

  return (this->mousedown==1 &&
          (check_mouse_pos(console_btn_x, console_btn_y, console_btn_w, console_btn_h)));

}



/*
Sprawdzenie ikony info (wcisniety klawisz myszy)
Zwracana wartosc: true jezeli przycisnieto
*/
bool UIState::check_info_button() {

  return (this->mousedown==1 && check_mouse_pos(info_btn_x, info_btn_y, info_btn_w, info_btn_h));

}



/*
Obsluzenie menu wyboru okna czasowego (spektrogram, wcisniety klawisz myszy)
*/
void UIState::check_spect_window_menu() {

  if (this->mousedown!=1 || this->module!=SPECTRUM) return;

  // wybor konkretnej opcji
  if (this->spect_window_menu_open)
   for (int i=sizeof(*window_types);i>=0;i--)
    if (check_mouse_pos(window_menu_list_x, window_menu_list_y-(i+1)*window_menu_list_h+1,
                        window_menu_list_w, window_menu_list_h-1)) {
      this->spect_window = i+1;
      this->spect_window_menu_open = false;
      break;
    }

  // otwarcie / zamkniecie menu
  if (check_mouse_pos(window_menu_open_x, window_menu_open_y,
                      window_menu_open_w, window_menu_open_h))
     this->spect_window_menu_open = !this->spect_window_menu_open;

}


/*
Funkcja zmienia nr modulu i wykonuje powiazane inicjujace operacje
Parametry wejsciowe:
n_modul - nr nowego modulu [1-5]
skala - skala osi Y
filtr - nr wybranego filtra [0-3]
*/
void UIState::change_module(int module) {

   this->module_prev = this->module;
   this->module =  module;

   // zmiana przyciskow w glownym menu
   render_module_button(this->module_prev);
   render_module_button(module, true);

   // dodatkowe opcje dla spektrogramu
   if (this->module == SPECTRUM) {
     apply_surface(bottom_panel_x, bottom_panel_y, screen_bottom_spect_panel, screen);
     out_text(screen, window_menu_tsel_x, window_menu_tsel_y,
              window_types[this->spect_window-1], cl_gray2, font_normal);
   }
   else if (this->module == GRAPH_XY) {
     apply_surface(bottom_panel_x, bottom_panel_y, screen_bottom_xy_panel, screen);
     uistate.sliders_changed = true;
   }
   else {
     apply_surface(bottom_panel_x, bottom_panel_y, screen_bottom_panel, screen);
     render_checkbox(timebase_mul_cbox_x, timebase_mul_cbox_y, this->timebase_mul);
     this->timebase_mul_changed = false;
   }

   this->module_changed = true;
   display_clean();


   SDL_Flip(screen);

}



/*
Funkcja sprawdza czy pozycja myszy zawiera sie w podanym prostokatnym obszarze
Parametry wejsciowe:
x,y - gorne wierzcholki obszaru
w,h - szerokosc i wysokosc obszaru
Zwracana wartosc: true/false
*/
bool UIState::check_mouse_pos(int x, int y, int w, int h) {

  return (this->mousex >= x && this->mousey >= y && this->mousex <= x + w && this->mousey <= y + h);

}



/*
Funkcja wylicza kat miedzy promieniem pokretla a osia OY,
zero to ustawienie w polowie skali (pionowo)
Parametry wejsciowe:
knob_x, knob_y - srodek danego pokretla
Zwracana wartosc:
Miara kata (w stopniach) miedzy promieniem pokretla a osia OY
*/
float UIState::get_knob_angle(int knob_x, int knob_y) {

  float fi = -180.0;
  // otoczenie badanego punktu na okregu
  float eps = 14.0;

  // @TODO tu mozna by zrobic jeszcze wstepne sprawdzenie kwadratu zawierajacego okrag
  // if (!check_mouse_pos(sens_knob_x-knob_r-eps, sens_knob_y-knob_r-eps, knob_r+2*eps, knob_r+2*eps))
  // nie sprawdzamy wspolrzednych ekranowych ponizej minimum i maksimum
  if (this->mousey>(float)knob_y+knob_r-eps) return fi;

  // rownanie okregu
  float circle = pow(this->mousex - knob_x, 2) + pow(this->mousey - knob_y, 2);

  // czy wspolrzedne myszy naleza do okregu (z dokl. co do otoczenia eps)
  if (pow(knob_r-eps, 2) <= circle  &&  circle <= pow(knob_r+eps, 2)) {

    //  arcus tangens zwraca miare kata, nie ma ograniczen co do dziedziny
    fi = -pi/2 + atanf(((float)this->mousey - (float)knob_y) / ((float)this->mousex - (float)knob_x));
    // przesuniecie o pi dla prawej czesci skali
    if (mousex>=(float)knob_x) fi += pi;
    // zamiana z radianow na stopnie
    fi = fi*180.0/pi;

  }

  return fi;

}


/*
Funkcja sprawdza warunek wyzwalania sygnalu
Parametry wejsciowe:
prev1, prev2, curr - 2 poprzednie i biezaca probka sygnalu
*/
bool UIState::check_trigger(float prev1, float prev2, float curr) {

  int ch = this->trigger.channel;
  if (!this->channel_on[ch] || this->trigger.catched) return (this->trigger.catched && this->channel_on[ch]);
  this->trigger.check(this->sensivity[ch], this->vert_offset[ch], prev1, prev2, curr);
  return (this->trigger.catched);

}


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
float knob_value_to_angle(float val, float min, float max, float mid) {

   if (val < mid) {

      float angle = ((mid-val) / (mid-min)) * fabs(knob_min_angle);
      return (-pi/2 - angle/180.0*pi);

   }

   else {

      float angle = ((val-mid) / (max-mid)) * knob_max_angle;
      return (-pi/2 + angle/180.0*pi);

   }


}


/*
Funkcja zamienia kat miedzy promieniem pokretla a osia OY na wartosc
Parametry wejsciowe:
angle - miara kata (w stopniach)
min - minimalna wartosc na pokretle
max - maksymalna wartosc na pokretle
mid - srodkowa wartosc na pokretle
*/
float knob_angle_to_value(float angle, float min, float max, float mid) {

   if (angle < 0) {

     return (mid - (-angle*(mid-min) / fabs(knob_min_angle)));


   }
   else {

     return (mid + (angle*(max-mid) / knob_max_angle));

   }

}


/*
Przeliczenie wartosci parametru na wspolrzedna x na suwaku
Parametry wejsciowe:
val - wartosc parametru skali
x1, x2 - poczatkowa i koncowa wspolrzedna obszaru suwaka
Zwracana wartosc:
Wspolrzedna x przesuwnego elementu na suwaku
*/
int slider_value_to_x(float val, int x1, int x2) {

   if (val < 1.0) {

     return x1 + (x2-x1) * (val*XY_SCALE_MAX - 1.0 + XY_SCALE_MIN) / (2 * XY_SCALE_MAX) + xy_slider_mx_offset;

   }
   else {

     return x1 + (x2-x1) * (val - 1.0 + XY_SCALE_MAX) / (2 * XY_SCALE_MAX) + xy_slider_mx_offset;

   }

}



/*
Przeliczenie wspolrzednej x na suwaku n wartosc parametru
Parametry wejsciowe:
x - wspolrzedna x przesuwnego elementu na suwaku
x1, x2 - poczatkowa i koncowa wspolrzedna obszaru suwaka
Zwracana wartosc:
Wartosc parametru skali
*/
float slider_x_to_value(int x, int x1, int x2) {

   float val = static_cast<float>(x-x1) * XY_SCALE_MAX * 2 / static_cast<float>(x2-x1) - XY_SCALE_MAX;

   if (val <= 1.0) val = XY_SCALE_MIN + (XY_SCALE_MAX + val) / XY_SCALE_MAX - 0.4;

   if (val < XY_SCALE_MIN)  val = XY_SCALE_MIN;

   return val;

}



/*
Okreslenie jednostki wartosci do wyswietlenia
Parametr wejsciowy:
value - wartosc ktora ma byc wyswietlona
precision - ilosc miejsc po przecinku (opcjonalnie)
Zwracana wartosc
Przeskalowana do wyswietlenia wartosc wraz z jednostka
*/
string get_value_unit(float value, int precision) {

  stringstream oss;

  oss << setprecision(precision) << fixed;

  // nano
  if (value>0.0 && value<=0.00000009)
    oss << (value*pow(10, 9)) << "n";

  // mikro
  else if (value>0.0 && value<=0.00009)
    oss << (value*pow(10, 6)) << "u";

  // mili
  else if (value>0.0 && value<=0.009)
    oss << (value*1000) << "m";

  else
    oss << value;


  return oss.str();

}




