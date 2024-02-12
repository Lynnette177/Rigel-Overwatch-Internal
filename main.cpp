#include <Windows.h>
#include "auth.hpp"
#include <string>
#include "utils.hpp"
#include "skStr.h"
#include "includes.hpp"
#include "SDK.hpp"
#include "Overwatch.hpp"
#include <io.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include "USEVEH.h"
#include "menu.h"

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);
using namespace KeyAuth;

HANDLE hMainThread;
CONTEXT c{};
std::string name = "imp"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "ss3JcGuVnA"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "f8ea20f06bac7ff17c73df0dec9d5bd5ab1cbadcddd8ace7a03f213e0643318d"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "3.5"; // leave alone unless you've changed version on website
std::string url = skCrypt("https://keyauth.win/api/1.2/").decrypt(); // change if you're self-hosting
void MainThread() {
	using namespace OW;
	/*while (!SDK->Initialize() || !SDK->GetGlobalKey())
	{
		std::cout << "等待开启守望先锋..\n";
		Sleep(2000);
	}*/
	std::cout << skCrypt("Launch in MainMenu then Go to Practice Range...\n");
	/*while (Config::got != 1) {
		Sleep(1000);
	}*/
	while (!SDK->GetGlobalKey()) {
		Sleep(10);
	}

	_beginthread((_beginthread_proc_type)entity_scan_thread, 0, 0);
	Sleep(50);
	_beginthread((_beginthread_proc_type)entity_thread, 0, 0);
	_beginthread((_beginthread_proc_type)viewmatrix_thread, 0, 0);
	Sleep(500);
	_beginthread((_beginthread_proc_type)aimbot_thread, 0, 0);
	//_beginthread((_beginthread_proc_type)overlay_thread, 0, 0);

	menu::Initialize();

	_beginthread((_beginthread_proc_type)configsavenloadthread, 0, 0);
	Sleep(10);
	//_beginthread((_beginthread_proc_type)playmusicthread, 0, 0);

	while (FindWindowA(skCrypt("TankWindowClass"), NULL))
	{
		auto viewMatrixVal = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base) ^ offset::offset_viewmatrix_xor_key;
		viewMatrixVal = SDK->RPM<uint64_t>(viewMatrixVal + 0x5C0);
		viewMatrixVal = SDK->RPM<uint64_t>(viewMatrixVal + 0x118);
		//mutex.lock();
		viewMatrix_xor_ptr = viewMatrixVal + 0x130;
		//mutex.unlock();
		auto view = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base_test) + offset::offset_viewmatrix_ptr;
		//mutex.lock();
		viewMatrixPtr = view;
		//mutex.unlock();
		//viewMatrixPtr = viewMatrix_xor_ptr;

		Sleep(100);
	}
}
/*
	Video on what ownerid and secret are https://youtu.be/uJ0Umy_C6Fg

	Video on how to add KeyAuth to your own application https://youtu.be/GB4XW_TsHqA

	Video to use Web Loader (control loader from customer panel) https://youtu.be/9-qgmsUUCK4
*/

api KeyAuthApp(name, ownerid, secret, version, url);
int i = 0;
unsigned short tempyaw = 0;
unsigned short temppitch = 0;



