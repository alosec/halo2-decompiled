#if H2_NATIVE_PORT

// @flags /O2 /Gr

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

// Native first-person weapon state and its model marker cache are not owned
// yet. Reporting no marker preserves the caller's recovered world-marker
// fallback without fabricating a local-user or weapon view.
struct s_first_person_marker;

// @retail 0x166244
short first_person_weapon_get_markers_internal(long, long,
	s_first_person_marker *, short)
{
	return 0;
}

// @retail 0x16640f
bool first_person_weapon_get_marker(long object_index, long marker_name,
	point3f *position, vector3f *forward, vector3f *up)
{
	(void)object_index;
	(void)marker_name;
	(void)position;
	(void)forward;
	(void)up;
	return false;
}

#endif
