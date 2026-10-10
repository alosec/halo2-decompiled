#include "h2/native_async_platform.hpp"

#include <array>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

namespace h2::port {
namespace {

constexpr std::uint32_t kHandleMagic = 0xa0000000u;
constexpr std::uint32_t kKindShift = 24;
constexpr std::uint32_t kGenerationShift = 8;
constexpr std::uint32_t kIndexMask = 0xffu;
constexpr std::uint32_t kGenerationMask = 0xffffu;

enum class HandleKind : std::uint32_t {
    mutex = 1,
    semaphore = 2,
    file = 3,
    thread = 4,
};

struct HandleObject {
    explicit HandleObject(HandleKind object_kind) : kind(object_kind) {}
    virtual ~HandleObject() = default;
    HandleKind kind;
};

struct MutexObject final : HandleObject {
    MutexObject() : HandleObject(HandleKind::mutex) {}
    std::recursive_timed_mutex mutex;
};

struct SemaphoreObject final : HandleObject {
    SemaphoreObject(std::int32_t initial, std::int32_t maximum)
        : HandleObject(HandleKind::semaphore), count(initial), maximum(maximum) {}

    std::mutex mutex;
    std::condition_variable condition;
    std::int32_t count;
    const std::int32_t maximum;
};

struct FileObject final : HandleObject {
    explicit FileObject(int descriptor) : HandleObject(HandleKind::file), fd(descriptor) {}
    ~FileObject() override {
        if (fd >= 0)
            ::close(fd);
    }

    std::mutex mutex;
    int fd;
};

struct ThreadObject final : HandleObject {
    ThreadObject() : HandleObject(HandleKind::thread) {}
    ~ThreadObject() override {
        if (started && !joined)
            ::pthread_detach(thread);
    }

    mutable std::mutex mutex;
    std::condition_variable condition;
    pthread_t thread{};
    bool started = false;
    bool exited = false;
    bool joined = false;
    std::uint32_t exit_code = 0;
};

struct ThreadContext {
    std::shared_ptr<ThreadObject> object;
    NativeAsyncPlatform::ThreadStart start;
    void* parameter;
};

void* thread_trampoline(void* raw_context) {
    std::unique_ptr<ThreadContext> context(static_cast<ThreadContext*>(raw_context));
    const std::uint32_t result = context->start(context->parameter);
    {
        std::lock_guard lock(context->object->mutex);
        context->object->exit_code = result;
        context->object->exited = true;
    }
    context->object->condition.notify_all();
    return nullptr;
}

std::uint32_t make_handle(std::uint32_t index, std::uint16_t generation,
                          HandleKind kind) {
    return kHandleMagic | (static_cast<std::uint32_t>(kind) << kKindShift) |
           (static_cast<std::uint32_t>(generation) << kGenerationShift) | index;
}

} // namespace

struct NativeAsyncPlatform::State {
    struct Slot {
        std::uint16_t generation = 1;
        std::shared_ptr<HandleObject> object;
    };

    mutable std::mutex mutex;
    std::array<Slot, 256> slots;

    Handle insert(std::shared_ptr<HandleObject> object) {
        std::lock_guard lock(mutex);
        for (std::uint32_t index = 0; index < slots.size(); ++index) {
            Slot& slot = slots[index];
            if (!slot.object) {
                slot.object = std::move(object);
                return make_handle(index, slot.generation, slot.object->kind);
            }
        }
        return invalid_handle;
    }

