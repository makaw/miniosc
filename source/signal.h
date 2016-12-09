/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 *  Biblioteka signal - obsluga pobierania i generowania sygnalu z wykorzystaniem biblioteki RtAudio
 *
 *****************************************************************************/





#ifndef SIGNAL_INCLUDED
#define SIGNAL_INCLUDED

#include <RtAudio.h>
#include <cstring>
#include <SDL/SDL.h>

#include "config.h"



// globalny bufor sygnalu (2 kanaly)
extern float signal_buffer[CHANNELS][BUFFER_SIZE];

// blokada watku - np. w trakcie pobierania, interpretowania sygnalu
extern SDL_mutex* signal_lock;
// czy bufor jest juz zapelniony
extern bool signal_buffer_ready;

// nazwa urzadzenia wejsciowego
extern std::string device_info;


/*
Rozpoczecie pobierania/wysylania sygnalu, otwarcie strumienia danych
Zwracana wartosc: false jezeli brak urzadzenia audio, true jezeli jest OK
*/
bool signal_init();

/*
Zatrzymanie pobierania/wysylania sygnalu, zamkniecie strumienia danych
*/
void signal_stop();



#endif
