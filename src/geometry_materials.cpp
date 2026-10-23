// @flags /O2 /Gr
/* GEOMETRY_MATERIALS.CPP: the types of geometry parts, and a material's
   bitmap (both read by the lightmap sampling of 0xd0690). The names stay
   placeholders: contributors' code calls both functions. */

#include "unknown_11c920.h"
#include "globals.h"
#include "geometry_materials.h"

/* true for the part types 1 to 3 and any type above 5 */
static inline bool geometry_part_type_is_not_4_or_5(long type)
{
	switch (type)
	{
	case 1:
	case 2:
	case 3:
		return true;
	case 4:
	case 5:
		return false;
	}
	return true;
}

/* whether a geometry part's type (the first word of a 0x48-byte part) is
   4 or 5 */
// @retail 0x13d9f0
bool function_13d9f0(long type)
{
	return !geometry_part_type_is_not_4_or_5(type);
}

/* the bitmap tag of material's shader that its first bitmap index picks, or
   NONE */
// @retail 0x13da30
long function_13da30(s_13da30_owner const *material)
{
	long result = NONE;
	if (material && material->shader_tag_index != NONE)
	{
		s_material_shader_data *data = ((s_material_shader *)g_4e3b44[material->shader_tag_index & 0xffff].bytes)->data;
		result = *data->bitmap_index_table;
		if (result != NONE)
		{
			result = data->bitmaps[result].bitmap_tag_index;
		}
	}
	return result;
}
