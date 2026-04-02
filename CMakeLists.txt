cmake_minimum_required(VERSION 3.16)
project(gtk-try)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_subdirectory(mayak-logger)

function(add_gtk4_app name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE ${GTK4_LIBRARIES})
    target_include_directories(${name} PRIVATE ${GTK4_INCLUDE_DIRS})
    target_link_directories(${name} PRIVATE ${GTK4_LIBRARY_DIRS})
endfunction()

# GTK4 apps
add_gtk4_app(hello-world hello-world.c)
add_gtk4_app(app-hello-world app-hello-world.cpp)
add_gtk4_app(let-window-be-window let-window-be-window.c)
add_gtk4_app(clicker clicker.c)
add_gtk4_app(clicker-state clicker-state.c)
add_gtk4_app(grid grid.c)
add_gtk4_app(log-viewer log-viewer.cpp)
target_link_libraries(log-viewer PRIVATE mayak_logger_core)
