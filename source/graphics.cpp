/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka graphics - operacje graficzne w SDL (rysowanie, kursory, warstwy, tekst)
 *
 *****************************************************************************/



#include <sstream>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "headers/cursors.h"
#include "headers/gui.h"



using namespace std;


// wskaznik do domyslnego kursora myszy
SDL_Cursor* default_cursor;


/*
Zapalenie jednego piksela. Wartstwa scr musi byc zablokowana!
Parametry wejsciowe:
scr - wskaznik do warstwy obrazu
x,y - wspolrzedne piksela
color - kolor piksela (HEX)
*/
void put_pixel(SDL_Surface *scr, Sint32 x, Sint32 y, Uint32 color) {

  * ((Uint32 *)scr->pixels + y * scr->w + x) = color;

}



/*
Funkcja rysuje linie przy pomocy algorytmu Bresenhama
Parametry wejsciowe:
scr - wskaznik do warstwy obrazu
x1,y1,x2,y2 - wspolrzedne krancow linii
color - kolor pikseli (HEX)
dashed - true jezeli linia przerywana (opcjonalnie)
*/
void draw_line(SDL_Surface *scr, Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2,  Uint32 color, bool dashed = false) {

     if(SDL_MUSTLOCK(scr)) SDL_LockSurface(scr);

     // zmienne pomocnicze
     int d, dx, dy, ai, bi, xi, yi;
     int x = x1, y = y1;
     // ustalenie kierunku rysowania
     if (x1 < x2)  {
         xi = 1;  dx = x2 - x1;
     }
     else  {
         xi = -1;  dx = x1 - x2;
     }

     if (y1 < y2) {
         yi = 1;  dy = y2 - y1;
     }
     else   {
         yi = -1;  dy = y1 - y2;
     }

     if (!dashed || (x+y)%4 == 0) put_pixel(scr, x, y, color);

     // oś wiodąca OX
     if (dx > dy) {

         ai = (dy - dx) * 2;
         bi = dy * 2;
         d = bi - dx;
         // pętla po kolejnych x
         while (x != x2)
         {
             // test współczynnika
             if (d >= 0) {
                 x += xi; y += yi; d += ai;
             } else {
                 d += bi; x += xi;
             }

             if (!dashed || (x+y)%4 == 0)  put_pixel(scr, x, y, color);

         }
     }
     // oś wiodąca OY
     else {

         ai = ( dx - dy ) * 2;
         bi = dx * 2;
         d = bi - dy;
         // pętla po kolejnych y
         while (y != y2)
         {
             // test współczynnika
             if (d >= 0) {
                 x += xi; y += yi; d += ai;
             } else {
                 d += bi;  y += yi;
             }

             if (!dashed || (x+y)%4 == 0) put_pixel(scr, x, y, color);

         }
     }

     if(SDL_MUSTLOCK(scr)) SDL_UnlockSurface(scr);

}


/*
Funkcja rysuje prostokat przy pomocy wyzej zdefiniowanej funkcji linii
Parametry wejsciowe:
scr - wskaznik do warstwy obrazu
x1,y1,x2,y2 - wspolrzedne wierzcholkow
color - kolor pikseli (HEX)
*/
void draw_rectangle(SDL_Surface *scr, Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2,  Uint32 color)  {

   draw_line(scr, x1, y1, x2, y1, color);
   draw_line(scr, x2, y1, x2, y2, color);
   draw_line(scr, x2, y2, x1, y2, color);
   draw_line(scr, x1, y2, x1, y1, color);

}


/*
Funkcja przygotowuje nowa warstwe obrazu
Parametry wejsciowe:
width, height - dlugosc, szerokosc
display - warstwa na podstawie ktorej ustalany jest format wyswietlania
Zwracana wartosc: wskaznik do utworzonej warstwy
*/
SDL_Surface* create_surface(int width, int height, const SDL_Surface* display) {

  const SDL_PixelFormat& fmt = *(display->format);
  return SDL_CreateRGBSurface(0, width, height, fmt.BitsPerPixel, fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask );

}


/*
Funkcja naklada warstwe source na warstwe destination
Parametry wejsciowe:
source, destination - wskazniki do warstw
x, y - wspolrzedne gornych wierzcholkow miejsca nalozenia
clip - koordynaty obciecia nakladanej warstwy (opcjonalnie)
*/
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL) {

    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );

}



/*
Funkcja do wykreslania tekstu
Parametry wejsciowe:
scr - warstwa obrazu
x,y - wspolrzedne poczatku tekstu
oss - tekst do wykreslenia
color - kolor tekstu
font - wskaznik do wybranej czcionki
*/
void out_text(SDL_Surface *scr, int x, int y, string oss, SDL_Color color, TTF_Font *font) {

    SDL_Surface* msg = TTF_RenderUTF8_Blended(font, oss.c_str(), color);
    apply_surface(x, y, msg, scr);

    SDL_FreeSurface(msg);

}


