cmake_minimum_required(VERSION 3.5)
project(turtle_teleop_and_pose)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 寻找必要的依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim_msgs REQUIRED)

# 添加 include 目录（如果有头文件）
include_directories(include)

# 添加可执行文件
add_executable(turtle_teleop_and_pose src/turtle_teleop_and_pose.cpp)

# 为目标文件添加所需的依赖包
ament_target_dependencies(turtle_teleop_and_pose rclcpp geometry_msgs turtlesim_msgs)

# 安装可执行文件
install(TARGETS
  turtle_teleop_and_pose
  DESTINATION lib/${PROJECT_NAME})

# 确保包可以被ament找到
ament_package()
