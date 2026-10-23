// @flags /O2 /GL-
// Havok (third-party physics library): clearing a flag in every entry of one
// array and a word in every entry of another.

#include <xmmintrin.h>

struct entry_30a810
{
	char unknown0[0x44];
	bool flag;
	char unknown45[0xb];
};

struct item_30a810
{
	char unknown0[0x10];
	long field_10;
	char unknown14[0xc];
};

struct face_30aac0;

struct edge_30aac0
{
	__m128 const *vertex;
	edge_30aac0 const *next;
	edge_30aac0 const *twin;
	face_30aac0 *face;
};

// a hull's triangle: its plane, its three edges, and a mark: 0 when not
// reached yet, otherwise what the plane test gave
struct face_30aac0
{
	__m128 normal;
	float distance;
	edge_30aac0 edges[3];
	char mark;
};

// a vertex: its position, and an edge leading out of it
struct vertex_30a880
{
	__m128 position;
	edge_30aac0 const *edge;
};

// a face seen as the plane that narrows an interval
class slab_30a770
{
public:
	int function_30a770(__m128 const *point, float *low, float *high) const;
};

class class_30a810
{
public:
	float low;
	float high;
	face_30aac0 **faces;
	int face_count;
	char unknown10[0x1b0 - 0x10];
	entry_30a810 *entries;
	int entry_count;
	char unknown1b8[0x2100 - 0x1b8];
	item_30a810 *items;
	int item_count;

	void clear_marks();
	int function_30a880(int *count, edge_30aac0 const **found, face_30aac0 *face, __m128 const *point);
};

// @retail 0x30a810
void class_30a810::clear_marks()
{
	int i;
	for (i = 0; i < entry_count; i++)
	{
		entries[i].flag = false;
	}
	for (i = 0; i < item_count; i++)
	{
		items[i].field_10 = 0;
	}
}

// visits the faces across the given face's edges that the interval along
// the point's ray has not ruled out yet, noting each edge whose face lies
// beyond; 2 when the interval became empty
// @retail 0x30a880
int class_30a810::function_30a880(int *count, edge_30aac0 const **found, face_30aac0 *face, __m128 const *point)
{
	int result = 0;
	for (int i = 0; i < 3; i++)
	{
		edge_30aac0 const *twin = face->edges[i].twin;
		face_30aac0 *next = twin->face;
		if (!next->mark)
		{
			char const mark = (char)((slab_30a770 const *)next)->function_30a770(point, &low, &high);
			next->mark = mark;
			if (mark == 1)
			{
				result = function_30a880(count, found, next, point);
				if (result == 2)
				{
					break;
				}
			}
			else if (mark == 4)
			{
				result = 2;
				break;
			}
		}
		if (next->mark == 2)
		{
			((vertex_30a880 *)twin->vertex)->edge = twin;
			(*count)++;
			*found = twin;
		}
	}
	face->distance = 3.40282347e+38f;
	faces[face_count] = face;
	face_count++;
	return result;
}
