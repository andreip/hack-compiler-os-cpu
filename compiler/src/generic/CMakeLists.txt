# Find all the cpp files in current directory.
file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

add_library(genericLib ${CPP_FILES})
