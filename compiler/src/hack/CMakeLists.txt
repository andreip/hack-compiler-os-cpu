# Find all the cpp files in current directory.
file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

add_library(hackLib ${CPP_FILES})
add_subdirectory(asm)
add_subdirectory(vm)
add_subdirectory(jack)
