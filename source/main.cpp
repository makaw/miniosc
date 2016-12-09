/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Program glowny
 *
 *****************************************************************************/



#include <SDL/SDL.h>
#include <SDL_image.h>

#include "graphics.h"
#include "display.h"
#include "gui.h"
#include "sig_buffer.h"
#include "config.h"
#include "signal.h"
#include "spectrum.h"
#include "info.h"


// uniwersalne Sleep()
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif


using namespace std;


// sprawdzenie stanu myszy i klawiatury
void interact();
// wizualizacja sygnalu zgodnie z wybranym modulem i parametrami
void draw_graph();
// posprzatanie na wyjsciu
void clean_up();


// globalna lista zapamietanych buforow sygnalu
SigBuffer *sig_buffer = NULL;

// czy udala sie inicjalizacja sygnalu audio
bool signal_ok;


int main(int argc, char **argv) {

  // inicjalizacja sygnalu audio
  signal_ok = signal_init();


  if(!SDL_Init(SDL_INIT_EVERYTHING)) {

    // cale sprzatanie na wyjsciu w clean_up()
    atexit(clean_up);

    // inicjalizacja graficznego UI
    gui_init();

    // jezeli blad z sygnalem, to wyswietlenie informacji i wyjscie
    if (!signal_ok) {

      info_signal_error();
      return 0;

    }


    // glowna petla programu
    while (!uistate.quit) {

       // sprawdzenie stanu myszy i klawiszy
       interact();

       // oczekiwanie na pobranie sygnalu, blokada watku
       while(!signal_buffer_ready) Sleep(0.2);
       SDL_mutexP(signal_lock);


       // wizualizacja sygnalu
       draw_graph();

       // ewentualne wyrysowanie elementow GUI
       render_knobs();
       render_cursor_coords();
       copy_console_area();
       render_tooltips();
       uistate.console.render();

       SDL_Flip(screen);

       // zezwolenie na dalsze pobieranie sygnalu, odblokowanie watku
       signal_buffer_ready = false;
       SDL_mutexV(signal_lock);


    }

  }

  else {

    cerr << SDL_GetError() << endl;
    signal_stop();

  }

  return 0;


}



/*
Funkcja sprawdza stan myszy i klawiatury i podejmuje odp.akcje
*/
void interact() {

   SDL_Event event;

   if (SDL_PollEvent(&event)) {

      uistate.quit = (event.type == SDL_QUIT);
      int mod = SDL_GetModState();

      // przekazanie obslugi zdarzen do watku konsoli
      uistate.console.check_events(event);

      switch (event.type) {

         default: break;

         // pobranie pozycji myszy
         case SDL_MOUSEMOTION:
            uistate.mousex = event.motion.x;
            uistate.mousey = event.motion.y;
            break;

         // pobranie stanu lewego przycisku myszy
         case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == 1) uistate.mousedown = 1;
            break;

         case SDL_MOUSEBUTTONUP:
            if (event.button.button == 1) uistate.mousedown = 0;
            break;


         // sprawdzenie wcisnietych klawiszy
         case SDL_KEYDOWN:


            switch (event.key.keysym.sym) {

               default:  break;

               // info o skrotach
               case SDLK_F1:

                 info_shortcut();
                 break;

               // przelaczenie konsoli
               case SDLK_F2:

                 uistate.console.toggle();
                 break;

               // zrzut ekranu
               case SDLK_F3:

                 display_screenshot();
                 break;

               // start/stop
               case SDLK_F5:

                 uistate.toggle_running();
                 break;

               // wyjscie z programu
               case SDLK_F10:

                  uistate.quit = true;
                  break;


               // przelaczenie mnoznika podstawy czasowej
               case SDLK_p:

                 if ((mod & KMOD_CTRL) != 0) uistate.toggle_timebase_mul();
                 break;


               // przelaczenie kanalow
               case SDLK_1:

                 if ((mod & KMOD_CTRL) != 0) uistate.toggle_channel(0);
                 break;

               case SDLK_2:

                 if ((mod & KMOD_CTRL) != 0) uistate.toggle_channel(1);
                 break;


               // zmiana modulu - CTRL + skroty klawiszowe
               case SDLK_o:

                 if ((mod & KMOD_CTRL) != 0) {
                   uistate.change_module(SCOPE);
                   uistate.change_made = true;
                 }
                 break;

               case SDLK_x:

                 if ((mod & KMOD_CTRL) != 0) {
                   uistate.change_module(GRAPH_XY);
                   uistate.change_made = true;
                 }
                 break;

               case SDLK_w:

                 if ((mod & KMOD_CTRL) != 0) {
                   uistate.change_module(SPECTRUM);
                   uistate.change_made = true;
                 }
                 break;



               // tylko do testow - sygnal testowy CTRL+T
               case SDLK_t:

                 //if ((mod & KMOD_CTRL) != 0) _TEST_SIGNAL = !_TEST_SIGNAL;
                 break;

            }

            uistate.mousedown = 0;

            break;

         }

    }

    change_cursor(uistate.check_cursor());

    // przycisk myszy wcisniety
    if (uistate.mousedown==1) {

       // wszystkie checkboksy
       uistate.check_checkboxes();

       // wszystkie pokretla
       uistate.check_knobs();

       // lista skrotow klawiszowych
       if (uistate.check_shortcut_button()) info_shortcut();
       // screenshot
       if (uistate.check_screenshot_button()) display_screenshot();
       // konsola
       if (uistate.check_console_button()) uistate.console.toggle();
       // o programie
       if (uistate.check_info_button()) info_about();

       // start/stop
       uistate.check_running();
       //  zmiana modulu po kliknieciu mysza w przycisk
       uistate.check_modules_menu();
       // menu wyboru okna czasowego
       uistate.check_spect_window_menu();
       // suwaki osi X i Y
       uistate.check_sliders();

       // jezeli nie ma przeciagania, to wyzerowanie stanu przycisku myszy
       if (!uistate.knob_mouse_drag && !uistate.slider_mouse_drag) uistate.mousedown=0;

    }


}


