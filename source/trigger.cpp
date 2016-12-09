/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Biblioteka trigger - wyzwalanie sygnalu
 *
 *****************************************************************************/




#include "headers/config.h"
#include "headers/trigger.h"


// konstruktor
Trigger::Trigger() {

  level = TRIGGER_LEVEL_DEF;
  slope = TRIGGER_SLOPE_DEF;
  channel = TRIGGER_CHANNEL_DEF;

  catched = false;
  cx = 0;
  cy = 0;

}


/*
Funkcja sprawdza warunek wyzwalania sygnalu
Parametry wejsciowe:
sens - czulosc
vert - przesuniecie pionowe
prev1, prev2, curr - 2 poprzednie i biezaca probka sygnalu
*/
void Trigger::check(float sens, float vert, float prev1, float prev2, float curr) {

  this->catched = false;

  if (this->slope == TRIGGER_OFF)  return;

  // zmieniam znaki, bo bede porownywac jako pionowe wspolrzedne ekranowe,
  // a im wieksza wspolrzedna, tym mniejsza wartosc na wykresie
  prev1 = -1.0 * prev1 * sens * _AMP_GAIN + vert;
  prev2 = -1.0 * prev2 * sens * _AMP_GAIN + vert;
  curr = -1.0 * curr * sens * _AMP_GAIN + vert;

  this->catched =  ((this->slope == TRIGGER_RISE && prev1 < prev2 && prev2 < curr && prev2 <= this->level && curr > this->level)
                    || (this->slope == TRIGGER_FALL && prev1 > prev2 && prev2 > curr && prev2 >= this->level && curr < this->level));


}


/*
Wyzerowanie flag spelnienia warunku
*/
void Trigger::clear_cond() {

  this->catched = false;
  this->cx = 0;
  this->cy = 0;

}



