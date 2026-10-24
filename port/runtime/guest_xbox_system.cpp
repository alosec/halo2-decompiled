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

/* the dashboard or another title: there is none to launch, so the run ends;
   the caller would otherwise spin waiting for the reboot */
extern "C" DWORD WINAPI XLaunchNewImage(LPCSTR image_path, PLAUNCH_DATA data)
{
    (void)data;
    host_h2_log(image_path ? "XLaunchNewImage: another title" : "XLaunchNewImage: the dashboard");
    host_h2_exit(3);
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

/* saved games: each save is a directory "<root><12 hex digits>\" on its drive,
 * named from its display name, with the name kept in SaveMeta.xbx */

namespace {

constexpr DWORD k_error_file_not_found = 2;
constexpr DWORD k_error_already_exists = 183;
constexpr DWORD k_error_no_more_files = 18;
constexpr DWORD k_save_name_characters = 64;

void save_directory(const char *root, const wchar_t *name, char *out, size_t size)
{
    unsigned long long hash = 0xcbf29ce484222325ull;
    for (const wchar_t *c = name; c && *c; ++c)
    {
        hash ^= (unsigned short)*c;
        hash *= 0x100000001b3ull;
    }
    static const char digits[] = "0123456789ABCDEF";
    char id[13];
    for (int i = 0; i < 12; ++i)
        id[i] = digits[(hash >> (i * 4)) & 15];
    id[12] = 0;
    size_t length = 0;
    for (const char *c = root; *c && length + 1 < size; ++c)
        out[length++] = *c;
    for (const char *c = id; *c && length + 1 < size; ++c)
        out[length++] = *c;
    if (length + 1 < size)
        out[length++] = '\\';
    out[length] = 0;
}

void save_meta_path(const char *directory, char *out, size_t size)
{
    size_t length = 0;
    for (const char *c = directory; *c && length + 1 < size; ++c)
        out[length++] = *c;
    for (const char *c = "SaveMeta.xbx"; *c && length + 1 < size; ++c)
        out[length++] = *c;
    out[length] = 0;
}

bool read_save_name(const char *directory, wchar_t *name, DWORD characters)
{
    char path[MAX_PATH];
    save_meta_path(directory, path, sizeof(path));
    memset(name, 0, characters * sizeof(wchar_t));
    const unsigned int file = host_h2_file_open(path, 1, OPEN_EXISTING);
    if (file == 0xffffffffu)
        return false;
    unsigned int read = 0;
    host_h2_file_read(file, name, (characters - 1) * sizeof(wchar_t), &read);
    host_h2_file_close(file);
    return true;
}

struct s_save_find
{
    unsigned int find;
    char root[8];
};

s_save_find g_save_finds[4];

bool fill_save_data(s_save_find &search, XGAME_FIND_DATA *data)
{
    for (;;)
    {
        if (data->wfd.dwFileAttributes & 0x10)
        {
            size_t length = 0;
            for (const char *c = search.root; *c && length + 2 < MAX_PATH; ++c)
                data->szSaveGameDirectory[length++] = *c;
            for (const char *c = data->wfd.cFileName; *c && length + 2 < MAX_PATH; ++c)
                data->szSaveGameDirectory[length++] = *c;
            data->szSaveGameDirectory[length++] = '\\';
            data->szSaveGameDirectory[length] = 0;
            if (read_save_name(data->szSaveGameDirectory, (wchar_t *)data->szSaveGameName,
                               k_save_name_characters))
                return true;
        }
        if (!host_h2_find_next(search.find, &data->wfd))
            return false;
    }
}

} // namespace

extern "C" DWORD __stdcall XCreateSaveGame(const char *root, const wchar_t *name, DWORD disposition,
                                           DWORD flags, char *directory, DWORD directory_size)
{
    (void)flags;
    char path[MAX_PATH];
    save_directory(root, name, path, sizeof(path));
    const bool exists = host_h2_file_attributes(path, 0, 0) != 0xffffffffu;
    if (disposition == CREATE_NEW && exists)
        return k_error_already_exists;
    if (disposition == OPEN_EXISTING && !exists)
        return k_error_file_not_found;
    if (!exists)
    {
        host_h2_directory_create(path);
        char meta[MAX_PATH];
        save_meta_path(path, meta, sizeof(meta));
        const unsigned int file = host_h2_file_open(meta, 3, CREATE_ALWAYS);
        if (file == 0xffffffffu)
            return 5;
        unsigned int written = 0;
        size_t characters = 0;
        while (name[characters] && characters + 1 < k_save_name_characters)
            ++characters;
        host_h2_file_write(file, name, (unsigned int)(characters * sizeof(wchar_t)), &written);
        host_h2_file_close(file);
    }
    if (directory && directory_size)
    {
        size_t length = 0;
        while (path[length] && length + 1 < directory_size)
        {
            directory[length] = path[length];
            ++length;
        }
        directory[length] = 0;
    }
    return 0;
}

extern "C" DWORD __stdcall XDeleteSaveGame(const char *root, const wchar_t *name)
{
    char path[MAX_PATH];
    save_directory(root, name, path, sizeof(path));
    return host_h2_directory_remove(path) ? 0 : k_error_file_not_found;
}

extern "C" HANDLE WINAPI XFindFirstSaveGame(LPCSTR root, XGAME_FIND_DATA *data)
{
    for (int index = 0; index < 4; ++index)
    {
        s_save_find &search = g_save_finds[index];
        if (search.find)
            continue;
        if (!root || !root[0])
            break;
        char pattern[MAX_PATH];
        size_t length = 0;
        for (const char *c = root; *c && length + 2 < sizeof(pattern); ++c)
            pattern[length++] = *c;
        pattern[length++] = '*';
        pattern[length] = 0;
        memset(data, 0, sizeof(*data));
        const unsigned int find = host_h2_find_first(pattern, &data->wfd);
        if (find == 0xffffffffu)
            break;
        size_t copy = 0;
        for (; root[copy] && copy + 1 < sizeof(search.root); ++copy)
            search.root[copy] = root[copy];
        search.root[copy] = 0;
        search.find = find;
        if (!fill_save_data(search, data))
        {
            host_h2_file_close(find);
            search.find = 0;
            break;
        }
        return reinterpret_cast<HANDLE>(0x4c000000u + index);
    }
    host_h2_set_last_error(k_error_no_more_files);
    return INVALID_HANDLE_VALUE;
}

extern "C" BOOL WINAPI XFindNextSaveGame(HANDLE find, XGAME_FIND_DATA *data)
{
    const unsigned int index = reinterpret_cast<unsigned int>(find) - 0x4c000000u;
    if (index >= 4 || !g_save_finds[index].find)
        return FALSE;
    s_save_find &search = g_save_finds[index];
    if (!host_h2_find_next(search.find, &data->wfd) || !fill_save_data(search, data))
    {
        host_h2_set_last_error(k_error_no_more_files);
        return FALSE;
    }
    return TRUE;
}

extern "C" BOOL WINAPI XFindClose(HANDLE find)
{
    const unsigned int index = reinterpret_cast<unsigned int>(find) - 0x4c000000u;
    if (index < 4 && g_save_finds[index].find)
    {
        host_h2_file_close(g_save_finds[index].find);
        g_save_finds[index].find = 0;
        return TRUE;
    }
    return FALSE;
}

extern "C" DWORD __stdcall XGetDiskClusterSize(const char *root)
{
    (void)root;
    return 0x4000;
}
