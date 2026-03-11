cmake_minimum_required(VERSION 3.21)

project(SmartHomeMonitorQt6 LANGUAGES CXX)

# 基础编译配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt自动生成配置（MOC/UI/RCC）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt6依赖模块（保留你原有所有模块）
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Sql Multimedia Charts)
# ========== 批量收集所有文件 ==========
# 1. 递归匹配src目录下所有头文件（自动感知新增文件）
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

# 2. 递归匹配src目录下所有源文件（自动感知新增文件）
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# 3. 递归匹配src目录下所有UI文件（自动感知新增文件）
file(GLOB_RECURSE FORMS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui"
)

# 4. 匹配音频资源文件（自动感知新增文件）
file(GLOB_RECURSE AUDIO_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/sounds/*.wav"
)

# 5. 资源文件（保留原有qrc）
set(RESOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.qrc"
)

# ========== 构建可执行文件（添加音频文件到项目） ==========
add_executable(${PROJECT_NAME}
  ${HEADERS}
  ${SOURCES}
  ${FORMS}
  ${AUDIO_FILES}  # 新增：加入音频文件
  ${RESOURCES}
)

# 头文件包含路径
target_include_directories(${PROJECT_NAME} PRIVATE src)

# 链接Qt6库
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt6::Core
  Qt6::Widgets
  Qt6::Network
  Qt6::Sql
  Qt6::Multimedia
  Qt6::Charts
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  APP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# ========== 按目录结构分组显示（让音频文件出现在项目视图） ==========
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADERS} ${SOURCES} ${FORMS} ${AUDIO_FILES})
source_group("Resources" FILES ${RESOURCES})
