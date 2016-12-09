/*****************************************************************************
 *
 * MiniOSC
 *
 * Autor: Maciej Kawecki 2015-2016
 *
 *****************************************************************************
 *
 * Plik naglowkowy cursors - definicje XPM niestandardowych kursorów dla GUI
 *
 *****************************************************************************/




#ifndef CURSORS_INCLUDED
#define CURSORS_INCLUDED


#define DEFAULT_CURSOR  0
#define HAND_CURSOR     1
#define CROSS_CURSOR    2



/* Kursor "hand" w XPM, na podstawie pliku z FQTerm (Ubuntu) */
const char *hand_cursor[]={
"32 32 3 1",                            // dlugosc szerokosc ilosc_kolorow znaki_na_px
"X c #000000",                          // dostepne kolory
". c #ffffff",
"  c None",
"     XX                         ",     // piksele
"    X..X                        ",
"    X..X                        ",
"    X..X                        ",
"    X..X                        ",
"    X..XXX                      ",
"    X..X..XXX                   ",
"    X..X..X..XX                 ",
"    X..X..X..X.X                ",
"XXX X..X..X..X..X               ",
"X..XX........X..X               ",
"X...X...........X               ",
" X..X...........X               ",
"  X.X...........X               ",
"  X.............X               ",
"   X............X               ",
"   X...........X                ",
"    X..........X                ",
"    X..........X                ",
"     X........X                 ",
"     X........X                 ",
"     XXXXXXXXXX                 ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"0,0"};


/* Kursor "krzyzyk" XPM */
const char *cross_cursor[] = {
"32 32 3 1",                           // dlugosc szerokosc ilosc_kolorow znaki_na_px
"X c #000000",                         // dostepne kolory
". c #ffffff",
"  c None",
"      XXX                       ",    // piksele
"      X.X                       ",
"      X.X                       ",
"      X.X                       ",
"      X.X                       ",
"      X.X                       ",
"XXXXXXX.XXXXXXX                 ",
"X.............X                 ",
"XXXXXXX.XXXXXXX                 ",
"      X.X                       ",
"      X.X                       ",
"      X.X                       ",
"      X.X                       ",
"      X.X                       ",
"      XXX                       ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"0,0"};



#endif
