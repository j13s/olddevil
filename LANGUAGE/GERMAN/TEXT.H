/* devil.c */
#define TXT_NOCFGFILE "Konfigurationsdatei wurde wegen\n"\
 "Kommandozeilenparameter nicht geladen."
#define TXT_CMDSTARTNEW "Letzten Status nicht wiederherstellen"
#define TXT_CMDDONTSHOWTITLE "Titelbild nicht laden"
#define TXT_CMDCONFIG "Devil neu konfigurieren"
#define TXT_CMDUNKNOWNPARAM "Unbekannter Parameter [%s]. Erlaubte"\
 " Parameter:\n"
/* initio.c */
#define TXT_TOP "Anfang"
#define TXT_UP "hoch"
#define TXT_DOWN "runter"
#define TXT_LIST "Liste"
#define TXT_TXT1 "Txt1"
#define TXT_TXT2 "Txt2"
/* tools.c */
#define TXT_TOOMANYSTARTS "WARNUNG: Zu viele StartplÑtze von diesem Typ."
#define TXT_READBMUNKNOWNDIR "Unbekannte Orientieruung in readbitmap: %d."
#define TXT_NORMALIZE "Normieren: Vektor hat die LÑnge Null."
#define TXT_ONLYONESECRETSTART \
 "Nur ein Start (%d) fÅr Geheimlevel pro Level erlaubt.\n"\
 "Descent wird den Start mit der kleineren Nummer benutzen."
#define TXT_NOSECRETDOORFORSTART \
 "Es existiert in dem Level kein geheimer Ausgang,\n"\
 "fÅr den dieser Startpunkt gebraucht wÅrde."
/* readlvl.c */
#define TXT_ADJLIGHTMODE "Vor dem Abspeichern bitte den\n"\
 "Licht-justieren-Modus fÅr WÅrfel %d Seite %d verlassen."
#define TXT_WRONGBLKHEAD "Datei %s hat den falschen Anfang:\n"\
 "\"%s\"!=\"DMB_BLOCK_FILE\""
#define TXT_NOCUBEFORFL "Kann WÅrfel %d fÅr flackerndes Licht %d nicht finden."
#define TXT_CORRINWORK "Der Hexer ist in diesem Level aktiv.\n"\
 "Der Level kann erst geschlossen werden,\n"\
 "wenn der Hexer deaktiviert worden ist."
#define TXT_CONVTEXTURES \
 "Ersetze Descent 1 textures mit Descent 2 textures."
#define TXT_NOTALLTXTSCONV \
 "Warnung: Einige der Textures aus diesem Level\n"\
 "existieren nicht in Descent 2 und\n"\
 "auch nicht in der Austausch-Tabelle.\n"\
 "Sie sind durch Texture 1 %s (%d)\n"\
 "oder Texture 2 %s (%d) ersetzt worden.\n"
#define TXT_DISCARDCHANGES "Level/Makro '%.30s'\n"\
 "nicht gesichert. énderungen lîschen?"
#define TXT_TOOMANYEDOORS \
 "Zu viele TÅren sollen am Ende des Levels geîffnet werden (max.: 10).\n"\
 "Alle ÅberzÑhligen ignorieren?" 
#define TXT_TOOMANYCUBES \
 "WARNUNG:\nMehr Segmente (%d) als Descent verwalten kann. (max.: %d)\nEgal?"
#define TXT_TOOMANYPTS \
 "WARNUNG:\nMehr Punkte (%d) als Descent verwalten kann. (max.: %d)\nEgal?"
#define TXT_TOOMANYTHINGS \
 "WARNUNG:\nMehr Dinge (%d) als Descent verwalten kann. (max.: %d)\nEgal?"
#define TXT_TOOMANYWALLS \
 "WARNUNG:\nMehr WÑnde (%d) als Descent verwalten kann. (max.: %d)\nEgal?"
#define TXT_TOOMANYRCENTERS \
 "WARNUNG:\nMehr Robotprod. (%d) als Descent verwalten kann. (max.: %d)\n"\
 "Egal?"
#define TXT_TOOMANYSWITCHES \
 "WARNUNG:\nMehr Schalter (%d) als Descent verwalten kann. (max.: %d)\nEgal?"
#define TXT_TOOMANYTURNOFFS \
 "WARNUNG:\nMehr Lichtquellen zum Ausschie·en/-schalten (%d)\n"\
 "als Descent verwalten kann. (max.: %d)\nEgal?"
#define TXT_TOOMANYCHANGEDLIGHTS \
 "WARNUNG:\nMehr Lichter mÅssen beim Ausschalten von Lichtquellen\n"\
 "geÑndert werden (%d), als Descent verwalten kann (max. %d).\nEgal?\n"
