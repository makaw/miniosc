/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka sig_buffer - implementacja listy jednokierunkowej buforow sygnalu
 *
 *****************************************************************************/




#ifndef SIG_BUFFER_INCLUDED
#define SIG_BUFFER_INCLUDED



#include "config.h"





// struktura elementu listy lokalnych buforow sygnalu
struct SigBuffer {

  float buffer[CHANNELS][BUFFER_SIZE];
  struct SigBuffer *next;

  // konstruktor elementu listy
  SigBuffer(float buffer[][BUFFER_SIZE]);


};



/*
Funkcja dodaje element na koniec listy
Parametry wejsciowe:
b - wskaznik referencji do listy
buffer - bufor do dodania
first_del - true, jezeli usunac pierwszy element z listy
*/
void add_sig_buffer(SigBuffer* &b, float buffer[][BUFFER_SIZE], bool first_del);


/*
Funkcja zwraca wskaznik do odpowiedniej pozycji listy
Parametry wejsciowe:
b - wskaznik do listy
pos - pozycja listy
Zwracana wartosc:
Wskaznik do danej pozycji listy
*/
SigBuffer* get_sig_buffer(SigBuffer *b, int pos);



/*
Wyczyszczenie listy
Parametry wejsciowe:
b - wskaznik referencji do listy
*/
void clear_sig_buffer(SigBuffer* &b);



#endif

