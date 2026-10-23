// @flags /O2 /GL-
// Havok (third-party physics library): passing an object paired with each
// entry of its list, from the last, to the dispatcher's second or first
// handler for the two kinds.

class kind_30b940
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long kind() const { return 0; }
};

struct item_30b940
{
	kind_30b940 *object;
};

struct input_30b940;

typedef void (__cdecl *handler_30b940)(item_30b940 const *first, item_30b940 const *second,
	input_30b940 const *input, long arg_3);

struct dispatcher_30b940
{
	char unknown0[0x118c];
	handler_30b940 first[32][32];
	handler_30b940 second[32][32];
};

struct input_30b940
{
	dispatcher_30b940 *dispatcher;
};

struct world_30b940
{
	char unknown0[0xcc];
	input_30b940 *input;
};

class class_30b940
{
public:
	char unknown0[8];
	world_30b940 *world;
	item_30b940 item;
	char unknown10[0x44];
	long kind;
	char unknown58[0x68];
	item_30b940 **items;
	int item_count;

	void function_30b940(long arg_3);
	void function_30b9b0(long arg_3);
};

// @retail 0x30b940
void class_30b940::function_30b940(long arg_3)
{
	input_30b940 *input = world->input;
	dispatcher_30b940 *dispatcher = input->dispatcher;
	for (int i = item_count - 1; i >= 0; i--)
	{
		item_30b940 *const &entry = items[i];
		long other = entry->object->kind();
		dispatcher->second[kind][other](&item, entry, input, arg_3);
	}
}

// @retail 0x30b9b0
void class_30b940::function_30b9b0(long arg_3)
{
	input_30b940 *input = world->input;
	dispatcher_30b940 *dispatcher = input->dispatcher;
	for (int i = item_count - 1; i >= 0; i--)
	{
		item_30b940 *const &entry = items[i];
		long other = entry->object->kind();
		dispatcher->first[kind][other](&item, entry, input, arg_3);
	}
}
