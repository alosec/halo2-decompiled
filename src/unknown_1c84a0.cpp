#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"
#include "unknown_1e46c0.h"
// @flags /O2 /Gr

class c_1e2990
{
public:
 void function_1e2990(bool arg_0);
};



PRIVATE inline s_actor_datum *squad_actor_next_1c84a0(s_squad_actor_iterator *iterator)
{
 s_actor_datum *actor = NULL;
 if (g_4f55d0->active && iterator->next_actor_index != NONE)
 {
  actor = actor_datum_get(iterator->next_actor_index);
  iterator->actor_index = iterator->next_actor_index;
  iterator->next_actor_index = actor->next_actor_index;
 }
 return actor;
}

// @retail 0x1c84a0
void function_1c84a0(long arg_0, bool arg_1)
{
 if (g_4f55d0->active)
 {
  if (arg_0 != NONE)
  {
   s_squad_actor_iterator local_0;
   function_204d30(&local_0, arg_0);
   while (squad_actor_next_1c84a0(&local_0) != NULL)
    ((c_1e2990 *)local_0.actor_index)->function_1e2990(arg_1);
  }
  else
  {
   s_actor_iterator local_4;
   function_x66da2b(&local_4, false);
   while (function_1e46c0(&local_4))
    ((c_1e2990 *)local_4.actor_index)->function_1e2990(arg_1);
  }
 }
}

struct s_1c88c0
{
 byte field_0;
 byte field_1[3];
 long field_4;
 long field_8;
};
struct s_1c88c1
{
 short field_0;
 short field_2;
 s_1c88c0 field_4[1];
};

// @retail 0x1c88c0
long __stdcall function_1c88c0(long arg_0, void *arg_1, long arg_2, bool *arg_3, void *arg_4, long arg_5)
{
 long local_0 = 0;
 s_1c88c1 *local_1 = (s_1c88c1 *)arg_1;
 if (local_1->field_2 < local_1->field_0)
 {
  s_1c88c0 *local_2 = &local_1->field_4[local_1->field_2];
  if (local_2->field_0)
  {
   local_0 = 1;
   ((c_1e2990 *)local_2->field_4)->function_1e2990(true);
  }
  else
  {
   long local_3 = local_2->field_4;
   local_0 = *(short *)(g_51e9d8->data + (local_3 & 0xffff) * 0x98 + 0xa);
   function_1c84a0(local_3, 1);
  }
  ++local_1->field_2;
 }
 *arg_3 = local_1->field_2 < local_1->field_0;
 return local_0;
}
