#include <catch2/catch.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "private/voot_graphics_context.hh"
#include "private/voot_graphics_context_opengl.cc"

TEST_CASE("GraphicsContext", "[]")
{}
