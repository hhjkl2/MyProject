# 服务器代码
# 最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称（需与文件夹名称SERVER对应）
project(SERVER)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成可执行文件
add_executable(${PROJECT_NAME} 
  main.cpp  # 包含你的主程序文件
)