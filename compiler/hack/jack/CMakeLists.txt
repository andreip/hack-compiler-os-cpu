file(GLOB TEST_CPP_FILES
     LIST_DIRECTORIES FALSE
     "test_*.cpp")

file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")
list(FILTER CPP_FILES EXCLUDE REGEX "test_.*")

message(STATUS "test cpp files: ${TEST_CPP_FILES}")
message(STATUS "cpp files: ${CPP_FILES}")

add_library(cplHackLib ${CPP_FILES} ${HACK_CPP_FILES})
