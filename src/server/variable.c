/* $Id$ */
/* File: variable.c */

/* Purpose: Angband variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define SERVER

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
cptr copyright[6] =
{
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"also Copyright (c) 1997 Keldon L. Jones",
	"",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};


/*
 * Hack -- Link the "version" into the executable
 */
byte version_major = SF_VERSION_MAJOR;
byte version_minor = SF_VERSION_MINOR;
byte version_patch = SF_VERSION_PATCH;
byte version_extra = SF_VERSION_EXTRA;


/*
 * Hack -- Savefile version
 */
byte sf_major;                  /* Savefile's "version_major" */
byte sf_minor;                  /* Savefile's "version_minor" */
byte sf_patch;                  /* Savefile's "version_patch" */
byte sf_extra;                  /* Savefile's "version_extra" */


/*
 * Hack -- Savefile information
 */
u32b sf_xtra;                   /* Operating system info */
u32b sf_when;                   /* Time when savefile created */
u16b sf_lives;                  /* Number of past "lives" with this file */
u16b sf_saves;                  /* Number of "saves" during this life */


/*
 * Hack -- Server Savefile version
 */
byte ssf_major;                  /* Savefile's "version_major" */
byte ssf_minor;                  /* Savefile's "version_minor" */
byte ssf_patch;                  /* Savefile's "version_patch" */
byte ssf_extra;                  /* Savefile's "version_extra" */


/*
 * Hack -- Run-time arguments
 */
bool server_generated;          /* The character exists */
bool server_dungeon;            /* The character has a dungeon */
//bool central_town_loaded = FALSE;
bool server_state_loaded;       /* The server state was loaded from a savefile */
bool server_saved;              /* The character was just saved to a savefile */

bool character_loaded;          /* The character was loaded from a savefile */

u32b seed_flavor;               /* Hack -- consistent object colors */
u32b seed_town;                 /* Hack -- consistent town layout */
u32b seed_wild_extra;		/* Hack -- random additional wilderness features (used in terrain_spot()) */

s16b command_new;               /* Command chaining from inven/equip view */

bool create_up_stair;           /* Auto-create "up stairs" */
bool create_down_stair;         /* Auto-create "down stairs" */

s16b num_repro;                 /* Current reproducer count */
s16b object_level;              /* Current object creation level */
s16b object_discount = 0;	/* for resulting items of a stolen acquirement scroll (!) */
s16b monster_level;             /* Current monster creation level */
s16b monster_level_min = 0;	/* Current monster creation minimum level, -1 = auto, 0 = none */

s32b turn, session_turn;	/* Current game turn; session_turn is the turn this server went online, ie start of current session */
s32b turn_overflow = 2000000000;/* Limit when to reset 'turn' to 1 to prevent overflow symptoms */

#ifdef ARCADE_SERVER
char tron_speed = 9;
char tron_dark = 0;
char tron_forget = 0;
worldpos arcpos[100];
#endif

s32b player_id;                 /* Current player ID */
u32b account_id;		/* Current account ID */

u16b panic_save = 0;            /* Track some special "conditions" */

s16b signal_count = 0;          /* Hack -- Count interupts */

s16b coin_type;                 /* Hack -- force coin type */

bool opening_chest;             /* Hack -- prevent chest generation */

bool scan_monsters;             /* Hack -- optimize multi-hued code, etc */
bool scan_objects;              /* Hack -- optimize multi-hued code, etc */

s32b o_nxt = 1;                 /* Object free scanner */
s32b m_nxt = 1;                 /* Monster free scanner */

s32b o_max = 1;                 /* Object heap size */
s32b m_max = 1;                 /* Monster heap size */

s32b o_top = 0;                 /* Object top size */
s32b m_top = 0;                 /* Monster top size */

s32b dungeon_store_timer = 0;	/* Timemout. Keeps track of its generation */
s32b dungeon_store2_timer = 0;	/* Timemout. Keeps track of its generation */
s32b great_pumpkin_timer = 0; /* Timeout. Keeps track of its generation, for HALLOWEEN */
bool night_surface = FALSE;

