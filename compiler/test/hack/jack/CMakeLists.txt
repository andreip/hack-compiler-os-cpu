function(make_test target cppFile)
  add_executable(${target} ${cppFile})
  target_link_libraries(${target}
                        cplLib hackLib genericLib
                        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
endfunction()

make_test(TestJackBuilder       test_builder.cpp)
make_test(TestJackBuilderVMCode test_builder_vm_code.cpp)
make_test(TestJackSymbolTable   test_symbol_table.cpp)
make_test(TestJackTokenizer     test_tokenizer.cpp)
