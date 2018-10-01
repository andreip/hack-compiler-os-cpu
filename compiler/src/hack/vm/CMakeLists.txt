file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

add_library(vmLib ${CPP_FILES})
