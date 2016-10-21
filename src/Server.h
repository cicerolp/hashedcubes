#pragma once

class Server {
public:
	static Server& getInstance();
	
	
	static void run(bool disable_multithreading, ulong port);
	
	void stop();

	static void handler(struct mg_connection* conn, int ev, void *p);

	static void printText(struct mg_connection* conn, const std::string& content, int code);
	static void printJson(struct mg_connection* conn, const std::string& content, int code);

	struct mg_mgr mgr;
	struct mg_connection *nc;

	bool running{ true };

protected:
	struct mg_serve_http_opts http_server_opts;

private:
	Server() = default;
	~Server() = default;
};
