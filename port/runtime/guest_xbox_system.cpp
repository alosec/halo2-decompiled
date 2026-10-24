/* The Xbox system calls the recovered game makes, for whole images: memory,
 * files and drives, video and launch queries, time and signatures. Native host
 * services (port/src/xbox_system.cpp) do the work; the async platform still
 * owns mutexes, semaphores and threads (guest_async_runtime.cpp). */
#include "h2/host_abi.h"
#include <xtl.h>
#include <string.h>

namespace {

HANDLE token_handle(unsigned int token)
{
    return reinterpret_cast<HANDLE>(token);
}

unsigned int handle_token(HANDLE handle)
{
    return reinterpret_cast<unsigned int>(handle);
}

bool is_file(HANDLE handle)
{
    return host_h2_file_is_handle(handle_token(handle)) != 0;
}

constexpr DWORD k_mem_top_down = 0x00100000;
constexpr DWORD k_error_invalid_handle = 6;

/* XC_VIDEO_STANDARD_NTSC_M */
constexpr DWORD k_video_standard_ntsc_m = 1;
/* XC_VIDEO_FLAGS_WIDESCREEN | XC_VIDEO_FLAGS_HDTV_480p */
constexpr DWORD k_video_flags = 0x00000001 | 0x00000002;
/* XC_GAME_REGION_NA */
constexpr DWORD k_game_region_north_america = 0x00000001;

void system_time_from_file_time(unsigned long long value, SYSTEMTIME *out)
{
    const unsigned long long milliseconds = value / 10000ull;
    const unsigned long long within = milliseconds % 86400000ull;
    /* 134774 days separate 1601-01-01 from 1970-01-01, a Thursday */
    const long long days = (long long)(milliseconds / 86400000ull) - 134774;
    out->wDayOfWeek = (WORD)(((days + 4) % 7 + 7) % 7);
    /* days since 1970 to a civil date */
    const long long z = days + 719468;
    const long long era = (z >= 0 ? z : z - 146096) / 146097;
    const long long doe = z - era * 146097;
    const long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const long long mp = (5 * doy + 2) / 153;
    const long long month = mp < 10 ? mp + 3 : mp - 9;
    out->wYear = (WORD)(yoe + era * 400 + (month <= 2));
    out->wMonth = (WORD)month;
    out->wDay = (WORD)(doy - (153 * mp + 2) / 5 + 1);
    out->wHour = (WORD)(within / 3600000ull);
    out->wMinute = (WORD)(within / 60000ull % 60);
    out->wSecond = (WORD)(within / 1000ull % 60);
    out->wMilliseconds = (WORD)(within % 1000);
}

} // namespace

/* memory */

extern "C" void *WINAPI VirtualAlloc(void *address, DWORD size, DWORD allocation_type,
                                       DWORD protection)
{
    (void)protection;
    if (address)
        return address; /* committing inside a reservation the title already owns */
    return reinterpret_cast<void *>(host_h2_xbox_memory_alloc(size, 0, 0,
        (allocation_type & k_mem_top_down) != 0));
}

extern "C" BOOL WINAPI VirtualFree(void *address, DWORD size, DWORD free_type)
{
    (void)size;
    (void)free_type;
    return host_h2_xbox_memory_free(reinterpret_cast<unsigned int>(address));
}

extern "C" void *__stdcall XPhysicalAlloc(DWORD size, DWORD physical_address, DWORD alignment,
                                            DWORD protection)
{
    (void)physical_address;
    (void)protection;
    return reinterpret_cast<void *>(host_h2_xbox_memory_alloc(size, alignment, 1, 0));
}

extern "C" void __stdcall XPhysicalFree(void *address)
{
    host_h2_xbox_memory_free(reinterpret_cast<unsigned int>(address));
}

extern "C" DWORD __stdcall XPhysicalSize(void *address)
{
    return host_h2_xbox_memory_size(reinterpret_cast<unsigned int>(address));
}

extern "C" void __stdcall XPhysicalProtect(void *address, DWORD size, DWORD protection)
{
    (void)address;
    (void)size;
    (void)protection;
}

