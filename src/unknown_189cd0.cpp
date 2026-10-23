// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_189CD0.CPP: plays a looping sound slot's sound, positioned on an
   object or not. Retail keeps the standard convention with all seven
   arguments (callers push seven words, ret 0x1c), though the fourth and
   fifth are unused. */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "sound_records.h"
#include "object_markers.h"
#include "local_cameras.h"

struct s_sound_source_description
{
	char value0;
	byte flags;
	byte unknown02[2];
	point3f position;
	vector3f direction;
	byte unknown1c[8];
	long value24;
};

long function_189ee0(long tag_index);
real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);
void object_dispatch_sound_started(long object_index, long tag_index, long sound_index);
extern s_sound_source_callbacks const g_444b3c;
void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description);
long function_18d5b0(long label);
long function_189fe0(s_sound_request const *request, long tag_index);
long function_189340(long object_index, long tag_index, long audible, s_sound_marker const *marker, s_sound_source_callbacks const *source, real scale, long platform_playback, char const *variant);

// @retail 0x189cd0 standard
void __stdcall function_189cd0(long tag_index, long object_index, real scale, long a, long b, long label, long duration_address)
{
	long const volatile *object_reference = &object_index;
	real *duration = (real *)duration_address;

	if (tag_index != NONE)
	{
		long slot_index = function_189ee0(tag_index);
		if (slot_index != NONE)
		{
			long sound_index;
			s_looping_sound_slot *slot = &g_4ed288->slots[slot_index];
			if (duration)
			{
				*duration = sound_permutation_reference_duration(tag_index, &slot->permutation);
			}
			s_sound_tag *tag = (s_sound_tag *)g_4e3b44[tag_index & 0xffff].bytes;
			s_sound_class_view *sound_class = &((s_sound_globals_view *)g_51ebd4)->classes[tag->class_index];
			s_sound_class_spatialization *spatialization = NULL;
			if (sound_class->spatialization.flags & 7)
				spatialization = &sound_class->spatialization;
			bool positioned = spatialization && (spatialization->flags & 1);

			if (scale < 0.0f)
				scale = 0.0f;
			else if (scale > 1.0f)
				scale = 1.0f;

			long object = *object_reference;
			if (object == NONE && !positioned)
			{
				s_sound_request request;
				request.location.unknown02 = 0;
				request.location.flags = 0x400;
				request.location.audible = 0;
				request.location.requested_audible = 0;
				request.location.scale = scale;
				request.location.unknown08 = 0;
				request.object_index = NONE;
				request.platform_playback = function_18d5b0(label);
				request.variant = (char const *)&slot->permutation;
				request.marker = NULL;
				request.source = &g_444b3c;
				sound_index = function_189fe0(&request, tag_index);
			}
			else
			{
				s_sound_source_description description;
				description.flags = 0;
				if (object != NONE)
				{
					s_object_marker local_4f80fe;
					function_b8d30(object, 0x4000095, &local_4f80fe, 1, false);
					function_1892a0(local_4f80fe.node_index, &local_4f80fe.node_matrix.position,
						&local_4f80fe.node_matrix.forward, tag_index, object, &description);
				}
				if (spatialization)
					description.flags |= 1;
				else
					description.flags &= ~1;
				long playback = function_18d5b0(label);
				long request_object = *object_reference;
				sound_index = function_189340(request_object, tag_index, positioned ? 2 : 1, (s_sound_marker *)&description,
					&g_444b3c, scale, playback, (char const *)&slot->permutation);
				if (sound_index != NONE && request_object != NONE)
				{
					object_dispatch_sound_started(request_object, tag_index, sound_index);
				}
			}
			slot->source_index = sound_index;
			if (sound_index != NONE)
				return;
		}
	}
	if (duration)
	{
		*duration = 0.0f;
	}
}
