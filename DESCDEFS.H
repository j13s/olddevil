/* Differences between Descent versions */
enum descent { d1_10_sw,d1_10_reg,d1_14_reg,d2_10_sw,d2_10_reg,
 d2_11_reg,d2_12_reg,desc_number };
extern const char *extnames[desc_number];
extern const char *ininames[desc_number];
extern const char *vernames[desc_number];
#define NUM_PALETTES 7 /* 6 for Descent 2, 1 for Descent 1 */
/* what can be switched (for me) */
enum sdoortypes { sdtype_none,sdtype_door,sdtype_cube,sdtype_side };
/* maximum number for Descent objects: */
#define MAX_DESCENT_VERTICES (init.d_ver>=d2_10_reg ? 3600 : 2800)
#define MAX_DESCENT_CUBES (init.d_ver>=d2_10_reg ? 900 : 800)
#define MAX_DESCENT_WALLS (init.d_ver>=d2_10_reg ? 254 : 175)
#define MAX_DESCENT_OBJECTS 350
#define MAX_DESCENT_RCENTERS 20
#define MAX_DESCENT_SWITCHES 100
#define MAX_DESCENT_TURNOFFS 500
#define MAX_DESCENT_CHANGEDLIGHTS 10000
/* maximum number for all Descent versions for robot (robotsize) */
#define NUM_ROBOTS 66
#define NUM_REACTORS 7
enum cubetypes { cube_normal=0,cube_fuel=1,cube_reactor=3,cube_producer=4,
 cube_blueflag=5,cube_redflag=6 };
/* several standard textures etc. */
enum miscel { std_anim_blowdoor=6,std_switch_t2=414,D1_std_emptytxt=289,
 D2_std_emptytxt=302,std_blowdoorhp=0x64<<16 };
enum thingtype1 { tt1_robot=2,tt1_hostage=3,tt1_dmstart=4,tt1_mine=5,
 tt1_item=7,tt1_reactor=9,tt1_coopstart=14,
 /* for me: */tt1_secretstart=0xff };
/* only the needed IDs */
enum itemid { item_id_bluekey=4,item_id_redkey=5,item_id_yellowkey=6,
 item_id_blueflag=46,item_id_redflag=47 };
/* subtypes of things */
enum controltype { ct_none=0,ct_ai=1,ct_slew=5,ct_mine=9,ct_powerup=13,
 ct_control=16 };
enum movementtype { mt_none=0,mt_physics=1,mt_spinning=3 };
enum rendertype { rt_polyobj=1,rt_hostage=4,rt_powerup=5 };
enum doortype1 { door1_blow=1,door1_normal=2,door1_onlytxt=3,
 door1_onlyswitch=4,door1_shootthrough=5,door1_switchwithwall=6,
 door1_cloaked=7 };
enum doortype2 { door2_locked=8,door2_closeagain=0x10,door2_illusionoff=0x20,
 door2_robotswontpass=0x80 };
/* switchtypes */
enum switchtypes { switch_opendoor=0,switch_closedoor=1,switch_producer=2,
 switch_exit=3,switch_secretexit=4,switch_illusion_on=5,
 switch_illusion_off=6,switch_unlockdoor=7,switch_lockdoor=8,
 switch_openwall=9,switch_closewall=10,switch_wall_to_ill=11,
 switch_turnofflight=12,switch_turnonlight=13,
 /* for me: */switch_nothing=0xff };
enum switchflags { switchflag_nomsg=1,switchflag_once=2 };