#define TXT_THINGOUTOFBOUNDS "WARNUNG:\nDing %d au·erhalb der Mine.\nEgal?"
#define TXT_ILLNOFORSTART \
 "WARNUNG:\nFalsche Nummer %d fÅr Startplatz %d.\nEgal?"
#define TXT_TOOMANYCOOPSTARTS \
 "WARNUNG:\nZu viele Coop-Starts (%d ist der fÅnfte).\nEgal?"
#define TXT_SAMENOFORSTART \
 "WARNUNG:\nZwei StartplÑtze mit der Nr. %d:\nDing %d und %d.\nEgal?"
#define TXT_TWOKEYSOFSAMECOLOR \
 "WARNUNG:\nZwei SchlÅssel mit derselben Farbe:\nDing %d und %d."
#define TXT_TWOREACTORS "WARNUNG: Doppelter Reaktor/Flagge: Ding %d und %d"
#define TXT_NOSECRETSTART \
 "WARNUNG:\nKein Startplatz nach RÅckkehr aus Geheimlevel.\n"\
 "(Ausgang zu Geheimlevel %d).\nEgal?"
#define TXT_NOSECRETEXIT \
 "WARNUNG:\nKein Ausgang zu Geheimlevel,\n"\
 "aber ein Startplatz fÅr RÅckkehr (Ding %d).\nEgal?"
#define TXT_NOSTART "WARNUNG:\nKein Startplatz.\nEgal?"
#define TXT_NEWILLUM "WARNUNG:\nDie Beleuchtung des Levels mu· vermutlich\n"\
 "neu berechnet werden, wenn das Ausschie·en der Lichter\n"\
 "richtig funktionieren soll. Soll das jetzt geschehen?"
#define TXT_CLICKEDTHING "Ding %d angewÑhlt"
#define TXT_CLICKEDDOOR "Wand %d angewÑhlt"
#define TXT_CLICKEDPNT "Punkt %d angewÑhlt"
#define TXT_CLICKEDEDGE "Segment %d, Seite %d, Punkt %d angewÑhlt"
#define TXT_CLICKEDWALL "Segment %d, Seite %d angewÑhlt"
#define TXT_CLICKEDCUBE "Segment %d angewÑhlt"
#define TXT_NOSIDE "Segment %d, Seite %d hat keine Mauer,\n"\
 "es sollte aber eine dort sein."
#define TXT_NOSWITCHDATA "Keine Daten fÅr Schalter fÅr Wand %d."
#define TXT_WRONGNUMBEROFEDOORS "Anzahl der sich am Ende îffnenden TÅren"\
 "ist falsch."
#define TXT_CANTREADLVL "Level %s kann nicht komplett gelesen werden.\n"\
 "Datei im Eimer."
#define TXT_READINGPTS "Lese %ld Punkte..."
#define TXT_INITCUBE "Initialisiere Segment %d/%d"
#define TXT_INITDOOR "Initialisiere TÅr %d/%d"
#define TXT_INITTURNOFF "Initialisiere schaltbare Lichtquelle %d/%d"
#define TXT_ERROPENATEND "Kann Segment %d fÅr sich am Ende îffnende TÅr"\
 "nicht finden."
#define TXT_ERROAENODOOR "In Segment %d, Seite %d ist keine TÅr,\n"\
 "die am Ende geîffnet werden kann."
#define TXT_LEVELREAD "Level %s\nvon Datei %s gelesen."
/* grfx.h */
#define TXT_DEFAULTPOGFILE "Die Standardtextures benutzen?"
#define TXT_POGFILE "POG-Datei auswÑhlen"
#define TXT_INFO "Info"
#define TXT_MOVEYOU "Bewege Dich"
#define TXT_MOVECURRENT "WÅrfel auswÑhlen"
#define TXT_MOVEOBJ "Bewege Objekt"
#define TXT_MOVETXT "Texture"
#define TXT_MODIFYWHAT "Bearbeite:"
#define TXT_MOVEMODE "Modus:"
#define TXT_LEVELPIGFILE "Texture-Set:"
#define TXT_BUTTONS "Kontrollpult"
#define TXT_CANTINITWINS "Kann Fenstersystem nicht initialisieren.\n"\
 "Vielleicht bringt ein anderer Graphik-Treiber was oder\n"\
 "ein residenter VESA-Treiber, wenn die Karte nicht VESA kompatibel ist.\n"
#define TXT_NOMENUFILE "Keine Ini-Datei fÅr das MenÅ: \"%s\".\n"
/* do_side.c */
#define TXT_CANTMAKETHING "Kann Ding nicht machen."
#define TXT_TAGGEDCUBENOPROD "Markiertes Segment %d ist kein Robot-Produzent."
#define TXT_TOOMUCHCUBESTAGGED "Zuviele Segmente markiert (max. 10)."
#define TXT_DOOROPENITSELF "TÅr kann sich nicht selbst îffnen."
#define TXT_TAGGEDWALLNODOOR "Markierte Wand %d ist keine TÅr."
#define TXT_TAGGEDWALLNOSWITCHILLWALL \
 "Markierte Wand %d ist nicht 'nur Schalter', 'Nur Texture' oder 'Wand'."
