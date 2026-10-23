// @flags /O2 /GL-
// Havok (third-party physics library): hands out a number of 0x50-byte
// records, taking freed ones first and then fresh ones from the block.

struct record_30a950
{
	char bytes[0x50];
};

struct list_30a950
{
	record_30a950 **data;
	long size;
};

class pool_30a950
{
public:
	long field_0;
	long field_4;
	record_30a950 **free_records;
	long free_count;
	char pad_10[0x1a0];
	record_30a950 *records;
	long used_count;

	void function_30a950(list_30a950 *out, long count);
};

// @retail 0x30a950
void pool_30a950::function_30a950(list_30a950 *out, long count)
{
	long i = free_count - 1;
	while (count != 0)
	{
		if (i < 0)
		{
			break;
		}
		out->data[out->size] = free_records[i];
		out->size++;
		free_count--;
		count--;
		i--;
	}
	while (count > 0)
	{
		record_30a950 *block = records;
		long index = used_count;
		used_count = index + 1;
		out->data[out->size] = &block[index];
		out->size++;
		count--;
	}
}
