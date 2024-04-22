#pragma once

#include <iostream>
#include <vector>
#include <stdlib.h>
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


class Network
{

private:
	std::vector<pcpp::Packet> packetvect;	
public:
	int create_dns_query(bool save, std::string fname, 
				std::string srcip, std::string destip);
};
