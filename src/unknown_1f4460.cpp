// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F4460.CPP: an actor's movement goal (actor +0x4ac..+0x4e8) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_20fe20.h"
#include "unknown_1f4460.h"
#include "unknown_0259d0.h"
#include "unknown_2626b0.h"
#include <string.h>
#include "props.h"

void function_1f86a0(long index);
bool __stdcall function_1f8a70(long actor_index, long unknown);
void __stdcall function_2628f0(long actor_index, s_reference reference);

struct s_1f4a20_entry
{
    byte field_0[0x5c];
    short type;
    byte field_5e[2];
    point3f point;
    vector3f direction;
};
struct s_1f4a20_source
{
    byte field_0[0x54];
    point2f direction;
    byte field_5c[4];
    point3f point;
    byte field_6c[4];
};
struct s_1f4a20_record
{
    short salt;
    short type;
    byte field_4[8];
    s_type_c3b527 point;
    byte field_1c[0x34 - 0x1c];
    long sector;
    byte field_38[0x484 - 0x38];
};
bool __stdcall function_29da40(long actor_index, s_type_c3b527 const *origin, long sector,
    short type, bool alternate, point3f const *point, vector3f const *direction,
    s_type_c3b527 *out, long *sector_out);
bool function_270240(long object_index, long mode, long set, point3f const *point,
    vector3f const *direction, transform4x3f *out);
bool function_26d290(point3f const *origin, point3f const *target, long sector, long *sector_out);
point3f *function_b9dd0(long object_index, point3f *point);
real function_30bf0(vector3f *vector);

// @retail 0x1f4a20
bool function_1f4a20(s_1f4a20_entry const *entries, long actor_index, s_reference reference, s_1f4a20_source const *source,
    bool alternate, s_type_c3b527 *arg_26ef00, vector3f *direction_out,
    signed char *type_out, bool *available_out)
{
    s_actor_view *actor = actor_get(actor_index);
    bool result = false;
    function_26c180(actor_index);
    s_262b40_result *target = function_262b40(reference);
    s_type_c3b527 origin;
    s_type_c3b527 destination;
    vector3f direction;
    long sector;
    signed char type;
    if (!(reference.unknown2 & 0x8000))
    {
        if (!entries->type)
            goto done;
        if (!function_29da40(actor_index, (s_type_c3b527 *)target, *(long *)((byte *)target + 0x14),
            entries->type, alternate, &entries->point, &entries->direction, &destination, &sector))
            goto done;
        origin.point = entries->point;
        origin.output_index = ((s_type_c3b527 *)target)->output_index;
        type = (signed char)entries->type;
        if (!function_2105b0(((s_type_c3b527 *)target)->output_index, &entries->direction, &direction))
            goto done;
    }
    else
    {
        if (!source)
            goto done;
        s_1f4a20_record *record = (s_1f4a20_record *)(g_51eca4->data + (reference.unknown2 & 0x7fff) * sizeof(s_1f4a20_record));
        if (record->type != 1)
            goto done;
        point3f const *point = &source[reference.unknown0].point;
        function_210690(record->point.output_index, point, &origin.point);
        origin.output_index = record->point.output_index;
        if (entries->type != 1 && entries->type != 2 && entries->type != 3)
            goto done;
        type = (signed char)entries->type;
        direction.i = -1.0f * source[reference.unknown0].direction.x;
        direction.j = -1.0f * source[reference.unknown0].direction.y;
        direction.k = 0.0f;
        if (!(function_30bf0(&direction) > 0.0f))
            goto done;
        long mode = NONE;
        switch (type)
        {
        case 1: mode = alternate ? 0x10000229 : 0x110001b4; break;
        case 2: mode = alternate ? 0x1100022a : 0x120001b5; break;
        case 3: mode = alternate ? 0x0b0006c4 : 0x0c0006c3; break;
        }
        point3f adjusted;
        if (actor_index != NONE)
        {
            transform4x3f matrix;
            s_actor_view *path_actor = actor_get(actor_index);
            if (!function_270240(path_actor->unknown018, mode, 0x5000049, point, &direction, &matrix))
                goto done;
            adjusted = matrix.position;
        }
        else
            adjusted = *point;
        function_210690(record->point.output_index, &adjusted, &destination.point);
        destination.output_index = record->point.output_index;
        if (!function_26d290(&record->point.point, &destination.point, record->sector, &sector))
            goto done;
    }
    *available_out = true;
    if (actor->unknown27c.point.output_index == ((s_type_c3b527 *)target)->output_index)
    {
        s_slot_object_view *unit = object_get(actor->unknown018);
        short animation = *(short *)((byte *)unit + *(short *)((byte *)unit + 0x346) + 0x36);
        if (animation == 6 || animation == 7)
        {
            point3f world;
            point3f position;
            function_210850(&origin, &world);
            function_b9dd0(actor->unknown018, &position);
            unit = object_get(actor->unknown018);
            vector3f forward;
            long parent_index = *(long *)((byte *)unit + 0x14);
            if (parent_index == NONE)
                forward = *(vector3f *)((byte *)unit + 0x70);
            else
            {
                signed char node = *(signed char *)((byte *)unit + 0x18);
                s_slot_object_view *parent = object_get(parent_index);
                transform4x3f const *matrix = (transform4x3f const *)((byte *)parent + *(short *)((byte *)parent + 0x116)) + node;
                vector3f const *vector = (vector3f const *)((byte *)unit + 0x70);
                real x = vector->i;
                real y = vector->j;
                real z = vector->k;
                forward.i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
                forward.j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
                forward.k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
            }
            real x = world.x - position.x;
            real y = world.y - position.y;
            real z = world.z - position.z;
            if (0.1f * 0.1f > z * z + y * y + x * x &&
                (forward.j * direction.j + forward.i * direction.i) + forward.k * direction.k > 0.99f)
                destination = actor->unknown27c.point;
        }
    }
    if (function_1f4460(actor_index, &destination, sector, NONE, false))
    {
        actor->unknown4e8 = true;
        result = true;
        *arg_26ef00 = origin;
        *direction_out = direction;
        *type_out = type;
        function_2628f0(actor_index, reference);
    }
done:
    return result;
}

