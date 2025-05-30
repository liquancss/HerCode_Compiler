cmake_minimum_required(VERSION 3.10)
project(SayCompiler C)

# 设置C标准
set(CMAKE_C_STANDARD 11)

# 包含目录
include_directories(include)

# 生成可执行文件
add_executable(hercode_compiler
    src/main.c
    src/lexer.c
    src/parser.c
    src/codegen.c
    src/ast.c
)
add_compile_options(-Wall -Werror -Wstrict-prototypes -Wmissing-prototypes -O2 -Os)

install(TARGETS hercode_compiler DESTINATION bin)