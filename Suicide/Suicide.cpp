#include "Suicide.h"

#pragma comment(lib, "ArkApi.lib")

extern "C" __declspec(dllexport) void Plugin_Unload()
{
	UnLoad();
}

extern "C" __declspec(dllexport) void Plugin_Init()
{
	Load();
}