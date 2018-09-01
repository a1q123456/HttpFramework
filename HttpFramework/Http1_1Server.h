#pragma once
#include "Socket.h"
#include "Http1_1Packet.h"
#include "Await.h"
#include <array>
#include "StringUtils.h"
#include "InvalidHttpMessageError.h"
#include "CodeConvertion.h"
#include <chrono>

namespace Net::Http
{
	class Http1_1Server
	{
		Net::Sockets::Socket _socket;
		bool keepalive = true;
		std::chrono::steady_clock::time_point deadTime;
		int requestCount = 0;
		int keepaliveMaxRequestCount = -1;
		std::chrono::seconds keepaliveTimeout;
	public:
		Http1_1Server(Net::Sockets::Socket&& socket) : _socket(std::move(socket))
		{
			using namespace std::chrono_literals;
			keepaliveTimeout = 5s;
		}

		Async::Awaiter<void> Serve()
		{
			while (keepalive)
			{
				bool isHeadRequest = false;
				auto requestMessage = co_await Parse();
				// server will ignore all response message
				if (requestMessage.direction != EHttpDirection::Request)
				{
					co_return;
				}
				if (requestMessage.method == "HEAD")
				{
					isHeadRequest = true;
				}

				Http1_1Packet responseMessage;

				// TODO



				auto responseData = Format(responseMessage);

				co_await _socket.SendAsync(responseData.data(), responseData.size());
				auto lifeTimeManager = [&]()
				{
					while (true)
					{
						auto now = std::chrono::steady_clock::now();
						if (
							(now >= deadTime && deadTime.time_since_epoch() != decltype(deadTime)::duration(0)) ||
							(requestCount >= keepaliveMaxRequestCount && keepaliveMaxRequestCount != -1)
							)
						{
							keepalive = false;
							_socket.Dispose();
						}
					}
				};
			}
		}

		std::vector<std::byte> Format(const Http1_1Packet& message)
		{
			auto tmp = Http1_1Packet(message);
			return Format(tmp);
		}

		std::vector<std::byte> Format(Http1_1Packet& message)
		{
			// real format

			std::vector<std::byte> ret;

			// TODO

			return ret;
		}

		std::vector<std::byte> Format(Http1_1Packet&& message)
		{
			return Format(message);
		}

