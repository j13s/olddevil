/* devil.c */
#define TXT_NOCFGFILE "Last status no restored due to command line parameter."
#define TXT_CMDSTARTNEW "don't restore last status."
#define TXT_CMDDONTSHOWTITLE "don't show the title screen"
#define TXT_CMDCONFIG "reconfigure Devil"
#define TXT_CMDUNKNOWNPARAM "Unknown parameter [%s]. Allowed parameters:\n"
/* initio.c */
#define TXT_TOP "top"
#define TXT_UP "up"
#define TXT_DOWN "down"
#define TXT_LIST "list"
#define TXT_TXT1 "Txt1"
#define TXT_TXT2 "Txt2"
/* tools.c */
#define TXT_TOOMANYSTARTS "WARNING: Too many startpoints of this type."
#define TXT_READBMUNKNOWNDIR "Unknown direction in readbitmap: %d."
#define TXT_NORMALIZE "Normalize: Vector has got length zero."
#define TXT_ONLYONESECRETSTART \
 "Only one secret start (%d) per level allowed.\n"\
 "Descent will use the start with the lower number."
#define TXT_NOSECRETDOORFORSTART "No secret exit for secret start."
/* readlvl.c */
#define TXT_LEAVEADJLIGHTMODE "Please turn out adjust-light-mode for\n"\
 "cube %d side %d before you save the level."
#define TXT_WRONGBLKHEAD "File %s has not the correct header:\n"\
 "\"%s\"!=\"DMB_BLOCK_FILE\""
#define TXT_NOCUBEFORFL "Can't find cube %d for flickering light %d."
#define TXT_CORRINWORK "The wizard is active in this level.\n"\
 "Can't close the level, you must first deactivate the wizard."
#define TXT_CONVTEXTURES \
 "Replacing Descent 1 textures with Descent 2 textures."
#define TXT_NOTALLTXTSCONV \
 "Warning: Some of the textures used in this level\n"\
 "don't exist in Descent 2 and in the conversion table.\n"\
 "They're replaced by texture 1 %s (%d)\n"\
 "or texture 2 %s (%d).\n"
#define TXT_DISCARDCHANGES "Level/Group '%.30s'\nnot saved. Discard changes?"
#define TXT_TOOMANYEDOORS \
 "Too many doors (%d) to be opened at end (max.: 10).\nIgnore rest?" 
#define TXT_TOOMANYCUBES \
 "WARNING:\nMore cubes (%d) than Descent can handle (max.: %d).\nIgnore?"
#define TXT_TOOMANYPTS \
 "WARNING:\nMore points (%d) than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_TOOMANYTHINGS \
 "WARNING:\nMore things (%d) than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_TOOMANYWALLS \
 "WARNING:\nMore walls (%d) than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_TOOMANYRCENTERS \
 "WARNING:\nMore producers (%d) than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_TOOMANYSWITCHES \
 "WARNING:\nMore switches (%d) than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_TOOMANYTURNOFFS \
 "WARNING:\nMore lightsources to turn/shoot off (%d)\n"\
 "than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_TOOMANYCHANGEDLIGHTS \
 "WARNING:\nMore lights to change when lights are turned off (%d)\n"\
 "than Descent can handle. (max.: %d)\nIgnore?"
#define TXT_THINGOUTOFBOUNDS "WARNING:\nThing %d may be out of bounds.\n"\
 "Ignore?"
#define TXT_ILLNOFORSTART \
 "WARNING:\nIllegal number %d in starting place %d.\nIgnore?"
#define TXT_TOOMANYCOOPSTARTS \
 "WARNING:\nToo many coop starts (%d is the fifth).\nIgnore?"
#define TXT_SAMENOFORSTART \
 "WARNING:\nTwo no. %d starting places:\nthing %d and thing %d.\nIgnore?"
#define TXT_TWOKEYSOFSAMECOLOR \
 "WARNING:\nTwo keys of the same color:\nthing %d and thing %d."
#define TXT_TWOREACTORS \
 "WARNING: Double reactor/flag: thing %d and %d"
#define TXT_NOSECRETSTART \
 "WARNING:\nNo secret start for secret exit (wall %d).\nIgnore?"
