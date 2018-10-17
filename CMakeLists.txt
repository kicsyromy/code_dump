cmake_minimum_required (VERSION 3.5)

project (Periscope)
enable_language (ASM_MASM)
set (can_use_assembler TRUE)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

list (APPEND CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (ShaderCompiler)

set (
	${PROJECT_NAME}_HEADERS
	"${PROJECT_SOURCE_DIR}/Capture/DXGI.h"
	"${PROJECT_SOURCE_DIR}/Capture/NvFBC.h"
	"${PROJECT_SOURCE_DIR}/CaptureFactory.h"
	"${PROJECT_SOURCE_DIR}/Com.h"
	"${PROJECT_SOURCE_DIR}/CrashHandler.h"
	"${PROJECT_SOURCE_DIR}/ICapture.h"
	"${PROJECT_SOURCE_DIR}/MFT/H264.h"
	"${PROJECT_SOURCE_DIR}/Service.h"
	"${PROJECT_SOURCE_DIR}/TextureConverter.h"
	"${PROJECT_SOURCE_DIR}/TraceUtil.h"
	"${PROJECT_SOURCE_DIR}/Util.h"
)

set (
	${PROJECT_NAME}_SOURCES
	"${PROJECT_SOURCE_DIR}/Capture/DXGI.cpp"
	"${PROJECT_SOURCE_DIR}/Capture/NvFBC.cpp"
	"${PROJECT_SOURCE_DIR}/common/memcpySSE.asm"
	"${PROJECT_SOURCE_DIR}/CrashHandler.cpp"
	"${PROJECT_SOURCE_DIR}/getopt/getopt.c"
	"${PROJECT_SOURCE_DIR}/main.cpp"
	"${PROJECT_SOURCE_DIR}/MFT/H264.cpp"
	"${PROJECT_SOURCE_DIR}/Service.cpp"
	"${PROJECT_SOURCE_DIR}/TextureConverter.cpp"
	"${PROJECT_SOURCE_DIR}/TraceUtil.cpp"
)

add_executable (
    ${PROJECT_NAME} WIN32
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
)

target_include_directories (
    ${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/common
)

target_compile_definitions (
    ${PROJECT_NAME} PRIVATE
	-DBUILD_VERSION=0.1
	-DUNICODE
    -D_UNICODE
)

target_link_libraries (
	${PROJECT_NAME} PRIVATE
	avrt
	evr
	shlwapi
	dxgi
	d3d11
	setupapi
	uuid
	ole32
	mfplat 
	mfuuid
	ntdll
)

compile_shader (
    "${PROJECT_SOURCE_DIR}/Shaders/Pixel.hlsl"
	"${PROJECT_BINARY_DIR}/Shaders"
	"PIXEL_SHADER"
	"g_Pixel"
	${PROJECT_NAME}
)

compile_shader (
    "${PROJECT_SOURCE_DIR}/Shaders/RGBtoYUV.hlsl"
	"${PROJECT_BINARY_DIR}/Shaders"
	"PIXEL_SHADER"
	"g_RGBtoYUV"
	${PROJECT_NAME}
)

compile_shader (
    "${PROJECT_SOURCE_DIR}/Shaders/Vertex.hlsl"
	"${PROJECT_BINARY_DIR}/Shaders"
	"VERTEX_SHADER"
	"g_Vertex"
	${PROJECT_NAME}
)
