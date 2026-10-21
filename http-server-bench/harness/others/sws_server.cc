#include "server_http.hpp"

#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using Response = HttpServer::Response;
using Request = HttpServer::Request;

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		return 2;
	}
	auto const port{static_cast<unsigned short>(std::atoi(argv[1]))};
	std::string const root{argv[2]};

	HttpServer server;
	server.config.port = port;
	server.config.address = "127.0.0.1";

	server.default_resource["GET"] = [root](std::shared_ptr<Response> response,
											std::shared_ptr<Request> request)
	{
		std::string path{request->path};
		if (path == "/")
		{
			path = "/index.html";
		}
		if (path.find("..") != std::string::npos)
		{
			response->write(SimpleWeb::StatusCode::client_error_bad_request, "bad path");
			return;
		}
		std::ifstream file{root + path, std::ios::binary};
		if (!file)
		{
			response->write(SimpleWeb::StatusCode::client_error_not_found, "not found");
			return;
		}
		std::string body{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
		auto const ct{path.size() >= 5 && path.compare(path.size() - 5, 5, ".html") == 0
						  ? "text/html"
						  : "application/octet-stream"};
		SimpleWeb::CaseInsensitiveMultimap header;
		header.emplace("Content-Type", ct);
		response->write(body, header);
	};

	server.start();
	return 0;
}
