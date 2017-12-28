SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(testchunkrender ${SRCS})

TARGET_LINK_LIBRARIES(testchunkrender
    libchunkrender
    libgtest
)

add_test(NAME testchunkrender
         COMMAND testchunkrender)
