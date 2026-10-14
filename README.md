# OneBot

基于NapCat的OneBotCpp实现

## 目录

```
OneBot.cpp            
OneBot.h              
CMakeLists.txt        构建定义
CMakePresets.json     配置预设
httplib.h             cpp-httplib，HTTP 
json.hpp              nlohmann/json
onebot11.example.json NapCat  OneBot 配置模板
```

## 依赖

除了上述两个单头库，不需要额外依赖。Windows 上会链接 `ws2_32`。

## 三个端口

| 方向 | 谁连谁 | 地址 | 代码位置 |
|---|---|---|---|
| 收群消息 | NapCat `httpClients` → 本程序 | `http://127.0.0.1:3001/onebot` | `OneBot.cpp:103` `svr.listen` |
| 发群消息 | 本程序 → NapCat `httpServers` | `http://127.0.0.1:3000` | `OneBot.cpp:13` `httplib::Client` |
| 调模型 | 本程序 → LM Studio | `http://127.0.0.1:1234/v1/chat/completions` | `OneBot.cpp:34` |

**NapCat 侧两个都要开**：`httpServers`（3000，用来发消息）和 `httpClients`（3001，用来收事件上报）。
只开 `httpServers` 的话事件不会推到本程序，机器人收不到任何消息。

## NapCat 

安装napcat [NapCat](https://github.com/NapNeko/NapCatQQ/releases/download/v4.18.38/NapCat.Shell.zip)
装好新环境时，把 `onebot11.example.json` 复制成 NapCat 的
`config\onebot11_<你的QQ号>.json`，再按需改端口和 token。

## 安全

NapCat 的 `config/` 目录里有 **webui token、passkey、协议配置** 等凭据
注意别 `git add -f` 把它的 `config/` 带进来。凭据一旦进 git 历史就删不掉了，只能吊销重发。
