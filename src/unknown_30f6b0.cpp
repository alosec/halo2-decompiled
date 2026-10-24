// @flags /O2 /GL-
// Havok (third-party physics library): the pairs of entities waiting to have
// their islands joined: each pair in different islands has them joined, with
// the first island guarded meanwhile; then the list is emptied.

struct s_278e80_state;
void __cdecl function_278eb0(void *context);
void __cdecl function_278ed0(s_278e80_state *state);

struct entity_30f6b0
{
	char unknown0[0x44];
	void *island;
};

struct pair_30f6b0
{
	entity_30f6b0 *a;
	entity_30f6b0 *b;
};

class class_30f030
{
public:
	char unknown0[0x20];
	pair_30f6b0 *pending;
	int pending_count;

	void *function_30f030(void *island, void *other);
	void function_30f6b0();
};

// @retail 0x30f6b0
void class_30f030::function_30f6b0()
{
	for (int i = 0; i < pending_count; i++)
	{
		pair_30f6b0 *pair = &pending[i];
		void *island = pair->a->island;
		void *other = pair->b->island;
		if (island != other)
		{
			function_278eb0(island);
			function_30f030(island, other);
			function_278ed0((s_278e80_state *)island);
		}
	}
	pending_count = 0;
}
