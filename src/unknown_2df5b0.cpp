// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out:
// the base's values, then one field of its own.
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

struct s_2df5b0 : s_2fc4e0
{
	long field_8;
};

class class_2df5b0 : public class_2fc4e0
{
public:
	long field_10;

	void function_2df5b0(s_2df5b0 *out) const;
};

// @retail 0x2df5b0
void class_2df5b0::function_2df5b0(s_2df5b0 *out) const
{
	function_2fc4e0(out);
	out->field_8 = field_10;
}
