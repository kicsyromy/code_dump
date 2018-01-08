#ifndef VK_TUTORIAL_GLOBAL_H
#define VK_TUTORIAL_GLOBAL_H

#define DEFAULT_COPY(klass)                    \
    klass(const klass &) = default;            \
    klass &operator=(const klass &) = default;

#define DISABLE_COPY(klass)                    \
    klass(const klass &) = delete;             \
    klass &operator=(const klass &) = delete;

#define DEFAULT_MOVE(klass)                    \
    klass(klass &&) = default;                 \
    klass &operator=(klass &&) = default;

#define DISABLE_MOVE(klass)                    \
    klass(klass &&) = delete;                  \
klass &operator=(klass &&) = delete;

#endif // !VK_TUTORIAL_GLOBAL_H