/* the actor's state at +0x4ac */
struct s_1f45c1
{
	short field_0;
	bool field_2;
	byte field_3;
	real field_4;
	real field_8;
	s_type_c3b527 field_c;
	long field_1c;
	real field_20;
	real field_24;
	bool field_28;
	bool field_29;
	byte field_2a[0x38 - 0x2a];
	long field_38;
	bool field_3c;
};

inline void function_1f45c1(s_1f45c1 *arg_0)
{
	arg_0->field_0 = 0;
	arg_0->field_2 = false;
	arg_0->field_4 = 0.0f;
	arg_0->field_8 = 0.0f;
	arg_0->field_20 = 0.0f;
	arg_0->field_24 = 0.0f;
	arg_0->field_28 = false;
	arg_0->field_29 = false;
	arg_0->field_38 = NONE;
	arg_0->field_3c = false;
}

// @retail 0x1f4280
bool __stdcall function_1f4280(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown270 == 4 && actor->unknown5d0)
	{
		s_type_c3b527 point;
		point.point = actor->position;
		point.output_index = NONE;
		return function_1f4460(actor_index, &point, NONE, NONE, true);
	}
	function_2628f0(actor_index, g_470fa0);
	if (actor->unknown4ac != 1)
		actor->unknown4ac = 1;
	return function_1f8a70(actor_index, 0);
}

// @retail 0x1f4460
bool function_1f4460(long actor_index, s_type_c3b527 const *point, long target_index, long unknown, bool unknown2)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown229 && target_index == NONE)
	{
		function_1f86a0(actor_index);
		result = false;
	}
	else
	{
		function_2628f0(actor_index, g_470fa0);
		if (actor->unknown4ac == 2 && actor->unknown4c8 == target_index &&
			!(function_210a30(point, &actor->unknown4b8) > 0.1f * 0.1f))
		{
			if (actor->unknown040 && !actor->unknown506)
				result = function_1f8a70(actor_index, 0);
		}
		else
		{
			function_1f86a0(actor_index);
			function_1f45c1((s_1f45c1 *)&actor->unknown4ac);
			actor->unknown4ac = 2;
			actor->unknown4ae = unknown2;
			actor->unknown4b8 = *point;
			actor->unknown4c8 = target_index;
			actor->unknown4e4 = unknown;
			if (actor->unknown229)
				actor->unknown656 = 0;
			result = function_1f8a70(actor_index, 0);
		}
	}
	return result;
}

