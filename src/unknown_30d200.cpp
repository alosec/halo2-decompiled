// @flags /O2 /GL-
// Havok (third-party physics library): three methods that report success
// through their first argument; the other arguments are unused.

struct class_30d200
{
    void function_30d200(bool *result, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5);
    void function_30d210(bool *result, long arg_1, long arg_2, long arg_3);
    void function_30d220(bool *result, long arg_1, long arg_2);
};

// @retail 0x30d200
void class_30d200::function_30d200(bool *result, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5)
{
    *result = true;
}

// @retail 0x30d210
void class_30d200::function_30d210(bool *result, long arg_1, long arg_2, long arg_3)
{
    *result = true;
}

// @retail 0x30d220
void class_30d200::function_30d220(bool *result, long arg_1, long arg_2)
{
    *result = true;
}
