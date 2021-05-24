cmake_minimum_required(VERSION 3.0.2)
project(topsens_realsense)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  std_srvs
)

catkin_package(
 CATKIN_DEPENDS roscpp std_msgs std_srvs
)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${catkin_INCLUDE_DIRS}
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

add_executable(${PROJECT_NAME}
               src/main.cpp
               src/mainwindow.cpp
               src/mainwindow.ui
               src/ulabel.cpp)
               
find_package(Qt5 COMPONENTS Widgets REQUIRED)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} Qt5::Widgets)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)