#define TXT_TOOMUCHDOORSTAGGED "Zuviele TÅren markiert (max. 10)."
#define TXT_LEVEL "Level"
#define TXT_ILLUM_BRIGHTNESS "Nur Werte zwischen 0.01 und 100 erlaubt."
#define TXT_NOSWITCHABLEWALL \
 "Es kînnen nur WÑnde mit zwei Seiten geschaltet werden."
#define TXT_WALLTYPENEEDSNB "Dieser Wandtyp benîtigt eine verbundene Seite."
/* do_event.c */
#define TXT_CLOSETXTWIN "Bitte die Texturefenster schliessen,\n"\
 "bevor die Fensterpositionen abgespeichert/geladen werden."
#define TXT_SAVEWINPOS "Fensterposition speichern"
#define TXT_LOADWINPOS "Fensterposition laden"
#define TXT_CANTSAVEWINPOS \
 "Kann Fensterpositionen nicht in Datei %s speichern."
#define TXT_CANTREADWINPOS "Kann Fensterpositionen nicht aus Datei %s laden."
#define TXT_WINPOSSAVED "Fensterpositionen in Datei %s gesichert."
#define TXT_WINPOSREAD "Fensterpositionen aus Datei %s geladen."
#define TXT_CANTOPENLVL "Kann Level '%s' nicht îffnen."
#define TXT_CANTSAVELVL "Level '%s' NICHT abgespeichert."
#define TXT_LEVELSAVED "Level '%s'\ngespeichert in Datei '%s'"
#define TXT_NOLEVEL "Kein aktueller Level."
#define TXT_NOFUNCTION "Funktion Nr. %d noch nicht implementiert."
#define TXT_LVLNAME "Level-Name:"
#define TXT_LOADLEVEL "Level laden"
#define TXT_SAVELEVEL "Level speichern"
#define TXT_NODRAWING "Male jetzt %s%s%s%s%s"
#define TXT_DWCUBES "Segmente "
#define TXT_DWWALLS "WÑnde "
#define TXT_DWTHINGS "Dinge "
#define TXT_DWLINES "alle Linien "
#define TXT_DWXTAGGED "extra Markierungen "
#define TXT_LEVELNAME "Level-Name"
#define TXT_CALCMAP "Berechne neue Karte....."
/* do_move.c */
#define TXT_POSSAVED "Position %d gesetzt."
#define TXT_NOCORR "Kein Korridor im Bau."
#define TXT_NOCURRCUBE "Kein aktuelles Segment."
#define TXT_NOCURRPNT "Kein aktueller Punkt."
#define TXT_NOCURRTHING "Kein aktuelles Ding."
#define TXT_NOCURRWALL "Keine aktuelle Wand."
#define TXT_SIDECUBENONEIGHBOUR "An der Seite ist kein Nachbar."
#define TXT_GRIDROT "Achtung: Wenn die WÅrfel mit eingeschaltetem Gitter\n"\
 "gedreht werden, werden sie etwas verzerrt."
#define TXT_NOMOVINGMODE "In diesem Modus ist Bewegen nicht mîglich."
#define TXT_NOTURNOBJECT "Das Objekt kann nicht gedreht werden."
#define TXT_CANTSEETXTWALL "Die Texture auf der Seite ist nicht sichtbar,\n"\
 "warum wollen Sie sie Ñndern?"
/* macros.c */
#define TXT_DEFAULTMACRONAME "Gruppe %d"
#define TXT_NOCURRSIDE "Keine Mauer an der aktuellen Seite."
#define TXT_CUBETAGGEDON "Diese Seite ist schon mit einem Segment verbunden."
#define TXT_TOOMANYITEMS "WARNUNG:\nMehr Segmente,Punkte,Dinge oder WÑnde\n"\
 "als Descent verwalten kann.\nEgal?"
#define TXT_CANTOPENMACROFILE "Kann Makro Datei '%s' nicht îffnen."
#define TXT_NOMACROHEAD "Kann fÅr Makro '%s' Makro-Kopf nicht lesen."
#define TXT_NOMACROFILE "Datei '%s' ist kein Makro mit altem Dateiformat."
#define TXT_ERRREADINGMACRO "Fehler beim Lesen von Makro %s."
#define TXT_NOCONNECTSIDE "In Makro %s ist keine Verbindungsseite definiert."
/* insert.c */
#define TXT_CONNCUBEITSELF "Kann Segment nicht mit sich selber verbinden."
#define TXT_CONNWALLINWAY \
 "Kann Seite %d,%d nicht mit Seite %d,%d verbinden.\n"\
 "Zuerst mu· der Schalter entfernt werden."