// @retail 0x1f45c0
bool function_1f45c0(long actor_index, s_type_c3b527 const *point, long target_index, bool unknown)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);
	function_2628f0(actor_index, g_470fa0);
	if (actor->unknown4ac == 3 && !(function_210a30(point, &actor->unknown4b8) > 0.1f * 0.1f))
	{
		if (actor->unknown040 && !actor->unknown506)
			result = function_1f8a70(actor_index, 0);
	}
	else
	{
		function_1f86a0(actor_index);
		function_1f45c1((s_1f45c1 *)&actor->unknown4ac);
		actor->unknown4ac = 3;
		actor->unknown4ae = unknown;
		actor->unknown4b8 = *point;
		actor->unknown4c8 = NONE;
		actor->unknown4e4 = target_index;
		actor->unknown656 = 0;
		result = function_1f8a70(actor_index, 0);
	}
	return result;
}

// @retail 0x1f46f0
bool __stdcall function_1f46f0(long actor_index, short type, s_reference reference, byte *scratch, bool unknown)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown4ac == type && *(long *)&actor->unknown4b8 == *(long *)&reference)
	{
		if (actor->unknown040 && !actor->unknown506)
		{
			if (actor->unknown5b4 > 0)
				scratch = NULL;
			result = function_1f8a70(actor_index, (long)scratch);
		}
	}
	else
	{
		function_1f86a0(actor_index);
		if (scratch)
			memcpy((byte *)actor + 0x5b4, scratch + 0x70, 0x1c);
		function_1f45c1((s_1f45c1 *)&actor->unknown4ac);
		actor->unknown4ac = type;
		*(s_reference *)&actor->unknown4b8 = reference;
		*((byte *)actor + 0x4bc) = false;
		actor->unknown3f1 = false;
		actor->unknown4ae = unknown;
		result = function_1f8a70(actor_index, (long)scratch);
	}
	return result;
}

/* the parts of a unit or vehicle that set how fast its actor can stop */
struct s_stopping_object
{
	long definition_index;
	byte unknown004[0x70 - 0x4];
	vector3f forward;
	byte unknown07c[0x88 - 0x7c];
	vector3f velocity;
	byte unknown094[0x3dc - 0x94];
	byte movement_type;
};

struct s_stopping_object_header
{
	byte unknown00[8];
	s_stopping_object *object;
};

struct s_stopping_vehicle_definition
{
	byte unknown000[0x1f4];
	real maximum_speed;
	byte unknown1f8[4];
	real acceleration;
};

