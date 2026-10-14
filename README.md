# OneBot

一个 C++ 写的 QQ 群聊机器人：收 NapCat 上报的群消息 → 转给本地 LM Studio → 把回复发回群里。

## 目录

```
OneBot.cpp            全部逻辑（单文件工程）
OneBot.h              函数声明（当前没有被 include，备用）
CMakeLists.txt        构建定义
CMakePresets.json     配置预设（x64/x86 Debug/Release）
httplib.h             cpp-httplib，HTTP 服务端 + 客户端
json.hpp              nlohmann/json
onebot11.example.json NapCat 侧 OneBot 配置模板（token 留空）
```

## 依赖

除了上述两个单头库，不需要额外依赖。Windows 上会链接 `ws2_32`。

## 构建

需要 MSVC（工程按 VS 的 Ninja preset 配的），先在 **VS 开发者命令行**里跑，或手动载入环境：

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake --preset x64-debug
cmake --build out\build\x64-debug
```

产物：`out\build\x64-debug\OneBot.exe`。

> 注意 `CMakePresets.json` 里只有 `configurePresets`，没有 `buildPresets`，
> 所以 `cmake --build --preset x64-debug` 会报 `No such build preset`，
> 用上面 `--build <目录>` 的写法。

## 三个端口

| 方向 | 谁连谁 | 地址 | 代码位置 |
|---|---|---|---|
| 收群消息 | NapCat `httpClients` → 本程序 | `http://127.0.0.1:3001/onebot` | `OneBot.cpp:103` `svr.listen` |
| 发群消息 | 本程序 → NapCat `httpServers` | `http://127.0.0.1:3000` | `OneBot.cpp:13` `httplib::Client` |
| 调模型 | 本程序 → LM Studio | `http://127.0.0.1:1234/v1/chat/completions` | `OneBot.cpp:34` |

**NapCat 侧两个都要开**：`httpServers`（3000，用来发消息）和 `httpClients`（3001，用来收事件上报）。
只开 `httpServers` 的话事件不会推到本程序，机器人收不到任何消息。

## NapCat 配置

NapCat 本体**不在本仓库里**，是独立安装在 `K:\XXFS\AppData\NapCat`（便携目录，launcher 用相对路径，
整个目录可搬）。仓库里只放一份配置模板。

装好新环境时，把 `onebot11.example.json` 复制成 NapCat 的
`config\onebot11_<你的QQ号>.json`，再按需改端口和 token。

## 安全

NapCat 的 `config/` 目录里有 **webui token、passkey、协议配置** 等凭据，**不要提交到仓库**。
本仓库的 `.gitignore` 已经排除了 `napcat/`；如果你把 NapCat 放在了别处，
注意别 `git add -f` 把它的 `config/` 带进来。凭据一旦进 git 历史就删不掉了，只能吊销重发。

## 已知注意点

- `OneBot.cpp:72` 过滤图片消息时写的是 `[CQ:inage`（`image` 拼错了），
  所以图片消息其实不会被过滤掉，会原样丢给模型。
- `onebot11.example.json` 里 `httpClients.reportSelfMessage` 为 `true`，
  即机器人自己发的消息也会被当成事件上报回来；而 `qq_event` 没有过滤 `self_id`，
  可能造成「自己回复自己」的循环。不需要这个行为的话关掉它。
- `Call_llm` 的请求体里没有 `reasoning_effort`。LM Studio 跑 GLM-4.7 系模型时，
  不显式传 `reasoning_effort: "none"` 会导致 `message.content` 返回空串，
  机器人就会发出空消息。