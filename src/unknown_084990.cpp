// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_08b110.h"
#include "unknown_096ed0.h"
#include "unknown_067e10.h"
#include "network_message_types.h"
#include <string.h>

struct s_simulation_controller
{
	long field_00;
	long field_04;
	long field_08;
	t_player_key key;
	s_machine_address machine;
	byte field_1e[2];
	c_class_6a600 *world;
	bool field_24;
	bool field_25;
	byte field_26[2];
	long field_28;
	s_player_action action;
};


struct s_object_relevance_result
{
 real first;
 real second;
 long object_index;
 long identifier;
};
struct s_weapon_activity_result
{
 byte unknown00[0x18];
 bool active[2][2];
 bool update_relevance;
 byte unknown1d[3];
 s_object_relevance_result relevance;
 bool consumed[2];
};

static inline bool controller_world_is_authority(c_class_6a600 *world)
{
	return world->state != 3 && world->state != 5;
}

c_simulation_view *function_6acb0(c_class_6a600 *world);
c_simulation_view *function_6ad40(c_class_6a600 *world, const s_machine_address *address);
void function_825e0(const s_weapon_activity_result *input, s_player_action *action);
bool function_14d0a0(const s_player_action *action);
extern real g_4ced44;

// @retail 0x84990
bool __stdcall function_84990(s_simulation_controller *controller, s_player_action *output)
{
 bool result = false;
 if (controller->field_08 >= 4 && controller->field_08 <= 5)
 {
  c_class_6a600 *world = controller->world;
  c_simulation_view *view;
  if (controller_world_is_authority(world))
   view = function_6ad40(world, &controller->machine);
  else
   view = function_6acb0(world);
  if (view)
  {
   c_vtable_450c94 *source = (c_vtable_450c94 *)((byte *)view->data + 0x5098);
   if (source)
   {
    long local_0 = controller->field_00;
    s_weapon_activity_result activity;
    if (source->take_data18(local_0, (s_dword34 *)&activity))
    {
     s_player_action action;
     function_825e0(&activity, &action);
     controller->action = action;
     controller->field_28 = g_510c54->game_time;
    }
   }
  }
 }
 if (controller->field_08 != 2 && controller->field_28 != NONE &&
  (g_510c54->game_time - controller->field_28) * g_510c54->rate < g_4ced44 &&
  function_14d0a0(&controller->action))
 {
  *output = controller->action;
  result = true;
 }
 return result;
}
