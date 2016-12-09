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




#ifndef GRAPHICS_INCLUDED
#define GRAPHICS_INCLUDED

#include <sstream>
#include <SDL/SDL.h>
#include <SDL_ttf.h>



// nr dostepnych kursorow myszy
#define DEFAULT_CURSOR 0
#define HAND_CURSOR 1
#define CROSS_CURSOR 2

// wskaznik do domyslnego kursora myszy
extern SDL_Cursor* default_cursor;


/*
Funkcja rysuje linie przy pomocy algorytmu Bresenhama
Parametry wejsciowe:
scr - wskaznik do warstwy obrazu
x1,y1,x2,y2 - wspolrzedne krancow linii
color - kolor pikseli (HEX)
dashed - true jezeli linia pozioma przerywana (opcjonalnie)
*/
void draw_line(SDL_Surface *scr, Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2,  Uint32 color, bool dashed = false);


/*
Funkcja rysuje prostokat przy pomocy wyzej zdefiniowanej funkcji linii
Parametry wejsciowe:
scr - wskaznik do warstwy obrazu
x1,y1,x2,y2 - wspolrzedne wierzcholkow
color - kolor pikseli (HEX)
*/
void draw_rectangle(SDL_Surface *scr, Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2,  Uint32 color);

/*
Funkcja przygotowuje nowa warstwe obrazu
Parametry wejsciowe:
width, height - dlugosc, szerokosc
display - warstwa na podstawie ktorej ustalany jest format wyswietlania
Zwracana wartosc : wskaznik do utworzonej warstwy
*/
SDL_Surface* create_surface(int width, int height, const SDL_Surface* display);

/*
Funkcja naklada warstwe source na warstwe destination
Parametry wejsciowe:
source, destination - wskazniki do warstw
x, y - wspolrzedne gornych wierzcholkow miejsca nalozenia
clip - koordynaty obciecia nakladanej warstwy (opcjonalnie)
*/
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL);

/*
Funkcja do wykreslania tekstu
Parametry wejsciowe:
scr - warstwa obrazu
x,y - wspolrzedne poczatku tekstu
oss - tekst do wykreslenia
color - kolor tekstu
font - wskaznik do wybranej czcionki
*/
void out_text(SDL_Surface *scr, int x, int y, std::string oss, SDL_Color color,  TTF_Font *font);

/*
Funkcja laduje obrazek z pliku do warstwy
Parametry wejsciowe:
fname - nazwa pliku
Zwracana wartosc - wskaznik do warstwy z zaladowanym obrazkiem
*/
SDL_Surface* img_load(const char* fname);

/*
Funkcja zmienia kursor na domyslny lub jeden z predefiniowanych
Parametry wejsciowe:
cursor_no - nr kursora (predefiniowane stale w cursor.h)
*/
void change_cursor(const int cursor_no);

/*
Funkcja konwertuje kolor z typu Uint32 na SDL_Color
Parametr wejsciowy:
color - kolor Uint32
Zwracana wartosc: kolor w SDL_Color
*/
SDL_Color get_sdl_color(Uint32 color);

/*
Funkcja konwertuje kolor z typu SDL_Color na Uint32
Parametr wejsciowy:
color - kolor SDL_Color
Zwracana wartosc: kolor w Uint32
*/
Uint32 get_uint32_color(SDL_Color color);


/*
Obcinanie odcinkow 2D przy pomocy algorytmu Cohena-Sutherlanda
Zwracana wartosc: true jezeli odcinek(rowniez po obcieciu) miesci sie w danym obszarze, false jezeli nie
Parametry wejsciowe:
left, top, right, bottom - krancowe wspolrzedne obszaru 2D
x1, y1, x2, y2 - krance sprawdzanego odcinka
Zwracana wartosc: true jezeli odcinek(rowniez po obcieciu) miesci sie w danym obszarze, false jezeli nie
*/
bool cs_cut(double left, double top, double right, double bottom, int &x1, int &y1, int &x2, int &y2);



#endif



