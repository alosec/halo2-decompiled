// @flags /O2 /arch:SSE /Gr
/* ONLINE_MATCH_ATTRIBUTES.CPP: the Live matchmaking attributes
   (include/online_attributes.h): the search parameters built from a search's
   ranges, the session attributes built from its properties, and the
   properties read back from a search result */

#include "unknown_11c920.h"
#include "online_attributes.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>

/* the seven integer properties of a search result, as longs */
struct s_long7
{
	long v[7];
};

/* the seven properties of a search result from its seven parsed attributes */
// @retail 0xb4a40
void online_match_properties_from_attributes(s_long7 *properties, const __int64 *attributes)
{
	memset(properties, 0, sizeof(s_long7));
	properties->v[0] = (long)attributes[0];
	properties->v[1] = (long)attributes[1];
	properties->v[2] = (long)attributes[2];
	properties->v[3] = (long)attributes[3];
	properties->v[4] = (long)attributes[4];
	properties->v[5] = (long)attributes[5];
	properties->v[6] = (long)attributes[6];
}

/* the twelve search parameters of a search: two values, an optional minimum and maximum, and seven flags, of which the one for its count (one to seven, or eight and more) is set; returns their count */
// @retail 0xb4a90
long online_match_search_build_parameters(s_entry_pair *parameters, const s_range_input *input)
{
	long i;

	parameters[0].attribute_id = X_ATTRIBUTE_DATATYPE_INTEGER;
	parameters[0].changed = 0;
	parameters[0].value = input->x;
	parameters[1].attribute_id = X_ATTRIBUTE_DATATYPE_INTEGER;
	parameters[1].changed = 0;
	parameters[1].value = input->y;
	if (input->has_min)
	{
		parameters[2].attribute_id = X_ATTRIBUTE_DATATYPE_INTEGER;
		parameters[2].value = input->min;
	}
	else
	{
		parameters[2].attribute_id = X_ATTRIBUTE_DATATYPE_NULL;
		parameters[2].value = 0;
	}
	parameters[2].changed = 0;
	if (input->has_max)
	{
		parameters[3].attribute_id = X_ATTRIBUTE_DATATYPE_INTEGER;
		parameters[3].value = input->max;
	}
	else
	{
		parameters[3].attribute_id = X_ATTRIBUTE_DATATYPE_NULL;
		parameters[3].value = 0;
	}
	parameters[3].changed = 0;
	i = 1;
	/* parameters 4 to 10 through a pointer at their value; an indexed loop
	   does not match */
	long *value = (long *)&parameters[4].value;
	do
	{
		if (input->has_count && input->count == i)
		{
			value[-2] = X_ATTRIBUTE_DATATYPE_INTEGER;
			*(__int64 *)value = 1;
		}
		else
		{
			value[-2] = X_ATTRIBUTE_DATATYPE_NULL;
			*(__int64 *)value = 0;
		}
		value[-1] = 0;
		value += 4;
		++i;
	} while (i < 8);
	if (input->has_count && input->count >= 8)
	{
		parameters[11].attribute_id = X_ATTRIBUTE_DATATYPE_INTEGER;
		parameters[11].value = 1;
	}
	else
	{
		parameters[11].attribute_id = X_ATTRIBUTE_DATATYPE_NULL;
		parameters[11].value = 0;
	}
	parameters[11].changed = 0;

	return 12;
}

/* the fifteen session attributes of a session's properties: six values, the flags, and each of the flags' first seven bits and whether any other is set; returns their count */
// @retail 0xb4b80
long online_match_session_build_attributes(long count, s_property_entry *attributes, const s_property_input *properties)
{
	memset(attributes, 0, count * sizeof(s_property_entry));

	attributes[0].attribute_id = 1;
	attributes[0].changed = 1;
	attributes[0].value = properties->v0;
	attributes[1].attribute_id = 2;
	attributes[1].changed = 1;
	attributes[1].value = properties->v1;
	attributes[2].attribute_id = 3;
	attributes[2].changed = 1;
	attributes[2].value = properties->v2;
	attributes[3].attribute_id = 5;
	attributes[3].changed = 1;
	attributes[3].value = properties->v3;
	attributes[4].attribute_id = 6;
	attributes[4].changed = 1;
	attributes[4].value = properties->v4;
	attributes[5].attribute_id = 7;
	attributes[5].changed = 1;
	attributes[5].value = properties->v5;
	attributes[6].attribute_id = 8;
	attributes[6].changed = 1;
	attributes[6].value = properties->flags;
	attributes[7].attribute_id = 9;
	attributes[7].changed = 1;
	attributes[7].value = (long)(properties->flags & 1);
	attributes[8].attribute_id = 10;
	attributes[8].changed = 1;
	attributes[8].value = (long)((properties->flags >> 1) & 1);
	attributes[9].attribute_id = 11;
	attributes[9].changed = 1;
	attributes[9].value = (long)((properties->flags >> 2) & 1);
	attributes[10].attribute_id = 12;
	attributes[10].changed = 1;
	attributes[10].value = (long)((properties->flags >> 3) & 1);
	attributes[11].attribute_id = 13;
	attributes[11].changed = 1;
	attributes[11].value = (long)((properties->flags >> 4) & 1);
	attributes[12].attribute_id = 14;
	attributes[12].changed = 1;
	attributes[12].value = (long)((properties->flags >> 5) & 1);
	attributes[13].attribute_id = 15;
	attributes[13].changed = 1;
	attributes[13].value = (long)((properties->flags >> 6) & 1);
	attributes[14].attribute_id = 16;
	attributes[14].changed = 1;
	attributes[14].value = (long)((properties->flags & 0xffffff80) != 0);

	return 15;
}
