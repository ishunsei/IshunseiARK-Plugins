#include "CrossServerChat.h"

#pragma comment(lib, "ArkApi.lib")

extern "C" __declspec(dllexport) void Plugin_Unload()
{
    UnLoad();
}

extern "C" __declspec(dllexport) void Plugin_Init()
{
#if defined(_WIN64)
    SetConsoleOutputCP(65001);
#endif
    Load();
}