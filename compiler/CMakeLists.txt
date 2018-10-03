cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project("HCC (Hack Compiler Collection)"
        DESCRIPTION "A set of tools for the hack platform: a jack compiler, a VM translator, and an assembler."
        HOMEPAGE_URL "https://github.com/andreip/hack-compiler-os-cpu")

# Enable C++14 features for gnu/clang.
if (NOT CONFIGURED_ONCE)
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|Clang)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
  endif()
endif()

# Uncomment this to get debug() function working and
# get a lot of verbosity in each translator.
#add_compile_definitions(DEBUG)

add_subdirectory(src)
function(create_hack_exec target targetLib)
  add_executable(${target} src/main.cpp)
  target_link_libraries(${target} genericLib hackLib ${targetLib})
endfunction()
create_hack_exec(AsmHack asmLib)
create_hack_exec(VMTranslator vmLib)
create_hack_exec(JackCompiler cplLib)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.59 COMPONENTS REQUIRED unit_test_framework)
if (${Boost_FOUND})
  include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
  enable_testing()
  add_subdirectory(test)
  add_test(NAME "JackBuilder" COMMAND TestJackBuilder)
  add_test(NAME "JackBuilderVMCode" COMMAND TestJackBuilderVMCode)
  add_test(NAME "JackTokenizer" COMMAND TestJackTokenizer)
  add_test(NAME "JackSymbolTable" COMMAND TestJackSymbolTable)
endif()

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake was configured at least once.")
