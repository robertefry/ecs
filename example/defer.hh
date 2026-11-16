
#ifndef DEFER_HH
#define DEFER_HH

#include <optional>

template <typename F>
class deferred_t {

    std::optional<F> func_{};

public:

    ~deferred_t() {
        if (func_) (*func_)();
    }

    deferred_t(deferred_t const& other) = delete;
    deferred_t& operator=(deferred_t const&) = delete;

    deferred_t(deferred_t&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
        : func_{std::exchange(other.func_, std::nullopt)}
    {}
    deferred_t& operator=(deferred_t&& other) = delete;

    deferred_t(F&& func)
        : func_{std::move(func)}
    {}

    inline void release() {
        // NB: Discard the function without executing it.
        func_ = std::nullopt;
    }

};

#define __defer_cat_expanded(x,y) x##y
#define __defer_cat(x,y) __defer_cat_expanded(x,y)
#define defer deferred_t const __defer_cat(__deferred_,__COUNTER__) = [&]

#endif /* DEFER_HH */
