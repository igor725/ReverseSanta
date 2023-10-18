cmake_minimum_required(VERSION 3.12)

project(ReverseSanta LANGUAGES CXX VERSION "1.0.0")
include(cmake/FindDirectX.cmake)

link_directories(ReverseSanta ${DirectX_LIBRARY_PATHS})
add_executable(ReverseSanta WIN32 src/main.cc)
set_target_properties(ReverseSanta PROPERTIES OUTPUT_NAME "se-${CMAKE_SYSTEM_PROCESSOR}")

include_directories(ReverseSanta
	src/
	imgui/
	imgui/backends/
	${DirectX_INCLUDE_DIRS}
)

target_sources(ReverseSanta PRIVATE
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/misc/cpp/imgui_stdlib.cpp
	imgui/backends/imgui_impl_win32.cpp
	imgui/backends/imgui_impl_dx9.cpp

	src/runners/editor/emenu.cc
	src/runners/game/gmenu.cc
	src/runners/game/player.cc
	src/runners/editor.cc
	src/runners/game.cc
	src/runners/menu.cc

	src/camera.cc
	src/dobject.cc
	src/elems.cc
	src/engine.cc
	src/exceptions.cc
	src/graphics.cc
	src/input.cc
	src/lang.cc
	src/level.cc
	src/menuctl.cc
	src/mesh.cc
	src/rescache.cc
	src/texture.cc
	src/virtfs.cc
	src/walkthrough.cc
	src/xpk.cc
)

target_link_libraries(ReverseSanta PRIVATE
	d3d9.lib
	d3dx9.lib
	dinput8.lib
	dxguid.lib
	shell32.lib
	dbghelp.lib
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
# target_compile_definitions(ReverseSanta PUBLIC _CRT_SECURE_NO_WARNINGS)
