/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka spectrum - analiza widmowa, transformata Fouriera z wykorzystaniem biblioteki FFTW
 *
 *****************************************************************************/




#include <cstring>
#include <cmath>
#include <fftw3.h>

#include "headers/spectrum.h"
#include "headers/signal.h"
#include "headers/gui.h"



using namespace std;



// usredniony blad wynikajacy z rozdzielczosci FFT (dla globalnego bufora)
const float freq_error = ((FREQ_MAX/BUFFER_SIZE)-1.0)/2.0;

// nazwy okien (string)
const char* window_types[] = {"Hamming", "Hann", "Bartlett", "Blackman", "FlatTop"};


/*
Przygotowanie okna czasowego
Parametry wejsciowe:
type   - typ okna
w_size - ilosc elementow, rozmiar okna
buffer - wskaznik do tablicy ktora bedzie wypelniona wspolczynnikami okna
*/
void prepare_window(int type, int w_size, float *buffer) {


  for (int i=0;i<w_size;i++) {

    switch (type) {

      default:
      case HAMMING_WINDOW:
         buffer[i] = 0.53836-0.46164*cos(2*pi*i/(w_size-1));
         break;

      case HANN_WINDOW:
         buffer[i] = 0.5*(1 - cos(2*pi*i/(w_size-1)));
         break;

      case BARTLETT_WINDOW:
         buffer[i] = 1-(2*fabs(i-(w_size-1)/2))/(w_size -1);
         break;

      case BLACKMAN_WINDOW:
         buffer[i] = 0.42 - 0.5 * cos(2*pi*i / (w_size -1)) + 0.08 * cos(4.0 *pi * i / (w_size -1));
         break;

      case FLATTOP_WINDOW:
         buffer[i] = 1.0 - 1.93 * cos(2*pi*i / (w_size -1)) + 1.29 * cos(4.0 *pi*i / (w_size -1)) -
                     0.388 * cos(6.0 *pi*i / (w_size -1)) + 0.0322 * cos(8.0 *pi*i / (w_size -1));
         break;

    }


  }


}



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
float fourier_transform(void *in, int s_size, int w_size, float *out, int scale) {

   float buffer_temp[FFT_SIZE];

   float *signal = (float *)in;
   memcpy(buffer_temp, signal, s_size * sizeof(float));

   fftw_complex *data, *fft_result;
   fftw_plan plan_forward;
   int i;
   float max = 0.0;
   int max_i = 0;

   data          = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * w_size);
   fft_result    = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * w_size);
   plan_forward  = fftw_plan_dft_1d(w_size, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);

   float window[w_size];
   prepare_window(uistate.spect_window, w_size, window);

   bool stop = false;
   int position = 0;


   // przetwarzanie po wycinku sygnalu
   while(position < s_size && !stop) {

     for(i = 0; i < w_size; i++) {

        if(position + i < s_size) {

          //  zastosowanie okna
          data[i][0] = buffer_temp[position + i] * window[i];
          data[i][1] = 0.0;

        } else {

          // koniec sygnalu
          data[i][0] = 0.0;
          data[i][1] = 0.0;
          stop = true;

       }

    }

    fftw_execute(plan_forward);
    position++;

  }


  // bierzemy pierwsza polowe okna, bo w drugiej polowie (od czestotliwosci Nyquista) wynik FFT bedzie symetryczny
  for (i = 0; i < w_size/2 + 1; i++) {

     // na wyjscie modul z zespolonej wartosci FFT, czesci rzeczywista i urojona przeskalowane
     out[i] = sqrt(pow(fft_result[i][0]/scale, 2) + pow(fft_result[i][1]/scale, 2));
     if (max<out[i]) {  max=out[i];  max_i=i;  }

  }

  // na w.wypadek - druga polowa wypelniana zerami
  for (i = w_size/2 + 1; i < w_size; i++) out[i] = 0.0;

  fftw_destroy_plan(plan_forward);
  fftw_free(data);
  fftw_free(fft_result);

  // usredniony blad wynikajacy z rozdzielczosci FFT
  float freq_error = ((FREQ_MAX/s_size)-1.0)/2.0;

  return ((float)max_i)*(FREQ_MAX/s_size)+freq_error;

}



