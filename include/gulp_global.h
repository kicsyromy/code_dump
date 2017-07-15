#ifndef GULP_GLOBAL_H
#define GULP_GLOBAL_H

#if defined(_MSC_VER)
/* Microsoft */
#define EXPORT_SYMBOL __declspec(dllexport)
#define IMPORT_SYMBOL __declspec(dllimport)
#elif defined(__GNUC__)
/* GCC */
#define EXPORT_SYMBOL __attribute__((visibility("default")))
#define IMPORT_SYMBOL
#else
#define EXPORT_SYMBOL
#define IMPORT_SYMBOL
#pragma warning Unknown dynamic link import / export semantics.
#endif

#ifndef GULP_NO_IMPORT_SEMANTICS
#if defined(GULP_LIB)
#define GULP_API EXPORT_SYMBOL
#else
#define GULP_API IMPORT_SYMBOL
#endif
#else
#define GULP_API
#endif

#define UNUSED(d) static_cast<void>(d)

#define DISABLE_COPY(klass)                       \
    klass(const klass &other) = delete;           \
    klass &operator=(const klass &other) = delete;

#define DISABLE_MOVE(klass)                       \
    klass(klass &&other) = delete;                \
    klass &operator=(klass &&other) = delete;

#endif // GULP_GLOBAL_H
