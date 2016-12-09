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




#include "headers/sig_buffer.h"



using namespace std;



/*
Konstruktor elementu listy
*/
SigBuffer::SigBuffer(float buffer[][BUFFER_SIZE]) {

    for (int i=0;i<CHANNELS;i++)
      for (int j=0;j<BUFFER_SIZE;j++) this->buffer[i][j] = buffer[i][j];

    next = NULL;

}



/*
Funkcja dodaje element na koniec listy
Parametry wejsciowe:
b - wskaznik referencji do listy
buffer - bufor do dodania
first_del - true, jezeli usunac pierwszy element z listy
*/
void add_sig_buffer(SigBuffer* &b, float buffer[][BUFFER_SIZE], bool first_del) {

  // usuniecie pierwszego elementu z listy
  if (b!=NULL && first_del) {

     SigBuffer *tmp = b;
     b = b->next;
     delete tmp;

  }

  SigBuffer *new_buf = new SigBuffer(buffer);

  // jest pierwszym elementem listy
  if (b==NULL) {  b = new_buf; }
  // w przeciwnym wypadku dopisujemy na koniec listy
  else {

    SigBuffer *tmp = b,  *tmp2 = NULL;

    // bierzemy wskaznik ostatniego elementu - tmp2
    while (tmp!=NULL) {

      tmp2 = tmp;
      tmp = tmp->next;

    }

    tmp2->next = new_buf;

  }


}


/*
Funkcja zwraca wskaznik do odpowiedniej pozycji listy
Parametry wejsciowe:
b - wskaznik do listy
pos - pozycja listy
Zwracana wartosc:
Wskaznik do danej pozycji listy
*/
SigBuffer* get_sig_buffer(SigBuffer *b, int pos) {

   int i = 0;

   while (b!=NULL && i<pos) {

      b = b->next;
      i++;

   }

   return b;

}



/*
Wyczyszczenie listy
Parametry wejsciowe:
b - wskaznik referencji do listy
*/
void clear_sig_buffer(SigBuffer* &b) {

  SigBuffer *tmp;

  while (b!=NULL) {

     tmp = b;
     b = b->next;
     delete tmp;

  }

}




