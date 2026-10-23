// @flags /O2 /GL-
// Havok (third-party physics library): a size from a count of 24-byte
// entries after a 0x24-byte header.

class class_2e0000
{
public:
	char unknown0[0x44];
	long field_44;
	char unknown48[0x28];
	long field_70;

	long entry_total() const;
};

// @retail 0x2e0000
long class_2e0000::entry_total() const
{
	return (field_44 - field_70) * 24 + 0x24;
}
