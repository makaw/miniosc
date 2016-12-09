/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka display - operacje na obszarze "wyswietlacza", wykresy
 *
 *****************************************************************************/


#include <iostream>
#include <ctime>
#include <SDL/SDL.h>
#include <sys/stat.h>
#include <iomanip>

#include "display.h"
#include "gui.h"
#include "graphics.h"
#include "spectrum.h"
#include "info.h"
#include "config.h"



// potrzebne w przypadku pakietowania dla Debiana
// -----------------------------------------------
#ifdef DEBIAN_PACKAGE
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
// -----------------------------------------------



using namespace std;



// wskaznik do zachowanego tla wyswietlacza
SDL_Surface* screen_display_bg = NULL;


// przygotowanie etykiet dla spektrogramu
void display_spectrum_labels(float *max_freq);
// przygotowanie etykiet dla oscylogramu
void display_scope_labels(int x_offset);
// przygotowanie etykiet dla wykresu XY
void display_graph_xy_labels();
// rysowanie siatki na obszarze wyswietlacza
void display_grid();
// etykiety zwiazane z wyzwalaniem sygnalu
void display_trigger_labels();


/*
Wyczyszczenie obszaru wyswietlacza
*/
void display_clean() {

   apply_surface(display_x1, display_y1, screen_display_bg, screen);
   display_grid();

}


/*
Przyciecie odcinka do wymiarow wyswietlacza
Parametry wejsciowe:
x1, y1, x2, y2 - referencje do wspolrzednych odcinka
Zwracana wartosc:
True, jezeli mozna rysowac
*/
bool display_wave_cut(int &x1, int &y1, int &x2, int &y2) {

  if (x1==x2 && y1==y2)  return false;

  return cs_cut(display_x1, display_y1, display_x2, display_y2, x1, y1, x2, y2);

}


/*
Funkcja wizualizuje na wyswietlaczu sygnal
Parametry wejsciowe:
data - wskaznik do bufora danych (tablicy)
b_size - ilosc elementow w buforze
b_pos - aktualna pozycja na liscie buforow
x_scale - wspolczynnik przeskalowania do wymiaru okna
sign - 1 lub -1, -1 odwraca wykres  (dla spektrogramu)
max_freq - maksymalne czestotliwosci   (dla spektrogramu)
x_offset - przesuniecie poziome (dla oscylogramu, dla malych podstaw czasu)
labels - czy wyswietlac etykiety
*/
void display_draw_wave(float data[][BUFFER_SIZE], int b_size, int b_pos, float x_scale, int sign, float *max_freq, int x_offset, bool labels) {


    int x,y, oldx[CHANNELS] = {}, oldy[CHANNELS] = {};

    sign = (sign < 0) ? -1.0 : 1.0;

    float y_offset = (uistate.module==SPECTRUM) ? display_y2 - 35 : grid_axis_y;

    // odwrocona kolejnosc, ostatni rysowany pierwszy kanal (na wierzchu)
    for (int ch=CHANNELS-1; ch>=0; ch--)
     if (uistate.channel_on[ch])
      for(int i=x_offset; i < b_size; i++)  {

        float vert_offset = uistate.vert_offset[ch]*GRID_DIV_H;
        if (uistate.module==SPECTRUM) vert_offset = fabs(vert_offset);

        y = sign * uistate.sensivity[ch] * GRID_DIV_W * _AMP_GAIN * data[ch][i] + y_offset - vert_offset;
        // przeskalowanie wykresu do wymiaru okna
        x = (i-x_offset) * x_scale + display_x1;
        if (b_pos>0) x += b_pos * BUFFER_SIZE * x_scale;

        // kreslenie wykresu sygnalu
        if (i>x_offset && display_wave_cut(oldx[ch], oldy[ch], x, y))
          draw_line(screen, oldx[ch], oldy[ch], x, y, channel_colors[ch]);

        oldx[ch] = x;  oldy[ch] = y;

    }

    // etykiety
    if (labels) {
      if (uistate.module==SPECTRUM) display_spectrum_labels(max_freq);
      else  display_scope_labels(x_offset);
    }

}


