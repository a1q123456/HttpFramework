// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define _CRTDBG_MAP_ALLOC  
#include <stdio.h>
#include <tchar.h>
#include <Ws2tcpip.h>
#include <WinSock2.h>
#define NOMINMAX
#include <Windows.h>
#include <experimental\coroutine>
#include <future>
#include <tuple>
#include <string>
#include <iostream>
#include <unordered_map>
#include <crtdbg.h> 
#include <algorithm>
#include <ctime>
#include <chrono>
#include <codecvt>

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
#include <experimental/resumable>
#endif

// TODO: 在此处引用程序需要的其他头文件

#include <Mswsock.h>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#endif