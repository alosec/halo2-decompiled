#ifndef H2_PORT_NATIVE_ASYNC_LIFECYCLE_HPP
#define H2_PORT_NATIVE_ASYNC_LIFECYCLE_HPP

#include <cstdint>
#include <memory>

namespace h2::port {

class NativeAsyncLifecycle {
public:
    static constexpr std::uint32_t infinite = 0xffffffffu;
    static constexpr std::uint32_t maximum_admitted = 150;

    enum class State {
        stopped,
        accepting,
        stopping,
    };

    NativeAsyncLifecycle();
    ~NativeAsyncLifecycle();
    NativeAsyncLifecycle(const NativeAsyncLifecycle&) = delete;
    NativeAsyncLifecycle& operator=(const NativeAsyncLifecycle&) = delete;

    bool start();
    bool try_admit();
    bool complete();
    bool begin_stop();
    bool wait_drained(std::uint32_t milliseconds);
    bool finish_stop();

    State state() const;
    std::uint32_t admitted_count() const;

private:
    struct Data;
    std::unique_ptr<Data> data_;
};

} // namespace h2::port

#endif