#define TXT_NOSECRETEXIT \
 "WARNING:\nNo secret exit but a secret start (thing %d).\nIgnore?"
#define TXT_NOSTART "WARNING:\nNo starting place.\nIgnore?"
#define TXT_NEWILLUM "WARNING:\nThis level must be illuminated again\n"\
 "if you want to shoot out the new lights.\n"\
 "Do this now?"
#define TXT_CLICKEDTHING "Clicked thing %d"
#define TXT_CLICKEDDOOR "Clicked wall %d"
#define TXT_CLICKEDPNT "Clicked point %d"
#define TXT_CLICKEDWALL "Clicked cube %d, side %d"
#define TXT_CLICKEDEDGE "Clicked cube %d, side %d, edge %d"
#define TXT_CLICKEDCUBE "Clicked cube %d"
#define TXT_NOSIDE "No side at cube %d, side %d but there should be one."
#define TXT_NOSWITCHDATA "No switchdata for wall %d."
#define TXT_WRONGNUMBEROFEDOORS "Number of open-at-end doors is wrong."
#define TXT_CANTREADLVL "Level %s cannot be read complete. File corrupted."
#define TXT_READINGPTS "Reading %ld pts..."
#define TXT_INITCUBE "Init cube %d/%d"
#define TXT_INITDOOR "Init door %d/%d"
#define TXT_INITTURNOFF "Init turnoff-light %d/%d"
#define TXT_ERROPENATEND "Can't find cube %d for open-at-end-door."
#define TXT_ERROAENODOOR "No door to open-at-end in cube %d,%d."
#define TXT_LEVELREAD "Level %s\nfrom file %s is read."
/* grfx.h */
#define TXT_DEFAULTPOGFILE "Use the default textures?"
#define TXT_POGFILE "Change pogfile"
#define TXT_INFO "Info"
#define TXT_MOVEYOU "Move you"
#define TXT_MOVEOBJ "Move object"
#define TXT_MOVECURRENT "Select cube"
#define TXT_MOVETXT "Move texture"
#define TXT_MODIFYWHAT "Modify what:"
#define TXT_MOVEMODE "Movemode:"
#define TXT_LEVELPIGFILE "Set of Txts.:"
#define TXT_BUTTONS "Buttons"
#define TXT_CANTINITWINS "Can't init window system.\n"\
 "Try a different graphics driver or try to load a VESA-BIOS.\n"
#define TXT_NOMENUFILE "No menu ini-file \"%s\".\n"
/* do_side.c */
#define TXT_CANTMAKETHING "Can't make thing."
#define TXT_TAGGEDCUBENOPROD "Tagged cube %d is not a producer."
#define TXT_TOOMUCHCUBESTAGGED "Too much cubes tagged (maximum number 10)."
#define TXT_DOOROPENITSELF "Door can't open itself."
#define TXT_TAGGEDWALLNODOOR "Tagged wall %d is not a door."
#define TXT_TAGGEDWALLNOSWITCHILLWALL \
 "Tagged wall %d is not a 'only switch', 'illusion' or 'wall'."
#define TXT_TOOMUCHDOORSTAGGED "Too much doors tagged (maximum number 10)."
#define TXT_LEVEL "Level"
#define TXT_ILLUM_BRIGHTNESS "Only values between 0.01 and 100 allowed."
#define TXT_NOSWITCHABLEWALL "You can only switch walls with two sides."
#define TXT_WALLTYPENEEDSNB "This walltype needs a connected side."
/* do_event.c */
#define TXT_CLOSETXTWIN "Please close the texture window\n"\
 "before you save/load your window positions."
