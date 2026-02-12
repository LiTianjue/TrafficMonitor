

# 指定最低支持的 CMake 版本
cmake_minimum_required(VERSION 3.14)

# 项目名称、版本和语言
project(MyProject
    VERSION 1.0.0
    DESCRIPTION "A sample C++ project using CMake"
    LANGUAGES CXX)

# 设置 C++ 标准（C++17 示例）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出构建类型提示（Debug / Release）
message(STATUS "Building ${PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 添加头文件搜索路径（全局）
include_directories(include)
include_directories(./)
include_directories(./third/Crow/include/)
include_directories(./third/yalantinglibs/include/)
include_directories(./third/yalantinglibs/include/ylt/thirdparty/)
include_directories(./third/yalantinglibs/include/ylt/standalone/)


# 链接选项

# 子目录：编译源码
#add_subdirectory(src)

set(SOURCES main.cpp)

#add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(myapp ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(myapp Threads::Threads)




# 启用测试（可选）
# enable_testing()
# add_subdirectory(tests)

