extern "C" const char h2_extract_constant[] = "retained recovered constant";

extern "C" __attribute__((noinline)) const char* h2_extract_selected()
{
    return h2_extract_constant;
}

extern "C" const char* h2_extract_alias()
    __attribute__((alias("h2_extract_selected")));

extern "C" int h2_extract_unrelated()
{
    return 7;
}
