CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(embryo)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(Git)
FIND_PACKAGE(Hg)
ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND}
    -D
    SRC=${CMAKE_SOURCE_DIR}/include/embryo/version.h.in
    -D
    DST=${CMAKE_SOURCE_DIR}/include/embryo/version.h
    -D
    GIT_EXECUTABLE=${GIT_EXECUTABLE}
    -D
    HG_EXECUTABLE=${HG_EXECUTABLE}
    -P
    ${CMAKE_SOURCE_DIR}/GenerateVersionHeader.cmake)

IF (BUILD_TESTS AND ENABLE_COVERAGE)
  ADD_COMPILE_OPTIONS(--coverage)
  ADD_LINK_OPTIONS(--coverage)
ENDIF ()

ADD_COMPILE_OPTIONS("$<$<CONFIG:DEBUG>:-DDEBUG>")
ADD_COMPILE_OPTIONS(-Werror -Wall -Wno-unknown-pragmas)

IF (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  INCLUDE(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  CONAN_BASIC_SETUP()
ELSE ()
  MESSAGE(
      FATAL_ERROR
      "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first"
  )
ENDIF ()

IF (UNIX AND NOT APPLE)
  SET(LINUX TRUE)
ENDIF ()

IF (LINUX)
  LINK_LIBRARIES(stdc++fs)
ELSE ()
  #  LINK_LIBRARIES(c++fs)
ENDIF ()

ADD_COMPILE_OPTIONS(-DFMT_HEADER_ONLY)

INCLUDE_DIRECTORIES(embryo PUBLIC include/)

ADD_LIBRARY(
    embryo
    include/embryo/compiler/lexer/lexer.h
    include/embryo/compiler/lexer/token.h
    include/embryo/utils/strings.h
    include/embryo/compiler/ast/block.h
    include/embryo/compiler/ast/stat.h
    include/embryo/compiler/ast/exp.h
    include/embryo/compiler/parser/parser.h
    include/embryo/compiler/ast/common.h
    include/embryo/utils/numbers.h
    include/embryo/utils/formatters.h
    include/embryo/utils/stringer.h
    include/embryo/utils/inspection.h
    include/embryo/utils/io.h
    include/embryo/compiler/parser/optimizers.h
    include/embryo/bin_chunk/bin_chunk.h
    include/embryo/bin_chunk/reader.h
    include/embryo/utils/bytes.h
    include/embryo/utils/types.h
    include/embryo/vm/opcodes.h
    include/embryo/vm/instruction.h
    include/embryo/api/constants.h
    include/embryo/state/lua_stack.h
    include/embryo/api/lua_state.h
    include/embryo/state/lua_state.h
    include/embryo/state/lua_value.h
    src/utils/strings.cc
    src/compiler/lexer/lexer.cc
    src/compiler/parser/parser.cc
    src/compiler/parser/parse_block.cc
    src/compiler/parser/parse_stat.cc
    src/compiler/lexer/token.cc
    src/compiler/parser/parse_exp.cc
    src/compiler/parser/parse_prefix_exp.cc
    src/utils/numbers.cc
    src/compiler/ast/stringers.cc
    src/utils/inspection.cc
    src/utils/io.cc
    src/compiler/parser/optimizers.cc
    src/bin_chunk/stringers.cc
    src/bin_chunk/reader.cc
    src/utils/bytes.cc
    src/vm/stringers.cc
    src/vm/instruction.cc
    src/state/lua_stack.cc
    src/state/lua_value.cc
    src/state/api_push.cc
    src/state/api_stack.cc
    src/state/lua_state.cc
    src/state/api_access.cc
    src/bin_chunk/pretty_print.cc
    src/state/api_arith.cc
    src/state/api_compare.cc
    src/state/api_misc.cc
    include/embryo/api/lua_vm.h
    src/state/api_vm.cc
    include/embryo/vm/inst_misc.h
    src/vm/opcodes.cc
    include/embryo/vm/constants.h
    src/vm/inst_misc.cc
    include/embryo/vm/inst_load.h
    src/vm/inst_load.cc
    include/embryo/vm/inst_operators.h
    src/vm/inst_operators.cc
    include/embryo/state/lua_table.h
    src/state/lua_table.cc
    include/embryo/utils/hashers.h
    src/state/api_get.cc
    src/state/api_set.cc
    include/embryo/vm/inst_table.h
    src/vm/inst_table.cc
    include/embryo/bin_chunk/closure.h
    src/bin_chunk/closure.cc
    src/state/api_call.cc
    src/vm/inst_call.cc
    include/embryo/vm/inst_call.h
    include/embryo/vm/inst_up_value.h
    src/vm/inst_up_value.cc
    include/embryo/utils/backtrace.h
    src/utils/backtrace.cc
    include/embryo/compiler/code_gen/func_info.h
    src/compiler/code_gen/func_info.cc
    include/embryo/compiler/code_gen/cg_block.h
    src/compiler/code_gen/cg_block.cc
    include/embryo/api/aux_lib.h
    include/embryo/utils/fpb.h
    src/utils/fpb.cc
    include/embryo/compiler/code_gen/cg_exp.h
    include/embryo/compiler/code_gen/cg_stat.h
    src/compiler/code_gen/cg_stat.cc
    include/embryo/compiler/code_gen/cg_helpers.h
    src/compiler/code_gen/cg_helpers.cc
    include/embryo/compiler/code_gen/fi_to_proto.h
    include/embryo/compiler/code_gen/code_gen.h
    src/compiler/code_gen/code_gen.cc
    src/compiler/code_gen/fi_to_proto.cc
    src/compiler/code_gen/cg_exp.cc
    include/embryo/compiler/compiler.h
    src/compiler/compiler.cc
    src/api/lua_state.cc
    include/embryo/bin_chunk/writer.h
    src/bin_chunk/writer.cc
    src/bin_chunk/bin_chunk.cc
    include/embryo/api/basic_api.h
    src/state/aux_lib.cc
    include/embryo/std_lib/lib_basic.h
    src/std_lib/lib_basic.cc
    include/embryo/utils/sort.h
    src/utils/sort.cc
    include/embryo/std_lib/lib_math.h
    src/std_lib/lib_math.cc
    include/embryo/std_lib/lib_table.h
    src/std_lib/lib_table.cc
    include/embryo/std_lib/lib_os.h
    src/std_lib/lib_os.cc
    include/embryo/std_lib/lib_string.h
    src/std_lib/lib_string.cc
    include/embryo/std_lib/str.h
    src/std_lib/str.cc
    include/embryo/utils/chan.h
    src/state/api_coroutine.cc
    include/embryo/std_lib/lib_coroutine.h
    src/std_lib/lib_coroutine.cc
    include/embryo/std_lib/lib_package.h
    src/std_lib/lib_package.cc)

ADD_DEPENDENCIES(embryo version)

LINK_LIBRARIES(embryo)

# IF (BUILD_TESTS) SET(GTEST_LIBS gtest_maind gtestd)
#
# ADD_COMPILE_DEFINITIONS(BUILD_TESTS) FIND_PACKAGE(Threads REQUIRED)
# LINK_LIBRARIES(${CMAKE_THREAD_LIBS_INIT} ${GTEST_LIBS}
# ${CMAKE_THREAD_LIBS_INIT})
#
# ENABLE_TESTING()
#
# ADD_EXECUTABLE(lexer_test tests/lexer.cc) TARGET_LINK_LIBRARIES(lexer_test
# ${GTEST_LIB}) ADD_TEST(NAME FSM_TEST COMMAND lexer_test) ENDIF ()

LINK_LIBRARIES(docopt)

ADD_EXECUTABLE(embryo-version src/cmd/embryo-version/main.cc)
ADD_EXECUTABLE(embryo-lex src/cmd/embryo-lex/main.cc)
ADD_EXECUTABLE(embryo-parse src/cmd/embryo-parse/main.cc)
ADD_EXECUTABLE(embryo-undump src/cmd/embryo-undump/main.cc)
ADD_EXECUTABLE(embryo-vm src/cmd/embryo-vm/main.cc)
ADD_EXECUTABLE(embryo-compile src/cmd/embryo-compile/main.cc)
ADD_EXECUTABLE(embryo-sort-benchmark src/cmd/embryo-sort-benchmark/main.cc)

#ADD_EXECUTABLE(tmp tmp.cc)
