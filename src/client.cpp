#include <iostream>
#include <string>
#include <stdexcept>
#include <libssh/libssh.h>

class Client
{
  private:
	ssh_session session;
	ssh_channel channel;
	char		buffer[1024];

  public:
	Client()
	{
		session = ssh_new();
		if (!session) {
			throw std::runtime_error("Failed to create SSH session.");
		}
	}

	~Client()
	{
		if (channel) {
			ssh_channel_free(channel);
		}
		if (session) {
			ssh_disconnect(session);
			ssh_free(session);
		}
	}

	void
	connect(const std::string & user, const std::string & ip, const std::string & pass)
	{
		ssh_options_set(session, SSH_OPTIONS_HOST, ip.c_str());
		ssh_options_set(session, SSH_OPTIONS_USER, user.c_str());
		//ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &amp; verbosity);

		int rc = ssh_connect(session);
		if (rc != SSH_OK) {
			throw std::runtime_error("Failed to connect: " + std::string(ssh_get_error(session)));
		}

		rc = ssh_userauth_password(session, nullptr, pass.c_str());
		if (rc != SSH_AUTH_SUCCESS) {
			throw std::runtime_error("Failed to authenticate: " + std::string(ssh_get_error(session)));
		}

		channel = ssh_channel_new(session);
		if (!channel) {
			throw std::runtime_error("Failed to create channel.");
		}

		rc = ssh_channel_open_session(channel);
		if (rc != SSH_OK) {
			throw std::runtime_error("Failed to open channel.");
		}
	}

	std::string
	runCommand(const std::string & cmd)
	{
		if (!channel) {
			throw std::runtime_error("Channel not opened.");
		}

		int rc = ssh_channel_request_exec(channel, cmd.c_str());
		if (rc != SSH_OK) {
			throw std::runtime_error("Failed to execute command.");
		}

		std::string result;
		int			nbytes;
		while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
			result.append(buffer, nbytes);
		}

		return result;
	}

	void
	disconnect()
	{
		if (channel) {
			ssh_channel_send_eof(channel);
			ssh_channel_close(channel);
		}
	}
};

int
main()
{
	try {
		Client client;
		client.connect("username", "hostname", "password");

		std::string output = client.runCommand("ls -l");
		std::cout << "Command output:\n"
				  << output << std::endl;

		client.disconnect();
	} catch (const std::exception & ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
