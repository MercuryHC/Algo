file(GLOB_RECURSE SOURCES "*.cpp")

add_library(${PROJECT_NAME}_lib ${SOURCES})

add_executable(${PROJECT_NAME}_main main.cpp)
target_link_libraries(${PROJECT_NAME}_main PRIVATE ${PROJECT_NAME}_lib)