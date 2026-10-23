// @flags /O1 /Gr
/* PLAYER_MODEL_NAMES.CPP: the string id of the name of a player profile's
   model (the profile settings' model byte, as the model list and the
   multiplayer settings list show it) */

#include "unknown_11c920.h"

/* the string id of model 0 or 1's name, or NONE */
// @retail 0x2365e0
long player_model_get_string_id(long model)
{
	long result = NONE;
	switch (model)
	{
	case 0:
		result = 0x70002ff;
		break;
	case 1:
		result = 0x8000300;
		break;
	}
	return result;
}
