file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

add_library(asmLib ${CPP_FILES})