/*
Funkcja laduje obrazek z pliku do warstwy
Parametry wejsciowe:
fname - nazwa pliku
Zwracana wartosc - wskaznik do warstwy z zaladowanym obrazkiem
*/
SDL_Surface* img_load(const char* fname) {

  // potrzebne w przypadku pakietowania dla Debiana
  // -----------------------------------------------
  #ifdef DEBIAN_PACKAGE
  stringstream oss;
  oss  << "/usr/share/miniosc/" << fname;
  SDL_Surface* temp = IMG_Load(oss.str().c_str());
  // -----------------------------------------------
  #else
  SDL_Surface* temp = IMG_Load(fname);
  #endif

  if (temp == NULL) {
      cerr << "Nie mozna wczytac obrazka: " << SDL_GetError() << endl;
      exit(EXIT_FAILURE);
  }

  return temp;

}


/*
Utworzenie kursora SDL z zadanego formatu XPM (na podstawie dokumentacji libSDL)
Parametr wejsciowy - kursor w formacie XPM (predefiniowane stale w cursors.h)
Zwracana wartosc: kursor SDL
*/
SDL_Cursor *get_custom_cursor(const char* image[]) {

  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for (row=0; row<32; ++row) {
    for (col=0; col<32; ++col) {
      if (col % 8) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}


/*
Funkcja zmienia kursor na domyslny lub jeden z predefiniowanych
Parametry wejsciowe:
cursor_no - nr kursora (predefiniowane stale w cursor.h)
*/
void change_cursor(const int cursor_no) {

  static int current_cursor_no = DEFAULT_CURSOR;

  if (current_cursor_no == cursor_no) return;

  switch (cursor_no) {

    default:  SDL_SetCursor(default_cursor);
               break;

    case HAND_CURSOR:
               SDL_SetCursor(get_custom_cursor(hand_cursor));
               break;

    case CROSS_CURSOR:
               SDL_SetCursor(get_custom_cursor(cross_cursor));
               break;

  }

  current_cursor_no = cursor_no;

}

/*
Funkcja konwertuje kolor z typu Uint32 na SDL_Color
Parametr wejsciowy:
color - kolor Uint32
Zwracana wartosc: kolor w SDL_Color
*/
SDL_Color get_sdl_color(Uint32 color) {

  SDL_Color c;
  c.r = color >> 16;
  c.g = color >> 8;
  c.b = color;
  return c;

}


/*
Funkcja konwertuje kolor z typu SDL_Color na Uint32
Parametr wejsciowy:
color - kolor SDL_Color
Zwracana wartosc: kolor w Uint32
*/
Uint32 get_uint32_color(SDL_Color color) {

  return SDL_MapRGB(screen->format, color.r, color.g, color.b);

}


/*
Funkcja zwraca kod regionu dla algorytmu Cohena-Sutherlanda
Parametry wejsciowe:
left, top, right, bottom - krancowe wspolrzedne obszaru 2D
x, y - sprawdzany punkt (uwzglednia tylko wspolrzedne odpowiednie dla przestrzeni 2-wymiarowej)
*/
unsigned char cs_region(double left, double top, double right, double bottom, int x, int y) {

  unsigned char r = 0;

  if (x < left)   r |= 0x1;
  if (x > right)  r |= 0x2;
  if (y > bottom) r |= 0x4;
  if (y < top)    r |= 0x8;

  return r;

}


/*
Obcinanie odcinkow 2D przy pomocy algorytmu Cohena-Sutherlanda
Parametry wejsciowe:
left, top, right, bottom - krancowe wspolrzedne obszaru 2D
x1, y1, x2, y2 - krance sprawdzanego odcinka
Zwracana wartosc: true jezeli odcinek(rowniez po obcieciu) miesci sie w danym obszarze, false jezeli nie
*/
bool cs_cut(double left, double top, double right, double bottom, int &x1, int &y1, int &x2, int &y2) {

  unsigned char rn;
  double x,y;

  unsigned char r1 = cs_region(left, top, right, bottom, x1, y1);
  unsigned char r2 = cs_region(left, top, right, bottom, x2, y2);

  if ((r1 & r2) != 0)  return false;
  if ((r1 | r2) == 0)  return true;

  do {

     if (r1 != 0) {  rn = r1; x=x1; y=y1; }
     else { rn = r2;  x=x2;  y=y2; }

     if (rn & 0x1) {

       y = y1+(y2-y1)*(left-x1)/(x2-x1);
       x = left;

     }

     else if (rn & 0x2) {

       y = y1+(y2-y1)*(right-x1)/(x2-x1);
       x = right;

     }

     else if (rn & 0x4) {

       x = x1+(x2-x1)*(bottom-y1)/(y2-y1);
       y = bottom;

     }

     else if (rn & 0x8) {

      x = x1+(x2-x1)*(top-y1)/(y2-y1);
      y = top;

     }


     if (rn == r1) {

        x1 = x;  y1 = y;
        r1 = cs_region(left, top, right, bottom, x1, y1);

     }

     else {

        x2 = x;  y2 = y;
        r2 = cs_region(left, top, right, bottom, x2, y2);

     }


   } while ((r1 & r2) == 0 && (r1 | r2) != 0);

   if ((r1 & r2) != 0)  return false;

   return true;

}

