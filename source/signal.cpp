/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka signal - obsluga pobierania i generowania sygnalu z wykorzystaniem biblioteki RtAudio
 *
 *****************************************************************************/



#include <cstring>
#include <cstdlib>
#include <cmath>
#include <RtAudio.h>
#include <SDL/SDL.h>

#include "config.h"
#include "gui.h"



using namespace std;


// blokada watku - w trakcie pobierania, interpretowania sygnalu
SDL_mutex* signal_lock;
// czy bufor jest juz zapelniony
bool signal_buffer_ready = false;

// globalny uchwyt do sygnalu
RtAudio signal_handler;

// globalny bufor sygnalu (2 kanaly)
float signal_buffer[CHANNELS][BUFFER_SIZE];

// nazwa urzadzenia wejsciowego
string device_info;


// Funkcja pobiera nazwe urzadzenia audio
string get_device_info();



/*
Funkcja callback - wywolywana okresowo zgodnie z czestotliwoscia probkowania
Parametry wejsciowe zgodnie z dokumentacja RtAudio
out - wskaznik na bufor wyjsciowy karty dzwiekowej
in - wskaznik na bufor wejsciowy karty (pobieranie sygnalu)
*/
int signal_callback( void *out, void *in, unsigned int numFrames,
		   double streamTime, RtAudioStreamStatus status, void * userData )  {


  // blokada glownego watku (rysowania)
  SDL_mutexP(signal_lock);


  // staly sygnal testowy - tylko do testow: 2 razy sinus
  if (_TEST_SIGNAL)
    for (unsigned int i=0;i<numFrames;i++) {

       signal_buffer[0][i] = 0.01*(8*sin(2*pi*1300*i/44100));

       signal_buffer[1][i] = 0.01*(8*sin(2*pi*4300*i/44100));

  }


  // sygnal z wejscia audio
  else if (uistate.running) {

    double *input = (double *)in;

    // obsluga wielu kanalow
    double stereo_buf[BUFFER_SIZE*2];
    memcpy(stereo_buf, input, numFrames * 2 * sizeof(double));


    for(unsigned int i=0; i<numFrames; i++) {
      for (int ch=0; ch<CHANNELS; ch++) {

	     signal_buffer[ch][i] = stereo_buf[i*CHANNELS+ch];

       }
	}

  }


  // odblokowanie glownego watku
  signal_buffer_ready = true;
  SDL_mutexV(signal_lock);

  return 0;

}



/*
Zatrzymanie pobierania/wysylania sygnalu, zamkniecie strumienia danych
*/
void signal_stop() {

  try { signal_handler.stopStream(); }
  catch(RtAudioError &e) {     // w starych wersjach RtError
     cerr << e.getMessage();
  }
  if (signal_handler.isStreamOpen()) signal_handler.closeStream();

  SDL_DestroyMutex(signal_lock);


}


/*
Rozpoczecie pobierania/wysylania sygnalu, otwarcie strumienia danych
Zwracana wartosc: false jezeli sie nie udalo, true jezeli jest OK
*/
bool signal_start() {

  try {  signal_handler.startStream();  }
  catch(RtAudioError &e) {     // w starych wersjach RtError
    cerr << e.getMessage();
    return false;
  }

  return true;

}



/*
Inicjalizacja karty dzwiekowej, ustawienie parametrow
Zwracana wartosc: false jezeli brak urzadzenia audio, true jezeli jest OK
*/
bool signal_init() {

  if (signal_handler.getDeviceCount()<1) {
     cerr << "Nie znaleziono urzadzenia audio!" << endl;
	 return false;
  }

  device_info = get_device_info();

  unsigned int bufferFrames = BUFFER_SIZE;
  RtAudio::StreamParameters iParams, oParams;
  iParams.deviceId = signal_handler.getDefaultInputDevice();
  iParams.nChannels = (int)CHANNELS;
  oParams.deviceId =  signal_handler.getDefaultOutputDevice();
  oParams.nChannels = (int)CHANNELS;


  try {

     signal_handler.openStream(&oParams, &iParams, RTAUDIO_FLOAT64, SAMP_RATE, &bufferFrames, &signal_callback, NULL);

  } catch(RtAudioError &e) {     // w starych wersjach RtError
    cerr << "Brak dostepu do urzadzenia audio!" << endl;
    cerr << e.getMessage();
	return false;
  }

  signal_lock = SDL_CreateMutex();

  signal_start();

  return true;


}


/*
Funkcja pobiera nazwe urzadzenia audio
Zwracana wartosc:
Nazwa urzadzenia audio
*/
string get_device_info() {

  RtAudio::DeviceInfo info = signal_handler.getDeviceInfo(signal_handler.getDefaultInputDevice());
  if (info.probed && info.inputChannels>=CHANNELS) {

        stringstream oss;
        oss << info.name << " (max kana\04\06w: " << info.inputChannels << ")";
        return oss.str();

  }

  return "nieznane";

}