/*
Dodanie biezacego bufora sygnalu na koniec listy
Parametry wejsciowe:
buffer_pos - referencja do biezacej pozycji na liscie
max_buffer_pos - aktualnie najwieksza mozliwa pozycja na liscie
trigger_on - true jezeli maja byc sprawdzane warunki wyzwalania sygnalu
Zwracana wartosc:
-1 lub indeks elementu w buforze ktory spelnil warunek wyzwalania
*/
int get_current_signal_buffer(int &buffer_pos, int max_buffer_pos, bool trigger_on) {

  float buffer_tmp[CHANNELS][BUFFER_SIZE];
  int index = -1;

  for (int i=0; i<CHANNELS; i++)
    for (int j=0; j<BUFFER_SIZE; j++) {

     // skopiowanie zawartosci bufora wejsciowego
	 buffer_tmp[i][j] = static_cast<float>(signal_buffer[i][j]);

	 // sprawdzanie warunku wyzwalania
	 if (trigger_on && i==uistate.trigger.channel && j>1 && index==-1
      && uistate.check_trigger(buffer_tmp[i][j-2], buffer_tmp[i][j-1], buffer_tmp[i][j])) index = j-1;

  }

  // dodanie biezacego buforu na koniec listy
  add_sig_buffer(sig_buffer, buffer_tmp, (buffer_pos>=max_buffer_pos));
  if (buffer_pos < max_buffer_pos)  buffer_pos++;

  return index;

}



