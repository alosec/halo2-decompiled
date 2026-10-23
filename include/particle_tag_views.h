#ifndef PARTICLE_TAG_VIEWS_H
#define PARTICLE_TAG_VIEWS_H

/* the tag data of particles ('prt3') and particle models ('PRTM'), as the
   two particle definition interfaces (src/visual_effects_particle_interfaces.cpp)
   read them */

#include <stddef.h>

struct s_effect_particle_system_definition;

/* the flags every particle and particle model definition starts with; the
   render functions (src/unknown_2243c0.cpp) take a definition by this name */
struct s_interface_definition
{
	unsigned long flag0 : 1;
	unsigned long flag1 : 1;
	unsigned long flag2 : 1;
	unsigned long flag3 : 1;
	unsigned long flag4 : 1;
	unsigned long flag5 : 1;
	unsigned long flag6 : 1;
	unsigned long multiplied : 1;
	unsigned long tinted : 1;
	unsigned long flag9 : 1;
	unsigned long flag10 : 1;
	unsigned long flag11 : 1;
	unsigned long flag12 : 1;
	unsigned long flag13 : 1;
	unsigned long flag14 : 1;
	unsigned long flag15 : 1;
	unsigned long flags_high : 16;
};

/* the head of a function's data: constant functions (type 0) carry their
   range here, the others run from 0 to 1 */
struct s_function_header
{
	byte unknown00;
	byte unknown01 : 4;
	byte type : 4;
	byte unknown02[2];
	real minimum;
	real maximum;
};

/* a function held in tag data (size, then address) */
struct s_function_data
{
	long size;
	s_function_header *header;

	/* the larger end of the function's output range */
	real get_range_maximum()
	{
		real minimum = (header->type == 0) ? header->minimum : 0.0f;
		real maximum = (header->type == 0) ? header->maximum : 1.0f;
		return (minimum > maximum) ? minimum : maximum;
	}
};

/* a particle ('prt3') */
struct s_particle_tag_view : s_interface_definition
{
	byte unknown04[0x44 - 0x04];
	s_function_data function44;
	byte unknown4c[0x6c - 0x4c];
	long value6c;
	long value70;
	long value74;
	long value78;
	long name_count;
	long const *names;
	long particle_system_count;
	s_effect_particle_system_definition *system_entries;
	long shader_count;
	long const *shaders;
};

/* a particle model ('PRTM') */
struct s_particle_model_definition : s_interface_definition
{
	byte unknown04[0x18 - 0x04];
	dword render_group_tag;
	long render_tag_index;
	byte unknown20[0x60 - 0x20];
	long value60;
	long value64;
	long value68;
	long value6c;
	long name_count;
	long const *names;
	long particle_system_count;
	s_effect_particle_system_definition *system_entries;
};

/* the tag a particle model renders: it holds a pointer to its shader
   reference at +0x24 */
struct s_particle_model_render_tag
{
	byte unknown00[0x24];
	long const *shader_reference;
};

typedef char assert_interface_definition_size[(sizeof(s_interface_definition) == 4) ? 1 : -1];
typedef char assert_function_header_size[(sizeof(s_function_header) == 0xc) ? 1 : -1];
typedef char assert_particle_function_offset[(offsetof(s_particle_tag_view, function44) == 0x44) ? 1 : -1];
typedef char assert_particle_names_offset[(offsetof(s_particle_tag_view, name_count) == 0x7c) ? 1 : -1];
typedef char assert_particle_shaders_offset[(offsetof(s_particle_tag_view, shaders) == 0x90) ? 1 : -1];
typedef char assert_particle_model_render_offset[(offsetof(s_particle_model_definition, render_tag_index) == 0x1c) ? 1 : -1];
typedef char assert_particle_model_systems_offset[(offsetof(s_particle_model_definition, system_entries) == 0x7c) ? 1 : -1];

#endif
