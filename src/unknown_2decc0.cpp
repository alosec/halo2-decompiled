// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out,
// which is only the base's.
struct s_2fc4e0
{
	long field_0;
	long field_4;
};

class class_2fc4e0
{
public:
	char field_0[0xc];
	long field_c;

	void function_2fc4e0(s_2fc4e0 *out) const;
};

class class_2decc0 : public class_2fc4e0
{
public:
	void function_2decc0(s_2fc4e0 *out) const;
};

// @retail 0x2decc0
void class_2decc0::function_2decc0(s_2fc4e0 *out) const
{
	function_2fc4e0(out);
}
