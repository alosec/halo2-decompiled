// Native name bridge for retail 0x0b9ce0.  The recovered implementation is
// named object_or_parent_hidden in unknown_0b8bd0.cpp, while some recovered
// callers still use its address-derived name.

bool object_or_parent_hidden(long object_index);

bool function_b9ce0(long object_index)
{
    return object_or_parent_hidden(object_index);
}
