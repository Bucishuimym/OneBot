#include<iostream>
#include<string>
#include"httplib.h"
#include"json.hpp"
#include<windows.h>
#include<chrono>
#include<thread>

using namespace std;
using json = nlohmann::json;

void NapCatINFO(int64_t group_id, const std::string& message) {
	httplib::Client qq_cli("127.0.0.1", 3000);

	json send_req = {
		{"action","send_group_msg"},
		{"params",{
			{"group_id",group_id},
			{"message",message}
		}}
	};
	auto res = qq_cli.Post("/send_group_msg", send_req.dump(), "application/json");

	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&time);

	if (!res || res->status != 200) {
		std::cerr << " " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [INFO]: error!" << std::endl;
	}
}

std::string Call_llm(const std::string& user_input) {
	httplib::Client llm_cli("127.0.0.1", 1234);
	llm_cli.set_read_timeout(300, 0);

	json body = {
	{"model", "zai-org/glm-4.7-flash"},
	{"messages",json::array({
		{{"role", "system"},{"content","你是一个人类扮演者，正常简短的回复QQ收到的消息"}},
		{{"role", "user"}, {"content", user_input}}
	})},
		{"temperature",0.6},
		{"max_tokens",512}
	};

	auto res = llm_cli.Post("/v1/chat/completions", body.dump(), "application/json");
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&time);

	if (res && res->status == 200) {
		auto resp = json::parse(res->body);
		return resp["choices"][0]["message"]["content"];
	}
	std::cout << " " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return "I'm unable to respond for now.";
}

void qq_event(json event) {
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&time);

	try {
		if (event.value("post_type", "") != "message" || event.value("message_type", "") != "group")
			return;

		if (event.value("user_id", 0LL) == event.value("self_id", 0LL)) return;

		int64_t group_id = event.value("group_id", 0LL);
		std::string user_msg = event["raw_message"];

		if (user_msg.empty() || user_msg.find("[CQ:image") != std::string::npos)return;
		std::cout << " " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [INFO] <- " << user_msg << endl;

		string reply_msg = Call_llm(user_msg);
		NapCatINFO(group_id, reply_msg);
		cout << " " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [REPLY] ->" << reply_msg << endl;
	}
	catch (const exception& e) {
		cerr << " " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [INFO]: Error processing event!" << e.what() << endl;
	}
}

int main() {
	SetConsoleOutputCP(CP_UTF8);

	httplib::Server svr;

	svr.Post("/onebot", [](const httplib::Request& req, httplib::Response& res) {
		res.status = 204;                          
		res.set_content("{}", "application/json");

		try {
			auto event = json::parse(req.body);
			std::thread([event] { qq_event(event); }).detach();   
		}
		catch (const std::exception& e) {
			std::cerr << " [INFO] [ERROR] Failed to pares: " << e.what() << "\n";
		}
		});

	std::cout << "bot listening on 127.0.0.1:3001\n";
	svr.listen("127.0.0.1", 3001);                 
}
