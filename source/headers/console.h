/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka console - konsola do recznego wprowadzania dokladnych wartosci parametrow
 *
 *****************************************************************************/




#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

#include <SDL/SDL.h>
#include "../libconsole/CON_console.h"



struct Console {

  // uchwyt do konsoli
  ConsoleInformation *console;
  // flaga widocznosci konsoli i wyrysowania konsoli
  bool active;

  // konstruktor
  Console();
  // inicjalizacja konsoli
  void init();
  // przelaczenie widocznosci konsoli
  void toggle();
  // narysowanie konsoli jezeli jest aktywna
  void render();
  // przekazanie obslugi zdarzen SDL do konsoli
  void check_events(SDL_Event &event);
  // sprzatniecie konsoli
  void stop();

};





#endif

