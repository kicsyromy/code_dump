set (SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "Compile with -fno-exceptions. Call abort() on any spdlog exceptions" FORCE)
set (SPDLOG_NO_THREAD_ID ON CACHE BOOL "prevent spdlog from querying the thread id on each log call if thread id is not needed" FORCE)
set (SPDLOG_NO_ATOMIC_LEVELS ON CACHE BOOL "prevent spdlog from using of std::atomic log levels (use only if your code never modifies log levels concurrently" FORCE)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set (SPDLOG_CLOCK_COARSE ON CACHE BOOL "Use the much faster (but much less accurate) CLOCK_REALTIME_COARSE instead of the regular clock," FORCE)
endif()

add_subdirectory (spdlog-1.5.0)

if (VOOT_BUILD_SHARED)
    set_property (TARGET spdlog PROPERTY POSITION_INDEPENDENT_CODE ON)
endif ()

