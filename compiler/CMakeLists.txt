cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project("HCC (Hack Compiler Collection)"
        DESCRIPTION "A set of tools for the hack platform: a jack compiler, a VM translator, and an assembler."
        HOMEPAGE_URL "https://github.com/andreip/hack-compiler-os-cpu")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Use c++14 compiler all the time.
if (NOT CONFIGURED_ONCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14"
      CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# Uncomment this to get debug() function working and
# get a lot of verbosity in each translator.
#add_compile_definitions(DEBUG)

add_subdirectory(hack)

# Find all the cpp files in current directory.
file(GLOB CPP_FILES
     LIST_DIRECTORIES FALSE
     "*.cpp")

function(create_exec target lib)
  add_executable(${target} ${CPP_FILES})
  target_link_libraries(${target} ${lib})
endfunction()

create_exec(AsmHack asmHackLib)
create_exec(VMTranslator vmHackLib)
create_exec(JackCompiler cplHackLib)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake was configured at least once.")
