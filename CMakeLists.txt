cmake_minimum_required(VERSION 3.10)

project(OneBot)

# 单文件工程，全部逻辑在 OneBot.cpp 里：
#   NapCatINFO  —— 调 OneBot API 发群消息（NapCat httpServers，3000）
#   Call_llm    —— 调 LM Studio 的 /v1/chat/completions（1234）
#   qq_event    —— 处理一条上报事件
#   main        —— httplib 服务器，监听 127.0.0.1:3001 收 httpClients 的事件上报
add_executable(OneBot OneBot.cpp OneBot.h httplib.h json.hpp)

# 头文件都在工程根目录（httplib.h / json.hpp / OneBot.h）
target_include_directories(OneBot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
	target_link_libraries(OneBot PRIVATE ws2_32)
endif()

if(CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET OneBot PROPERTY CXX_STANDARD 20)
	set_property(TARGET OneBot PROPERTY CXX_STANDARD_REQUIRED NO)
endif()

# 源码按 UTF-8 解析，避免 GBK 代码页下对 httplib.h 报 C4819
if(MSVC)
	target_compile_options(OneBot PRIVATE /utf-8)
endif()