s16b MaxSimultaneousPlayers = 0;	/* Tracks very high amounts of simultaneously logged-in players,
					   writes to log file then: 'SimultaneousPlayers ...' - C. Blue */

char serverStartupTime[40];	/* String containing the clock time when the server was started */
char *sST = serverStartupTime;

/*
 * Server options, set in tomenet.cfg
 */


server_opts cfg =
{
	/* others */
	6, 0, 0,	// runlevel, runtime, closetime (NOT config options)

	/* char * */
        "meta.tomenet.net",		// meta_address
        8800,           // meta port

	"",		// bind_name
	"changeme",		// console_password
	"DungeonWizard",		// admin_wizard
	"DungeonMaster",	// dungeon_master
	"", 	// wserver,

	"",	// pass
	/* s32b */
	201,		// preserve_death_level
	300,50000,	// unique_respawn_time, unique_max_respawn_time
	3,		// level_unstatic_chance,

	8,		// min_unstatic_level
	-1,18348,18349,18400,	// retire_timer, game_port, console_port, gw_port

	10,200,		// spell_interfere, spell_stack_limit
	/* s16b */
	60,5,5,		// fps, newbies_cannot_drop, running_speed,

	25, 150,	// anti_scum, dun_unusual,
	32,32,		// town_x, town_y
	0, 		// town_base, 

	1,		//dun_base
	127, 200,	// dun_max, store_turns
	/* char */
	3, 2,		// resting_rate, party_xp_boost

	0, 1,		// use_pk_rules, quit_ban_mode
	33, 67, 33,	// zang_monsters, pern_monsters, cth_monsters
	0, 67, 100, 0,		// joke_monsters, cblue_monsters, vanilla_monsters, pet_monsters
	/* bool */
	TRUE,TRUE,	// report_to_meta, secret_dungeon_master

	// anti_arts_hoard, anti_arts_house, anti_arts_wild, anti_arts_shop, anti_arts_pickup, 
	FALSE,TRUE,TRUE,FALSE,TRUE,TRUE,
	FALSE,TRUE, //anti_arts_send, anti_cheeze_pickup, anti_cheeze_telekinesis
	10,		// surface_item_removal (minutes for scan_objs)
	30,		// dungeon_item_removal (minutes for scan_objs)
	1440,		// death_wild_item_removal
	21060,		// long_wild_item_removal
	100, 999, 5,		// dungeon_shop_chance (*10), dungeon_shop_type (999=random), dungeon_shop_timeout

	FALSE,		// mage_hp_bonus
	7,FALSE,0,	// door_bump_open, no_ghost, lifes (0 = infinite),
	7,1,TRUE,	// maxhouses = 50/_houses_per_player_, castles_per_player, castles_for_kings
	TRUE,TRUE,TRUE,	// maximize, kings_etiquette, fallenkings_etiquette

	FALSE,FALSE,	// public_rfe, auto_purge
	FALSE,114,0,	// log_u, replace_hiscore, unikill_format
	"",		// server notes for meta list
	FALSE,		// artifact creation disabled for maintenance reasons? (arts_disabled)
	TRUE,		// total winners may not find true arts anymore? (winners_find_randarts)
	1,		// arts_level_req - Only SPECIAL_GENE arts from DROP_CHOSEN have 0.
	FALSE,		// surface_summoning is disabled by default to prevent BAD cheeze.
	3,		/* clone_summoning - how many times may a monster summon before the summmons become clones
				    (and summon -if they can do that- more clones themselves).*/
	3,		/* henc_strictness - how easily monsters adjust their exp to their highest player encounter */
	1,		/* bonus_calc_type - how HP are calculated (0 = old, 1 = new way) */
	6,		/* charmode_trading_restrictions FLAGS: how restricted is trading? 0 = no exchange, +1 = ever can take (nonever&&nonpvp) items (bad idea), +2 = pvp can take ever, +4 = pvp can take nonever (double-edged) */
	0,		/* item_awareness - how easily the player becomes aware of sofar un-identified items
					    0 = normal, 1 = seeing in standard town shop (1 to 6), 2 = seeing in any shop while carrying it, 3 = seeing in any shop */
	TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,	/* types of messages which will be transmitted through the world server (if available). */
};