    std::shared_ptr<HandleObject> get(Handle handle, HandleKind expected) const {
        if ((handle & 0xf0000000u) != kHandleMagic ||
            ((handle >> kKindShift) & 0xfu) != static_cast<std::uint32_t>(expected))
            return {};
        const std::uint32_t index = handle & kIndexMask;
        const std::uint16_t generation = static_cast<std::uint16_t>(
            (handle >> kGenerationShift) & kGenerationMask);
        std::lock_guard lock(mutex);
        const Slot& slot = slots[index];
        if (!slot.object || slot.generation != generation || slot.object->kind != expected)
            return {};
        return slot.object;
    }
};

NativeAsyncPlatform::NativeAsyncPlatform() : state_(std::make_unique<State>()) {}
NativeAsyncPlatform::~NativeAsyncPlatform() = default;

NativeAsyncPlatform::Handle NativeAsyncPlatform::create_mutex(bool initial_owner) {
    auto object = std::make_shared<MutexObject>();
    if (initial_owner)
        object->mutex.lock();
    return state_->insert(std::move(object));
}

NativeAsyncPlatform::Handle NativeAsyncPlatform::create_semaphore(
    std::int32_t initial_count, std::int32_t maximum_count) {
    if (maximum_count <= 0 || initial_count < 0 || initial_count > maximum_count)
        return invalid_handle;
    return state_->insert(std::make_shared<SemaphoreObject>(initial_count, maximum_count));
}

NativeAsyncPlatform::Handle NativeAsyncPlatform::open_read_only(
    const std::filesystem::path& path) {
    const int descriptor = ::open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (descriptor < 0)
        return invalid_handle;
    struct stat status {};
    if (::fstat(descriptor, &status) != 0 || !S_ISREG(status.st_mode)) {
        ::close(descriptor);
        return invalid_handle;
    }
    return state_->insert(std::make_shared<FileObject>(descriptor));
}

NativeAsyncPlatform::Handle NativeAsyncPlatform::create_thread(
    std::uint32_t stack_size, ThreadStart start, void* parameter) {
    if (!start)
        return invalid_handle;
    auto object = std::make_shared<ThreadObject>();
    const Handle handle = state_->insert(object);
    if (handle == invalid_handle)
        return invalid_handle;

    pthread_attr_t attributes;
    if (::pthread_attr_init(&attributes) != 0) {
        close(handle);
        return invalid_handle;
    }
    if (stack_size) {
        const std::size_t page = static_cast<std::size_t>(::sysconf(_SC_PAGESIZE));
        std::size_t adjusted = stack_size < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : stack_size;
        adjusted = (adjusted + page - 1) & ~(page - 1);
        if (::pthread_attr_setstacksize(&attributes, adjusted) != 0) {
            ::pthread_attr_destroy(&attributes);
            close(handle);
            return invalid_handle;
        }
    }
    auto* context = new ThreadContext{object, start, parameter};
    const int result = ::pthread_create(&object->thread, &attributes,
                                        thread_trampoline, context);
    ::pthread_attr_destroy(&attributes);
    if (result != 0) {
        delete context;
        close(handle);
        return invalid_handle;
    }
    {
        std::lock_guard lock(object->mutex);
        object->started = true;
    }
    return handle;
}

bool NativeAsyncPlatform::thread_exit_code(Handle handle,
                                            std::uint32_t* exit_code) const {
    if (!exit_code)
        return false;
    auto base = state_->get(handle, HandleKind::thread);
    if (!base)
        return false;
    auto object = std::static_pointer_cast<ThreadObject>(std::move(base));
    std::lock_guard lock(object->mutex);
    if (!object->exited)
        return false;
    *exit_code = object->exit_code;
    return true;
}

bool NativeAsyncPlatform::set_file_pointer(Handle handle, std::int32_t distance,
                                            std::int32_t* distance_high,
                                            std::uint32_t method,
                                            std::uint32_t* position_low) {
    auto base = state_->get(handle, HandleKind::file);
    if (!base || method > 2)
        return false;
    auto object = std::static_pointer_cast<FileObject>(std::move(base));
    std::int64_t offset = distance;
    if (distance_high)
        offset = static_cast<std::int64_t>(*distance_high) * 0x100000000ll +
                 static_cast<std::uint32_t>(distance);
    const int whence = method == 0 ? SEEK_SET : (method == 1 ? SEEK_CUR : SEEK_END);
    std::lock_guard lock(object->mutex);
    const off_t position = ::lseek(object->fd, static_cast<off_t>(offset), whence);
    if (position < 0)
        return false;
    const std::uint64_t value = static_cast<std::uint64_t>(position);
    if (distance_high)
        *distance_high = static_cast<std::int32_t>(value >> 32);
    if (position_low)
        *position_low = static_cast<std::uint32_t>(value);
    return true;
}

bool NativeAsyncPlatform::read_file(Handle handle, void* buffer,
                                     std::uint32_t bytes_to_read,
                                     std::uint32_t* bytes_read) {
    if (bytes_read)
        *bytes_read = 0;
    if (bytes_to_read && !buffer)
        return false;
    auto base = state_->get(handle, HandleKind::file);
    if (!base)
        return false;
    auto object = std::static_pointer_cast<FileObject>(std::move(base));
    std::lock_guard lock(object->mutex);
    const ssize_t result = ::read(object->fd, buffer, bytes_to_read);
    if (result < 0)
        return false;
    if (bytes_read)
        *bytes_read = static_cast<std::uint32_t>(result);
    return true;
}

NativeAsyncPlatform::WaitResult NativeAsyncPlatform::wait(
    Handle handle, std::uint32_t milliseconds) {
    if (auto base = state_->get(handle, HandleKind::mutex)) {
        auto object = std::static_pointer_cast<MutexObject>(std::move(base));
        if (milliseconds == infinite) {
            object->mutex.lock();
            return WaitResult::signaled;
        }
        return object->mutex.try_lock_for(std::chrono::milliseconds(milliseconds))
            ? WaitResult::signaled : WaitResult::timeout;
    }
    if (auto base = state_->get(handle, HandleKind::semaphore)) {
        auto object = std::static_pointer_cast<SemaphoreObject>(std::move(base));
        std::unique_lock lock(object->mutex);
        const auto available = [&] { return object->count > 0; };
        if (milliseconds == infinite)
            object->condition.wait(lock, available);
        else if (!object->condition.wait_for(lock, std::chrono::milliseconds(milliseconds),
                                              available))
            return WaitResult::timeout;
        --object->count;
        return WaitResult::signaled;
    }
    if (auto base = state_->get(handle, HandleKind::thread)) {
        auto object = std::static_pointer_cast<ThreadObject>(std::move(base));
        std::unique_lock lock(object->mutex);
        const auto exited = [&] { return object->exited; };
        if (milliseconds == infinite)
            object->condition.wait(lock, exited);
        else if (!object->condition.wait_for(lock, std::chrono::milliseconds(milliseconds),
                                              exited))
            return WaitResult::timeout;
        return WaitResult::signaled;
    }
    return WaitResult::failed;
}

bool NativeAsyncPlatform::release_mutex(Handle handle) {
    auto base = state_->get(handle, HandleKind::mutex);
    if (!base)
        return false;
    std::static_pointer_cast<MutexObject>(std::move(base))->mutex.unlock();
    return true;
}

bool NativeAsyncPlatform::release_semaphore(Handle handle, std::int32_t release_count,
                                            std::int32_t* previous_count) {
    auto base = state_->get(handle, HandleKind::semaphore);
    if (!base || release_count <= 0)
        return false;
    auto object = std::static_pointer_cast<SemaphoreObject>(std::move(base));
    {
        std::lock_guard lock(object->mutex);
        if (release_count > object->maximum - object->count)
            return false;
        if (previous_count)
            *previous_count = object->count;
        object->count += release_count;
    }
    object->condition.notify_all();
    return true;
}

bool NativeAsyncPlatform::close(Handle handle) {
    if ((handle & 0xf0000000u) != kHandleMagic)
        return false;
    const std::uint32_t index = handle & kIndexMask;
    const std::uint16_t generation = static_cast<std::uint16_t>(
        (handle >> kGenerationShift) & kGenerationMask);
    std::lock_guard lock(state_->mutex);
    State::Slot& slot = state_->slots[index];
    if (!slot.object || slot.generation != generation)
        return false;
    if (slot.object->kind == HandleKind::thread) {
        auto object = std::static_pointer_cast<ThreadObject>(slot.object);
        bool should_join = false;
        {
            std::lock_guard thread_lock(object->mutex);
            should_join = object->started && object->exited && !object->joined &&
                          !::pthread_equal(object->thread, ::pthread_self());
        }
        if (should_join) {
            if (::pthread_join(object->thread, nullptr) != 0)
                return false;
            std::lock_guard thread_lock(object->mutex);
            object->joined = true;
        }
    }
    slot.object.reset();
    ++slot.generation;
    if (!slot.generation)
        slot.generation = 1;
    return true;
}

std::uint32_t NativeAsyncPlatform::active_handle_count() const {
    std::lock_guard lock(state_->mutex);
    std::uint32_t count = 0;
    for (const State::Slot& slot : state_->slots)
        count += slot.object ? 1u : 0u;
    return count;
}

void NativeAsyncPlatform::yield() {
    std::this_thread::yield();
}

} // namespace h2::port
