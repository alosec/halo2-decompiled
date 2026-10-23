// @flags /O2 /GL- /GX
// Havok (third-party physics library): a hull builder started from four
// points: room for 100 face pointers, 100 faces and 55 vertices held inside
// it, the four points as its first vertices, and the starting tetrahedron's
// faces built over them.
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"

struct face_30aac0;

struct vertex_309a50
{
	__m128 position;
	void *horizon_edge;
	char unknown14[0xc];
};

struct face_309a50
{
	__m128 normal;
	char unknown10[0x40];
};

// an array of `type` with room for `count` elements held inside it
#define EMBEDDED_ARRAY_309A50(name, type, count, tag) \
	struct name \
	{ \
		type *data; \
		int size; \
		int capacity_and_flags; \
		type embedded[count]; \
	\
		name() \
		{ \
			data = embedded; \
			size = 0; \
			capacity_and_flags = 0x80000000 | count; \
		} \
		~name() \
		{ \
			if ((capacity_and_flags & 0x80000000) == 0) \
			{ \
				g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(type), tag); \
			} \
		} \
	};

EMBEDDED_ARRAY_309A50(face_pointers_309a50, face_30aac0 *, 100, 0x12)
EMBEDDED_ARRAY_309A50(faces_309a50, face_309a50, 100, 0x12)
EMBEDDED_ARRAY_309A50(vertices_309a50, vertex_309a50, 55, 0x12)

class class_30a810
{
public:
	char unknown0[8];
	face_pointers_309a50 face_pointers;
	faces_309a50 faces;
	vertices_309a50 vertices;

	class_30a810(__m128 const *a, __m128 const *b, __m128 const *c, __m128 const *d);
	void function_30ad20();
};

// @retail 0x309a50
class_30a810::class_30a810(__m128 const *a, __m128 const *b, __m128 const *c, __m128 const *d)
{
	vertices.size = 4;
	vertices.data[0].position = *a;
	vertices.data[0].horizon_edge = 0;
	vertices.data[1].position = *b;
	vertices.data[1].horizon_edge = 0;
	vertices.data[2].position = *c;
	vertices.data[2].horizon_edge = 0;
	vertices.data[3].position = *d;
	vertices.data[3].horizon_edge = 0;
	function_30ad20();
}