struct ip_ban *banlist=NULL;
struct swear_info swear[MAX_SWEAR];
char nonswear[MAX_NONSWEAR][NAME_LEN];

/* Temporary server feature flags (for LUA files) */
u32b sflags_TEMP = 0x0;

/*
 * Software options (set via the '=' command).  See "tables.c"
 */

/* Option Set 1 -- User Interface */

bool use_color;                         /* Use color if possible (slow) */

/* Option Set 2 -- Disturbance */

/* Option Set 3 -- Game-Play */

bool auto_scum;                         /* Auto-scum for good levels */
bool dungeon_align;                     /* Generate dungeons with aligned rooms */

/* Option Set 4 -- Efficiency */

bool avoid_other;                       /* Avoid processing special colors */

/* Special options */

s16b hitpoint_warn;             /* Hitpoint warning (0 to 9) */

struct npc_type *Npcs[MAX_NPCS];
/* The array of players */
player_type **Players;

/* The party information */
party_type parties[MAX_PARTIES];
guild_type guilds[MAX_GUILDS];

struct quest_type quests[20];	/* server quest data */

/* The information about houses */
house_type *houses;
s32b num_houses = 0;
u32b house_alloc = 0;

#ifdef PLAYER_STORES
store_type *fake_store;
int fake_store_visited[MAX_VISITED_PLAYER_STORES]; /* lock an instance while visited by a player */
#endif

/* An array to access a Player's ID */
int GetInd[MAX_ID];

/* Buffer to hold the current savefile name */
//char savefile[MAX_PATH_LENGTH];


/*
 * Array of grids lit by player lite (see "cave.c")
 */
/*s16b lite_n;
byte lite_y[LITE_MAX];
byte lite_x[LITE_MAX];*/

/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
byte temp_y[TEMP_MAX];
byte temp_x[TEMP_MAX];

/*
 * The number of quarks
 */
s16b quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;

/*
 * The array of indexes of "live" objects
 */
s16b o_fast[MAX_O_IDX];

/*
 * The array of indexes of "live" monsters
 */
s16b m_fast[MAX_M_IDX];


/*
 * The array of "cave grids" [MAX_WID][MAX_HGT].
 * Not completely allocated, that would be inefficient
 * Not completely hardcoded, that would overflow memory
 */
/* For wilderness, I am hacking this to extend in the negative direction.
   I currently have a huge number (4096?) of wilderness levels allocated.
   -APD-
*/ 

/* I have moved the cave_type stuff to the wilderness and dungeon
   level structures now. So, to reference the cave_type array for
   the level (x,y,z) you would first check z for being zero. If it
   is, return the wilderness cave_type array pointer. Otherwise
   select the correct dungeon pointer and lookup the dungeon level
   you want. Then return its cave_type array pointer. Of course, 
   use (cave_type**)zcave=getcave(struct worldpos *).
    Evileye
 */
wilderness_type wild_info[MAX_WILD_Y][MAX_WILD_X];   /* seems my world may be a bit bigger */
struct town_type *town=NULL;
u16b numtowns=0;

/*
 * The array of dungeon items [MAX_O_IDX]
 */
object_type *o_list;

/*
 * The array of dungeon monsters [MAX_M_IDX]
 */
monster_type *m_list;

/*
 * The array of dungeon traps [MAX_T_IDX]
 * (elsewhere)
 */
// trap_type *t_list;

/*
 * Hack -- Quest array
 */
quest q_list[MAX_Q_IDX];

/*
 * The stores [MAX_STORES]
 */
//store_type *store;

/*
 * The size of "alloc_kind_table" (at most MAX_K_IDX * 4)
 */
s16b alloc_kind_size;

/*
 * The entries in the "kind allocator table"
 */
alloc_entry *alloc_kind_table;

/*
 * Index numbers for looking up level-specific entries
 */
s16b *alloc_kind_index_level;


/*
 * The size of "alloc_race_table" (at most MAX_R_IDX)
 */
s16b alloc_race_size;

/*
 * The entries in the "race allocator table"
 */
alloc_entry *alloc_race_table;