		Async::Awaiter<Http1_1Packet> Parse(std::function<void(const Http1_1Packet&)> messageHandler = nullptr, int receiveBufferLength = -1, bool responseToHeadRequest = false)
		{
			using namespace std::chrono_literals;

			std::string startLine;
			Http1_1Packet ret;
			// read start line
			while (startLine.empty())
			{
				startLine = co_await _socket.ReceiveLineAsync<char>();
			}

			std::size_t count;

			try
			{
				auto startLineParts = Text::StringUtils::Split<std::array<std::string, 3>>(startLine, ' ', count);

				if (Text::StringUtils::IsInteger(startLineParts[1]))
				{
					ret.direction = EHttpDirection::Response;
					ret.version = startLineParts[0];
					ret.statusCode = std::atoi(startLineParts[1].c_str());
					ret.statusText = startLineParts[2];
				}
				else
				{
					ret.direction = EHttpDirection::Request;
					ret.method = startLineParts[0];
					ret.uri = startLineParts[1];
					auto uriQuery = Text::StringUtils::Split<std::array<std::string, 2>>(ret.uri, '?', count);
					if (count == 2)
					{
						ret.query = uriQuery[1];
					}
					ret.version = startLineParts[2];
				}

				// read headers
				while (true)
				{
					auto headerLine = co_await _socket.ReceiveLineAsync<char>();
					if (headerLine.empty())
					{
						break;
					}
					Text::StringUtils::ToLower(headerLine);
					std::string_view sv(headerLine.c_str());
					auto delim = sv.find_first_of(":");
					if (delim == sv.npos)
					{
						throw InvalidHttpMessageError();
					}
					auto k = std::string(sv.substr(0, delim));
					std::string v;
					if (delim == sv.length - 1)
					{
						v = "";
					}
					else
					{
						v = std::string(sv.substr(delim + 1));
					}

					if (ret.headers.find(k) == ret.headers.end())
					{
						ret.headers.insert(std::make_pair(k, std::vector<std::string>()));
					}
					

					Text::StringUtils::ToLower(Text::StringUtils::Trim(v, ' '));
					auto values = Text::StringUtils::Split<std::vector<std::string>>(v, ',', count);
					ret.headers[k].insert(ret.headers[k].end(), values.begin(), values.end());


					headerLine.clear();
				}
				
				if (ret.headers.find("connection") != ret.headers.end())
				{
					std::string& connectionPolicy = ret.headers.at("connection").front();
					if (connectionPolicy == "keep-alive")
					{
						keepalive = true;
					}
					else
					{
						keepalive = false;
					}
				}

				int contentLength = -1;

				if (ret.headers.find("content-length") != ret.headers.end())
				{
					std::string& contentLengthValue = ret.headers.at("content-length").front();
					if (Text::StringUtils::IsInteger(contentLengthValue))
					{
						contentLength = std::atoi(contentLengthValue.c_str());
					}
				}

				if (ret.direction == EHttpDirection::Response)
				{
					// all message which status code is 1xx 204 304 or a reponse to HEAD request must not have a message body
					if (ret.statusCode == 204 || ret.statusCode == 304 || ret.statusCode / 100 == 1 || responseToHeadRequest)
					{
						return ret;
					}
				}

				if (contentLength == 0)
				{
					return ret;
				}
				std::vector<std::string> encodingChain;
				bool chunked = false;

				if (ret.headers.find("transfer-encoding") != ret.headers.end())
				{
					std::vector<std::string>& transferEncodingValue = ret.headers.at("transfer-encoding");
					if (std::count_if(transferEncodingValue.begin(), transferEncodingValue.end(), [](const std::string& s) { return s != "identity"; }) != 0)
					{
						chunked = true;
						contentLength = -1;
					}
					encodingChain.insert(encodingChain.begin(), transferEncodingValue.rbegin(), transferEncodingValue.rend());
				}

				std::transform(encodingChain.begin(), encodingChain.end(), encodingChain.begin(), [](const std::string& e) -> std::string
				{
					if (e == "x-gzip")
					{
						return "gzip";
					}
					return e;
				});

				// search from last to begin, then remove encodings from content-encoding if it's duplicated to transfer-encoding
				std::remove_if(encodingChain.rbegin(), encodingChain.rend(), [&](const std::string& e) 
				{
					return std::count(encodingChain.begin(), encodingChain.end(), e) >= 2 || e == "chunked";
				});

				ret.encodingChain = std::move(encodingChain);

				if (contentLength == -1 && !chunked)
				{
					if (keepalive)
					{
						throw InvalidHttpMessageError();
					}
					else
					{
						// TODO read until socket closed
					}
				}

				if (keepalive && ret.headers.find("keep-alive") != ret.headers.end())
				{
					std::vector<std::string>& param = ret.headers.at("keep-alive");
					for (auto& i : param)
					{
						auto paramPair = Text::StringUtils::Split<std::array<std::string, 2>>(i, '=', count);
						if (paramPair[0] == "timeout")
						{
							int timeout = atoi(paramPair[1].c_str());
							keepaliveTimeout = std::chrono::seconds(timeout);
						}
						else if (paramPair[0] == "max")
						{
							keepaliveMaxRequestCount = atoi(paramPair[1].c_str());
						}
					}
				}

				deadTime = std::chrono::steady_clock::now() + keepaliveTimeout;

				if (chunked)
				{
					while (true)
					{
						auto lengthStr = co_await _socket.ReceiveLineAsync<char>();
						auto length = std::strtoul(lengthStr.c_str(), nullptr, 16);
						if (errno == ERANGE || length <= 0)
						{
							throw InvalidHttpMessageError();
						}
						std::unique_ptr<std::byte[]> buf(new std::byte[length]);
						_socket.ReceiveAsync(buf.get(), length);
						ret.bodyBytes.insert(ret.bodyBytes.end(), buf.get(), buf.get() + length);
						if (messageHandler)
						{
							messageHandler(ret);
						}
					}
				}
				else
				{
					if (receiveBufferLength <= 0)
					{
						std::unique_ptr<std::byte[]> buf(new std::byte[contentLength]);
						co_await _socket.ReceiveAsync(buf.get(), contentLength);
						ret.bodyBytes.insert(ret.bodyBytes.end(), buf.get(), buf.get() + contentLength);
					}
					else
					{
						if (receiveBufferLength < contentLength)
						{
							int chunkSize = receiveBufferLength;
							std::unique_ptr<std::byte[]> buf(new std::byte[chunkSize]);
							ZeroMemory(buf.get(), chunkSize);
							int totalCount = int(std::ceil(double(chunkSize) / double(receiveBufferLength)));
							for (int count = 1; count <= totalCount; count++)
							{
								co_await _socket.ReceiveAsync(buf.get(), chunkSize);
								ret.bodyBytes.insert(ret.bodyBytes.end(), buf.get(), buf.get() + chunkSize);
								if (messageHandler)
								{
									messageHandler(ret);
								}

								// last chunk
								if (count * chunkSize > receiveBufferLength)
								{
									chunkSize = count * chunkSize - receiveBufferLength;
								}
							}
						}
						else
						{
							std::unique_ptr<std::byte[]> buf(new std::byte[contentLength]);
							co_await _socket.ReceiveAsync(buf.get(), contentLength);
							ret.bodyBytes.insert(ret.bodyBytes.end(), buf.get(), buf.get() + contentLength);
							if (messageHandler)
							{
								messageHandler(ret);
							}
						}
					}
				}
				requestCount++;
				if (!keepalive)
				{
					_socket.Dispose();
				}
			}
			catch (const std::bad_alloc&)
			{
				throw;
			}
			catch (const std::out_of_range&)
			{
				// TODO return 400
				keepalive = false;
				_socket.Dispose();
			}
			catch (const InvalidHttpMessageError&)
			{
				// TODO return 400
				keepalive = false;
				_socket.Dispose();
			}
			catch (const std::exception&)
			{
				// TODO return 5xx
				keepalive = false;
				_socket.Dispose();
			}
			catch (...)
			{
				// TODO return 5xx
				keepalive = false;
				_socket.Dispose();
			}
		}
	};
}