extern "C" void *__stdcall MmAllocateContiguousMemory(DWORD size)
{
    return reinterpret_cast<void *>(host_h2_xbox_memory_alloc(size, 0, 1, 0));
}

extern "C" void __stdcall MmFreeContiguousMemory(void *base)
{
    host_h2_xbox_memory_free(reinterpret_cast<unsigned int>(base));
}

extern "C" LPVOID WINAPI XMemAllocDefault(SIZE_T size, DWORD attributes)
{
    (void)attributes;
    return reinterpret_cast<void *>(host_h2_xbox_memory_alloc(size, 0, 0, 0));
}

extern "C" void WINAPI GlobalMemoryStatus(MEMORYSTATUS *status)
{
    memset(status, 0, sizeof(*status));
    status->dwLength = sizeof(*status);
    status->dwTotalPhys = 64u << 20;
    status->dwAvailPhys = 32u << 20;
    status->dwTotalVirtual = 2048u << 20;
    status->dwAvailVirtual = 1024u << 20;
}

/* errors */

extern "C" DWORD __stdcall GetLastError(void)
{
    return host_h2_last_error();
}

extern "C" void WINAPI SetLastError(DWORD error)
{
    host_h2_set_last_error(error);
}

/* files */

extern "C" HANDLE WINAPI CreateFileA(LPCSTR path, DWORD access, DWORD share_mode, void *security,
                                       DWORD disposition, DWORD flags, HANDLE template_file)
{
    (void)share_mode;
    (void)security;
    (void)flags;
    (void)template_file;
    unsigned int mode = 0;
    if (access & (GENERIC_READ | 0x1))
        mode |= 1;
    if (access & (GENERIC_WRITE | 0x2))
        mode |= 2;
    if (!mode)
        mode = 1;
    return token_handle(host_h2_file_open(path, mode, disposition));
}

extern "C" BOOL WINAPI ReadFile(HANDLE file, void *buffer, DWORD bytes_to_read, DWORD *bytes_read,
                                void *overlapped)
{
    (void)overlapped;
    unsigned int count = 0;
    BOOL result;
    if (is_file(file))
        result = host_h2_file_read(handle_token(file), buffer, bytes_to_read, &count);
    else
        result = host_h2_async_read_file(handle_token(file), buffer, bytes_to_read, &count);
    if (bytes_read)
        *bytes_read = count;
    return result;
}

extern "C" BOOL WINAPI WriteFile(HANDLE file, LPCVOID buffer, DWORD bytes_to_write,
                                 DWORD *bytes_written, void *overlapped)
{
    (void)overlapped;
    unsigned int count = 0;
    const BOOL result = host_h2_file_write(handle_token(file), buffer, bytes_to_write, &count);
    if (bytes_written)
        *bytes_written = count;
    return result;
}

extern "C" DWORD WINAPI SetFilePointer(HANDLE file, LONG distance, LONG *distance_high,
                                       DWORD method)
{
    if (!is_file(file))
    {
        /* the async platform's cache files */
        DWORD position = 0;
        return host_h2_async_set_file_pointer(handle_token(file), distance,
                                              reinterpret_cast<int *>(distance_high), method,
                                              reinterpret_cast<unsigned int *>(&position))
                   ? position
                   : INVALID_SET_FILE_POINTER;
    }
    long long offset = distance_high ? ((long long)*distance_high << 32) | (unsigned long)distance
                                     : (long long)distance;
    unsigned long long position = 0;
    if (!host_h2_file_seek(handle_token(file), offset, method, &position))
        return INVALID_SET_FILE_POINTER;
    if (distance_high)
        *distance_high = (LONG)(position >> 32);
    return (DWORD)position;
}

extern "C" DWORD WINAPI GetFileSize(HANDLE file, DWORD *size_high)
{
    unsigned long long size = 0;
    if (!host_h2_file_size(handle_token(file), &size))
    {
        host_h2_set_last_error(k_error_invalid_handle);
        return INVALID_FILE_SIZE;
    }
    if (size_high)
        *size_high = (DWORD)(size >> 32);
    return (DWORD)size;
}

