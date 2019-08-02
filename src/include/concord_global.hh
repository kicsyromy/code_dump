#ifndef CONCORD_GLOBAL_HH
#define CONCORD_GLOBAL_HH

#define DISABLE_COPY(klass)                                                                        \
    klass(const klass &other) = delete;                                                            \
    klass &operator=(const klass &other) = delete

#define DISABLE_MOVE(klass)                                                                        \
    klass(klass &&other) = delete;                                                                 \
    klass &operator=(klass &&other) = delete

#define WAYLAND_PTR(type, name, ...)                                                               \
    std::unique_ptr<type, decltype(&type##_destroy)> name                                          \
    {                                                                                              \
        type##_create(__VA_ARGS__), &type##_destroy                                                \
    }
#define WLROOTS_PTR(type, name, ...) WAYLAND_PTR(type, name, __VA_ARGS__)

#define DECLARE_CONVERT_FROM_SMART_PTR(ptr)                                                        \
    decltype(ptr.get()) operator()() noexcept { return ptr.get(); }

#define DECLARE_CONVERT_FROM_RAW_PTR(ptr)                                                          \
    decltype(ptr) operator()() noexcept { return ptr; }

#endif /* !CONCORD_GLOBAL_HH */