#define TXT_CONNCUBETOOFARAWAY \
 "Kann Segment %d, Seite %d nicht verbinden mit Segment %d, Seite %d\n"\
 "Der Abstand zwischen den Seiten ist zu gro·."
#define TXT_CONNAMBIGUOUS "Verbindung zwischen Segment %d, Seite %d und\n"\
 "Segment %d, Seite %d ist nicht eindeutig."
#define TXT_CUBESCONNECTED \
 "Segment %d, Seite %d mit Segment %d, Seite %d verbunden."
#define TXT_TPLINETOOLONG \
 "Strecke %d-%d in Seite %d von Segment %d ist zu kurz oder zu lang."
#define TXT_TPCUBENOTCONVEX "Segment %d ist am Punkt %d nicht konvex."
#define TXT_TPWEIRDWALL "Seite %d von Segment %d ist verdreht."
#define TXT_CANTKILLTARGET "Kann Ziel %d nicht lîschen."
#define TXT_CLEANNODOORFORSWITCH "Kann TÅr %d fÅr Schalter nicht finden."  
#define TXT_NOCUBEFORDOOR "Kann Segment %lu fÅr TÅr nicht finden."
#define TXT_WRONGWALLINDOOR "Falsche Seitennr. %lu (Segment=%lu) fÅr TÅr."
#define TXT_NOWALLFORDOOR "TÅr: Wand %lu,%lu existiert nicht."
#define TXT_NONCFORDOOR \
 "WARNUNG: Segment %lu, Seite %lu fÅr Wand hat keinen Nachbar."
#define TXT_NOSWITCHFORDOOR "Kann Schalter %d fÅr TÅr %d nicht finden."
#define TXT_NOCUBEFORSWITCH "Kann Segment %hd fÅr TÅr nicht finden."
#define TXT_WRONGWALLFORSWITCH "Falsche Seitennr. %d fÅr Schalter %d."
#define TXT_NODOORFORSWITCH \
 "Kann TÅr an Segment %d, Seite %d fÅr Schalter %d nicht finden."
#define TXT_NOCUBEPNT "Kann Segmentpunkt %d nicht finden.\nLevel schlie·en?"
#define TXT_NONCFORCUBE \
 "Kann Nachbarsegment %d fÅr Segment %d nicht finden.\nLevel schlie·en?"
#define TXT_NODOORFORCUBE \
 "Kann TÅr %d fÅr Segment %d nicht finden.\nLevel schlie·en?"
#define TXT_NOPRODFORCUBE \
 "Kann Robot-Produzent %d fÅr Segment %d nicht finden.\nLevel schlie·en?"
#define TXT_NOMEMFOROLDCOORDS \
 "Kein Speicher fÅr das Abspeichern der alten Koordinaten mehr."
#define TXT_TO_CANTFINDCUBE \
 "Kann WÅrfel %d fÅr schaltbare Lichtquelle %d nicht finden."
#define TXT_TO_CANTFINDCL \
 "Kann zu Ñnderndes Lich %d fÅr schaltbare Lichtquelle %d nicht finden."
/* do_ins.c */
#define TXT_SPLITCANTCONNECT \
 "Kann Verbindung von Seite %d mit WÅrfel %d,%d nicht wiederherstellen!?"
#define TXT_SPLITCANTCONNECT2 "Kann Seite %d,%d nicht verbinden!?"
#define TXT_SPLITDOOR "Kann WÅrfel %d nicht unterteilen.\n"\
 "Die Wand an Seite %d mu· entfernt werden."
#define TXT_MAKEROOM \
 "Alle Seiten in den markierten WÅrfeln sind verbunden.\n"\
 "Die nicht verbundenen Seiten sind markiert."
#define TXT_CANTINSCUBE "Kann hier keinen WÅrfel einfÅgen.\n"\
 "Der Schalter mu· vorher entfernt werden."
#define TXT_STDLEVELNAME "Neuer Level"
#define TXT_MAKEMACROCONNCUBE \
 "Eine mit einem markiertem WÅrfel verbundene Seite\n"\
 "kann nicht die aktuelle Seite sein, wenn eine Gruppe erstellt wird."
