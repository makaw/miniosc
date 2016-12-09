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





#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED

#include <SDL/SDL.h>

#include "config.h"
#include "gui_coords.h"





// wskaznik do zachowanego tla wyswietlacza
extern SDL_Surface* screen_display_bg;

/*
Funkcja wizualizuje na wyswietlaczu sygnal (dla spektrogramu)
Parametry wejsciowe:
data - wskaznik do bufora danych (tablicy)
b_size - ilosc elementow w buforze
max_freq - maksymalne czestotliwosci
*/
void display_draw_wave(float data[][BUFFER_SIZE], int b_size, float *max_freq);


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
void display_draw_wave(float data[][BUFFER_SIZE], int b_size, int b_pos, float x_scale, int x_offset = 0, bool labels = true);


/*
Funkcja wizualizuje na wyswietlaczu sygnal w ukladzie X-Y (odpowiednio kanal 1-2)
Parametry wejsciowe:
data - wskaznik do bufora danych (tablicy)
b_size - ilosc elementow w buforze
labels - true jezeli maja byc rysowane etykiety
*/
void display_draw_wave_xy(float data[][BUFFER_SIZE], int b_size, bool labels);

/*
Wyczyszczenie obszaru wyswietlacza
*/
void display_clean();


/*
Funkcja wykonuje screenshot obszaru wyswietlacza
*/
void display_screenshot();



#endif

