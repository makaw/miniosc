/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 *  Biblioteka spectrum - analiza widmowa, transformata Fouriera z wykorzystaniem biblioteki FFTW
 *
 *****************************************************************************/




#ifndef SPECTRUM_INCLUDED
#define SPECTRUM_INCLUDED


// typy okien dostepne w aplikacji
#define HAMMING_WINDOW   1
#define HANN_WINDOW      2
#define BARTLETT_WINDOW   3
#define BLACKMAN_WINDOW  4
#define FLATTOP_WINDOW   5

#define DEFAULT_WINDOW   HAMMING_WINDOW

// domyslny wspolczynnik skalowania modulu zespolonej wartosci FFT
#define FFT_SCALE 8

// usredniony blad wynikajacy z rozdzielczosci FFT
extern const float freq_error;

// nazwy okien (string)
extern const char* window_types[];

/*
Funkcja liczy krotkoczasowa transformate Fouriera przy pomocy biblioteki FFTW
Parametry wejsciowe:
in - sygnal do przeliczenia, wskaznik do tablicy
s_size - dlugosc sygnalu
w_size - rozmiar okna czasowego
out - wyjscie, wskaznik do tablicy wypelnianej modulem z zespolonej wartosci FFT
scale - dodatkowe przeskalowanie czesci rzeczywistej i urojonej (przed modulem)
Zwracana wartosc:  czestotliwosc o najwyzszej amplitudzie
*/
float fourier_transform(void *in, int s_size, int w_size, float *out, int scale=FFT_SCALE);


#endif
