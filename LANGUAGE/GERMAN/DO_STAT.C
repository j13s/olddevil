#define STARTMSG(no) (impobjs[no]==1 ? 'J' : (impobjs[no]==0 ? 'N' :  \
 (impobjs[no]<10 ? impobjs[no]+48 : '#')))
char buffer[10240];
void dec_statistics(int ec)
 {
 int impobjs[22],hostages=0,items=0,robots=0,i,d_items=0,d_robots=0;
 struct node *n;
 sprintf(buffer,"Statistik\n\n");
 if(l==NULL) { printmsg("Kein Level angew„hlt."); return; }
 sprintf(&buffer[strlen(buffer)],"Anzahl der Segmente: %d\n",l->cubes.size);
 sprintf(&buffer[strlen(buffer)],"Anzahl der W„nde: %d\n",l->doors.size);
 sprintf(&buffer[strlen(buffer)],"Anzahl der Dinge: %d\n",l->things.size);
 for(i=0;i<20;i++) impobjs[i]=0;
 for(n=l->things.head;n->next!=NULL;n=n->next)
  switch(n->d.t->type1)
   {
   case tt1_robot: robots++;
    switch(n->d.t->contain_type1)
     {
     case tt1_robot: d_robots+=n->d.t->contain_count; break;
     case tt1_item: d_items+=n->d.t->contain_count; 
      switch(n->d.t->type2)
       { case item_id_bluekey: case item_id_redkey: case item_id_yellowkey: 
          impobjs[n->d.t->contain_type2+13]+=n->d.t->contain_count; break; }
      break;
     } 
    break;
   case tt1_hostage: hostages++; break;
   case tt1_dmstart:
    if(n->d.t->type2<8) impobjs[n->d.t->type2]++;
    else impobjs[11]++;
    break;
   case 7: items++; 
    switch(n->d.t->type2)
     { case item_id_redkey: case item_id_bluekey: case item_id_yellowkey:
        impobjs[10+n->d.t->type2]++; break;
       case item_id_blueflag: impobjs[20]++; break;
       case item_id_redflag: impobjs[21]++; break; }
    break;
   case tt1_reactor: impobjs[13]++; break;
   case tt1_coopstart:
    if(init.d_ver<d2_10_sw)
     { if(n->d.t->type2<=10 && n->d.t->type2>=8) impobjs[n->d.t->type2]++;
       else impobjs[12]++; }
    else impobjs[12]++;
   }
 if(init.d_ver<d2_10_sw)
  {
  sprintf(&buffer[strlen(buffer)],
   "    Normal     Deathmatch         Coop   Falsche Nr.\n");
  sprintf(&buffer[strlen(buffer)],
   "        || |------------------| |------| |-|         \n");
  sprintf(&buffer[strlen(buffer)],
   "Starts: 00 01 02 03 04 05 06 07 08 09 10 Ill         \n");
  sprintf(&buffer[strlen(buffer)],
   "        %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c %3.3d        \n\n",
   STARTMSG(0),STARTMSG(1),STARTMSG(2),STARTMSG(3),STARTMSG(4),STARTMSG(5),
   STARTMSG(6),STARTMSG(7),STARTMSG(8),STARTMSG(9),STARTMSG(10),
   impobjs[11]+impobjs[12]);
  }
 else
  {
  sprintf(&buffer[strlen(buffer)],
   "    Normal     Deathmatch     Falsch Coops\n");
  sprintf(&buffer[strlen(buffer)],
   "        || |------------------| |-|  |-|  \n");
  sprintf(&buffer[strlen(buffer)],
   "Starts: 00 01 02 03 04 05 06 07 |-|  |-|  \n");
  sprintf(&buffer[strlen(buffer)],
   "        %c  %c  %c  %c  %c  %c  %c  %c %3.3d  %3.3d\n\n",
   STARTMSG(0),STARTMSG(1),STARTMSG(2),STARTMSG(3),STARTMSG(4),STARTMSG(5),
   STARTMSG(6),STARTMSG(7),impobjs[11],impobjs[12]);
  for(n=l->cubes.head,impobjs[8]=impobjs[9]=-1;n->next!=NULL;n=n->next)
   if(n->d.c->type==cube_blueflag) impobjs[8]=impobjs[8]!=-1 ? -2 : n->no;
   else if(n->d.c->type==cube_redflag) impobjs[9]=impobjs[9]!=-1 ? -2: n->no;
  if(impobjs[8]<0)
   sprintf(&buffer[strlen(buffer)],"Blauer Wrfel: %s ",impobjs[8]==-1 ?
    "keiner" : "mehr als einer");
  else
   sprintf(&buffer[strlen(buffer)],"Blauer Wrfel: %d ",impobjs[8]);
  if(impobjs[9]<0)
   sprintf(&buffer[strlen(buffer)]," Roter Wrfel: %s",impobjs[9]==-1 ?
    "keiner" : "mehr als einer");
  else
   sprintf(&buffer[strlen(buffer)]," Roter Wrfel: %d",impobjs[9]);
  sprintf(&buffer[strlen(buffer)],"\nBlaue Flagge: %c   Rote Flagge: %c\n",
   STARTMSG(20),STARTMSG(21));
  }
 sprintf(&buffer[strlen(buffer)],"Reaktor: %c\n",STARTMSG(13));
 sprintf(&buffer[strlen(buffer)],"Anzahl der Geiseln: %d\n",hostages);
 sprintf(&buffer[strlen(buffer)],"Schlssel: blau %c / rot %c / gelb %c\n",
  STARTMSG(14),STARTMSG(15),STARTMSG(16));
 sprintf(&buffer[strlen(buffer)],"Anzahl der Gegenst„nde: %d\n",items);
 sprintf(&buffer[strlen(buffer)],"Anzahl der Roboter: %d\n",robots);
 sprintf(&buffer[strlen(buffer)],"Von Robotern fallengelassene Schlssel:\n");
 sprintf(&buffer[strlen(buffer)],"blau %c / rot %c / gelb %c\n",
  STARTMSG(17),STARTMSG(18),STARTMSG(19));
 sprintf(&buffer[strlen(buffer)],
  "Anzahl der fallengelassenen Gegenst„nde: %d\n",d_items);
 sprintf(&buffer[strlen(buffer)],"Anzahl der fallengelassenen Roboter: %d\n",
  d_robots);
 waitmsg(buffer);
 }