/*
 * Dungeon specific race allocation tables
 */
alloc_entry **alloc_race_table_dun;

/*
 * Index numbers for looking up level-specific entries
 */
s16b *alloc_race_index_level;


/*
 * Unique monster mask arrays.
 */
char *allow_uniques;
char *reject_uniques;


/*
 * Specify attr/char pairs for inventory items (by tval)
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte tval_to_attr[128];
char tval_to_char[128];

/*
 * Simple keymap method, see "init.c" and "cmd6.c".
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte keymap_cmds[128];
byte keymap_dirs[128];


/*
 * Global table of color definitions
 * Be sure to use "index & 0xFF" to avoid illegal access
 */
byte color_table[256][4];

/*
 * The vault generation arrays
 */
header *v_head;
vault_type *v_info;
char *v_name;
char spacer;
const char spacer2;
char *v_text;

/*
 * The terrain feature arrays
 */
header *f_head;
feature_type *f_info;
char *f_name;
char *f_text;

/*
 * The object kind arrays
 */
header *k_head;
object_kind *k_info;
char *k_name;
char *k_text;

/*
 * The artifact arrays
 */
header *a_head;
artifact_type *a_info;
char *a_name;
char *a_text;

/*
 * The ego-item arrays
 */
header *e_head;
ego_item_type *e_info;
char *e_name;
char *e_text;
s16b *e_tval_size;
s16b **e_tval;

/* jk / Jir */
/* the trap-arrays/variables */
header *t_head;
trap_kind *t_info;
char *t_name;
char *t_text;


/*
 * The monster race arrays
 */
header *r_head;
monster_race *r_info;
char *r_name;
char *r_text;

/*
 * The monster ego race arrays
 */
#ifdef RANDUNIS
header *re_head;
monster_ego *re_info;
char *re_name;

#endif

/*
 * The dungeon types arrays
 */
header *d_head;
dungeon_info_type *d_info;
char *d_name;
char *d_text;

/*
 * Player skills arrays
 */
header *s_head;
skill_type *s_info;
char *s_name;
char *s_text;

#if 1
/*
 * The store/building types arrays
 */
header *st_head;
store_info_type *st_info;
char *st_name;
/* char *st_text; */

/*
 * The building actions types arrays
 */
header *ba_head;
store_action_type *ba_info;
char *ba_name;
/* char *ba_text; */
#endif	// 0

/*
 * The owner types arrays
 */
header *ow_head;
owner_type *ow_info;
char *ow_name;
/* char *ow_text; */


/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Path name: The main "lib" directory
 * This variable is not actually used anywhere in the code
 */
cptr ANGBAND_DIR;


/*
 * Scripts
 */
cptr ANGBAND_DIR_SCPT;

/*
 * Various data files for the game, such as the high score and
 * the mangband logs.
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_GAME;

/*
 * Various user editable text files (ascii), such as the help and greeting
 * screen. These files may be portable between platforms
 */
cptr ANGBAND_DIR_TEXT;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Mangband configuration file
 * Usually it's 'tomenet.cfg'
 */
cptr MANGBAND_CFG;

/*
 * Total Hack -- allow all items to be listed (even empty ones)
 * This is only used by "do_cmd_inven_e()" and is cleared there.
 */
bool item_tester_full;


/*
 * Here is a "pseudo-hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
byte item_tester_tval;


/*
 * Here is a "hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
bool (*item_tester_hook)(object_type*);



/*
 * Current "comp" function for ang_sort()
 */
bool (*ang_sort_comp)(int Ind, vptr u, vptr v, int a, int b);
bool (*ang_sort_extra_comp)(int Ind, vptr u, vptr v, vptr w, int a, int b);


/*
 * Current "swap" function for ang_sort()
 */
void (*ang_sort_swap)(int Ind, vptr u, vptr v, int a, int b);
void (*ang_sort_extra_swap)(int Ind, vptr u, vptr v, vptr w, int a, int b);



/*
 * Hack -- function hook to restrict "get_mon_num_prep()" function
 */
bool (*get_mon_num_hook)(int r_idx);
bool (*get_mon_num2_hook)(int r_idx);



