/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    buyman.c - encourage the author to continue with updating Devil
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program (file COPYING); if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

void buy_manual(FILE *f)
 { 
 char count[2],weiter;
 fseek(f,SEEK_SET,0);
 if(fscanf(f,"%c%c",&count[0],&count[1])!=2 || count[0]<'A' || 
  count[1]<'A' || count[0]>'Z' || count[1]>'Z')
  { count[0]=count[1]='A'; printf("Counter Reset.\n"); }
 if(++count[0]>'Z') { count[0]='A'; if(count[1]++>'Z') count[1]='Z'; }
 fseek(f,SEEK_SET,0);
 fprintf(f,"%c%c",count[0],count[1]);
 switch((count[1]-'A')*26+count[0]-'A')
  {
  case 25: 
   printf("\nHallo. Devil wird gerade zum 25. Mal gestartet.\n");
   printf("Haben Sie die vollstÑndige, deutsche Anleitung schon? (j/n)\n");
   while(!isalnum(weiter=ws_waitforkey()));
   if(toupper(weiter)=='J') { printf("Dann viel Spa·.\n"); return; }
   printf("\nIch hoffe, Sie konnten das Programm bisher ohne\n");
   printf("Probleme benutzen und haben schon den einen oder\n");
   printf("anderen kleinen Level erstellt.\n");
   printf("Ich glaube, es ist nun Zeit, Sie darauf hinzuweisen,\n");
   printf("da· es eine Mîglichkeit gibt, den Autor zum Weiterentwickeln\n");
   printf("von Devil zu ermutigen und gleichzeitig davon zu profitieren.\n");
   printf("Senden Sie einfach 20DM (oder mehr ;-) an folgende Adresse:\n");
   printf(" Achim Stremplat\n Veilchenstr. 11\n 76131 Karlsruhe\n");
   printf("Wenn Sie den 20DM Ihre Adresse beilegen, erhalten Sie die\n");
   printf("vollstÑndige, deutsche Devil-Anleitung zugeschickt und\n");
   printf("kînnen so den Editor besser ausnutzen.\n");
   printf("Ich danke Ihnen fÅr Ihre Aufmerksamkeit,\n");
   printf("Bitte drÅcken Sie eine Taste.\n"); ws_waitforkey();
   break;
  case 50:
   printf("Hallo, da bin ich wieder.\n");
   printf("Haben Sie die vollstÑndige, deutsche Anleitung schon? (j/n)\n");
   while(!isalnum(weiter=ws_waitforkey()));
   if(toupper(weiter)=='J') { printf("Dann viel Spa·.\n"); return; }
   printf("\nSie starten Devil gerade zum 50. Mal, und ich habe Grund zur\n");
   printf("Hoffnung, da· Sie mit dem Programm etwas anfangen kînnen.\n");
   printf("Da Sie dann wahrscheinlich auch an weiteren Devil Versionen\n");
   printf("interessiert sind, wÑre ich Ihnen dankbar, wenn Sie dieses\n");
   printf("Interesse dadurch ausdrÅcken kînnten, da· Sie an folgende\n");
   printf("Adresse 20DM (oder mehr ;-) schicken:\n");
   printf(" Achim Stremplat\n Veilchenstra·e 11\n 76131 Karlsruhe\n");
   printf("Sie erhalten dafÅr auch, wenn Sie Ihre Adresse beilegen, die\n");
   printf("vollstÑndige, deutsche Devil-Anleitung zugeschickt und kînnen.\n");
   printf("so den Editor noch besser ausnutzen, als Sie es wohl ohnehin\n");
   printf("schon tun werden.\n");
   printf("Bitte drÅcken Sie die Leertaste.\n");
   while(ws_waitforkey()!=' ');
   break;
  case 100:
   printf("Guten Tag. Wir haben lange nichts mehr voneinander gehîrt.\n");
   printf("Haben Sie die vollstÑndige, deutsche Anleitung schon? (j/n)\n");
   while(!isalnum(weiter=ws_waitforkey()));
   if(toupper(weiter)=='J') { printf("Dann viel Spa·.\n"); return; }
   printf("\nSie haben Devil gerade zum 100. Mal gestartet,\n");
   printf("das hei·t wohl, Sie benutzen den Editor recht regelmÑ·ig.\n");
   printf("Sie wÅrden also von einer Weiterentwicklung von Devil sehr\n");
   printf("profitieren. Um diese Weiterentwicklung zu ermîglichen,\n");
   printf("senden sie bitte 20DM an folgende Adresse:\n");
   printf(" Achim Stremplat\n Veilchenstra·e 11\n 76131 Karlsruhe\n");
   printf("DafÅr gibt es auch (wenn Sie Ihre Adresse beifÅgen), die\n");
   printf("vollstÑndige, deutsche Devil-Anleitung zugeschickt, so da·\n");
   printf("Sie Feinheiten einiger Funktionen und Parameter nachlesen\n");
   printf("kînnen, was Ihnen beim Bauen Ihrer eigenen Levels sicher\n");
   printf("sehr hilfreich ist.\n");
   printf("Bitte drÅcken Sie 'd'\n");
   while(ws_waitforkey()!='d');
   break;
  case 200:
   printf("Huhu. Das ist das letzte Mal, da· ich mich bei Ihnen melde.\n");
   printf("Haben Sie die vollstÑndige, deutsche Anleitung schon? (j/n)\n");
   while(!isalnum(weiter=ws_waitforkey()));
   if(toupper(weiter)=='J') { printf("Dann viel Spa·.\n"); return; }
   printf("\nSo langsam kînntest Du eigentlich mal 20DM abdrÅcken.\n");
   printf("Du startest Devil gerade zum 200. Mal und das Programm war\n");
   printf("wirklich nicht wenig Arbeit und au·erdem gibt es ja die\n");
   printf("vollstÑndige Anleitung fÅr die 20DM. Also:\n");
   printf("20DM und Adresse an:\n");
   printf(" Achim Stremplat\n Veilchenstra·e 11\n 76131 Karlsruhe\n");
   printf("Mit 'D' gehts weiter. Und Tschuess....\n");
   while(ws_waitforkey()!='D');
   break;
  }
 }
