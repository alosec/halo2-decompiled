// @flags /O2 /GL-
// Havok (third-party physics library): reports failure through its
// argument.

struct class_2fe820
{
    void function_2fe820(bool *result);
};

// @retail 0x2fe820
void class_2fe820::function_2fe820(bool *result)
{
    *result = false;
}
