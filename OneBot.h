// OneBot.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#ifndef ONEBOT_H
#define ONEBOT_H

#include <iostream>
#include<string>
#include"httplib.h"
#include"json.hpp"
#include<windows.h>
#include<chrono>
#include<thread>

void NapCatINFO(int64_t group_id, const std::string& message);
std::string Call_llm(const std::string& user_input);
void qq_event(nlohmann::json event);

#endif // !ONEBOT_H



// TODO: 在此处引用程序需要的其他标头。