/*
Funkcja wizualizuje na wyswietlaczu sygnal (dla spektrogramu)
Parametry wejsciowe:
data - wskaznik do bufora danych (tablicy)
b_size - ilosc elementow w buforze
max_freq - maksymalne czestotliwosci
*/
void display_draw_wave(float data[][BUFFER_SIZE], int b_size, float *max_freq) {

  display_draw_wave(data, b_size, -1, base_x_scale, -1, max_freq, 0, true);

}


/*
Funkcja wizualizuje na wyswietlaczu sygnal (dla oscylogramu)
Parametry wejsciowe:
data - wskaznik do bufora danych (tablicy)
b_size - ilosc elementow w buforze
b_pos - aktualna pozycja na liscie buforow
x_scale - wspolczynnik przeskalowania do wymiaru okna
x_offset - przesuniecie poziome (opcjonalnie, dla malych podstaw czasu)
labels - czy wyswietlac etykiety (opcjonalnie)
*/
void display_draw_wave(float data[][BUFFER_SIZE], int b_size, int b_pos, float x_scale, int x_offset, bool labels) {

  display_draw_wave(data, b_size, b_pos, x_scale, 1, NULL, x_offset, labels);

}



/*
Funkcja wizualizuje na wyswietlaczu sygnal w ukladzie X-Y (odpowiednio kanal 1-2)
Parametry wejsciowe:
data - wskaznik do bufora danych (tablicy)
b_size - ilosc elementow w buforze
labels - true jezeli maja byc rysowane etykiety
*/
void display_draw_wave_xy(float data[][BUFFER_SIZE], int b_size, bool labels) {

   if (CHANNELS<2)  return;

   int x,y, oldx = 0, oldy = 0;

   int grid_w = GRID_DIV_W;
   int grid_h = GRID_DIV_H;

   if (uistate.module==GRAPH_XY) {
     grid_w *= uistate.xy_x_scale;
     grid_h *= uistate.xy_y_scale;
   }

   for(int i=0; i < b_size; i++)  {

        x = (!uistate.channel_on[0]) ? 0
            : uistate.sensivity[0] * grid_w * _AMP_GAIN * data[0][i] + grid_axis_x + (uistate.vert_offset[0]*grid_w);
        y = (!uistate.channel_on[1]) ? 0
            : uistate.sensivity[1] * grid_h * _AMP_GAIN * data[1][i] + grid_axis_y - (uistate.vert_offset[1]*grid_h);


        if (i>0 && display_wave_cut(oldx, oldy, x, y))
             draw_line(screen, oldx, oldy, x, y, xy_color);

        oldx = x;  oldy = y;

    }

    // etykiety
    if (labels) display_graph_xy_labels();


}



/*
Funkcja rysuje siatke na obszarze wyswietlacza
*/
void display_grid() {

   if (!uistate.grid_on) return;

   int grid_w = GRID_DIV_W;
   int grid_h = GRID_DIV_H;

   if (uistate.module==GRAPH_XY) {
     grid_w *= uistate.xy_x_scale;
     grid_h *= uistate.xy_y_scale;
   }

   // rysowanie siatki od srodka
   for (int i=grid_axis_x + grid_w; i<display_x2; i+=grid_w)
       draw_line(screen, i, display_y1+1, i, display_y2-1, grid_color);

   for (int i=grid_axis_x - grid_w; i>display_x1; i-=grid_w)
       draw_line(screen, i, display_y1+1, i, display_y2-1, grid_color);

   for (int i=grid_axis_y+grid_h;i<display_y2;i+=grid_h)
     draw_line(screen, display_x1+1, i, display_x2-1, i, grid_color);

   for (int i=grid_axis_y-grid_h;i>display_y1;i-=grid_h)
     draw_line(screen, display_x1+1, i, display_x2-1, i, grid_color);

   // srodek ukladu
   draw_line(screen, grid_axis_x, display_y1+1, grid_axis_x, display_y2-1,
             uistate.module!=SPECTRUM ? grid_center_color : grid_color);
   draw_line(screen, display_x1+1, grid_axis_y, display_x2-1, grid_axis_y,
             uistate.module!=SPECTRUM ? grid_center_color : grid_color);


}