/*
 * Hack -- function hook to restrict "get_obj_num_prep()" function
 */
int (*get_obj_num_hook)(int k_idx, u32b resf);

/* the dungeon master movement hook, is called whenever he moves
 * (to make building large buildings / summoning hoards of mosnters 
 * easier)
 */
//bool (*master_move_hook)(int Ind, char * args) = master_acquire;
//bool (*master_move_hook)(int Ind, char * args) = NULL; /*evileye - multi DMs! */


/* new Hacks */
int artifact_bias;
char summon_kin_type;

/*
 * radius tables
 */
char tdy[662];
char tdx[662];
s32b tdi[18];	// PREPARE_RADIUS + 2

/* How many times project() is called in this turn? */
#ifdef PROJECTION_FLUSH_LIMIT
s16b count_project = 0;
s16b count_project_times = 0;
#endif	// PROJECTION_FLUSH_LIMIT

/* Hack -- 'default' values for obj_theme.	- Jir -
 * Only a makeshift till d_info thingie will be implemented. */
obj_theme default_obj_theme = {20, 20, 20, 20};

/*
 * The spell list of schools
 */
s16b max_spells;
spell_type *school_spells;
s16b max_schools;
school_type *schools;

/*
 * Lasting spell effects
 */
int project_interval = 0;
int project_time = 0;
s32b project_time_effect = 0;
effect_type effects[MAX_EFFECTS];


/*
 * Most of max_*_idx below is not used for now, but will be
 * used in near future.		- Jir -
 */
/*
 * Maximum number of skills in s_info.txt
 */
u16b old_max_s_idx = 0;
u16b max_s_idx;

/*
 * Maximum number of monsters in r_info.txt
 */
u16b max_r_idx;

/*
 * Maximum number of ego monsters in re_info.txt
 */
u16b max_re_idx;

/*
 * Maximum number of items in k_info.txt
 */
u16b max_k_idx;

/*
 * Maximum number of vaults in v_info.txt
 */
u16b max_v_idx;

/*
 * Maximum number of terrain features in f_info.txt
 */
u16b max_f_idx;

/*
 * Maximum number of artifacts in a_info.txt
 */
u16b max_a_idx;

/*
 * Maximum number of ego-items in e_info.txt
 */
u16b max_e_idx;

/*
 * Maximum number of randarts in ra_info.txt
 */
u16b max_ra_idx;

/*
 * Maximum number of dungeon types in d_info.txt
 */
u16b max_d_idx;

/*
 * Maximum number of stores types in st_info.txt
 */
u16b max_st_idx;

/*
 * Item sets
 */
s16b max_set_idx = 1;

/*
 * Maximum number of players info in p_info.txt
 */
u16b max_rp_idx;
u16b max_rmp_idx;
u16b max_c_idx;
u16b max_mc_idx;

/*
 * Maximum number of actions types in ba_info.txt
 */
u16b max_ba_idx;

/*
 * Maximum number of owner types in ow_info.txt
 */
u16b max_ow_idx;

/*
 * Maximum number of objects in the level
 */
u16b max_o_idx;

/*
 * Maximum number of monsters in the level
 */
u16b max_m_idx;

/*
 * Maximum number of traps in tr_info.txt
 */
u16b max_t_idx;

/*
 * Maximum number of wilderness features in wf_info.txt
 */
u16b max_wf_idx;

/* EVILEYE GAMES */
int teamscore[2];
int teams[2]; // for counting players in teams - mikaelh
int gametype;

/* Private notes for fellow players
 * see '/note' command in util.c. -C. Blue
 */
char priv_note[MAX_NOTES][MAX_CHARS_WIDE], priv_note_sender[MAX_NOTES][NAME_LEN], priv_note_target[MAX_NOTES][NAME_LEN];
char party_note[MAX_PARTYNOTES][MAX_CHARS_WIDE], party_note_target[MAX_PARTYNOTES][NAME_LEN];
char guild_note[MAX_GUILDNOTES][MAX_CHARS_WIDE], guild_note_target[MAX_GUILDNOTES][NAME_LEN];
char admin_note[MAX_ADMINNOTES][MAX_CHARS], server_warning[MSG_LEN];