/*
Wizualizacja sygnalu zgodnie z wybranym modulem i parametrami
*/
void draw_graph() {

  // pozycja biezacego bufora sygnalu na liscie
  static int buffer_pos = 0;

  // poprzednia podstawa czasowa, poprzedni stan start/stop, poprzedni poziom wyzwalania
  static float timebase_tmp = uistate.timebase;
  static bool running_tmp = uistate.running;
  static Trigger trigger_tmp = uistate.trigger;

  // maksymalna potrzebna ilosc buforow na ekranie
  float max_buffer_pos_f = SAMP_RATE * uistate.timebase / static_cast<float>(BUFFER_SIZE);
  int max_buffer_pos = ceil(max_buffer_pos_f);
  // indeks elementu w buforze, ktory spelnil warunek wyzwalania
  int trigger_index = -1;

  // czy sprawdzac warunek wyzwalania
  bool trigger_on = (uistate.running && uistate.trigger.slope!=TRIGGER_OFF && !uistate.trigger.catched);

  // ew. odblokowanie triggera
  if (uistate.running || uistate.knobs_changed || (uistate.trigger.catched &&
     (uistate.trigger.level != trigger_tmp.level || uistate.trigger.channel != trigger_tmp.channel
      || uistate.trigger.slope != trigger_tmp.slope)))   uistate.trigger.clear_cond();


  // dodanie biezacego bufora sygnalu na koniec listy
  if (uistate.running) trigger_index = get_current_signal_buffer(buffer_pos, max_buffer_pos, trigger_on);


  // wyczyszczenie listy w razie zmiany podstawy czasowej lub wznowienia rysowania sygnalu
  if (((timebase_tmp != uistate.timebase || (uistate.module==GRAPH_XY && uistate.tb_xy_mul)) && uistate.running)
       || (!running_tmp && uistate.running)) {
     clear_sig_buffer(sig_buffer);
     buffer_pos = 0;
  }


   // odswiezenie obszaru wyswietlacza
  switch (uistate.module) {

    default:
    case SCOPE:

       // odswiezenie oscylogramu, jezeli konieczne
       if (uistate.running || uistate.change_made) {

          // czy jest konieczne wyczyszczenie ekranu wyswietlacza
          bool ds_clean = (uistate.change_made || buffer_pos==1 || buffer_pos>=max_buffer_pos);
          if (ds_clean) display_clean();

          // wspolczynnik przeskalowania dla biezacej podstawy czasowej
          float tmp_x_scale = base_x_scale / max_buffer_pos_f;

          // przesuniecie w prawo w poziomie dla triggera przy malej podstawie
          static int x_offset = 0;

          // ewentualne wyznaczenie wsp.punktu spelniajacego warunek wyzwalania
          if (uistate.trigger.catched && trigger_index>-1 && uistate.trigger.cx == 0 && uistate.trigger.cy == 0) {

            if (max_buffer_pos_f<1.0) { x_offset = trigger_index-1; }
            uistate.trigger.cx = (trigger_index-x_offset) * tmp_x_scale + display_x1
                                + ((buffer_pos>=0) ? (buffer_pos-1) * BUFFER_SIZE * tmp_x_scale : 0) + 5;
            uistate.trigger.cy = grid_axis_y - (uistate.trigger.level * GRID_DIV_H) - 7;
            if (uistate.trigger.cx>=display_x2-5) uistate.trigger.cx = display_x2-5;

          }

          if (!uistate.trigger.catched)  x_offset = 0;

          // wyrysowanie buforow
          for (int i=((!ds_clean && uistate.running) ? buffer_pos-1 : 0);i<buffer_pos;i++) {

             // co 10 buforow ponownie interakcja - zeby GUI nie przycinalo przy dluzszej podstawie czasu
             if ((i+1)%10==0)  interact();
             // na wypadek zmiany modulu w trakcie interakcji
             if (uistate.module != SCOPE) break;

             SigBuffer* sig_buf_tmp = get_sig_buffer(sig_buffer, i);
             if (sig_buf_tmp != NULL)
               display_draw_wave(sig_buf_tmp->buffer, BUFFER_SIZE, i, tmp_x_scale, x_offset, (i==0));

          }

          uistate.change_made = false;

       }


       break;



    case GRAPH_XY:

       // odswiezenie wykresu XY, jezeli konieczne
       if (uistate.running || uistate.change_made) {

          // czy jest konieczne wyczyszczenie ekranu wyswietlacza
          bool ds_clean = (uistate.change_made || uistate.tb_xy_mul || buffer_pos==1 || buffer_pos>=max_buffer_pos);
          if (ds_clean) display_clean();
          // wyzerowanie flagi mnoznika podstawy czasowej w XY
          if (uistate.tb_xy_mul) uistate.tb_xy_mul = false;

          // wyrysowanie buforow
          for (int i=(!ds_clean && uistate.running ? buffer_pos-1 : 0);i<buffer_pos;i++) {

             // co 10 buforow ponownie interakcja - zeby GUI nie przycinalo przy dluzszej podstawie czasu
             if ((i+1)%10==0)  interact();
             // na wypadek zmiany modulu
             if (uistate.module != GRAPH_XY) break;

             int bsize = (max_buffer_pos_f >= 1.0) ? BUFFER_SIZE : ceil(max_buffer_pos_f * static_cast<float>(BUFFER_SIZE));

             SigBuffer* sig_buf_tmp = get_sig_buffer(sig_buffer, i);
             if (sig_buf_tmp != NULL)
               display_draw_wave_xy(sig_buf_tmp->buffer, bsize<2 ? 2 : bsize, (i==0));

          }

          uistate.change_made = false;


       }

       render_xy_sliders();

       break;



    case SPECTRUM:

       // odswiezenie spektrogramu, jezeli konieczne
       if (uistate.running || uistate.change_made) {

          // bufor FFT o polowe krotszy, bo druga polowa jest wypelniona zerami
          float fft_buffer[CHANNELS][BUFFER_SIZE], buffer_tmp[CHANNELS][BUFFER_SIZE], max_freq[CHANNELS];

          // wyswietlany jest tylko biezacy bufor
          SigBuffer* sig_buf_tmp = get_sig_buffer(sig_buffer, buffer_pos-1);
          if (sig_buf_tmp != NULL)
          for (int i=0; i<CHANNELS; i++)
            for (int j=0; j<BUFFER_SIZE; j++)  buffer_tmp[i][j] = sig_buf_tmp->buffer[i][j];

          // wyliczenie modulow transformaty Fouriera na lokalnych buforach
          for (int i=0; i<CHANNELS; i++)
             max_freq[i] = fourier_transform(buffer_tmp[i],  BUFFER_SIZE, FFT_SIZE, fft_buffer[i]);

          display_clean();
          display_draw_wave(fft_buffer, BUFFER_SIZE, max_freq);
          uistate.change_made = false;

       }

       render_spect_window_menu();

       break;



  }


  // zatrzymanie rysowania sygnalu po spelnieniu warunku wyzwalania // if buffer_pos>=max_buffer_pos && t...
  if (buffer_pos>0 && trigger_on && uistate.trigger.catched) {

     uistate.running = false;
     uistate.change_made = true;
     uistate.trigger.catched = true;
     render_running_button(false);

  }

  timebase_tmp = uistate.timebase;
  running_tmp = uistate.running;
  trigger_tmp = uistate.trigger;


}




/*
Posprzatanie na wyjsciu
*/
void clean_up() {

  if (signal_ok) signal_stop();
  gui_stop();

  clear_sig_buffer(sig_buffer);

}


