/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins - A GUI for DOS
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de) 
    Further info see .c-files. */

void w_setpermanentroutine(void (*permrout)(void));
int w_handleuser(int num_endbuts,struct w_button **endbuttons,int num_wins,
 struct w_window **wins,int num_keycodes,struct w_keycode *ec_keys,
 void (**keyhandling)(int ec));
void w_display_msgs(int on);
void w_killcurrentmessage(void);
int w_okcancel(const char *txt,const char *ok,const char *cancel,
 const char *help);
