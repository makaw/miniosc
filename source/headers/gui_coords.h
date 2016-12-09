/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Plik naglowkowy gui_coords - wspolrzedne ekranowe dla GUI
 *
 *****************************************************************************/



#ifndef GUI_COORDS_INCLUDED
#define GUI_COORDS_INCLUDED

#include <cmath>

#include "config.h"


// wspolrzedne przyciskow gornego menu modulow wyswietlacza
const int module_btn_x[MODULES] = {230, 334, 438};
const int module_btn_y = 12, module_btn_w = 104, module_btn_h = 21;

// wspolrzedne ikon-przyciskow: skroty klaw., screenshot, konsola, info (prawy gorny rog)
const int shortcut_btn_x = 668, shortcut_btn_y = 8,
           shortcut_btn_w = 20, shortcut_btn_h = 18;
const int screenshot_btn_x = 728, screenshot_btn_y = 9,
           screenshot_btn_w = 21, screenshot_btn_h = 16;
const int console_btn_x = 699, console_btn_y = 8, console_btn_w = 20,
           console_btn_h = 18;
const int info_btn_x = 764, info_btn_y = 9, info_btn_w = 16, info_btn_h = 16;


// wspolrzedne przycisku start stop
const int running_btn_x = 140, running_btn_y = 268, running_btn_w = 70,
           running_btn_h = 65;

// wysokosc i szerokosc checkboksow
const int cbox_w = 16, cbox_h = 16;

// wspolrzedne checkboksow kanalow
const int channel_cbox_x[2] = {35, 132}, channel_cbox_y = 56;

// wspolrzedne checkboksow etykiet i siatki
const int labels_cbox_x = 620, labels_cbox_y = 474, grid_cbox_x = 719, grid_cbox_y = 474;

// checkbox podstawy czasowej x1000
const int timebase_mul_cbox_x = 244, timebase_mul_cbox_y = 474;

// wysokosc i szerokosc przyciskow radio
const int radio_w = 14, radio_h = 14;

// wspolrzedne przyciskow radio - tryb zbocza wyzwalania
const int trigger_slope_radio_x = 130, trigger_slope_radio_y[3] = {381, 405, 429};

// wspolrzedne przyciskow radio - kanal wyzwalania
const int trigger_channel_radio_x[2] = {90, 153},  trigger_channel_radio_y = 472;

// dolny panel pod wyswietlaczem
const int bottom_panel_x = 223, bottom_panel_y = 462,
           bottom_panel_w = 380, bottom_panel_h = 38;

// wspolrzedne przycisku rozwijania listy wyboru okna czasowego
const int window_menu_open_x = 330, window_menu_open_y = 472,
           window_menu_open_w = 16, window_menu_open_h = 17;
// wspolrzedne opcji rozwinietej listy wyboru okna czasowego
const int window_menu_list_x = 330, window_menu_list_y = 470,
           window_menu_list_w = 99, window_menu_list_h = 20;
// wspolrzedne tekstu z wybranym oknem czasowym
const int window_menu_tsel_x = 353, window_menu_tsel_y = 475;

// wymiary tooltipow
const int tooltip_w = 80, tooltip_h = 17;

// promien pokretla
const int knob_r = 31;
// minimalny i maksymalny kat w stopniach
const float knob_min_angle = -138.0, knob_max_angle = 138.0;
// wspolrzedne srodka pokretel
const int sens_knob_x[2] = {72, 170}, sens_knob_y = 120;
const int vert_knob_x[2] = {71, 170}, vert_knob_y = 211;
const int timebase_knob_x = 72, timebase_knob_y = 311;
const int trigger_knob_x = 72, trigger_knob_y = 413;

// wspolrzedne suwakow pod wykresem XY - przesuniecie ze wzgledu na rozmiar przesuwnika i kursor-lapke
const int xy_slider_mx_offset = 11;
// wspolrzedne suwakow pod wykresem XY
const int xy_sliderx_x1 = 266-xy_slider_mx_offset, xy_sliderx_x2 = 370-xy_slider_mx_offset,
           xy_sliderx_y = 480;
const int xy_slidery_x1 = 444-xy_slider_mx_offset, xy_slidery_x2 = 548-xy_slider_mx_offset,
           xy_slidery_y = 480;
const int xy_slider_h = 10;


// wymiary obszaru wyswietlacza
const int display_x1 = 225, display_y1 = 34, display_x2 = 781, display_y2 = 461;
const int display_w = display_x2 - display_x1, display_h = display_y2 - display_y1;

// wspolrzedne srodka siatki wyswietlacza
const int grid_axis_x = display_x1 + (floor(display_w/GRID_DIV_W/2) ) * GRID_DIV_W;
const int grid_axis_y = display_y1 + (floor(display_h/GRID_DIV_H/2) + 1 ) * GRID_DIV_H;

// wymiary i polozenie konsoli
const int con_w = 330, con_h = 290;
const int con_x = display_x2 - con_w, con_y = display_y1;

// miejsce wyswietlania wspolrzednych kursora na oscylogramie
const int cursor_coords_x = 450, cursor_coords_y = 472, cursor_coords_w = 150, cursor_coords_h = 22;


// wspolczynnik przeskalowania wykresu w poziomie (tak aby wypelnic okno)
// nie uwzglednia podstawy czasowej
const float base_x_scale = round(100.0 * display_w / static_cast<float>(BUFFER_SIZE))/100.0;


// wspolrzedne obszaru tooltipow
const int tooltip_area_x = 600, tooltip_area_y = display_y1,
          tooltip_area_w = display_x2 - tooltip_area_x, tooltip_area_h = 80;


#endif

