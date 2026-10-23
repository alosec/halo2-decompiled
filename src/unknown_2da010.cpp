// @flags /O2 /GL-
// Havok (third-party physics library): clearing eight words when given a
// place to clear.

struct s_2da010
{
	long values[8];
};

// @retail 0x2da010
void __cdecl function_2da010(s_2da010 *block)
{
	if (block)
	{
		block->values[0] = 0;
		block->values[1] = 0;
		block->values[2] = 0;
		block->values[3] = 0;
		block->values[4] = 0;
		block->values[5] = 0;
		block->values[6] = 0;
		block->values[7] = 0;
	}
}