struct s_stopping_unit_definition
{
	byte unknown000[0x2dc];
	real maximum_speed;
	byte unknown2e0[4];
	real acceleration;
	real deceleration;
	byte unknown2ec[0x2f4 - 0x2ec];
	real speed_scale;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

static inline byte stopping_object_get_movement_type(s_stopping_object *object)
{
	return object->movement_type;
}

/* how far the actor's unit (or vehicle) travels before it stops, and how far
   it travels while it speeds up to its top speed and stops again */
// @retail 0x1f40b0
void function_1f40b0(long actor_index, real *stopping_distance, real *round_trip_distance)
{
	s_actor_view *actor = actor_get(actor_index);
	real maximum_speed = 2.5f;
	real speed = 0.0f;
	real acceleration = 15.0f;
	real deceleration = 24.0f;

	if (actor->unknown26c != NONE)
	{
		if (actor->unknown270 == 3)
		{
			s_stopping_object *vehicle = ((s_stopping_object_header *)g_4e0300->data)[actor->unknown26c & 0xffff].object;
			s_stopping_vehicle_definition *definition = (s_stopping_vehicle_definition *)g_4e3b44[vehicle->definition_index & 0xffff].bytes;

			speed = dot3f(&vehicle->velocity, &vehicle->forward);
			maximum_speed = definition->maximum_speed;
			acceleration = definition->acceleration;
			deceleration = definition->acceleration;
		}
	}
	else if (actor->unknown018 != NONE)
	{
		s_stopping_object *unit = (s_stopping_object *)function_badc0(actor->unknown018, 1);

		if (unit)
		{
			s_stopping_unit_definition *definition = (s_stopping_unit_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;

			speed = dot3f(&unit->velocity, &unit->forward);
			if (stopping_object_get_movement_type(unit) == 2)
			{
				maximum_speed = definition->maximum_speed;
				acceleration = definition->acceleration;
				deceleration = definition->deceleration;
				if (actor->unknown5d4 && definition->speed_scale > 0.0f)
				{
					maximum_speed = definition->speed_scale * maximum_speed;
					acceleration = definition->speed_scale * acceleration;
					deceleration = definition->speed_scale * deceleration;
				}
			}
		}
	}

	if (stopping_distance)
		*stopping_distance = speed * speed / (deceleration * 2.0f);
	if (round_trip_distance)
	{
		if (speed > maximum_speed)
			maximum_speed = speed;
		*round_trip_distance = (maximum_speed * maximum_speed - speed * speed) / (acceleration * 2.0f) +
			maximum_speed * maximum_speed / (deceleration * 2.0f);
	}
}


PRIVATE __forceinline void function_1f4811(s_1f45c1 *arg_0)
{
	arg_0->field_0 = 0;
	arg_0->field_2 = false;
	arg_0->field_4 = 0.0f;
	arg_0->field_8 = 0.0f;
	arg_0->field_20 = 0.0f;
	arg_0->field_24 = 0.0f;
	arg_0->field_28 = false;
	arg_0->field_29 = false;
	arg_0->field_38 = NONE;
	arg_0->field_3c = false;
}


void __stdcall function_26c2d0(long arg_0);
void function_265c30(long arg_0, long arg_1, bool arg_2);

// @retail 0x1f4810
bool __stdcall function_1f4810(long arg_0, long arg_1, real arg_2, bool arg_3)
{
	long const *local_0 = &arg_0;
	long const *local_1 = &arg_1;
	real const *local_2 = &arg_2;
	bool const *local_3 = &arg_3;
	s_actor_view *local_5 = actor_get(*local_0);
	long local_16 = *local_1;
	bool local_6 = false;
	bool local_4 = local_6;
	function_2628f0(*local_0, g_470fa0);
	local_5->unknown4ae = *local_3;
	if (local_5->unknown4ac == 7 && *(long *)&local_5->unknown4b8 == local_16 && local_5->unknown4d0 == *local_2)
	{
		real local_7 = 0.0f;
		s_type_5cfb45 *local_8 = function_25d690((s_prop_datum *)prop_node_get(local_16));
		function_26c2d0(local_16);
		if (local_5->unknown229)
			local_7 = function_210ac0((s_type_c3b527 *)((byte *)local_5 + 0x4ec), (point3f *)((byte *)local_8 + 0x10));
		else
		{
			s_type_c3b527 *local_9 = (s_type_c3b527 *)((byte *)local_5 + 0x4ec);
			*local_9 = *(s_type_c3b527 *)&local_8->unknown48;
			if (local_5->unknown510.output_index == local_8->unknown54 && *(long *)((byte *)local_5 + 0x520) == local_8->unknown44)
			{
				local_7 = function_210970(&local_5->unknown510, (s_type_c3b527 *)&local_8->unknown48);
				local_6 = *(bool const volatile *)&local_4;
			}
			else
				local_6 = true;
		}
		if (local_7 > *local_2)
			*(bool volatile *)&local_4 = true;
	}
	else
		*(bool volatile *)&local_4 = true;
	if (local_4 || local_6)
	{
		s_prop_node_view *local_10 = prop_node_get(local_16);
		s_type_5cfb45 *local_11 = function_25d690((s_prop_datum *)local_10);
		function_1f86a0(*local_0);
		function_1f4811((s_1f45c1 *)&local_5->unknown4ac);
		local_5->unknown4ac = 7;
		*(long *)&local_5->unknown4b8 = local_16;
		local_5->unknown4d0 = *local_2;
		local_5->unknown4e4 = local_11->unknown3c != NONE ? local_11->unknown3c : local_10->object_index;
		if (local_5->unknown229)
			local_5->unknown656 = 0;
	}
	bool local_12 = function_1f8a70(*local_0, 0);
	function_265c30(local_16, *local_0, !local_12);
	return local_12;
}
