// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4805a4
{
    long unknown0;
};

object_4805a4 g_4805a4;

// @retail 0x301a40
object_4805a4 *__cdecl function_301a40()
{
    return &g_4805a4;
}
