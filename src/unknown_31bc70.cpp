// @flags /O2 /GL-
// Havok (third-party physics library): a function that gives back the object
// it is passed (the constructor at 0x31bc60 calls it, and game code).

// @retail 0x31bc70
void *__fastcall function_31bc70(void *arg_0)
{
	return arg_0;
}