#define TXT_NOMACRO "Keine aktuelle Gruppe."
#define TXT_MACROLOADED "Gruppe '%s' geladen."
#define TXT_MACROMADE "Gruppe erzeugt."
#define TXT_CANTMAKEMACRO "Kann Gruppe nicht erzeugen."
#define TXT_CANTOPENMACRO "Kann Gruppe '%s' nicht îffnen."
#define TXT_CANTSAVEMACRO "Gruppe '%s' NICHT abgespeichert."
#define TXT_MACROSAVED "Gruppe '%s'\nabgespeichert als Datei '%s'"
#define TXT_SAVEMACRO "Gruppe speichern."
#define TXT_LOADMACRO "Gruppe laden."
#define TXT_NOINSERTMODE "Kann im Modus %s nichts einfÅgen."
#define TXT_NODELETEMODE "Kann im Modus %s nichts lîschen."
#define TXT_NOSDELETEMODE "Kein spezielles Lîschen beim éndern von %s"
#define TXT_GETSCALE "Faktor"
#define TXT_SCALERANGE "Faktor %f ist zu gro·/klein\n"\
 "Es ist nur 0.1 .. 10.0 erlaubt."
#define TXT_INSTOOMANYPTS "WARNUNG:\nDer Level hat jetzt mehr Punkte"\
 "als Descent verwalten kann."
#define TXT_NONEIGHBOUR "Kein Nachbarsegment an der Seite %d von Segment %d."
#define TXT_DOORINTHEWAY \
 "Kann Verbindung an Seite %d von Segment %d nicht lîschen.\n"\
 "Die Wand mu· vorher entfernt werden."
#define TXT_POINTINSERTED \
 "Punkt an Ecke %d von Segment %d, Seite %d eingefÅgt."
#define TXT_TAGGEDPOINTSINSERTED "Markierte Punkte eingefÅgt."
#define TXT_INSTOOMANYTHINGS "WARNUNG:\nWenn das Ding eingefÅgt wird,\n"\
 "wird die Anzahl der Dinge, die Descent verwalten kann, Åberschritten\n"\
 "Egal?"
#define TXT_INSTOOMANYDOORS "WARNUNG:\nDurch EinfÅgen dieser Wand wird die"\
 " maximale Anzahl von WÑnden,\ndie Descent verwalten kann, Åberschritten."
#define TXT_INSTOOMANYRCENTERS "WARNUNG:\nDurch EinfÅgen dieses Robotprod."\
 " wird die maximale Anzahl von Prod.,\ndie Descent verwalten kann,"\
 " Åberschritten."
#define TXT_WALLINTHEWAY "Kann hier keine Wand einfÅgen.\n"\
 "Die Seite mu· vorher verbunden werden."
#define TXT_DOORALREADYTHERE "Kann hier keine Wand einfÅgen.\n"\
 "Da ist schon eine ;-)."
#define TXT_CANTKILLLASTCUBE "Kann nicht das letzte Segment lîschen."
#define TXT_CUBEALREADYCONN "Kann Seite %d von Segment %d nicht verbinden.\n"\
 "Die Seite ist schon verbunden."  
#define TXT_CANTKILLLASTTHING "Kann das letzte Ding nicht lîschen\n"\
 "(das sollte eigentlich jetzt ein Startpunkt sein)."
#define TXT_SDELTAGONEWALL "Es mu· genau eine Seite markiert sein.\n"\
 "Diese Seite und die aktuelle Seite werden dann verbunden."
#define TXT_KILLALL \
 "Wirklich alle markierten %d Segmente, %d WÑnde and %d Dinge lîschen?"
#define TXT_NOCONNCUBEFOUND \
 "Kann keine 'nahe' Seite fÅr Segment %d, Seite %d finden."
#define TXT_MACROITSELF \
 "Ein Makro kann nicht in sich selbst eingefÅgt werden."
#define TXT_REALLYDELONE \
 "Wirklich das aktuelle Objekt lîschen?"
#define TXT_REALLYDELTAGGED "Wirklich die %d markierten Objekte lîschen?"
#define TXT_CANTINSERTTHING "Kann Ding nicht einfÅgen!???"
/* do_opts.c */
#define TXT_DOUBLEADJLIGHT \
 "Es wird schon eine andere Lichtquelle (cube %d, side %d) bearbeitet."
#define TXT_CANCELADJLIGHT "Die Lichteffekte wirklich nicht abspeichern?"
#define TXT_ADJUSTLIGHT "Licht justieren"
#define TXT_LIGHTEFFECTS "ausgesendetes Licht"
#define TXT_EMPTYWALLNOTLOCK "Kann eine Seite ohne Mauer nicht sperren."
#define TXT_LOCKEDTAGGEDSIDES "%d Seiten gesperrt."
#define TXT_UNLOCKEDTAGGEDSIDES "%d Seiten entsperrt ;-)."
#define TXT_LOCKED "Gesperrt"
#define TXT_DOORNOTDEFWALL "Kann nicht eine Wand als Vorbild nehmen."
#define TXT_EMPTYNOTDEFWALL \
 "Kann keine Seite ohne Mauer als Vorbild fÅr Seiten nehmen."
