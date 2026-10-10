#include "h2/native_async_platform.hpp"

#include <array>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

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
