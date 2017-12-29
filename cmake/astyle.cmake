find_program(ASTYLE_EXECUTABLE astyle DOC "astyle code formatter")

# Add a custom target
add_custom_target("format" COMMAND
  ${ASTYLE_EXECUTABLE}
  --style=java
  --indent-namespaces
  --add-brackets
  --keep-one-line-statements
  --convert-tabs
  --max-code-length=120
  --recursive
  ${CMAKE_SOURCE_DIR}/libchunkrender/*.c
  ${CMAKE_SOURCE_DIR}/libchunkrender/*.h
  ${CMAKE_SOURCE_DIR}/test/testchunkrender/*.cpp
  VERBATIM
)
