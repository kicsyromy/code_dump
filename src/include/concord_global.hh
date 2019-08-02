#ifndef CONCORD_GLOBAL_HH
#define CONCORD_GLOBAL_HH

#define DISABLE_COPY(klass)                                                    \
    klass(const klass &other) = delete;                                        \
    klass &operator=(const klass &other) = delete

#define DISABLE_MOVE(klass)                                                    \
    klass(klass &&other) = delete;                                             \
    klass &operator=(klass &&other) = delete


#endif /* !CONCORD_GLOBAL_HH */