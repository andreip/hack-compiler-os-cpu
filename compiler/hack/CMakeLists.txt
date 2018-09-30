# Find all the cpp files in current directory.
file(GLOB HACK_CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

add_subdirectory(asm)
add_subdirectory(vm)
add_subdirectory(jack)
