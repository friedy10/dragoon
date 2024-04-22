/*
 *	Copyright (c) 2024 Friedrich Doku (frd20@pitt.edu)
 */
#include <iostream>
#include <string>
#include <stdexcept>
#include <libssh/libssh.h>

class Client
{
  private:
	ssh_session session;
	ssh_channel channel;
	char buffer[1024];

  public:
	Client();

	~Client();

	void
	connect(const std::string & user, const std::string & ip, const std::string & pass);

	std::string
	runCommand(const std::string & cmd);

	void
	disconnect();
};