#define TXT_SAVEWINPOS "Save window pos"
#define TXT_LOADWINPOS "Load window pos"
#define TXT_CANTSAVEWINPOS "Can't save window positions to file %s"
#define TXT_CANTREADWINPOS "Can't read window positions from file %s"
#define TXT_WINPOSSAVED "Saved window positions to file %s"
#define TXT_WINPOSREAD "Read window positions from file %s"
#define TXT_CANTOPENLVL "Can't open level '%s'"
#define TXT_CANTSAVELVL "Level '%s' NOT saved"
#define TXT_LEVELSAVED "Level '%s'\nsaved as file '%s'"
#define TXT_NOLEVEL "No current level."
#define TXT_NOFUNCTION "Function no. %d not yet implemented."
#define TXT_LVLNAME "Levelname:"
#define TXT_LOADLEVEL "Load level"
#define TXT_SAVELEVEL "Save level"
#define TXT_NODRAWING "Now drawing %s%s%s%s%s"
#define TXT_DWCUBES "cubes "
#define TXT_DWWALLS "walls "
#define TXT_DWTHINGS "things "
#define TXT_DWLINES "lines "
#define TXT_DWXTAGGED "X-tagged "
#define TXT_LEVELNAME "Levelname"
#define TXT_CALCMAP "Calculating map....."
/* do_move.c */
#define TXT_NOCURRCUBE "No current cube."
#define TXT_NOCURRPNT "No current point."
#define TXT_NOCURRTHING "No current thing."
#define TXT_NOCURRWALL "No current wall."
#define TXT_SIDECUBENONEIGHBOUR "On this side is no neighbour."
#define TXT_GRIDROT "Warning: If you turn the cube with the grid on\n"\
 "it will be slightly distorted."
#define TXT_NOMOVINGMODE "You can't move anything in this mode."
#define TXT_NOTURNOBJECT "You can't turn this object."
#define TXT_POSSAVED "Position %d saved."
#define TXT_NOCORR "No corridor is active."
#define TXT_CANTSEETXTWALL "You can't see this texture,\n"\
 "so why do you want to change it?"
/* macros.c */
#define TXT_DEFAULTMACRONAME "Group %d"
#define TXT_NOCURRSIDE "No current side."
#define TXT_CUBETAGGEDON "Already a cube tagged on."
#define TXT_TOOMANYITEMS "WARNING:\nMore cubes, pts, things or walls\n"\
 "than Descent can handle.\nIgnore?"
#define TXT_CANTOPENMACROFILE "Can't open group file %s."
#define TXT_NOMACROHEAD "Can't read group head for group %s."
#define TXT_NOMACROFILE "%s is no old group."
#define TXT_ERRREADINGMACRO "Error reading group %s."
#define TXT_NOCONNECTSIDE "No connection side is defined in group %s."
/* insert.c */
#define TXT_CONNCUBEITSELF "Can't connect cube %d with itself."
#define TXT_CONNWALLINWAY "Can't connect side %d,%d with side %d,%d.\n"\
 "You must remove the switch."
#define TXT_CONNCUBETOOFARAWAY \
 "Can't connect cube %d side %d with cube %d side %d.\n"\
 "The distance between the sides is too large."
#define TXT_CONNAMBIGUOUS \
 "Connection between cube %d side %d and cube %d side %d is ambiguous."
#define TXT_CUBESCONNECTED "Connected cube %d,%d with cube %d,%d"
#define TXT_TPLINETOOLONG "Line %d-%d in wall %d in cube %d too short/long."
#define TXT_TPCUBENOTCONVEX "Cube %d at point %d not convex"
#define TXT_TPWEIRDWALL "Wall %d of cube %d is twisted."
#define TXT_CANTKILLTARGET "Can't kill target %d"
#define TXT_CLEANNODOORFORSWITCH "Can't find corresponding door %d to switch."
#define TXT_NOCUBEFORDOOR "Can't find cube %lu for door."
#define TXT_WRONGWALLINDOOR "Wrong wall number %lu (cube=%lu) in door."
#define TXT_NOWALLFORDOOR "Door: Wall %lu,%lu does not exist."
#define TXT_NONCFORDOOR "Warning: Door cube has no neighbour: %lu %lu"
#define TXT_NOSWITCHFORDOOR "Can't find switch %d for door %d."
#define TXT_NOCUBEFORSWITCH "Can't find cube %hd for switch."
#define TXT_WRONGWALLFORSWITCH "Illegal wall number %d for switch %d."
#define TXT_NODOORFORSWITCH "No door on cube %d wall %d for switch %d."
#define TXT_NOCUBEPNT "Can't find cubepoint %d.\nClose level?"
#define TXT_NONCFORCUBE "Can't find neighbour cube %d for cube %d.\n"\
 "Close level?"
