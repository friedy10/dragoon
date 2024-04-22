#include "gdbmi.hpp"
#include "client.hpp"
#include <csignal>
#include <memory>

std::unique_ptr<Client> client;

Client::Client()
{
	session = ssh_new();
	if (!session) {
		throw std::runtime_error("Failed to create SSH session.");
	}
}

Client::~Client()
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
Client::connect(const std::string & user, const std::string & ip, const std::string & pass)
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
Client::run_command(const std::string & cmd)
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
Client::disconnect()
{
	if (channel) {
		ssh_channel_send_eof(channel);
		ssh_channel_close(channel);
	}
}

void
signal_handler(int signum)
{
	std::cout << "Detected SIGINT\n" << std::endl;
	if(client != nullptr){
		client->disconnect();	
	}
	exit(1);	
}


int
main(int argc, char** argv)
{

	client = std::make_unique<Client>();
	
	if(argc < 3){
		std::cout << "Invalid arguments" << std::endl;
		std::cout << "Usage: ./dragoon pi 192.168.1.1 PASSWORD" << std::endl;
	}

	/* catch signals */
	signal(SIGINT, signal_handler);
			
	try {
		client->connect(argv[1], argv[2], argv[3]);


		/* Get the proc that we want to loock at */
		std::string proc;
		std::cout << "Enter process name to debug" << std::endl;
		std::cin >> proc;
	
		std::string output = client->run_command("ps aux | grep " + proc);
		std::cout << output;

		int pid;
		std::cout << "Enter pid to attach to." << std::endl;	
		std::cin >> pid;	

		std::cout << "Attaching to process " << proc << " with PID: " 
			<< pid << std::endl;
		std::cin >> pid;	
		
		output = client->run_command("gdb --interpreter=mi attach " +
			std::to_string(pid) + " -ex cont");
					
		for(;;)
		{
			/* TODO: Implement command stuff */
			
			
		}
	
	
		output = client->run_command("ls -l");
		std::cout << "Command output:\n"
				  << output << std::endl;

		client->disconnect();
	} catch (const std::exception & ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
