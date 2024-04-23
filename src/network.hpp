#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <thread>
#include <vector>
#include <stdlib.h>
#include <getopt.h>
#include <pcapplusplus/SystemUtils.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/EthLayer.h>
#include <pcapplusplus/VlanLayer.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/TcpLayer.h>
#include <pcapplusplus/HttpLayer.h>
#include <pcapplusplus/UdpLayer.h>
#include <pcapplusplus/DnsLayer.h>
#include <pcapplusplus/PcapFileDevice.h>
#include <pcapplusplus/PcapLiveDevice.h>
#include <pcapplusplus/PcapLiveDeviceList.h>


class Network
{

private:
	std::string mac_addr_src;
	std::string mac_addr_dest;
	std::vector<pcpp::Packet> packet_vect;
		
public:
	int create_dns_query(bool save, std::string fname, 
				std::string srcip, std::string destip);
	int create_http_packet(bool save, std::string fname,
				std::string srcip, std::string destip);

	int send_packets(std::string interface_ip);
	
	int flood_attack(std::string interfaceip, std::string srcip, std::string destip);
};

#endif
