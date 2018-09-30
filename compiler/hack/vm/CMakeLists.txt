file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

add_library(vmHackLib ${CPP_FILES} ${HACK_CPP_FILES})
