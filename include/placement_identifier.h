/* PLACEMENT_IDENTIFIER.H: the identifier a scenario placement gives its object
   (unique id, origin bsp, type, source), kept in the placement and at +0xa4 of
   the object, and the comparison the placement lookup (0xb7a40) and the object
   lookup (0xbf760) inline */

#ifndef PLACEMENT_IDENTIFIER_H
#define PLACEMENT_IDENTIFIER_H

struct s_scenario_identifier_ab
{
	long unique_id;
	short origin_bsp;
	char type;
	char source;
};
typedef char assert_scenario_identifier_ab_size[(sizeof(s_scenario_identifier_ab) == 8) ? 1 : -1];

/* true when the unique id, type and source agree; the origin bsp counts only
   for source 0 */
static inline bool placement_identifier_equal(s_scenario_identifier_ab const *identifier, s_scenario_identifier_ab const *other)
{
	bool match = true;
	match &= identifier->unique_id == other->unique_id;
	match &= identifier->type == other->type;
	match &= identifier->source == other->source;
	if (match && identifier->source == 0)
		match &= identifier->origin_bsp == other->origin_bsp;
	return match;
}

#endif
