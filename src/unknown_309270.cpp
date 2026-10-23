// @flags /O2 /GL-
// Havok (third-party physics library): three pseudo-random values from a
// linear congruential sequence, mapped by a lower value and a range.

class random_309270
{
public:
	void function_309270(long seed, float low, float high, float *out);
};

// @retail 0x309270
void random_309270::function_309270(long seed, float low, float high, float *out)
{
	float const range = high - low;
	for (int i = 0; i < 3; i++)
	{
		seed = (seed * 1103515245 + 12345) & 0x7fffffff;
		float const unit = seed * 4.656612873077393e-10f;
		out[i] = range * (unit + low);
	}
}