DWORD WINAPI MainProject(LPVOID lpParam)
{
	if (OW::Config::loginornot) {
		std::string consoleTitle = (std::string)skCrypt("Loader - Built at:  ") + compilation_date + " " + compilation_time;
		SetConsoleTitleA(consoleTitle.c_str());
		std::cout << skCrypt("\n\n连接服务器..");
		KeyAuthApp.init();
		if (!KeyAuthApp.data.success)
		{
			std::cout << skCrypt("\n 状态: ") << KeyAuthApp.data.message;
			Sleep(1500);
			exit(0);
		}


		//Optional - check if HWID or IP blacklisted

		if (KeyAuthApp.checkblack()) {
			abort();
		}


		//std::cout << skCrypt("\n\n App data:");
		//std::cout << skCrypt("\n Number of users: ") << KeyAuthApp.data.numUsers;
		//std::cout << skCrypt("\n Number of online users: ") << KeyAuthApp.data.numOnlineUsers;
		//std::cout << skCrypt("\n Number of keys: ") << KeyAuthApp.data.numKeys;
		std::cout << skCrypt("\n 当前版本: ") << KeyAuthApp.data.version;
		//std::cout << skCrypt("\n Customer panel link: ") << KeyAuthApp.data.customerPanelLink;
		//std::cout << skCrypt("\n Checking session validation status (remove this if causing your loader to be slow)");
		KeyAuthApp.check();
		std::cout << skCrypt("\n 状态: ") << KeyAuthApp.data.message;

		if (std::filesystem::exists(".\\test.json")) //change test.txt to the path of your file :smile:
		{
			if (!CheckIfJsonKeyExists(".\\test.json", "username"))
			{
				std::string key = ReadFromJson(".\\test.json", "license");
				KeyAuthApp.license(key);
				if (!KeyAuthApp.data.success)
				{
					std::remove(".\\test.json");
					std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
					Sleep(1500);
					exit(0);
				}
				std::cout << skCrypt("\n成功自动登入，若更换卡密请自行删除test.json");
			}
			else
			{
				std::string username = ReadFromJson(".\\test.json", "username");
				std::string password = ReadFromJson(".\\test.json", "password");
				KeyAuthApp.login(username, password);
				if (!KeyAuthApp.data.success)
				{
					std::remove(".\\test.json");
					std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
					Sleep(1500);
					exit(0);
				}
				std::cout << skCrypt("\n成功自动登入");
			}
			KeyAuthApp.log("禁止顶号");
		}
		else
		{
			std::string username;
			std::string password;
			std::string key;
			std::cout << skCrypt("\n 输入您的卡号: ");
			std::cin >> key;
			KeyAuthApp.license(key);
			if (!KeyAuthApp.data.success)
			{
				std::cout << skCrypt("\n 状态: ") << KeyAuthApp.data.message;
				Sleep(1500);
				exit(0);
			}
			if (username.empty() || password.empty())
			{
				WriteToJson(".\\test.json", "license", key, false, "", "");
				std::cout << skCrypt("成功创建自动登录文件，若更换卡密请自行删除");
			}
			else
			{
				WriteToJson(".\\test.json", "username", username, true, "password", password);
				std::cout << skCrypt("成功创建自动登录文件，若更换卡密请自行删除");
			}


		}

		std::cout << skCrypt("\n用户数据:");
		//std::cout << skCrypt("\n Username: ") << KeyAuthApp.data.username;
		std::cout << skCrypt("\n IP地址: ") << KeyAuthApp.data.ip;
		std::cout << skCrypt("\n HWID: ") << KeyAuthApp.data.hwid;
		std::cout << skCrypt("\n 激活日期: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.createdate)));
		std::cout << skCrypt("\n 上一次登录: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.lastlogin)));
		std::cout << skCrypt("\n 卡密信息:\n ");

		for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) { // Prompto#7895 was here
			auto sub = KeyAuthApp.data.subscriptions.at(i);
			//std::cout << skCrypt("\n name: ") << sub.name;
			std::cout << skCrypt("卡号过期日期: ") << tm_to_readable_time(timet_to_tm(string_to_timet(sub.expiry)));
			expiretime = string_to_timet(sub.expiry);
		}

		std::cout << skCrypt("\n 检查激活状态..");
		KeyAuthApp.check();
		std::cout << skCrypt("\n 状态: ") << KeyAuthApp.data.message;

		_beginthread((_beginthread_proc_type)OW::looprpmthread, 0, 0);
		MainThread();
	}
	if (!OW::Config::loginornot) {
		_beginthread((_beginthread_proc_type)OW::looprpmthread, 0, 0);
		MainThread();
	}
	while (true) {}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		printf("Injected Successfully!\n");

		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, 0, MainProject, lpReserved, 0, NULL);
	}
	return 1;
	

	using namespace OW;

	bool is_login = false;
	std::string license{};

	std::cout << "Loading..\n";
	MainThread();
	Sleep(10000);
	exit(0);
}
std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}