/*
Funkcja przygotowuje podpis podzialki i inne etykiety dla spektrogramu
Parametry wejsciowe:
max_freq - maksymalne czestotliwosci dla poszczegolnych kanalow
*/
void display_spectrum_labels(float* max_freq) {

  if (!uistate.labels_on) return;

  int gridx = GRID_DIV_W/4;

  for (int i=0;i<display_w;i+=GRID_DIV_W) {

     stringstream oss;
     oss  << round(i*(FREQ_MAX/BUFFER_SIZE)/base_x_scale);
     string s = oss.str();
     int tx = display_x1+gridx+i-(5.8*(float)(s.length()));
     if (tx<display_x2-GRID_DIV_W)
       out_text(screen, display_x1+gridx+i-round(6.2*s.length()), display_y2-15, s, cl_olive, font_lcd);

  }

  out_text(screen, display_x2-30, display_y2-15, "[Hz]", cl_olive, font_lcd);

  // ew wypisanie maksymalnych czestotliwosci dla kanalow
  if (max_freq != NULL) {

    for (int ch=0;ch<CHANNELS;ch++)
     if (uistate.channel_on[ch]) {

       { stringstream oss;
       oss << "K" << (ch+1) << " max " << max_freq[ch];
       out_text(screen, display_x1 + 10, display_y1 + 10 + 16*ch, oss.str(),
                get_sdl_color(channel_colors[ch]), font_lcd); }


       { stringstream oss;
       oss << "±" << freq_error << " [Hz]";
       out_text(screen, display_x1 + 100, display_y1 + 10 + 16*ch, oss.str(),
                 get_sdl_color(channel_colors[ch]), font_lcd); }


     }


  }

  // wypisanie nazwy okna czasowego
  stringstream oss;
  oss << "# " << window_types[uistate.spect_window-1];
  out_text(screen, display_x2-88, display_y1+26, oss.str(), cl_olive, font_lcd);

  // etykiety zwiazane z wyzwalaniem
  display_trigger_labels();

}



/*
Funkcja przygotowuje etykiety dla oscylogramu
Parametr wejsciowy:
x_offset - przesuniecie w poziomie w prawo (trigger, mala podstawa)
*/
void display_scope_labels(int x_offset) {

  if (!uistate.labels_on) return;

  float div_s =  (uistate.timebase/BUFFER_SIZE)/base_x_scale;

  int gridx = GRID_DIV_W/4;

  for (int i=GRID_DIV_W;i<display_w;i+=GRID_DIV_W*2) {

     string s = get_value_unit((i+x_offset)*div_s);
     int tx = display_x1+gridx+i-(5.8*(float)(s.length()));
     if (tx<display_x2-GRID_DIV_W)
       out_text(screen, display_x1+gridx+i-round(6.2*s.length()), display_y2-15, s, cl_olive, font_lcd);

  }

  out_text(screen, display_x2-30, display_y2-15, "[s]", cl_olive, font_lcd);

  // wypisanie skali czasowej dla dzialki
  stringstream oss;
  oss << "1 DIV = " << get_value_unit(div_s * GRID_DIV_W) << " [s]";
  out_text(screen, display_x1 + 10, display_y1+10, oss.str(), cl_olive, font_lcd);

  // etykiety zwiazane z wyzwalaniem
  display_trigger_labels();


}


/*
Funkcja przygotowuje etykiety dla wykresu XY
*/
void display_graph_xy_labels() {

  if (!uistate.labels_on) return;

  // opisanie osi
  out_text(screen, grid_axis_x + 10, display_y1 + 10, "X [K1]",
           uistate.channel_on[0] ? cl_olive : cl_red, font_lcd);
  out_text(screen, display_x2-50, grid_axis_y + 10, "Y [K2]",
           uistate.channel_on[1] ? cl_olive : cl_red, font_lcd);

  // etykiety zwiazane z wyzwalaniem
  display_trigger_labels();

}