#define TXT_TAG "Mark./Entmark."
#define TXT_INSERT "EinfÅgen"
#define TXT_DELETE "Lîschen"
#define TXT_NONE "Nichts"
#define TXT_MACRO "EinfÅgen:"
#define TXT_DEFAULT "Vorbild"
/* askcfg.c */
#define TXT_CANTWRITECFG "Kann cfg-Datei %s nicht lesen?!\n"
#define TXT_CFGFILENOTWRITTEN "Abbruch. CFG-file nicht geschrieben.\n"\
 "Wird Devil mit \"devil /config\" gestartet,\n"\
 "kann es neu konfiguriert werden. Bitte eine Taste drÅcken.\n"
#define TXT_CFGFILEWRITTEN \
 "OK. Devil kann jetzt mit \"devil\" gestartet werden.\n"\
 "Wird Devil mit \"devil /config\" gestartet,\n"\
 "kann es neu konfiguriert werden. Bitte eine Taste drÅcken.\n"
/* config.c */
#define TXT_CANTREADCTDEF \
 "Kann 'default' EintrÑge in Austauschtabelle nicht lesen."
#define TXT_CANTREADCTTXT \
 "Kann Texture Eintrag %d in Austauschtabelle nicht lesen."
#define TXT_CANTREADCTANIM \
 "Kann Animations Eintrag %d in Austauschtabelle nicht lesen."
#define TXT_CANTOPENCTFILE \
 "Kann Datei %s fÅr Austauschtabelle nicht îffnen.\n"
#define TXT_CANTFINDCTMARKER \
 "Kann Marker '%s' in Datei fÅr Austauschtabelle nicht finden.\n"
#define TXT_READCONVTABLEFILE "Lese Datei %s fÅr Austauschtabelle.\n"
#define TXT_CANTREADCFG "Kann CFG-Datei %s nicht lesen.\n"
#define TXT_CFGWRONGVERSION "CFG-Datei %s ist von einer alten Version.\n"
#define TXT_CANTWRITESTATUSFILE "Kann Status-Datei '%s' nicht schreiben."
#define TXT_CANTREADSTATUSFILE "Kann Status-Datei '%s' nicht lesen.\n"\
 "Das ist richtig so, wenn Devil\nzum ersten Mal gestartet worden ist."
#define TXT_READKEYS "Lese Tastenbelegung...\n"
#define TXT_ILLEGALDESCENTVERSION "UngÅltige Descent Version in cfg-Datei.\n"
#define TXT_NOEDITABLEDVER "Die Descent Version (%s) geht nicht mit Devil.\n"
#define TXT_CANTOPENVERINIFILE \
 "Kann die ini-Datei fÅr diese Descent Version (%s) nicht laden.\n"
#define TXT_READINIFILEFORDV "Lese ini-Datei fÅr Descent Version %s.\n"
#define TXT_MUSTUPDATECFG \
 "Diese cfg-Datei mu· neu geschrieben werden.\n"\
 "Anzahl der Descent Versionen ist falsch.\n"
#define TXT_CANTFINDPALETTE "Kann Farbpalette %s nicht finden.\n"\
 "Benutze voreingestellte Farbpalette %s."
#define TXT_CANTFINDHAMFILE "Kann HAM-Datei %s nicht finden.\n"
#define TXT_CANTREADHAMFILE "Kann HAM-Datei %s nicht lesen.\n"
#define TXT_NOHAMFILE "Datei %s ist keine gÅltige HAM-Datei.\n"
#define TXT_WRONGHAMVERSION "HAM-Datei %s hat die falsche Versionsnummer:\n"\
 "%d statt %d."
#define TXT_CANTSAVEPLAYMSN "Kann msn-Datei %s nicht speichern." 
#define TXT_CANTSAVEPLAYHOG "Kann hog-Datei %s nicht speichern."
/* options.c */
#define TXT_CANTUSETAGFILTER \
 "Kann Markierungs-Filter nicht mit diesen Daten benutzen."
#define TXT_TAGFILTERTAGGED \
 "%d Objekte markiert. %d Objekte waren schon markiert."
#define TXT_TAGFILTERUNTAGGED "Bei %d Objekten Markierung gelîscht.\n"\
 "%d Objekte waren bereits nicht markiert."