#define TXT_NODOORFORCUBE "Can't find door %d for cube %d.\nClose level?"
#define TXT_NOPRODFORCUBE "Can't find producer %d for cube %d.\nClose level?"
#define TXT_NOMEMFOROLDCOORDS "No mem for saving old coords."
#define TXT_TO_CANTFINDCUBE "Can't find cube %d for turnoff-light %d."
#define TXT_TO_CANTFINDCL "Can't find change light %d for turnoff-light %d."
/* do_ins.c */
#define TXT_SPLITCANTCONNECT \
 "Can't restore connection of side %d with cube %d,%d??"
#define TXT_SPLITCANTCONNECT2 "Can't connect side %d,%d??"
#define TXT_SPLITDOOR "Can't split cube %d.\n"\
 "You must remove the wall on side %d."
#define TXT_MAKEROOM "All sides in the tagged cubes are connected.\n"\
 "The unconnected sides are tagged."
#define TXT_CANTINSCUBE "Can't insert a cube here.\n"\
 "You must remove the switch."
#define TXT_STDLEVELNAME "New level"
#define TXT_MAKEMACROCONNCUBE "You can't use a side connected to a\n"\
 "tagged cube as current side when you make a group."
#define TXT_NOMACRO "No current group."
#define TXT_MACROLOADED "Group '%s' loaded."
#define TXT_MACROMADE "Group made."
#define TXT_CANTMAKEMACRO "Can't make group."
#define TXT_CANTOPENMACRO "Can't open level '%s'"
#define TXT_CANTSAVEMACRO "Level '%s' NOT saved"
#define TXT_MACROSAVED "Level '%s'\nsaved as file '%s'"
#define TXT_SAVEMACRO "Save group"
#define TXT_LOADMACRO "Load group"
#define TXT_NOINSERTMODE "Can't insert in mode %s"
#define TXT_NODELETEMODE "Can't delete in mode %s"
#define TXT_NOSDELETEMODE "No special delete in mode %s"
#define TXT_GETSCALE "Scale"
#define TXT_SCALERANGE "Scale %f is too large/small\n"\
 "Only 0.1 .. 10.0 is allowed"
#define TXT_INSTOOMANYPTS "WARNING:\nThis level has now more points "\
 "than Descent can handle."
#define TXT_NONEIGHBOUR "No neighbour cube at side %d of cube %d."
#define TXT_DOORINTHEWAY "Can't delete connection at side %d of cube %d.\n"\
 "You must remove the wall."
#define TXT_POINTINSERTED "At corner %d of cube %d side %d point inserted."
#define TXT_TAGGEDPOINTSINSERTED "Tagged points inserted."
#define TXT_INSTOOMANYTHINGS "WARNING:\nIf you insert this thing,\n"\
 "the number of things Descent can handle\nwill be exceeded\nIgnore?"
#define TXT_INSTOOMANYDOORS "WARNING:\nThis level has now more walls "\
 "than Descent can handle."
#define TXT_INSTOOMANYRCENTERS "WARNING:\nThis level has now more producers "\
 "than Descent can handle."
#define TXT_WALLINTHEWAY "Can't insert a wall here.\n"\
 "You must connect this side first"
#define TXT_DOORALREADYTHERE "Can't insert a wall here.\n"\
 "There is already one."
#define TXT_CANTKILLLASTCUBE "Can't delete last cube."
#define TXT_CUBEALREADYCONN "Can't connect side %d of cube %d.\n"\
 "The side is already connected."
#define TXT_CANTKILLLASTTHING "Can't kill last thing (should be the start)."
#define TXT_SDELTAGONEWALL "You must tag exactly one side.\n"\
 "This side and the current side will be connected."
#define TXT_KILLALL \
 "Really kill all tagged %d cubes, %d doors and %d things?"
#define TXT_NOCONNCUBEFOUND "Can't find near side for cube %d side %d"
#define TXT_MACROITSELF "You can't insert the macro in itself."
#define TXT_REALLYDELONE \
 "Do you really want to delete the current object?"
#define TXT_REALLYDELTAGGED "Do you really want to delete the\n"\
 "%d tagged objects?"
