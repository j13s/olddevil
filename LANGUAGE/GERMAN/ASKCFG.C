const char *grfxdescription[NUM_GRFXDRVS]=
 { "Der Modus sollte Åberall laufen\n",
   "Der hier ist nut fÅr S3 ChipsÑtze und hat kleine Fehler.\n"\
   "               Aber er ist schneller als die anderen.",
   "Tseng ET4000 Chipsatz; nur bei Problemen mit dem VESA-Treiber\n"\
   "               zu empfehlen.",
   "Cirrus Logic Chipsatz; s.o.",
   "ATI Mach 64; s.o." };
#define NUM_RESOLUTIONS 4
int res_xysize[NUM_RESOLUTIONS][2]=
 { { 640,480 }, { 800,600 }, { 1024,768 }, { 1280,1024 } };

int checkpath(char *path,const char *testfile,const char *text)
 {
 char testpath[300];
 FILE *f=NULL;
 makedospath(path,testpath); strcat(testpath,"/"); strcat(testpath,testfile);
 f=fopen(testpath,"rb"); 
 if(f) { fclose(f); return 1; }
 else
  { printf("Kann Datei %s in Verzeichnis \"%s\" nicht finden.\n",text,path);
    printf("Devil braucht fÅr Descent 2 modus die\n");
    printf("'Medium'-Installation von Descent 2 (48MB).\n");
    printf("Wenn Sie kein Descent 2 haben, mÅssen Sie zum\n");
    printf("Descent 1 Modus umschalten (MenÅpunkt 'D')\n"); return 0; }
 }
 
void askpath(char *oldpath,const char *text,const char *testfile)
 {
 char testpath[300],path[300];
 int ok=1;
 makedospath(oldpath,path);
 do
  { 
  printf("%s Verzeichnis (bisher: \"%s\", abbrechen mit 'q'): ",text,oldpath);
  scanf("%199s",testpath);
  if(strlen(testpath)==1 && tolower(testpath[0])=='q') return;
  if(strlen(testpath)==0) strcpy(testpath,path);
  makedospath(testpath,path);
  if(testfile) ok=checkpath(path,testfile,text);
  }
 while(!ok);
 strcpy(oldpath,path);
 }
 
void askgrfxdriver(int *old_grfxdriver)
 {
 int grfxdriver,i;
 printf("Bitte einen Grafiktreiber auswÑhlen:\n"); 
 do
  {
  printf("0. Abbruch\n");
  for(i=0;i<NUM_GRFXDRVS;i++)
   printf("%1d. %6s  --  %s\n",i+1,grfxdrvs[i],grfxdescription[i]);
  printf("\n0..%d? ",NUM_GRFXDRVS); scanf(" \n%d",&grfxdriver);
  if(grfxdriver<=0) return;
  }
 while(--grfxdriver>=NUM_GRFXDRVS);
 *old_grfxdriver=grfxdriver;
 }
 
void askresolution(int *old_resolution)
 {
 int resolution,i;
 printf("Bitte eine Auflîsung auswÑhlen:\n");
 do
  {
  printf("0. Abbruch\n");
  for(i=0;i<NUM_RESOLUTIONS;i++)
   printf("%d. %d x %d\n",i+1,res_xysize[i][0],res_xysize[i][1]);
  printf("\n0..%d? ",NUM_RESOLUTIONS); scanf(" \n%d",&resolution);
  if(resolution<=0) return;
  }
 while(--resolution>=NUM_RESOLUTIONS);
 *old_resolution=resolution;
 }
 
#define TESTTEXT1 \
 "Wenn Debuggen der Vorgang ist, Fehler aus einem Programm auszubauen,"
#define TESTTEXT2 \
 "dann ist Programmieren der Vorgang, Fehler einzubauen. :-)"
