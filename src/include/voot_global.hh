#pragma once

#if defined(_MSC_VER)
#define VOOT_EXPORT_SYMBOL __declspec(dllexport)
#define VOOT_IMPORT_SYMBOL __declspec(dllimport)
#elif defined(__GNUC__)
#define VOOT_EXPORT_SYMBOL __attribute__((visibility("default")))
#define VOOT_IMPORT_SYMBOL
#else
#define VOOT_EXPORT_SYMBOL
#define VOOT_IMPORT_SYMBOL
#pragma warning Unknown dynamic link import / export semantics.
#endif

#ifdef BUILD_LIBRARY
#define VOOT_API EXPORT_SYMBOL
#else
#define VOOT_API IMPORT_SYMBOL
#endif

#define VOOT_BEGIN_NAMESPACE \
    namespace voot           \
    {
#define VOOT_END_NAMESPACE }

#define VOOT_SINGLETON(klass) klass() = delete;

#define VOOT_DISABLE_COPY(klass)            \
    explicit klass(const klass &) = delete; \
    klass &operator=(const klass &) = delete

#define VOOT_DISABLE_MOVE(klass)       \
    explicit klass(klass &&) = delete; \
    klass &operator=(klass &&) = delete