#define TXT_CANTINSERTTHING "Can't insert thing???"
/* do_opts.c */
#define TXT_DOUBLEADJLIGHT "Another light (cube %d, side %d) is modified\n"\
 "in the moment."
#define TXT_CANCELADJLIGHT "Do you really don't want to save the lights?"
#define TXT_ADJUSTLIGHT "Adjust light"
#define TXT_LIGHTEFFECTS "Light from side"
#define TXT_DOORNOTDEFWALL "Can't use a wall as a default side."
#define TXT_EMPTYNOTDEFWALL "Can't use a empty side as a default side."
#define TXT_EMPTYWALLNOTLOCK "Can't lock a side without a texture."
#define TXT_LOCKEDTAGGEDSIDES "Locked %d sides."
#define TXT_UNLOCKEDTAGGEDSIDES "Unlocked %d sides."
#define TXT_TAG "Tag/Untag"
#define TXT_INSERT "Insert"
#define TXT_DELETE "Delete"
#define TXT_NONE "None"
#define TXT_MACRO "Group:"
#define TXT_DEFAULT "Default"
#define TXT_LOCKED "Locked"
/* askcfg.c */
#define TXT_CANTWRITECFG "Can't write cfg-file %s?!\n"
#define TXT_CFGFILENOTWRITTEN "Aborted. CFG-file not written.\n"\
 "If you want to reconfigure Devil, start it with \"devil /config\"\n"\
 "Press a key.\n"
#define TXT_CFGFILEWRITTEN "OK. Please start Devil with \"devil\".\n"\
 "If you want to reconfigure Devil, start it with \"devil /config\"\n"\
 "Press a key.\n"
/* config.c */
#define TXT_CANTREADCTDEF "Can't read default entries in conversion table."
#define TXT_CANTREADCTTXT "Can't read texture entry %d in conversion table."
#define TXT_CANTREADCTANIM \
 "Can't read animation entry %d in conversion table."
#define TXT_CANTOPENCTFILE "Can't open conversion table file %s.\n"
#define TXT_CANTFINDCTMARKER \
 "Can't find marker '%s' in conversion table file.\n"
#define TXT_READCONVTABLEFILE "Reading conversion table file %s.\n"
#define TXT_CANTREADCFG "Can't read cfg-file %s.\n"
#define TXT_CFGWRONGVERSION "CFG-File %s is from an old version.\n"
#define TXT_CANTWRITESTATUSFILE "Can't write status-file:\n%s"
#define TXT_CANTREADSTATUSFILE "Can't read status-file:\n%s\n"\
 "This is correct if you start\nDevil for the first time."
#define TXT_READKEYS "Reading keycodes...\n"
#define TXT_ILLEGALDESCENTVERSION "Illegal Descent version in cfg-file.\n"
#define TXT_NOEDITABLEDVER \
 "This Descent version (%s) won't work with Devil.\n"
#define TXT_CANTOPENVERINIFILE \
 "Can't open the ini-file for this Descent version (%s).\n"
#define TXT_READINIFILEFORDV "Reading ini-file for Descent version '%s'.\n"
#define TXT_MUSTUPDATECFG \
 "This cfg-file must be updated. Number of Descent versions is wrong.\n"
#define TXT_CANTFINDPALETTE "Can't find color palette %s.\n"\
 "Using default color palette %s."
#define TXT_CANTFINDHAMFILE "Can't find HAM-file %s.\n"
#define TXT_CANTREADHAMFILE "Can't read HAM-file %s.\n"
#define TXT_NOHAMFILE "File %s is not a valid HAM-file.\n"
#define TXT_WRONGHAMVERSION "HAM-file %s has the wrong version number:\n"\
 "%d instead of %d."
#define TXT_CANTSAVEPLAYMSN "Can't save msn-file %s." 
#define TXT_CANTSAVEPLAYHOG "Can't save hog-file %s."
/* options.c */
#define TXT_CANTUSETAGFILTER "Can't use tag filter with this data."
#define TXT_TAGFILTERTAGGED \
 "Tagged %d objects. %d objects were already tagged."
#define TXT_TAGFILTERUNTAGGED \
 "Untagged %d objects. %d objects were already untagged."
