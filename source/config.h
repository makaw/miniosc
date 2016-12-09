/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Plik naglowkowy config - stale konfiguracyjne
 *
 *****************************************************************************/



#ifndef CONFIG_INCLUDED
#define CONFIG_INCLUDED

#include <SDL/SDL.h>

// to ponizej jezeli pakietujemy dla Debiana
//#define DEBIAN_PACKAGE

// nazwa programu (naglowek)
#define TITLE                   "MiniOSC"
// wersja
#define VERSION                 "1.0"

// wielkosc glownego bufora sygnalu
#define BUFFER_SIZE             512
// wielkosc bufora transformaty Fouriera
#define FFT_SIZE                (BUFFER_SIZE * 2)
// czestotliwosc probkowania (Hz)
#define SAMP_RATE               44100
// maksymalna czestotliwosc
#define FREQ_MAX                (SAMP_RATE / 2)
// ilosc kanalow (1-2)
#define CHANNELS                2
// staly sygnal testowy (0/1), zamiast pobierania z wejscia audio
#define TEST_SIGNAL             0
//  szerokosc i wysokosc podzialki(siatki) wyswietlacza (px)
#define GRID_DIV_W              45
#define GRID_DIV_H              43
// minimalna, maksymalna, srodkowa, domyslna czulosc
#define SENSIVITY_MIN           0.009
#define SENSIVITY_MAX           10.0
#define SENSIVITY_MID           1.0
#define SENSIVITY_DEF           SENSIVITY_MID
// minimalne, maksymalne, srodkowe, domyslne przesuniecie pionowe
#define VERT_OFFSET_MIN         -7.0
#define VERT_OFFSET_MAX         7.0
#define VERT_OFFSET_MID         0.0
#define VERT_OFFSET_DEF         VERT_OFFSET_MID

// minimalna podstawa czasowa
#define TIMEBASE_MIN            0.00001
// maksymalna, srodkowa, domyslna podstawa czasowa
// (w sekundach, przed wymnozeniem przez TIMEBASE_MUL)
#define TIMEBASE_MAX            0.013
#define TIMEBASE_MID            0.0005
#define TIMEBASE_DEF            0.01
// dodatkowy mnoznik podstawy czasowej
// powinno byc x100, ale zmniejszone tak zeby nie tracic zakresu
#define TIMEBASE_MUL            85.0

// minimalny, maksymalny, srodkowy, domyslny poziom wyzwalania
#define TRIGGER_LEVEL_MIN       -5.0
#define TRIGGER_LEVEL_MAX       5.0
#define TRIGGER_LEVEL_MID       0.0
#define TRIGGER_LEVEL_DEF       -4.0


// tryby wyzwalania
#define TRIGGER_OFF             0
#define TRIGGER_FALL            1
#define TRIGGER_RISE            2

// domyslny tryb i kanal[0-1] wyzwalania
#define TRIGGER_SLOPE_DEF       TRIGGER_OFF
#define TRIGGER_CHANNEL_DEF     0


// minimalne, maksymalne, domyslne wyskalowanie osi X i Y dla wykresu XY
#define XY_SCALE_MIN            0.25
#define XY_SCALE_MAX            6.0
#define XY_SCALE_DEF            1.0

// dodatkowe podbicie amplitudy
#define AMP_GAIN                2.0
#define AMP_GAIN_MIN            0.2
#define AMP_GAIN_MAX            4.0


// ilosc modulow
#define MODULES                 3




const double pi = 3.14159265;


// kolory dla poszczegolnych kanalow
const Uint32 channel_colors[CHANNELS] = { 0x49703A, 0xED5507 };
// kolor wykresu XY
const Uint32 xy_color = 0xCD5501;
// kolor linii siatki
const Uint32 grid_color = 0x181818;
// kolor srodkowych linii siatki
const Uint32 grid_center_color = 0x404040;


// uzmiennienie stalych: wzmocnienie amplitudy i sygnal testowy
extern float _AMP_GAIN;
extern bool _TEST_SIGNAL;




#endif