#define TESTTEXT3 "Bitte eine Taste drÅcken."
void testscreen(int grfxdrv,int res) 
 {
 int xs,ys;
 if(grfxdrv<0 || grfxdrv>=NUM_GRFXDRVS || res<0 || res>=NUM_RESOLUTIONS)
  return;
 ws_setdriver(grfxdrvs[grfxdrv]);
 printf("Teste Grafiktreiber %s mit Auflîsung %dx%d:\n"\
  "Es sollte ein gro·es, wei·es Kreuz erscheinen,\n"\
  "ein blaues Rechteck in der Mitte und roter Text Åber dem Rechteck.\n"\
  "Der Test kann mit irgendeiner Taste beendet werden.\n"\
  "Bitte Taste drÅcken.",grfxdrvs[grfxdrv],xs=res_xysize[res][0],
  ys=res_xysize[res][1]);
 ws_waitforkey();
 if(ws_initgrfx(xs,ys,256,init.fontname))
  {
  ws_setcolor(1,255,255,255); ws_setcolor(2,0,0,255);
  ws_setcolor(3,255,0,0);
  ws_drawline(0,0,xs-1,ys-1,1,0);
  ws_drawline(xs-1,0,0,ys-1,1,0);
  ws_drawfilledbox(xs*3/8,ys*3/8,xs/4,ys/4,2,0);
  ws_drawtext((xs-ws_pixstrlen(TESTTEXT1))/2,10,xs,TESTTEXT1,3,-1);
  ws_drawtext((xs-ws_pixstrlen(TESTTEXT2))/2,25,xs,TESTTEXT2,3,-1);
  ws_drawtext((xs-ws_pixstrlen(TESTTEXT3))/2,40,xs,TESTTEXT3,3,-1);   
  ws_waitforkey();
  ws_textmode();
  }
 else
  { ws_textmode();
    printf("Huppsa. Kann Grafikmodus nicht initialisieren.\n"\
     "Sie kînnten probieren einen residenten VESA-Treiber zu laden,\n"\
     "wie z.B. von SciTech UniVBE bzw. DisplayDoctor.\n"\
     "Bitte drÅcken Sie eine Taste.");
    ws_waitforkey(); }
 }
   
int checkd1version(char *path)
 {
 FILE *f;
 char buffer[100];
 unsigned long d;
 printf("öberprÅfe Descent 1 Version...\n");
 makedospath(path,buffer); strcat(buffer,"/"); strcat(buffer,"descent.pig");
 checkmem(f=fopen(buffer,"rb"));
 my_assert(fread(&d,sizeof(long),1,f)==1);
 fclose(f);
 return (d>=0x10000) ? d1_14_reg : d1_10_reg;
 }
 
int checkd2version(char *path)
 {
 FILE *f;
 char fname[200],buffer[100];
 printf("öberprÅfe Descent 2 Version...\n");
 makedospath(path,fname); strcat(fname,"/"); strcat(fname,"readme.txt");
 if((f=fopen(fname,"rb"))!=NULL)
  {
  fseek(f,0xb7,SEEK_SET);
  if(fread(&buffer,1,15,f)==15)
   { 
   buffer[15]=0;
   if(strncmp(buffer,"DESCENT 2  v1.",14)==0)
    {
    fclose(f);
    switch(buffer[14])
     {
     case '0': return d2_10_reg;
     case '1': return d2_11_reg;
     default: return d2_12_reg;
     }
    }
   }
  printf("Kann nicht automatisch die Descent 2 Version feststellen.\n");
  fclose(f);
  }
 else printf("Kann Datei \"%s\" nicht im Descent 2 Verzeichnis finden.\n",
  fname);
 printf("Bitte geben Sie die Versionsnummer von Descent 2 ein:\n");
 printf("Habe Sie Descent 2 Version V1.1 oder hîher? (j/n)");
 do { scanf("%c",buffer); }
 while(toupper(buffer[0])!='J' && toupper(buffer[1])!='N');
 return toupper(buffer[0])=='N' ? d2_10_reg : d2_11_reg;
 }
 
#define NUM_Q 8
const char *q[NUM_Q]={ "Ihre Wahl?","Was wollen Sie? ",
 "Ja, Sire? ","Was ist los?","Haeh? ","Zu Befehl? ",
 "Hmmm..? ","A..G,Q,0 ?" };