#define TXT_BMFITTEDTOSIDE "Shape of side fitted to side."
#define TXT_BMFITTEDTOBM "Shape of side fitted to texture."
#define TXT_FITBMTOSIDE "Fit uv-coords to side shape?"
#define TXT_FITBMTOBM "Fit shape of side to texture?"
#define TXT_FITBITMAP "Fit shape"
#define TXT_NOTHING "Nothing"
#define TXT_UNKNOWN "Unknown (%lu)"
#define TXT_CHANGEDTAGGED "Changed %d objects"
/* userio.c */
#define TXT_OK "OK"
#define TXT_CANCEL "Cancel"
#define TXT_ILLEGALPATH "Illegal path %s"
#define TXT_CANTFINDPATH "Can't find path %s"
#define TXT_OVERWRITEFILE "File '%s' exists.\nOverwrite?"
#define TXT_CANTOPENHOGFILE "Can't open hog-file %s."
#define TXT_NOHOGSIGNATURE "This is no valid hog-file."
#define TXT_BUT_SAVEHOG "Save hog-file"
#define TXT_BUT_OPENHOG "Open hog-file"
#define TXT_BUT_EXTRACT "Extract levels"
#define TXT_BUT_ANARCHY "Anarchy level"
#define TXT_BUT_MISSIONNAME "Mission Name:"
#define TXT_BUT_CLOSE "Close" 
#define TXT_LEVELSEXTRACTED "Extracted levels from hogfile %s"
#define TXT_CANTFINDLEVINHOG "Can't find level %s\nin hogfile %s"
#define TXT_CANTREADLEVFROMHOG "Can't read level %s\nfrom hogfile %s"
#define TXT_CANTWRITEHOGLEV "Can't write level %s to file %s"
#define TXT_CANTWRITESIGNATURE "Can't write signature\nto hogfile %s"
#define TXT_NOHOGLEVELS "No levels to write into the hogfile."
#define TXT_CANTREADLEVEL "Can't read level %s."
#define TXT_CANTOPENMSNFILE "Can't open mission file %s."
#define TXT_CANTOVERWRITEORIGHOG "You can't overwrite the\n"\
 "original Descent HOG-file."
#define TXT_NOMISSIONNAME "You must name the mission." 
#define TXT_DOUBLESECRETLEVEL "For level %d: %s already\n"\
 "exists a secret level: %s"
#define TXT_CHANGEDSECRETLEV "Changed secret level %s to normal level."
/* do_mod.c */
#define TXT_CUSTOM "Polynom"
#define TXT_STRAIGHT "Straight"
#define TXT_CIRCLE "Ellipse"
#define TXT_STDFORM "Type:"
#define TXT_CORRCUBEDEPTH "Cubesize:"
#define TXT_CORRWEIRD "This is not possible. The corridor would be weird."
#define TXT_CORRWIN "Wizard: "
#define TXT_CORRSETEND "Connect end"
#define TXT_NOCUBETAGGED "No cube tagged."
#define TXT_NOSHRINKMODE "No enlarge/shrink in mode %s."
#define TXT_NOTHINGTAGGED \
 "Nothing to do because nothing is tagged in mode %s."
#define TXT_EXITNONEIGHBOUR \
 "You must set the exitside/connectside on a side without a neighbour."
#define TXT_NOTXTCALC "No 'shape-of-side' calculating in mode %s."  
#define TXT_NOLIGHTSHADING "Light shading only in cube mode."
#define TXT_PATHTOOSHORT "Path is only one cube."
#define TXT_FINISHCORRIDOR "Have you finished the corridor?"
#define TXT_DELETECORRIDOR "Delete the corridor?"
/* calctxt.c */
#define TXT_AMBIGUOUSPATH "Path is ambiguous.\n"\
 "From cube %d to cube %d or cube %d?"
#define TXT_CANTFINDARRCUBE "Can't find arrange cube!?"
/* do_tag.c */
#define TXT_MODEUSECUBETAG "Only in side-,point- or edgemode."
#define TXT_MODEUSESIDETAG "Only in point- or edgemode."
#define TXT_MODEUSEPNTTAG "Only in edgemode."
#define TXT_FLATTAGGED "%d sides tagged."
/* initio.c */
#define TXT_CANTFINDAPIGFILE "Can't find a .PIG-file in path %s.\n"
#define TXT_UNZIPWITHD \
 "You must unzip the Devil-Zip-File with '-d' in your Descent directory.\n"
