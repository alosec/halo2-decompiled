// @flags /O2 /GL-
// Havok (third-party physics library): a method that always returns 1 and
// ignores its two arguments.

struct class_2d9840
{
    long function_2d9840(long arg_0, long arg_1);
};

// @retail 0x2d9840
long class_2d9840::function_2d9840(long arg_0, long arg_1)
{
    return 1;
}
