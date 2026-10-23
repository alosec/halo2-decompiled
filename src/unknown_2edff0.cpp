// @flags /O2 /GL-
// Havok (third-party physics library): keeping the child objects for a pair
// of shapes given the other way round.

struct description_2eda30;
struct typed_2edbd0;
struct context_2edbd0;

class list_2eda30
{
public:
	void function_2edbd0(description_2eda30 const *description, typed_2edbd0 **other, context_2edbd0 const *context);
	void function_2edff0(typed_2edbd0 **other, description_2eda30 const *description, context_2edbd0 const *context);
};

// @retail 0x2edff0
void list_2eda30::function_2edff0(typed_2edbd0 **other, description_2eda30 const *description, context_2edbd0 const *context)
{
	function_2edbd0(description, other, context);
}