extern "C" BOOL WINAPI SetEndOfFile(HANDLE file)
{
    return host_h2_file_truncate(handle_token(file));
}

extern "C" BOOL WINAPI FlushFileBuffers(HANDLE file)
{
    return host_h2_file_flush(handle_token(file));
}

extern "C" BOOL WINAPI CloseHandle(HANDLE handle)
{
    if (host_h2_file_close(handle_token(handle)))
        return TRUE;
    return host_h2_async_close(handle_token(handle));
}

extern "C" DWORD WINAPI GetFileAttributesA(LPCSTR path)
{
    return host_h2_file_attributes(path, 0, 0);
}

extern "C" BOOL WINAPI GetFileAttributesExA(LPCSTR path, GET_FILEEX_INFO_LEVELS level,
                                            LPVOID information)
{
    (void)level;
    unsigned long long size = 0;
    unsigned long long write_time = 0;
    const unsigned int attributes = host_h2_file_attributes(path, &size, &write_time);
    if (attributes == 0xffffffffu)
        return FALSE;
    WIN32_FILE_ATTRIBUTE_DATA *data = (WIN32_FILE_ATTRIBUTE_DATA *)information;
    memset(data, 0, sizeof(*data));
    data->dwFileAttributes = attributes;
    data->ftCreationTime.dwLowDateTime = (DWORD)write_time;
    data->ftCreationTime.dwHighDateTime = (DWORD)(write_time >> 32);
    data->ftLastAccessTime = data->ftCreationTime;
    data->ftLastWriteTime = data->ftCreationTime;
    data->nFileSizeHigh = (DWORD)(size >> 32);
    data->nFileSizeLow = (DWORD)size;
    return TRUE;
}

extern "C" BOOL WINAPI SetFileAttributesA(LPCSTR path, DWORD attributes)
{
    (void)attributes;
    return host_h2_file_attributes(path, 0, 0) != 0xffffffffu;
}

extern "C" BOOL WINAPI DeleteFileA(LPCSTR path)
{
    return host_h2_file_delete(path);
}

extern "C" BOOL WINAPI CreateDirectoryA(LPCSTR path, void *security)
{
    (void)security;
    return host_h2_directory_create(path);
}

extern "C" BOOL WINAPI RemoveDirectoryA(LPCSTR path)
{
    return host_h2_directory_remove(path);
}

extern "C" HANDLE WINAPI FindFirstFileA(LPCSTR pattern, WIN32_FIND_DATAA *data)
{
    return token_handle(host_h2_find_first(pattern, data));
}

extern "C" BOOL WINAPI FindNextFileA(HANDLE find, WIN32_FIND_DATAA *data)
{
    return host_h2_find_next(handle_token(find), data);
}

extern "C" BOOL WINAPI FindClose(HANDLE find)
{
    return host_h2_file_close(handle_token(find));
}

/* drives and launch */

extern "C" BOOL WINAPI XMountSecondaryUtilityDrive(void)
{
    return TRUE;
}

extern "C" DWORD WINAPI XGetLaunchInfo(DWORD *type, LAUNCH_DATA *data)
{
    /* not launched with title data: ERROR_NOT_FOUND */
    if (type)
        *type = 0xffffffffu;
    if (data)
        memset(data, 0, sizeof(*data));
    return 1168;
}

extern "C" DWORD WINAPI XLaunchNewImage(LPCSTR image_path, PLAUNCH_DATA data)
{
    (void)image_path;
    (void)data;
    return 0x80004005u;
}

extern "C" void *WINAPI XLoadSection(const char *name)
{
    (void)name;
    return reinterpret_cast<void *>(1); /* every section is resident natively */
}

extern "C" void WINAPI XFreeSection(const char *name)
{
    (void)name;
}

/* video and console settings */

extern "C" DWORD WINAPI XGetVideoStandard(void)
{
    return k_video_standard_ntsc_m;
}

extern "C" DWORD WINAPI XGetVideoFlags(void)
{
    return k_video_flags;
}

extern "C" DWORD WINAPI XGetGameRegion(void)
{
    return k_game_region_north_america;
}

