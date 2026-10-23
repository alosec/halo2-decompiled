// @flags /O2 /GL-
// Havok (third-party physics library): an object that hands back its
// entries' ids to their owner and then destroys itself.

class class_3029d0_owner;
struct s_3029d0_entry
{
	char unknown0[0x1c];
	unsigned short id;
	char unknown1e[0x12];
};

void __cdecl function_3029d0(s_3029d0_entry *entries, int *count, class_3029d0_owner *owner);

class class_2ee2b0
{
public:
	virtual void destroy(unsigned int flags) = 0;

	long field_4;
	class_3029d0_owner *owner;
	char field_c[0xc];
	int count;
	char field_1c[0x14];
	s_3029d0_entry entries[1];

	void function_2ee2b0();
};

// @retail 0x2ee2b0
void class_2ee2b0::function_2ee2b0()
{
	function_3029d0(entries, &count, owner);
	destroy(1);
}
