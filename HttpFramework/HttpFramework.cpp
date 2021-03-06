// NewHdwApi.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "Socket.h" 
#include <experimental/resumable>
#include "Await.h"
#include <chrono>

constexpr const int TEST_COUNT = 500;

Async::Awaiter<int> compute_value()
{
	int result = co_await std::async([]()
	{
		return 30;
	});

	co_return result;
}

void SetOutputColor(uint8_t c)
{
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, c);
}

class Console
{
public:
	template <typename CharT, typename ...Args>
	static void Write(uint8_t color, CharT* str, Args ...args)
	{
		static std::mutex mutex;
		std::unique_lock<std::mutex> guard;
		SetOutputColor(color);
		printf(str, args...);
	}
};
 

Async::Awaiter<void> serve(Net::Sockets::Socket clientSocket)
{
	try
	{
		std::byte buffer[2] = { std::byte(1), std::byte(2) };
		co_await clientSocket.SendAsync(buffer);
		co_await clientSocket.ReceiveAsync(buffer);

		if (buffer[0] == std::byte(3) && buffer[1] == std::byte(4))
		{
			Console::Write(10, "server success\n");
		}
		else
		{
			Console::Write(12, "server error\n");
		}
	}
	catch (const Net::Sockets::SocketError& /* e */)
	{
		Console::Write(12, "server exception\n");
	}
	catch (...)
	{
		Console::Write(12, "catch ...");
	}
}

Async::Awaiter<void> startServer()
{
	using namespace Net::Sockets;
	try
	{
		Socket socket(EAddressFamily::InternetworkV4, ESocketType::Stream, EProtocolType::Tcp);
		socket.Bind("0.0.0.0", 1568);
		socket.Listen(2);
		for (int i = 0; i < TEST_COUNT; i++)
		{
			Socket client = co_await socket.AcceptAsync();
			serve(std::move(client));
		}
	}
	catch (const SocketError& e)
	{
		int len = WideCharToMultiByte(CP_THREAD_ACP, WC_COMPOSITECHECK, e.Message().c_str(), -1, NULL, 0, NULL, NULL);
		auto tmp = std::unique_ptr<char[]>(new char[len]);
		ZeroMemory(tmp.get(), len);
		WideCharToMultiByte(CP_THREAD_ACP, WC_COMPOSITECHECK, e.Message().c_str(), -1, tmp.get(), len, NULL, NULL);
		Console::Write(12, tmp.get());
	}
}

Async::Awaiter<void> client()
{
	using namespace Net::Sockets;
	try
	{
		Socket client(EAddressFamily::InternetworkV4, ESocketType::Stream, EProtocolType::Tcp);
		co_await client.ConnectAsync("127.0.0.1", 1568);
		std::byte buffer[2] = { std::byte(3), std::byte(4) };
		co_await client.SendAsync(buffer);
		co_await client.ReceiveAsync(buffer);
		if (buffer[0] == std::byte(1) && buffer[1] == std::byte(2))
		{
			Console::Write(10, "client success\n");
		}
		else
		{
			Console::Write(12, "client error\n");
		}
	}
	catch (const SocketError& e)
	{
		int len = WideCharToMultiByte(CP_THREAD_ACP, WC_COMPOSITECHECK, e.Message().c_str(), -1, NULL, 0, NULL, NULL);
		auto tmp = std::unique_ptr<char[]>(new char[len]);
		ZeroMemory(tmp.get(), len);
		WideCharToMultiByte(CP_THREAD_ACP, WC_COMPOSITECHECK, e.Message().c_str(), -1, tmp.get(), len, NULL, NULL);
		Console::Write(12, tmp.get());
	}
}

void startClient()
{
	using namespace Net::Sockets;

	for (int i = 0; i < 500; i++)
	{
		client();
		Sleep(10);
	}
}

void do_test()
{
	try
	{
		auto serverFuture = startServer();
		startClient();
		Async::Awaiter<void>::WaitAll(serverFuture);
	}
	catch (...)
	{
		Console::Write(15, "err");
	}
}

int main() 
{
	do_test();

	_CrtDumpMemoryLeaks();
	return 0;
}

