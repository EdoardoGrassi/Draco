if(EXISTS "C:/Users/edo18/source/repos/EdoardoGrassi/Draco/out/build/x64-Debug/_deps/uuid-cpp-build/test/uuid-cpp_tests[1]_tests.cmake")
  include("C:/Users/edo18/source/repos/EdoardoGrassi/Draco/out/build/x64-Debug/_deps/uuid-cpp-build/test/uuid-cpp_tests[1]_tests.cmake")
else()
  add_test(uuid-cpp_tests_NOT_BUILT uuid-cpp_tests_NOT_BUILT)
endif()