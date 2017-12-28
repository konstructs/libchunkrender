
file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(testchunkrender ${SRCS})

TARGET_LINK_LIBRARIES(testchunkrender
    libchunkrender
    libgtest
)

add_test(NAME testchunkrender
         COMMAND testchunkrender)

set_property(TARGET testchunkrender PROPERTY CXX_STANDARD 11)
set_property(TARGET testchunkrender PROPERTY CXX_STANDARD_REQUIRED ON)
