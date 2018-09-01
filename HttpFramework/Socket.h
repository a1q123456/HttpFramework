#pragma once

#include "EAddressFamily.h"
#include "EAddressType.h"
#include "EProtocolType.h"
#include "SocketError.h"
#include <experimental\coroutine>
#include <future>
#include <string>
#include <type_traits>
#include "Await.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

namespace Net::Sockets
{
	enum class ESocketLineBreak
	{
		CR,
		LF,
		CRLF
	};

	class Socket
	{
	private:
		static constexpr uint32_t portMax = 65535;
		static constexpr uint32_t portMin = 0;
		EAddressFamily addressFamily;
		ESocketType socketType;
		EProtocolType protocol;
		SOCKET _socket = INVALID_SOCKET;
		PTP_IO _io = nullptr;
		bool server_mode;
		bool client_mode;
		bool disposed = false;
		Socket(SOCKET socket);
	public:
		explicit Socket(EAddressFamily addressFamily, ESocketType addressType, EProtocolType protocol) noexcept;
		Socket(const Socket&) = delete;
		Socket() noexcept {}
		Socket& operator=(const Socket&) = delete;
		Socket& operator=(Socket&&) noexcept;
		Socket(Socket&& another) noexcept;
		void Bind(std::string ip, uint32_t port);
		void Listen(int backlog);
		Async::Awaiter<int> ConnectAsync(std::string ip, uint32_t port);

		Async::Awaiter<int> ReceiveAsync(std::byte* buffer, std::size_t size);

		template<std::size_t size>
		Async::Awaiter<int> ReceiveAsync(std::byte (&buffer)[size])
		{
			return ReceiveAsync(buffer, size);
		}

		template<typename CharT>
		Async::Awaiter<std::basic_string<CharT>> ReceiveLineAsync(ESocketLineBreak br = ESocketLineBreak::CRLF)
		{
			static_assert(std::is_integral_v<CharT>, "CharT must be an integral type");
			constexpr const char CR = '\r';
			constexpr const char LF = '\n';
			std::basic_string<CharT> line;
			std::byte buffer[sizeof(CharT)];
			while (true)
			{
				co_await ReceiveAsync(buffer);
				CharT character;
				// copy bytes to character
				memcpy_s(&character, sizeof(CharT), buffer, sizeof(CharT));
				// append character to line string
				if (br == ESocketLineBreak::CRLF && buffer[0] == CR)
				{
					std::byte buffer2[sizeof(CharT)];
					co_await ReceiveAsync(buffer2);

					CharT character2;
					memcpy_s(&character2, sizeof(CharT), buffer2, sizeof(CharT));
					if (character2 == LF)
					{
						co_return line;
					}

					line += character;
					line += character2;
				}
				else if (character == CR && ESocketLineBreak::CR == br)
				{
					co_return line;
				}
				else if (character == LF && ESocketLineBreak::LF == br)
				{
					co_return line;
				}
				else
				{
					line += character;
				}
			}
		}

		Async::Awaiter<int> SendAsync(std::byte* buffer, std::size_t size);

		template<std::size_t size>
		Async::Awaiter<int> SendAsync(std::byte(&buffer)[size])
		{
			return SendAsync(buffer, size);
		}
		
		Async::Awaiter<Socket> AcceptAsync();

		virtual ~Socket() noexcept;
		void Dispose();
		bool IsDisposed() const;
	};
}