int askconfigdata(int *d_ver,char *devilpath,char *d1path,char *d2path,
 char *missionpath,int *grfxdriver,int *resolution)
 {
 char x;
 int start=0;
 do
  {
  printf("\n\n");
  printf("Wilkommen zu Devil "VERSION", dem Descent 1 und 2 Editor.\n");
  printf("---------------------------------------------------------\n\n");
  printf("A. éndere Devil Verzeichnis: \"%s\"\n",devilpath);
  printf("B. éndere Descent 1 (Vollversion) Verzeichnis: \"%s\"\n",d1path);
  printf("C. éndere Descent 2 (Vollversion) Verzeichnis: \"%s\"\n",d2path);
  printf("D. éndere in Devil benutzte Descent Version: %d\n",*d_ver);
  printf("E. éndere Grafiktreiber: \"%s\"\n",grfxdrvs[*grfxdriver]);
  printf("F. éndere Auflîsung: %dx%d\n",res_xysize[*resolution][0],
   res_xysize[*resolution][1]);
  printf("G. Grafik testen.\n");
  printf("Q. énderungen speichern und Programm verlassen.\n");
  printf("0. énderungen nicht speichern und Programm verlassen.\n\n");
  srand(clock());
  printf(q[start++==0 ? 0 : (int)((double)(rand()-1)/RAND_MAX*NUM_Q)]);
  scanf(" \n%c",&x); x=toupper(x);
  switch(x)
   {
   case 'A': askpath(devilpath,"Devil","config/devilg.ini"); break;
   case 'B': askpath(d1path,"Descent 1","descent.pig"); break;
   case 'C': askpath(d2path,"Descent 2","groupa.pig"); break;
   case 'D': *d_ver=((*d_ver)&1)+1;
    if(*d_ver==2)
     printf("\nDevil ist jetzt fÅr Descent 2 konfiguriert.\n"\
      "Sie kînnen auch Descent 1 Level laden.\n");
    else
     printf("\nDevil ist jetzt fÅr Descent 1 konfiguriert.\n"\
      "Sie werden keine Descent 2 Level laden/editieren kînnen.\n");
    printf("Bitte eine Taste drÅcken.\n"); ws_waitforkey(); break;
   case 'E': askgrfxdriver(grfxdriver); break;
   case 'F': askresolution(resolution); break;
   case 'G': testscreen(*grfxdriver,*resolution); break;
   case '0': return 0;
   case 'Q':
    if(!checkpath(devilpath,"config/devilg.ini","Devil")) break;
    if(*d_ver==1 && !checkpath(d1path,"descent.pig","Descent 1")) break;
    if(*d_ver==2 && !checkpath(d2path,"groupa.pig","Descent 2")) break;
    if(*d_ver==2)
     { strcpy(missionpath,d2path); strcat(missionpath,"/missions");
       *d_ver=checkd2version(d2path);
       printf("BestÑtige Descent 2 version %s in %s\n",
        *d_ver==d2_10_reg ? "1.0" : (*d_ver==d2_11_reg?"1.1":"1.2"),d2path); }
    else /* determine Descent version */
     { strcpy(missionpath,d1path); *d_ver=checkd1version(d1path); 
       printf("BestÑtige Descent 1 version %s in %s\n",
        *d_ver==d1_10_reg ? "1.0" : "1.4 or 1.5",d1path); }
    return 1;
   }
  }
 while(1);
 }
 