/*
Wyswietla etykiety zwiazane z wyzwalaniem sygnalu
*/
void display_trigger_labels() {


  if (!uistate.labels_on || uistate.trigger.slope==TRIGGER_OFF) return;

  SDL_Color col = !uistate.trigger.catched ? cl_yellow2 : cl_red;

  {

    stringstream oss;
    oss << "TRIG K" << (uistate.trigger.channel+1) << " @ " << fixed << setprecision(2) << uistate.trigger.level << " [DIV]";
    out_text(screen, display_x2-150, display_y1+10, oss.str(), col, font_lcd);

  }


  if (uistate.module == SCOPE) {

    int _y = grid_axis_y - (uistate.trigger.level * GRID_DIV_H);

    if (_y > display_y1 && _y< display_y2) {
      draw_line(screen, display_x1 + 1, _y, display_x2 - 1, _y, get_uint32_color(col), true);
    }

    if (_y - 14 > display_y1 && _y-4 < display_y2) {
      stringstream oss;
      oss << "K" << (uistate.trigger.channel+1) << " " << ((uistate.trigger.slope == TRIGGER_FALL) ? "OPADA" : "WZRASTA");
      out_text(screen, display_x1 + 10, _y - 14, oss.str(), col, font_lcd);
    }

    // ewentualnie miejsce spelnienia warunku wyzwalania
    if (uistate.trigger.catched)
      out_text(screen, uistate.trigger.cx, uistate.trigger.cy, "•", cl_red, font_big);

  }

}



/*
Funkcja wykonuje screenshot obszaru wyswietlacza
*/
void display_screenshot() {

  SDL_Surface* scr = create_surface(display_w+1, display_h+1, screen);

  // przygotowanie obrazu
  SDL_Rect offset;
  offset.x = display_x1; offset.y = display_y1;
  offset.w = display_w;  offset.h = display_h;
  SDL_BlitSurface(screen, &offset, scr, NULL);

  // w razie potrzeby nalozenie warstwy spod konsoli/tooltipow
  if (uistate.tooltip_screenshot || uistate.tooltip_console || uistate.tooltip_shortcut ||
      uistate.tooltip_info || uistate.console.active)
    apply_surface(display_w-con_w, 0, screen_console_tmp, scr);

  // przygotowanie nazwy pliku
  time_t curr_time;
  char ftime[80];
  time(&curr_time);
  strftime (ftime, 80, "%d-%m-%Y_%H-%M-%S", localtime(&curr_time));

  // -----------------------------------------------
  // potrzebne w przypadku pakietowania dla Debiana
  #ifdef DEBIAN_PACKAGE
  const char* homedir;
  if ((homedir = getenv("HOME")) == NULL) homedir = getpwuid(getuid())->pw_dir;

  // zapis
  stringstream oss;
  oss << homedir << "/osc_screenshots/" << ftime << ".bmp";
  const string fname = oss.str();

  stringstream os2;
  os2 << homedir << "/osc_screenshots";
  stringstream os3;
  os3 << "mkdir " << os2.str();

  struct stat info;
  int mkd = 0;
  if(stat(os2.str().c_str(), &info)!= 0) mkd = system(os3.str().c_str());
  // -----------------------------------------------
  #else

  // zapis
  stringstream oss;
  oss << "osc_screenshots/" << ftime << ".bmp";
  const string fname = oss.str();

  struct stat info;
  int mkd = 0;
  // windows/linux - tak samo
  if(stat("osc_screenshots", &info)!= 0) mkd = system("mkdir osc_screenshots");

  #endif

  vector<string> s = vector<string>();

  s.push_back("Zrzut ekranu");

  if (mkd != -1 && SDL_SaveBMP(scr, fname.c_str()) != -1) {

    s.push_back("Zrzut ekranu obszaru wyświetlacza w formacie BMP został");
    s.push_back("zapisany w pliku: ");
    s.push_back(" ");
    s.push_back(fname);

  }

  else  s.push_back("Niestety wystąpił problem z zapisem pliku ....");

  info_text(10, 10, s);

  SDL_FreeSurface(scr);

}