#define TXT_BMFITTEDTOSIDE "Umri· der Seite an Seite angepa·t."
#define TXT_BMFITTEDTOBM "Umri· der Seite an Texture angepa·t."
#define TXT_FITBMTOSIDE "Umri· der Seite an Seite anpassen?"
#define TXT_FITBMTOBM "Umri· der Seite an Texture anpassen?"
#define TXT_FITBITMAP "Umri·"
#define TXT_NOTHING "Nichts"
#define TXT_UNKNOWN "Unbekannt (%lu)"
#define TXT_CHANGEDTAGGED "%d Objekte geÑndert."
/* userio.c */
#define TXT_OK "OK"
#define TXT_CANCEL "Abbrechen"
#define TXT_ILLEGALPATH "Pfad %s ist ungÅltig."
#define TXT_CANTFINDPATH "Kann Pfad %s nicht finden."
#define TXT_OVERWRITEFILE "Datei '%s' vorhanden.\nöberschreiben?"
#define TXT_CANTOPENHOGFILE "Kann Hog-Datei %s nicht îffnen."
#define TXT_NOHOGSIGNATURE "Dies ist keine korrekte Hog-Datei."
#define TXT_BUT_SAVEHOG "Hog sichern"
#define TXT_BUT_OPENHOG "Hog îffnen"
#define TXT_BUT_EXTRACT "Level kopieren"
#define TXT_BUT_ANARCHY "Anarchie level"
#define TXT_BUT_MISSIONNAME "Missionsname:"
#define TXT_BUT_CLOSE "Ende"
#define TXT_LEVELSEXTRACTED "Level aus Hog-Datei %s kopiert."
#define TXT_CANTFINDLEVINHOG "Kann Level %s nicht\nin Hog-Datei %s finden."
#define TXT_CANTREADLEVFROMHOG "Kann Level %s nicht\naus Hog-Datei %s lesen."
#define TXT_CANTWRITEHOGLEV "Kann Level %s nicht in Datei %s schreiben."
#define TXT_CANTWRITESIGNATURE "Kann Kennung nicht in\n"\
 "Hog-Datei %s schreiben."
#define TXT_NOHOGLEVELS "Keine Levels zum Schreiben in die Hog-Datei."
#define TXT_CANTREADLEVEL "Kann Level %s nicht lesen."
#define TXT_CANTOPENMSNFILE "Kann Msn-Datei %s nicht finden."
#define TXT_CANTOVERWRITEORIGHOG "Die original Descent Hog-Datei darf"\
 "nicht Åberschrieben werden."
#define TXT_NOMISSIONNAME "Die Mission mu· einen Namen haben."
#define TXT_DOUBLESECRETLEVEL "FÅr Level %d: %s existiert bereits\n"\
 "ein Secret Level: %s"
#define TXT_CHANGEDSECRETLEV "Secret Level %s ist jetzt ein normaler Level."
/* do_mod.c */
#define TXT_CUSTOM "Polynom"
#define TXT_STRAIGHT "Gerade"
#define TXT_CIRCLE "Ellipse"
#define TXT_STDFORM "Typ:"
#define TXT_CORRCUBEDEPTH "WÅrfelgrî·e:"
#define TXT_CORRWEIRD "Das geht leider nicht. Der Korridor wÑre verdreht."
#define TXT_CORRWIN "Hexer: "
#define TXT_CORRSETEND "Setze Ende"
#define TXT_NOCUBETAGGED "Kein WÅrfel markiert."
#define TXT_NOSHRINKMODE "Kein Vergrî·ern/Verkleinern im Modus %s."
#define TXT_NOTHINGTAGGED \
 "Nichts gemacht, da nichts markiert war vom Typ %s."
#define TXT_EXITNONEIGHBOUR "Die Seite mit dem Ausgang/Verbindungsseite\n"\
 "mu· eine Seite ohne Nachbarn sein."
#define TXT_NOTXTCALC "Kein neues Berechnen der Umrisse der Seiten beim"\
 "éndern von %s."  
#define TXT_NOLIGHTSHADING "Lichtabstufung nur im Segmentmodus."
#define TXT_PATHTOOSHORT "Pfad fÅr Lichtabstufung ist nur ein Segment."
#define TXT_FINISHCORRIDOR "Ist der Korridor fertig?"
#define TXT_DELETECORRIDOR "Korridor loeschen?"
/* calctxt.c */
#define TXT_AMBIGUOUSPATH "Pfad ist nicht eindeutig.\n"\
 "Von Segment %d zu Segment %d oder zu Segment %d?"
#define TXT_CANTFINDARRCUBE "Kann Segment nicht finden!?"
/* do_tag.c */
#define TXT_MODEUSEPNTTAG "Nur im Eckenmode."
#define TXT_MODEUSECUBETAG "Nur im Seiten-, Punkt- oder Eckenmodus."
#define TXT_MODEUSESIDETAG "Nur im Ecken- oder Punktmodus."
#define TXT_FLATTAGGED "%d Seiten markiert."
#define TXT_NOSPECIALTAGMODE "Kein Spezial-Markieren definiert im Modus %s."
/* initio.c */
#define TXT_CANTOPENPIGFILE "Kann Descent-Bitmap-Datei %s nicht finden.\n"
#define TXT_UNZIPWITHD \
 "Das Devil-Zip-File mu· mit '-d' im Descent Verzeichnis entpackt werden.\n"