/* for a list of keycodes see file do_event.c, function dec_help. */
#define NUM_HOTKEYS 99
struct hotkey { int kbstat,key,event; const char *txt; } 
hotkeys[NUM_HOTKEYS]={ 
{ 0x01, 27, 59, "schnell beenden" },
{ 0x00, 27,  0, "beenden" },
{ 0x00,315, 39, "Tastaturbelegung" },
{ 0x00,316, 60, "schnell abspeichern" },
{ 0x00,317, 25, "Raum erzeugen" },
{ 0x00,318, 26, "WÅrfel teilen" },
{ 0x00,319, 23, "Descent-Karte an/aus" },
{ 0x01,271, 84, "WÅrfellinien an/aus" },
{ 0x05, 32,116, "Reinitialisiere Grafik" },
{ 0x00,  9, 58, "Bewegungsmodus Ñndern" },
{ 0x00, 99,117, "doppelte/einfache Ansicht" },
{ 0x01,'!',118, "Rendering aus" },
{ 0x01,'"',119, "Render aktuellen WÅrfel" },
{ 0x01,245,120, "Durchfliegen" },
{ 0x01,'#',120, "Durchfliegen" },
{ 0x01,'$',121, "Durchfliegen m. Kollision" },
{ 0x00, 53, 28, "gehe zu akt. Objekt" },
{ 0x00, 55, 29, "gehe zu Wand" },
{ 0x00, 57, 30, "gehe zu NachbarwÅrfel" },
{ 0x04, 32, 20, "spezial markieren" },
{ 0x01, 32, 72, "alles markieren" },
{ 0x00, 32, 73, "markieren" },
{ 0x01,595, 11, "Markiertes lîschen" },
{ 0x00,595, 10, "Aktuelles Objekt lîschen" },
{ 0x00,  8, 12, "spezial lîschen" },
{ 0x01,594,  7, "schnell einfÅgen" },
{ 0x00,594,  6, "einfÅgen" },
{ 0x00, 43, 13, "Objekt vergrî·ern" },
{ 0x00, 45, 14, "Objekt verkleinern" },
{ 0x00,109,  3, "Gruppe erzeugen" },
{ 0x01, 77, 16, "Verbindungsseite setzen" },
{ 0x00,103, 61, "Gitter an/aus" },
{ 0x04,119, 53, "Modus: WÅrfel" },
{ 0x04,115, 54, "Modus: Seite" },
{ 0x04,101,126, "Modus: Ecke" },
{ 0x04,112, 55, "Modus: Punkt" },
{ 0x04,100, 56, "Modus: Ding" },
{ 0x04,122, 57, "Modus: Zwischenwand" },
{ 0x01, 76, 82, "vorheriger Level" },
{ 0x00,108, 83, "nÑchster Level" },
{ 0x01, 87, 62, "vorheriger WÅrfel" },
{ 0x00,119, 63, "nÑchster WÅrfel" },
{ 0x01, 83, 64, "vorherige Seite" },
{ 0x00,115, 65, "nÑchste Seite" },
{ 0x01, 69,125, "vorherige Ecke" },
{ 0x00,101,124, "nÑchste Ecke" },
{ 0x01, 80, 66, "vorheriger Punkt" },
{ 0x00,112, 67, "nÑchster Punkt" },
{ 0x01, 68, 68, "vorheriges Ding" },
{ 0x00,100, 69, "nÑchstes Ding" },
{ 0x01, 90, 70, "vorherige Zwischenand" },
{ 0x00,122, 71, "nÑchste Zwischenwand" },
{ 0x00, 48,129, "setze Seite&Ecke auf 0" },
{ 0x05, 88, 34, "ausrichten an -x-axis" },
{ 0x04,120, 31, "ausrichten an +x-axis" },
{ 0x05, 89, 35, "ausrichten an -y-axis" },
{ 0x04,121, 32, "ausrichten an +y-axis" },
{ 0x05, 90, 36, "ausrichten an -z-axis" },
{ 0x04,122, 33, "ausrichten an +z-axis" },
{ 0x01, 42, 76, "Drehwinkel +" },
{ 0x01, 47, 77, "Drehwinkel -" },
{ 0x00, 42, 74, "Schrittweite +" },
{ 0x00, 47, 75, "Schrittweite -" },
{ 0x01, 65, 78, "Sichtweite +" },
{ 0x01, 89, 79, "Sichtweite -" },
{ 0x01, 90, 79, "Sichtweite -" },
{ 0x04,350, 90, "Setze Pos 1" },
{ 0x04,351, 91, "Setze Pos 2" },
{ 0x04,352, 92, "Setze Pos 3" },
{ 0x04,353, 93, "Setze Pos 4" },
{ 0x04,354, 94, "Setze Pos 5" },
{ 0x04,355, 95, "Setze Pos 6" },
{ 0x04,356, 96, "Setze Pos 7" },
{ 0x04,357, 97, "Setze Pos 8" },
{ 0x04,358, 98, "Setze Pos 9" },
{ 0x04,359, 99, "Setze Pos 10" },
{ 0x04,393,100, "öberblick" },
{ 0x04,394,101, "KorridorÅberblick" },
{ 0x01,340,102, "Gehe zu Pos 1" },
{ 0x01,341,103, "Gehe zu Pos 2" },
{ 0x01,342,104, "Gehe zu Pos 3" },
{ 0x01,343,105, "Gehe zu Pos 4" },
{ 0x01,344,106, "Gehe zu Pos 5" },
{ 0x01,345,107, "Gehe zu Pos 6" },
{ 0x01,346,108, "Gehe zu Pos 7" },
{ 0x01,347,109, "Gehe zu Pos 8" },
{ 0x01,348,110, "Gehe zu Pos 9" },
{ 0x01,349,111, "Gehe zu Pos 10" },
{ 0x01,391,112, "ZurÅck von öberblick" },
{ 0x01,392,113, "ZurÅck von KorridorÅb." },
{ 0x00,121, 41, "RÅckwÑrts bewegen" },
{ 0x00,335, 41, "RÅckwÑrts bewegen" },
{ 0x00,328, 42, "Runter bewegen" },
{ 0x00, 97, 43, "VorwÑrts bewegen" },
{ 0x00,337, 43, "VorwÑrts bewegen" },
{ 0x00,331, 44, "Nach links bewegen" },
{ 0x00,336, 45, "Nach oben bewegen" },
{ 0x00,333, 46, "Nach rechts bewegen" },
{ 0x00, 49, 47, "Nach links rotieren" },
{ 0x00, 56, 48, "Nach unten drehen" },
{ 0x00, 51, 49, "Nach rechts rotieren" },
{ 0x00, 52, 50, "Nach links drehen" },
{ 0x00, 50, 51, "Nach oben drehen" },
{ 0x00, 54, 52, "Nach rechts drehen" }
};

