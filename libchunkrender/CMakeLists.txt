set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(libchunkrender_SRCS
    chunkrender.h
    chunkrender.c
)

# Declare the library
add_library(libchunkrender STATIC
    ${libchunkrender_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(libchunkrender PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(TARGET libchunkrender PROPERTY C_STANDARD 11)
set_property(TARGET libchunkrender PROPERTY C_STANDARD_REQUIRED ON)
