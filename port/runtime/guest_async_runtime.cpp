#include "h2/host_abi.h"
#include <xtl.h>

namespace {

unsigned int handle_token(HANDLE handle)
{
    return reinterpret_cast<unsigned int>(handle);
}

HANDLE token_handle(unsigned int token)
{
    return reinterpret_cast<HANDLE>(token);
}

} // namespace

extern "C" HANDLE WINAPI CreateMutexA(void *attributes, BOOL initial_owner,
                                        const char *name)
{
    if (attributes || name)
        return 0;
    return token_handle(host_h2_async_create_mutex(initial_owner));
}

extern "C" HANDLE WINAPI CreateSemaphoreA(void *attributes, LONG initial_count,
                                            LONG maximum_count, const char *name)
{
    if (attributes || name)
        return 0;
    return token_handle(host_h2_async_create_semaphore(initial_count, maximum_count));
}

extern "C" DWORD WINAPI WaitForSingleObject(HANDLE handle, DWORD milliseconds)
{
    return host_h2_async_wait(handle_token(handle), milliseconds);
}

extern "C" BOOL WINAPI ReleaseMutex(HANDLE mutex)
{
    return host_h2_async_release_mutex(handle_token(mutex));
}

extern "C" BOOL WINAPI ReleaseSemaphore(HANDLE semaphore, LONG release_count,
                                          LONG *previous_count)
{
    return host_h2_async_release_semaphore(handle_token(semaphore), release_count,
                                            reinterpret_cast<int *>(previous_count));
}

extern "C" DWORD WINAPI SetFilePointer(HANDLE file, LONG distance,
                                         LONG *distance_high, DWORD method)
{
    DWORD position = 0;
    return host_h2_async_set_file_pointer(handle_token(file), distance,
                                           reinterpret_cast<int *>(distance_high), method,
                                           reinterpret_cast<unsigned int *>(&position))
               ? position
               : 0xffffffffu;
}

extern "C" BOOL WINAPI ReadFile(HANDLE file, void *buffer, DWORD bytes_to_read,
                                  DWORD *bytes_read, void *overlapped)
{
    if (overlapped)
        return FALSE;
    return host_h2_async_read_file(handle_token(file), buffer, bytes_to_read,
                                   reinterpret_cast<unsigned int *>(bytes_read));
}

extern "C" BOOL WINAPI SetThreadPriority(HANDLE thread, int priority)
{
    return host_h2_async_set_thread_priority(handle_token(thread), priority);
}

extern "C" BOOL WINAPI GetExitCodeThread(HANDLE thread, DWORD *exit_code)
{
    return host_h2_async_thread_exit_code(handle_token(thread),
                                          reinterpret_cast<unsigned int *>(exit_code));
}

extern "C" BOOL WINAPI CloseHandle(HANDLE handle)
{
    return host_h2_async_close(handle_token(handle));
}

extern "C" BOOL __stdcall SwitchToThread(void)
{
    host_h2_async_yield();
    return TRUE;
}

extern "C" int h2_native_async_try_admit(void)
{
    return host_h2_async_try_admit();
}

extern "C" int h2_native_async_publish_admitted(void)
{
    return host_h2_async_publish_admitted();
}

extern "C" int h2_native_async_abort_admitted(void)
{
    return host_h2_async_abort_admitted();
}