extern "C" unsigned long __stdcall XGetAutoLogonFlag(void)
{
    return 0;
}

/* time */

extern "C" void __stdcall GetSystemTimeAsFileTime(FILETIME *time)
{
    const unsigned long long now = host_h2_system_time();
    time->dwLowDateTime = (DWORD)now;
    time->dwHighDateTime = (DWORD)(now >> 32);
}

extern "C" BOOL WINAPI FileTimeToLocalFileTime(const FILETIME *file_time, FILETIME *local_file_time)
{
    unsigned long long value = ((unsigned long long)file_time->dwHighDateTime << 32) |
                               file_time->dwLowDateTime;
    value += (long long)host_h2_local_time_offset() * 10000000ll;
    local_file_time->dwLowDateTime = (DWORD)value;
    local_file_time->dwHighDateTime = (DWORD)(value >> 32);
    return TRUE;
}

extern "C" BOOL WINAPI FileTimeToSystemTime(const FILETIME *file_time, SYSTEMTIME *system_time)
{
    system_time_from_file_time(((unsigned long long)file_time->dwHighDateTime << 32) |
                               file_time->dwLowDateTime, system_time);
    return TRUE;
}

extern "C" void WINAPI GetLocalTime(SYSTEMTIME *time)
{
    const unsigned long long now = host_h2_system_time() +
                                   (long long)host_h2_local_time_offset() * 10000000ll;
    system_time_from_file_time(now, time);
}

extern "C" int __stdcall QueryPerformanceCounter(__int64 *count)
{
    *count = (__int64)host_h2_performance_counter();
    return 1;
}

extern "C" int __stdcall QueryPerformanceFrequency(__int64 *frequency)
{
    *frequency = 1000000000ll;
    return 1;
}

extern "C" DWORD __stdcall GetCurrentThreadId(void)
{
    return 1;
}

extern "C" LONG WINAPI XSaveFloatingPointStateForDpc(void)
{
    return 0;
}

extern "C" LONG WINAPI XRestoreFloatingPointStateForDpc(void)
{
    return 0;
}

/* signatures: saves and profiles only need to verify their own signatures,
 * so a keyed 20-byte FNV-1a digest stands in for the console's HMAC */

namespace {

struct s_signature_context
{
    unsigned long long lanes[3];
    bool used;
};

s_signature_context g_signature_contexts[8];

} // namespace

extern "C" HANDLE WINAPI XCalculateSignatureBegin(DWORD flags)
{
    for (int index = 0; index < 8; ++index)
    {
        if (!g_signature_contexts[index].used)
        {
            s_signature_context &context = g_signature_contexts[index];
            context.used = true;
            context.lanes[0] = 0xcbf29ce484222325ull ^ flags;
            context.lanes[1] = 0x84222325cbf29ce4ull;
            context.lanes[2] = 0x2325cbf29ce48422ull;
            return token_handle(0x4a000000u + index);
        }
    }
    return INVALID_HANDLE_VALUE;
}

extern "C" DWORD WINAPI XCalculateSignatureUpdate(HANDLE handle, const BYTE *data, DWORD size)
{
    const unsigned int index = handle_token(handle) - 0x4a000000u;
    if (index >= 8 || !g_signature_contexts[index].used)
        return 0x80004005u;
    s_signature_context &context = g_signature_contexts[index];
    for (DWORD offset = 0; offset < size; ++offset)
    {
        for (int lane = 0; lane < 3; ++lane)
        {
            context.lanes[lane] ^= (unsigned long long)(data[offset] + lane);
            context.lanes[lane] *= 0x100000001b3ull;
        }
    }
    return 0;
}

extern "C" DWORD WINAPI XCalculateSignatureEnd(HANDLE handle, XCALCSIG_SIGNATURE *signature)
{
    const unsigned int index = handle_token(handle) - 0x4a000000u;
    if (index >= 8 || !g_signature_contexts[index].used)
        return 0x80004005u;
    s_signature_context &context = g_signature_contexts[index];
    memcpy(signature->bytes, context.lanes, sizeof(signature->bytes));
    context.used = false;
    return 0;
}
