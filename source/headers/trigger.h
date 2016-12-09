/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 *  Biblioteka trigger - wyzwalanie sygnalu
 *
 *****************************************************************************/



#ifndef TRIGGER_INCLUDED
#define TRIGGER_INCLUDED


struct Trigger {

  // poziom wyzwalania
  float level;
  // tryb zbocza wyzwalania [0-2]
  int slope;
  // kanal wyzwalania [0-1]
  int channel;

  // zlapany sygnal
  bool catched;

  // wspolrzedne ekranowe punktu spelniajacego warunek
  int cx, cy;

  // konstruktor
  Trigger();


  // sprawdzenie warunkow wyzwolenia sygnalu
  void check(float sens, float vert, float prev1, float prev2, float curr);
  // wyzerowanie flag spelnienia warunku
  void clear_cond();

};



#endif