#define TXT_CANTOPENINI "Can't open initfile %s!\n"
#define TXT_READINI "Reading initdata...\n"
/* do_light.c */
#define TXT_LSFILENAME "Lightsource file: %s"
#define TXT_SETLSFILE "Lightsource file"
#define TXT_CANTOPENLSFILE "Can't open lightsource file %s"
#define TXT_NOTALSFILE "File %s is not a valid lightsource file.\n"\
 "Can't find marker 'LightSources'."
#define TXT_CANTCALCLIGHT "Can't calculate light from wall %d,%d.\n"\
 "Shape of side is not valid."
#define TXT_CALCLIGHT "Calculating light effect of cube %d\n"\
 "Progress: %6.2f%% Time: %.2f sec\nUsing lightsource file:\n%s\n"\
 "Press ESC to abort."
#define TXT_ENDSETCORNERLIGHT \
 "Light of corners in tagged cubes set.\nElapsed time: %.2f sec"
#define TXT_ENDSETINNERCUBELIGHT "Inner light of tagged cubes set."
#define TXT_ENDCALCLIGHT "Tagged cubes illuminated.\nElapsed time: %.2f sec"
#define TXT_SMOOTHINGLIGHT "Smoothing light: %d %%"
/* opt_txt.c */
#define TXT_TLW_ALL "All"
#define TXT_TLW_TYPE "Side-type"
#define TXT_TLW_USER "User"
#define TXT_TLW_LEVEL "Level"
#define TXT_TLW_CLEAR "New"
#define TXT_TLW_LOAD "Load"
#define TXT_TLW_SAVE "Save"
#define TXT_TLW_UP "/\\"
#define TXT_TLW_DOWN "\\/"
#define TXT_TLW_MODE "Mode:"
#define TXT_TLW_DISPLAY "Display:"
#define TXT_TLW_MARK "Mark:"
#define TXT_TLW_LISTMODE "List"
#define TXT_TLW_TXTMODE "Txts"
#define TXT_TLW_THINGTEXTURE "Thing Texture"
#define TXT_TLW_SAVEALLTXTS \
 "Do you really want to save all textures as a texture list?"
#define TXT_TLW_TXTLISTEMPTY "The texture list is empty."
#define TXT_TLW_TITLESAVE "Save texture list"
#define TXT_TLW_TITLELOAD "Load texture list"
#define TXT_TLW_TXTLISTNOTSAVED "Texture list NOT saved."
#define TXT_TLW_CANTSAVETL "Can't save texture list %s."
#define TXT_TLW_CANTLOADTL "Can't read texture list %s."
#define TXT_TLW_TLSAVED "Texture list saved as %s."
#define TXT_FB_CLOSEWIN "Discard changes?"
#define TXT_FB_STDSHAPE "Make standard shape?"
#define TXT_FB_RECALCUV "Recalculate uv-coords?"
#define TXT_FB_RESETUV "Reset uv-coords with current side?"
#define TXT_FB_TITLE "Fit texture"
#define TXT_FB_SAVE "Save changes?"
/* readtxt.c */
#define TXT_POGINVALIDINDEX "Invalid index in pogfile: %d for %d. texture."
#define TXT_POGINVALIDSIZE "Texture %d in pogfile has size %d*%d != 64*64."
#define TXT_POGFILENOTCOMPLETE \
 "Pogfile not complete: Aborted at directory entry %d."
#define TXT_READINGPOGFILE "Reading additional textures..."
#define TXT_LOOKINGFORPOGFILE "Searching for pogfile %s..."
#define TXT_POGERRORINHEADER "Can't read header from pogfile."
/* plottxt.h */
#define TXT_FRAMERATETOOSMALL \
 "Warning: Your computer needs %g s to draw the level.\n"\
 "One of the displayed lightsources (cube %d side %d) changes\n"\
 "the light each %g s. Therefore the flickering of this lightsource\n"\
 "will not be displayed correctly. I suggest you to stretch the timescale\n"\
 "with the factor %g to avoid this problem. Should I do this now?"
