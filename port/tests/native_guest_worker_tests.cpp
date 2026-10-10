#include "h2/guest_arena.hpp"
#include "h2/guest_elf_image.hpp"
#include "h2/host_abi.h"
#include "h2/host_services.hpp"
#include "h2/native_async_lifecycle.hpp"
#include "h2/native_async_platform.hpp"
#include "h2/native_guest_worker.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace {

bool check(bool condition, const std::string& message) {
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

} // namespace

int main(int argc, char** argv) {
    using Lifecycle = h2::port::NativeAsyncLifecycle;
    using Platform = h2::port::NativeAsyncPlatform;
    using Worker = h2::port::NativeGuestWorker;

    if (argc != 2) {
        std::cerr << "usage: native_guest_worker_tests <guest-elf>\n";
        return 2;
    }
    bool ok = true;
    std::string error;
    auto arena = h2::port::GuestArena::create(error);
    ok &= check(arena != nullptr, error);
    if (!arena)
        return 1;
    auto image = h2::port::GuestElfImage::load(argv[1], *arena, error);
    ok &= check(image.has_value(), error);
    if (!image)
        return 1;
    const auto entry = image->symbol_address("h2_worker_step_probe");
    const auto counter = image->symbol_address("h2_worker_step_calls");
    ok &= check(entry.has_value() && counter.has_value(),
                "guest worker fixture symbols are missing");
    if (!entry || !counter)
        return 1;

    Platform platform;
    Lifecycle lifecycle;
    Worker worker(*arena, platform, lifecycle);
    ok &= check(!worker.start(*counter, error) &&
                    error.find("not mapped executable") != std::string::npos,
                "guest worker accepted a writable data entry");
    ok &= check(worker.start(*entry, error), error);
    ok &= check(!worker.start(*entry, error),
                "running guest worker started twice");
    ok &= check(worker.try_admit_and_wake(),
                "guest worker rejected an admission");
    ok &= check(worker.shutdown(Lifecycle::infinite),
                "guest worker did not drain and join");
    std::uint32_t calls = 0;
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 2 && lifecycle.state() == Lifecycle::State::stopped &&
                    lifecycle.admitted_count() == 0 &&
                    platform.active_handle_count() == 0,
                "guest worker did not retry then complete on its low stack");

    ok &= check(worker.start(*entry, error) && worker.try_admit_and_wake() &&
                    worker.shutdown(Lifecycle::infinite),
                "guest worker did not restart cleanly");
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 3 && platform.active_handle_count() == 0,
                "restarted guest worker did not execute exactly once");

    ok &= check(h2::port::configure_async_services(*arena, error) &&
                    h2::port::start_async_worker(*entry, error) &&
                    host_h2_async_try_admit() &&
                    host_h2_async_publish_admitted() &&
                    h2::port::shutdown_async_worker(Lifecycle::infinite, error),
                "host ABI admission transaction did not execute the guest step");
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 4 &&
                    h2::port::start_async_worker(*entry, error) &&
                    host_h2_async_try_admit() && host_h2_async_abort_admitted() &&
                    h2::port::shutdown_async_worker(Lifecycle::infinite, error),
                "host ABI admission abort did not drain without execution");
    std::memcpy(&calls, arena->pointer(*counter), sizeof(calls));
    ok &= check(calls == 4 && h2::port::reset_async_services(error) &&
                    !host_h2_async_try_admit(),
                "host ABI admission teardown remained configured or ran aborted work");
    return ok ? 0 : 1;
}
