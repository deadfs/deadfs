file(GLOB LSOURCES "*.c")

SET(SOURCES ${SOURCES} ${LSOURCES} CACHE INTERNAL "")

add_subdirectory(utils)
add_subdirectory(fs)

add_executable(deadfs ${SOURCES})

target_link_libraries(deadfs fuse)
