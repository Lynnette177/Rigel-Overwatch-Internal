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
void MainThread() {
	using namespace OW;
	/*while (Config::got != 1) {
		std::cout << skCrypt("Waiting Initialize...\n");
		Sleep(3000);
	}*/
	std::cout << skCrypt("Launch in MainMenu then Go to Practice Range...\n");
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
int i = 0;
unsigned short tempyaw = 0;
unsigned short temppitch = 0;



DWORD WINAPI MainProject(LPVOID lpParam)
{
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
