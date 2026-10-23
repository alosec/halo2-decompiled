// stubs for the game functions outside 0x230000..0x23ffff that lane G's code
// calls and that are not decompiled yet (and a few of lane G's own, until
// they are written)
#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "unknown_234c64.h"
#include "user_interface_lists.h"
#include "unknown_2312b4.h"

/* UI lane round 4: callees of the campaign level select list and the game
   engine variant category list */








/* lane G's own, not written yet */


/* the screens' create functions (lane G, not written yet) */


/* callees of the screen widget code */



struct s_bitmap_view;


/* UI lane round 2: the custom game profile list (unknown_2c9ddb.cpp) */




/* unknown_2b116a.cpp's list (only the member the stub defines) */
class c_potential_squad_leader_player_list
{
public:
	void handle_item(s_controller_reference **controller, long *item);
};


/* UI lane round 3: callees of user_interface_text_parser.cpp */

/* UI lane round 3: callees of the actions list (unknown_2b116a.cpp) */


/* UI lane round 5: callees of the press start screen */



/* lane D */
struct _XONLINE_USER;

/* my own, not written yet */



/* lane D */



/* UI lane round 7: my own, not written yet */


struct s_widget_item;
class c_class_1a2c81;

/* UI lane round 14: callees of the campaign options list */

struct s_saved_game_header;
struct s_saved_game_read;
class c_campaign_options_list;

