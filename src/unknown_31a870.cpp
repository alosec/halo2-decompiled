// @flags /O2 /GL-
// Havok (third-party physics library): a motion object built on 0x31ad80's
// from the same two vectors, with its own table.

class class_31ad80
{
public:
	class_31ad80(void const *position, void const *orientation);
	virtual void slot_0() {}
};

class class_31a870 : public class_31ad80
{
public:
	class_31a870(void const *position, void const *orientation);
};

// @retail 0x31a870
class_31a870::class_31a870(void const *position, void const *orientation)
	: class_31ad80(position, orientation)
{
}
