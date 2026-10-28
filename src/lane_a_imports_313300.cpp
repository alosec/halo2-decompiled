// @flags /O2 /Ob1 /GL-
#include "lane_a_constraints.h"
// @retail 0x3137f0
void lane_a_limited_hinge::import_description(lane_a_limited_hinge_description const *d)
{
	import_base(d);
	import_fields(d);
}
// @retail 0x313300
void __cdecl lane_a_import_limited_hinge(lane_a_limited_hinge *object, lane_a_limited_hinge_description const *d)
{
	object->import_description(d);
}
// @retail 0x313b10
void __cdecl lane_a_import_hinge(lane_a_hinge *object, lane_a_hinge_description const *d)
{
	object->import_description(d);
}