/* in-game bbs :) - C. Blue */
char bbs_line[BBS_LINES][MAX_CHARS_WIDE];
/* party/guild-internal bbs'es: */
char pbbs_line[MAX_PARTIES][BBS_LINES][MAX_CHARS_WIDE];
char gbbs_line[MAX_GUILDS][BBS_LINES][MAX_CHARS_WIDE];

int global_luck = 0;
int regen_boost_stamina = 4;

/* default 'updated_savegame' value for newly created chars [0].
   usually modified by lua (server_startup()) instead of here. */
int updated_savegame_birth = 0;
/* like 'updated_savegame' is for players, this is for (lua) server state [0].
   usually modified by lua (server_startup()) instead of here. */
int updated_server = 0;
/* for automatic artifact resets via lua */
int artifact_reset = 0;

/* Watch if someone enters Nether Realm or challenges Morgoth - C. Blue
   Dungeon masters will be paged if they're not AFK or if they have
   'watch' as AFK reason! */
bool watch_nr = 0;
bool watch_morgoth = 0;

/* for lua_bind.c */
bool first_player_joined = TRUE;

/* lua-dependant 'constants' */
int __lua_HHEALING;
int __lua_HBLESSING;
int __lua_MSCARE;
int __lua_M_FIRST;
int __lua_M_LAST;

/* for cron_1h (using turns % 3600 isn't precise enough, might happen that
   one hour gets skipped, eg if transition is 1:59 -> 3:00; so now we're
   polling the timer instead. */
int cron_1h_last_hour = -1; /* -1 to call immediately after server restart */

/* for global events (xtra1.c, slash.c) */
global_event_type global_event[MAX_GLOBAL_EVENTS];
int sector00separation = 0; /* some events separate sector 0,0 from the worldmap
			 to use it in a special way - WoR won't work either */
int sector00downstairs = 0; /* remember that sector 0,0 contains staircases downwards at the moment */
int ge_special_sector = 0; /* to make it known that a certain sector (or multiple sectors, given in
			    defines.h as WPOS_ constants) are now in special use for global events. */
u32b ge_contender_buffer_ID[128]; /* Remember account IDs of players who are supposed to receive */
int ge_contender_buffer_deed[128]; /* contender's deeds on different characters (Highlander Tournament!) */
u32b achievement_buffer_ID[128]; /* Remember account IDs of players who are supposed to receive */
int achievement_buffer_deed[128]; /* an achievement deeds on different characters (PvP Mode) */

/* for dungeon master/wizard summoning, to override all validity checks and
   definitely summon what his/her heart desires! - C. Blue */
u32b summon_override_checks = SO_NONE;
/* Morgoth may override no-destroy, with his shattering hits */
bool override_LF1_NO_DESTROY = FALSE;

/* the four seasons */
#ifdef WINTER_SEASON /* backward code compatibility till seasons have finished being implemented...*/
byte season = SEASON_WINTER;
#else
byte season = SEASON_SPRING; /* default is spring on server startup */
#endif


/* SERVER-SIDE WEATHER AND GLOBAL CLIENT-SIDE WEATHER: */
/* for snowfall during WINTER_SEASON mainly */
int weather = 0;	/* note: for old server-side or global-client-side weather this is a flag (0 or 1),
			   for modern client-side weather this is 0, 1 (rain), 2 (snow). */
int weather_duration = 0;
#ifdef WINTER_SEASON /* backward code compatibility till seasons have finished being implemented...*/
byte weather_frequency = WINTER_SEASON;
#else
byte weather_frequency = 2; /* <never rain/snow> 0..4 <always rain/snow> ; [2] */
#endif
int wind_gust = 0;
int wind_gust_delay = 0;


