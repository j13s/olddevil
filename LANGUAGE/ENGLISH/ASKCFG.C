const char *grfxdescription[NUM_GRFXDRVS]=
 { "This is the mode which should work everywhere\n",
   "This one is only for S3 chipsets and has minor bugs.\n"\
   "               But it is faster than the others,\n"\
   "               so check if it works with your system.",
   "Tseng ET4000 chipset; choose if you have trouble with VESA.",
   "Cirrus Logic chipset; choose if you have trouble with VESA.",
   "ATI Mach 64; choose if you have trouble with VESA." };
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
  { printf("Can't find %s in path \"%s\".\n",text,path); 
    printf("Devil needs 'Medium' installation of Descent 2\n");
    printf("for Descent 2 mode. If you don't have Descent 2\n");
    printf("switch to Descent 1 mode (menupoint 'D').\n"); return 0; }
 }
 
void askpath(char *oldpath,const char *text,const char *testfile)
 {
 char testpath[300],path[300];
 int ok=1;
 makedospath(oldpath,path);
 do
  { 
  printf("%s directory (old: \"%s\", abort with 'q'): ",text,oldpath);
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
 printf("Please choose a graphics driver:\n"); 
 do
  {
  printf("0. Abort\n");
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
 printf("Choose a resolution:\n");
 do
  {
  printf("0. Abort\n");
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
#define TESTTEXT3 "Please press a key."
void testscreen(int grfxdrv,int res) 
 {
 int xs,ys;
 if(grfxdrv<0 || grfxdrv>=NUM_GRFXDRVS || res<0 || res>=NUM_RESOLUTIONS)
  return;
 ws_setdriver(grfxdrvs[grfxdrv]);
 printf("Testing graphics driver %s with resolution %dx%d:\n"\
  "You must see a big white cross, a blue rectangle in the middle\n"\
  "and some red text above the rectangle.\n"\
  "You can end the test with any key (well, don't try shift :-).\n"\
  "Press a key.",grfxdrvs[grfxdrv],xs=res_xysize[res][0],
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
    printf("Woops. Couldn't init grfx-mode. Press a key.\n"); 
    printf("Maybe you should load a resident VESA-driver (like UniVBE).\n");
    ws_waitforkey(); }
 }

int checkd1version(char *path)
 {
 FILE *f;
 char buffer[100];
 unsigned long d;
 printf("Checking Descent 1 version...\n");
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
 printf("Checking Descent 2 version...\n");
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
  printf("Can't automatically determine the Descent 2 version.\n");
  fclose(f);
  }
 else printf("Can't find \"%s\" in the Descent 2 directory.\n",fname);
 printf("Please enter the version number of your Descent 2:\n");
 printf("Do you have a Descent 2 version V1.1 or better? (y/n)");
 do { scanf("%c",buffer); }
 while(toupper(buffer[0])!='Y' && toupper(buffer[1])!='N');
 return toupper(buffer[0])=='N' ? d2_10_reg : d2_11_reg;
 }
 
#define NUM_Q 9
const char *q[NUM_Q]={ "Please choose:","What do you want? ",
 "Yes, Sire? ","Your choice: ","What's up? ","What? ","Your command? ",
 "Hmmm..? ","A..G,Q,0 ?" };
int askconfigdata(int *d_ver,char *devilpath,char *d1path,char *d2path,
 char *missionpath,int *grfxdriver,int *resolution)
 {
 char x;
 int start=0;
 do
  {
  printf("\n");
  printf("Welcome to Devil "VERSION", the Descent 1 and Descent 2 Editor.\n");
  printf("-----------------------------------------------------------\n");
  printf("If you will use Devil, please do me a favor and\n");
  printf("send me an E-Mail (if possible) with the subject\n");
  printf("'devil user count'. I just want to know how many people\n");
  printf("are using Devil. Thanks, Achim (ubdb@rz.uni-karlsruhe.de)\n");
  printf("-----------------------------------------------------------\n");
  printf("A. Change Devil path: \"%s\"\n",devilpath);
  printf("B. Change Descent 1 path (commercial version): \"%s\"\n",d1path);
  printf("C. Change Descent 2 path (commercial version): \"%s\"\n",d2path);
  printf("D. Change Descent version for Devil: %d\n",*d_ver);
  printf("E. Change graphics driver: \"%s\"\n",grfxdrvs[*grfxdriver]);
  printf("F. Change resolution: %dx%d\n",res_xysize[*resolution][0],
   res_xysize[*resolution][1]);
  printf("G. Test graphics.\n");
  printf("Q. Exit with saving changes.\n");
  printf("0. Exit without saving changes.\n\n");
  srand(clock());
  printf(q[start++==0 ? 0 : (int)((double)(rand()-1)/RAND_MAX*NUM_Q)]);
  scanf(" \n%c",&x); x=toupper(x);
  switch(x)
   {
   case 'A': askpath(devilpath,"Devil","config/devil.ini"); break;
   case 'B': askpath(d1path,"Descent 1","descent.pig"); break;
   case 'C': askpath(d2path,"Descent 2","groupa.pig"); break;
   case 'D': *d_ver=((*d_ver)&1)+1;
    if(*d_ver==2)
     printf("\nDevil is now configured for Descent 2.\n"\
      "You can load Descent 1 levels, too.\n");
    else
     printf("\nDevil is now configured for Descent 1.\n"\
      "You won't be able to load/edit Descent 2 levels.\n");
    printf("Press a key.\n"); ws_waitforkey(); break;
   case 'E': askgrfxdriver(grfxdriver); break;
   case 'F': askresolution(resolution); break;
   case 'G': testscreen(*grfxdriver,*resolution); break;
   case '0': return 0;
   case 'Q':
    if(!checkpath(devilpath,"config/devil.ini","Devil")) break;
    if(*d_ver==1 && !checkpath(d1path,"descent.pig","Descent 1")) break;
    if(*d_ver==2 && !checkpath(d2path,"groupa.pig","Descent 2")) break;
    if(*d_ver==2)
     { strcpy(missionpath,d2path); strcat(missionpath,"/missions");
       *d_ver=checkd2version(d2path);
       printf("Confirmed Descent 2 version %s in %s\n",
       *d_ver==d2_10_reg ? "1.0" : (*d_ver==d2_11_reg ? "1.1":"1.2"),d2path); }
    else /* determine Descent version */
     { strcpy(missionpath,d1path); *d_ver=checkd1version(d1path); 
       printf("Confirmed Descent 1 version %s in %s\n",
        *d_ver==d1_10_reg ? "1.0" : "1.4 or 1.5",d1path); }
    return 1;
   }
  }
 while(1);
 }
 
/* for a list of keycodes see file do_event.c, function dec_help. */
#define NUM_HOTKEYS 105
struct hotkey { int kbstat,key,event; const char *txt; } 
hotkeys[NUM_HOTKEYS]={ 
{ 0x01, 27, 59, "fast quit" },
{ 0x00, 27,  0, "quit" },
{ 0x00,315, 39, "hot keys" },
{ 0x00,316, 60, "fast save" },
{ 0x00,317, 25, "make room" },
{ 0x00,318, 26, "split cube" },
{ 0x00,319, 23, "all lines on/off" },
{ 0x01,271, 84, "wireframe on/off" },
{ 0x05, 32,116, "reinit graphics" },
{ 0x00,  9, 58, "change movemode" },
{ 0x00,100,117, "change single/double" },
{ 0x01,'!',118, "rendering off" },
{ 0x01,'"',119, "render current cube" },
{ 0x01,245,120, "fly through" },
{ 0x01,'#',120, "fly through" },
{ 0x01,'$',121, "fly with collision" },
{ 0x00, 53, 28, "beam to current object" },
{ 0x00, 55, 29, "goto wall" },
{ 0x00, 57, 30, "goto side cube" },
{ 0x02, 32, 20, "tag special" },
{ 0x01, 32, 72, "tag all" },
{ 0x00, 32, 73, "tag" },
{ 0x01,595, 11, "delete tagged" },
{ 0x00,595, 10, "delete current" },
{ 0x00,  8, 12, "delete special" },
{ 0x01,594,  7, "insert fast" },
{ 0x00,594,  6, "insert" },
{ 0x00, 43, 13, "enlarge object" },
{ 0x00, 45, 14, "shrink object" },
{ 0x00,109,  3, "make group" },
{ 0x01, 77, 16, "set connection side/exit" },
{ 0x00,103, 61, "grid on/off" },
{ 0x04, 99, 53, "mode: cube" },
{ 0x04,115, 54, "mode: side" },
{ 0x04,101,126, "mode: edge" },
{ 0x04,112, 55, "mode: point" },
{ 0x04,116, 56, "mode: thing" },
{ 0x04,119, 57, "mode: wall" },
{ 0x01, 76, 82, "previous level" },
{ 0x00,108, 83, "next level" },
{ 0x01, 67, 62, "previous cube" },
{ 0x00, 99, 63, "next cube" },
{ 0x01, 83, 64, "previous side" },
{ 0x00,115, 65, "next side" },
{ 0x01, 69,125, "previous edge" },
{ 0x00,101,124, "next edge" },
{ 0x01, 80, 66, "previous point" },
{ 0x00,112, 67, "next point" },
{ 0x01, 84, 68, "previous thing" },
{ 0x00,116, 69, "next thing" },
{ 0x01, 87, 70, "previous wall" },
{ 0x00,119, 71, "next wall" },
{ 0x00, 48,129, "set side&edge to 0" },
{ 0x05, 88, 34, "align to neg. x-axis" },
{ 0x04,120, 31, "align to pos. x-axis" },
{ 0x05, 89, 35, "align to neg. y-axis" },
{ 0x04,121, 32, "align to pos. y-axis" },
{ 0x05, 90, 36, "align to neg. z-axis" },
{ 0x04,122, 33, "align to pos. z-axis" },
{ 0x01, 42, 76, "increase rotation angle" },
{ 0x01, 47, 77, "decrease rotation angle" },
{ 0x00, 42, 74, "increase movefactor" },
{ 0x00, 47, 75, "decrease movefactor" },
{ 0x01, 65, 78, "increase visibility" },
{ 0x01, 89, 79, "decrease visibility" },
{ 0x01, 90, 79, "decrease visibility" },
{ 0x04,350, 90, "save pos 1" },
{ 0x04,351, 91, "save pos 2" },
{ 0x04,352, 92, "save pos 3" },
{ 0x04,353, 93, "save pos 4" },
{ 0x04,354, 94, "save pos 5" },
{ 0x04,355, 95, "save pos 6" },
{ 0x04,356, 96, "save pos 7" },
{ 0x04,357, 97, "save pos 8" },
{ 0x04,358, 98, "save pos 9" },
{ 0x04,359, 99, "save pos 10" },
{ 0x04,393,100, "overview" },
{ 0x04,394,101, "corridor overview" },
{ 0x01,340,102, "goto pos 1" },
{ 0x01,341,103, "goto pos 2" },
{ 0x01,342,104, "goto pos 3" },
{ 0x01,343,105, "goto pos 4" },
{ 0x01,344,106, "goto pos 5" },
{ 0x01,345,107, "goto pos 6" },
{ 0x01,346,108, "goto pos 7" },
{ 0x01,347,109, "goto pos 8" },
{ 0x01,348,110, "goto pos 9" },
{ 0x01,349,111, "goto pos 10" },
{ 0x01,391,112, "back from overview" },
{ 0x01,392,113, "back from cor. overview" },
{ 0x00,121, 41, "move backward" },
{ 0x00,122, 41, "move backward" },
{ 0x00,335, 41, "move backward" },
{ 0x00,328, 42, "move down" },
{ 0x00, 97, 43, "move forward" },
{ 0x00,337, 43, "move forward" },
{ 0x00,331, 44, "move left" },
{ 0x00,336, 45, "move up" },
{ 0x00,333, 46, "move right" },
{ 0x00, 49, 47, "bank left" },
{ 0x00, 56, 48, "turn down" },
{ 0x00, 51, 49, "bank right " },
{ 0x00, 52, 50, "turn left" },
{ 0x00, 50, 51, "turn up" },
{ 0x00, 54, 52, "turn right" }
};