#define TXT_CANTOPENINI "Kann die Initdatei %s îffnen.\n"
#define TXT_READINI "Lese Grunddaten...\n"
/* do_light.c */
#define TXT_LSFILENAME "Lightquellen Datei: %s"
#define TXT_SETLSFILE "Lightquellen Datei"
#define TXT_CANTOPENLSFILE "Kann Datei %s fÅr Lichtquellen nicht îffnen."
#define TXT_NOTALSFILE "Datei %s ist keine Lichtquellen Datei.\n"\
 "Kann Marker 'LightSources' nicht finden."
#define TXT_CANTCALCLIGHT "Kann Seite %d,%d nicht beleuchten.\n"\
 "Umri· der Seite ist nicht korrekt."
#define TXT_CALCLIGHT "Berechne ausgestrahltes Licht von WÅrfel %d.\n"\
 "Berechnet: %6.2f%% Zeit: %.2f s\nDrÅcken Sie ESC um abzubrechen."
#define TXT_ENDCALCLIGHT "Markierte WÅrfel beleuchtet.\n"\
 "Benîtigte Zeit: %.2f s"
#define TXT_ENDSETCORNERLIGHT \
 "Licht der Ecken der markierten WÅrfel gesetzt.\nBenîtigte Zeit: %.2f s"
#define TXT_ENDSETINNERCUBELIGHT \
 "Inneres Licht der markierten WÅrfel gesetzt."
#define TXT_SMOOTHINGLIGHT "Verschmiere Licht: %d %%" 
/* opt_txt.c */
#define TXT_TLW_ALL "Alle"
#define TXT_TLW_TYPE "Seiten-Typ"
#define TXT_TLW_USER "Liste"
#define TXT_TLW_LEVEL "Lev."
#define TXT_TLW_CLEAR "Neu"
#define TXT_TLW_LOAD "Lad."
#define TXT_TLW_SAVE "Sich."
#define TXT_TLW_UP "/\\"
#define TXT_TLW_DOWN "\\/"
#define TXT_TLW_MODE "Modus:"
#define TXT_TLW_DISPLAY "Anzeigen:"
#define TXT_TLW_MARK "Mark.:"
#define TXT_TLW_LISTMODE "Liste"
#define TXT_TLW_TXTMODE "Txts"
#define TXT_TLW_THINGTEXTURE "Ding Texture"
#define TXT_TLW_SAVEALLTXTS \
 "Wirklich alle Textures als eine Texture-Liste abspeichern?"
#define TXT_TLW_TXTLISTEMPTY "Die Texture Liste ist leer."
#define TXT_TLW_TITLESAVE "Texture-Liste speichern."
#define TXT_TLW_TITLELOAD "Texture-Liste laden."
#define TXT_TLW_TXTLISTNOTSAVED "Texture-Liste NICHT gespeichert."
#define TXT_TLW_CANTSAVETL "Kann Texture-Liste %s nicht speichern."
#define TXT_TLW_CANTLOADTL "Kann Texture-Liste %s nicht laden."
#define TXT_TLW_TLSAVED "Texture-Liste als %s gespeichert."
#define TXT_FB_CLOSEWIN "énderungen verwerfen?"
#define TXT_FB_STDSHAPE "Standard Umri· Åbernehmen?"
#define TXT_FB_RECALCUV "UV-Koordinaten neu berechnen?"
#define TXT_FB_RESETUV "UV-Koordinaten von aktueller Seite Åbernehmen?"
#define TXT_FB_TITLE "Texture anpassen"
#define TXT_FB_SAVE "énderungen schreiben?"
/* readtxt.c */
#define TXT_POGINVALIDINDEX \
 "UngÅltiger Index in Pog-Datei: %d fÅr %d. Texture"
#define TXT_POGINVALIDSIZE "Texture %d in Pog-Datei hat Grî·e %d*%d != 64*64."
#define TXT_POGFILENOTCOMPLETE \
 "Pog-Datei nicht vollstÑndig: Abgebrochen bei Textureeintrag %d."
#define TXT_READINGPOGFILE "Lese zusÑtzliche Textures..."
#define TXT_LOOKINGFORPOGFILE "Suche Pog-Datei %s..."
#define TXT_POGERRORINHEADER "Kann Kopf von Pog-Datei nicht lesen."
/* plottxt.h */
#define TXT_FRAMERATETOOSMALL \
 "Warnung: Dieser Computer braucht %g s um den Level zu malen.\n"\
 "Eine der dargestellten Lichtquellen (WÅrfel %d Seite %d) Ñndert\n"\
 "ihr Licht alle %g s. Das hat zur Folge, da· das Flackern dieser\n"\
 "Lichtquelle nicht richtig dargestellt wird. Ich schlage deshalb vor,\n"\
 "die Zeitskala um den Faktor %g zu dehnen, um das Problem zu vermeiden.\n"\
 "Soll ich die Zeitskala neu setzen?"
