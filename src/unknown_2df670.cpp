// @flags /O2 /GL-
// Havok (third-party physics library): a constructor that clears the field
// at offset 0x44.

struct class_2df670
{
    char unknown0[0x44];
    long field_44;

    class_2df670();
};

// @retail 0x2df670
class_2df670::class_2df670()
{
    field_44 = 0;
}
