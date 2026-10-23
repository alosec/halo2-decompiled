// @flags /O1 /Ob1 /arch:SSE /Gr
/* USER_INTERFACE_HUD_FUNCTIONS.CPP: the named values the new hud draws
   with, and the function context that hands them to its shaders (named to
   link after unknown_*.cpp, which keeps 0x73360 matching) */

#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "unknown_07f720.h"
#include "new_hud_functions.h"
#include <stddef.h>

/* the hud's draw inputs, cleared each map: the color at 0x30 and the
   values before it are set by whatever is drawing, the per-user values by
   function_139296 */
struct s_4e6950
{
	byte unknown00[0x18];
	real value18;
	real value1c;
	real value20;
	real value24;
	byte unknown28[8];
	color4f color;
	byte unknown40[0x30];
	real user_values[4];
};

extern s_4e6950 g_4e6950;
struct s_510c4c;
extern s_510c4c *g_510c4c;

typedef char check_interface_value_view_size[sizeof(s_4e6950) == 0x80 ? 1 : -1];
typedef char check_interface_color_offset[offsetof(s_4e6950, color) == 0x30 ? 1 : -1];

bool function_22acb4(long local_player_index);
bool player_get_function_value(long player_index, long name, real *value);
bool player_appearance_get_function_value(long name, s_player_appearance const *appearance, real *value);

/* the value of the hud function name (a string id): the draw inputs, the
   shown player's own values, or 1 */
// @retail 0x22ace7
real __stdcall new_hud_function_evaluate(long context, long name)
{
	real result = 1.0f;
	switch (name)
	{
	case 0x090005ce:
		result = function_22acb4(g_4b9ed8) ? 1.0f : 0.0f;
		break;
	case 0x0a0005cf:
		result = g_4e6950.color.alpha;
		break;
	case 0x0a0005d0:
		result = g_4e6950.color.red;
		break;
	case 0x0a0005d1:
		result = g_4e6950.color.green;
		break;
	case 0x0a0005d2:
		result = g_4e6950.color.blue;
		break;
	case 0x0c0005cb:
		result = g_4e6950.value1c;
		break;
	case 0x0c0005cc:
		result = g_4e6950.value20;
		break;
	case 0x0d0005ca:
		result = g_4e6950.value18;
		break;
	case 0x0e0005cd:
		result = g_4e6950.value24;
		break;
	case 0x11000600:
		result = ((s_new_hud_function_inputs *)g_510c4c)->quantity * (1.0f / 64.0f);
		break;
	case 0x0d0005ff:
	case 0x170005fa:
	case 0x170005fb:
	case 0x180005fc:
	case 0x1d0005fd:
	case 0x1d0005fe:
		{
			s_new_hud_function_inputs *inputs = (s_new_hud_function_inputs *)g_510c4c;
			if (inputs->player_index != NONE)
				player_get_function_value(inputs->player_index, name, &result);
			else
				player_appearance_get_function_value(name, &inputs->appearance, &result);
		}
		break;
	}
	return result;
}

/* the function context the hud draws with: its values come from
   new_hud_function_evaluate */
struct s_interface_function_context
{
	long index;
	real (__stdcall *evaluate)(long context, long name);
	byte unknown08[0x70];
};

s_interface_function_context g_5021d0;
void function_2c3b0(s_interface_function_context *context);

/* sets up the hud's function context */
// @retail 0x22a648
void new_hud_function_context_initialize(void)
{
	function_2c3b0(&g_5021d0);
	g_5021d0.index = NONE;
	g_5021d0.evaluate = new_hud_function_evaluate;
}
