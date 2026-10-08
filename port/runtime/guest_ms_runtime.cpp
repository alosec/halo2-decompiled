/* Microsoft-ABI C++ runtime hooks for the recovered user-interface image. */

extern void __stdcall function_1a4826(void* pointer);

void operator delete(void* pointer, unsigned int) noexcept {
    if (pointer) {
        function_1a4826(pointer);
    }
}
