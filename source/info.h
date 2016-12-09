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





#ifndef INFO_INCLUDED
#define INFO_INCLUDED

#include <SDL/SDL.h>
#include <vector>
#include <iomanip>


/*
Funkcja wyswietla ekran informacyjny "o programie"
*/
void info_about();

/*
Funkcja wyswietla ekran informacyjny z komunikatem
Parametry wejsciowe:
x,y - pozycja pierwszej linii tekstu w oknie
string - lista linijek tekstu do wyswietlenia
*/
void info_text(int x, int y, std::vector<std::string> text);


/*
Funkcja wyswietla informacje o bledzie urzadzenia audio
*/
void info_signal_error();


/*
Funkcja wyswietla informacje o dostepnych skrotach klawiszowych
*/
void info_shortcut();


#endif

