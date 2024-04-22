#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <libssh/libssh.h>

class Client
{

public:
	Client();

	~Client();

	void
	connect(const std::string & user, const std::string & ip, const std::string & pass);

	std::string
	run_command(const std::string & cmd);

	void
	disconnect();
  
private:
	ssh_session session;
	ssh_channel channel;
	char buffer[1024];
};

#endif