/* NON-GLOBAL, CLIENT-SIDE WEATHER: */
/* moving clouds, partially buffered client-side */
int cloud_x1[MAX_CLOUDS], cloud_y1[MAX_CLOUDS], cloud_x2[MAX_CLOUDS], cloud_y2[MAX_CLOUDS], cloud_dsum[MAX_CLOUDS];
int cloud_xm100[MAX_CLOUDS], cloud_ym100[MAX_CLOUDS], cloud_mdur[MAX_CLOUDS], cloud_xfrac[MAX_CLOUDS], cloud_yfrac[MAX_CLOUDS];
int cloud_dur[MAX_CLOUDS], cloud_state[MAX_CLOUDS];
int clouds; /* tracking variable */
int max_clouds_seasonal = MAX_CLOUDS; /* maximum number of clouds to use depending
					 on current season, thereby determining the
					 weather frequency (ie rain/snow probability). */
/* Winds. Moving clouds and determining direction and speed of raindrops and snowflakes.
   Winds affect a very large area, so we need only few of them.
   It's efficient if we reduce the possible winds a bit, sufficient should be:
   For raindrops and snowflakes:
    Just west and east.
   For cloud movement:
    All sorts of directions, todo: implement. */
int wind_dur[16], wind_dir[16];


/* special seasons */
int season_halloween = 0;
int season_xmas = 0;
int season_newyearseve = 0;

/* for controlling fireworks on NEW_YEARS_EVE */
int fireworks = 0;
int fireworks_delay = 0;


char last_chat_line[MSG_LEN];  /* What was said */
char last_chat_owner[NAME_LEN]; /* Who said it */
// char last_chat_prev[MSG_LEN];  /* What was said before the above*/

auction_type *auctions;
u32b auction_alloc;

int store_debug_mode = 0, store_debug_quickmotion = 10, store_debug_startturn = 0;

/* Array used by everyone_lite_later_spot */
struct worldspot *lite_later;
int lite_later_alloc;
int lite_later_num;

/* Timers for specific events - C. Blue */
int timer_pvparena1 = 1, timer_pvparena2 = 1, timer_pvparena3 = 0; /* defaults */

/* Recall-shutdown timer for /shutrec */
int shutdown_recall_timer = 0, shutdown_recall_state = 0;

#ifdef MONSTER_ASTAR
astar_list_open astar_info_open[ASTAR_MAX_INSTANCES];
astar_list_closed astar_info_closed[ASTAR_MAX_INSTANCES];
#endif

#ifdef USE_SOUND_2010
/* copy the audio_sfx[] array from audio.lua over, otherwise
   Sound() calls in functions that are called from within LUA
   (such as fire_ball()) will cause LUA errors. - C. Blue */
char audio_sfx[SOUND_MAX_2010][30];
#endif

/* nserver.c players' connection array */
//connection_t **Conn = NULL;

/* Ironman Deep Dive Challenge */
int deep_dive_level[20];
//char deep_dive_name[20][NAME_LEN]; /* store just the name */
char deep_dive_name[20][MAX_CHARS]; /* store name, race, class, level */

/* Global projection counter for m_ptr->hit_proj_id */
int mon_hit_proj_id, mon_hit_proj_id2;

/* remember school for each spell */
int spell_school[512];
/* Also remeber the first and last school of each magic resort */
int SCHOOL_HOFFENSE, SCHOOL_HSUPPORT;
int SCHOOL_DRUID_ARCANE, SCHOOL_DRUID_PHYSICAL;
int SCHOOL_ASTRAL;
int SCHOOL_PPOWER, SCHOOL_MINTRUSION;

#ifdef DUNGEON_VISIT_BONUS
# ifdef DUNGEON_VISIT_BONUS_DEPTHRANGE
u16b depthrange_visited[20]; //(levels 0..9 each, so [19] := 190..199)
# endif
int dungeon_id_max = 0;
/* Note: doubling MAX_D_IDX to also account for possible custom dungeons ("Wilderness") along with the default d_info[] dungeons */
int dungeon_x[MAX_D_IDX * 2], dungeon_y[MAX_D_IDX * 2];
u16b dungeon_visit_frequency[MAX_D_IDX * 2];   /* how often players enter this dungeon */
bool dungeon_tower[MAX_D_IDX * 2], dungeon_visit_check[MAX_D_IDX * 2];
int dungeon_bonus[MAX_D_IDX * 2];
#endif

bool censor_swearing = TRUE;
bool jails_enabled = TRUE;
bool allow_requesting_estate = FALSE;
