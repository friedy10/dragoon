#include "network.hpp"

int
Network::create_dns_query(bool save, std::string fname, 
				std::string srcip, std::string destip)
{
	/* create ethernet layer */
	pcpp::EthLayer eth_layer(pcpp::MacAddress("00:50:43:11:22:33"), 
				pcpp::MacAddress("aa:bb:cc:dd:ee:ff"));

	/* create ipv4 layer */
	pcpp::IPv4Layer ip_layer(pcpp::IPv4Address(srcip.c_str()), pcpp::IPv4Address(destip.c_str()));
	ip_layer.getIPv4Header()->ipId = pcpp::hostToNet16(2000);
	ip_layer.getIPv4Header()->timeToLive = 64;

	/* create udp layer */
	pcpp::UdpLayer udp_layer(12345, 53);

	/* create dns layer */
	pcpp::DnsLayer dns_layer;
	dns_layer.addQuery("www.ebay.com", pcpp::DNS_TYPE_A, pcpp::DNS_CLASS_IN);

	/* packet size 100 bytes */
	pcpp::Packet new_packet(100);

	/* add layers to packet */
	new_packet.addLayer(&eth_layer);
	new_packet.addLayer(&ip_layer);
	new_packet.addLayer(&udp_layer);
	new_packet.addLayer(&dns_layer);

	/* calculate computed fields */
	new_packet.computeCalculateFields();

	this->packet_vect.push_back(new_packet);

	if(save){
		// write the new packet to a pcap file
		pcpp::PcapFileWriterDevice writer2(fname);
		writer2.open();
		writer2.writePacket(*(new_packet.getRawPacket()));
		writer2.close();
	}

	return 0;
}

int
Network::create_http_packet(bool save, std::string fname,
				std::string srcip, std::string destip)
{
	pcpp::EthLayer eth_layer(pcpp::MacAddress("00:50:43:11:22:33"), 
				pcpp::MacAddress("aa:bb:cc:dd:ee:ff"));
	
	pcpp::IPv4Layer ip_layer(pcpp::IPv4Address(srcip.c_str()), pcpp::IPv4Address(destip.c_str()));
	ip_layer.getIPv4Header()->ipId = pcpp::hostToNet16(2000);
	ip_layer.getIPv4Header()->timeToLive = 64;

    	pcpp::TcpLayer tcp_layer(12345, 80);
    	tcp_layer.getTcpHeader()->ackFlag = 1; // Set ACK flag
    	tcp_layer.getTcpHeader()->windowSize = 8192; // Set window size

    	pcpp::HttpRequestLayer http_layer(pcpp::HttpRequestLayer::HttpMethod::HttpGET,
					"http://127.0.0.1:80",pcpp::HttpVersion::OneDotOne);

	/* Inital size will be 100 (it grows as needed) */
	pcpp::Packet new_packet(100);	
	new_packet.addLayer(&eth_layer);
	new_packet.addLayer(&ip_layer);
	new_packet.addLayer(&tcp_layer);
	new_packet.addLayer(&http_layer);
	
	/* calculate computed fields */
	new_packet.computeCalculateFields();
	
	this->packet_vect.push_back(new_packet);

	if(save){
		// write the new packet to a pcap file
		pcpp::PcapFileWriterDevice writer2(fname);
		writer2.open();
		writer2.writePacket(*(new_packet.getRawPacket()));
		writer2.close();
	}

	return 0;
}


int
Network::flood_attack(std::string interfaceip, std::string srcip, std::string destip)
{
	uint32_t n = std::thread::hardware_concurrency();
	std::cout << "Running flood with " << n << " threads" << std::endl;


	std::vector<std::thread> threads(n);

	for(auto &t : threads){
		t = std::thread([&]{
			this->send_packets(interfaceip);
		});
		t.detach();
	}
	
	std::cout << "Started " << n << " threads" << std::endl;
}

int
Network::send_packets(std::string interface_ip)
{
	
	std::unique_ptr<pcpp::PcapLiveDevice> dev = 
	std::unique_ptr<pcpp::PcapLiveDevice>(pcpp::PcapLiveDeviceList::getInstance().
						getPcapLiveDeviceByIp(interface_ip));

	if (dev == nullptr)
	{
		std::cerr << "Cannot find interface with IPv4 address of '" 
			<< interface_ip << "'" << std::endl;
		return -1;
	}

	std::cout << "Interface info:" << std::endl 
		<< "   Interface name:        " 
		<< dev->getName() << std::endl 
		<< "   Interface description: " 
		<< dev->getDesc() << std::endl 
		<< "   MAC address:           "
		<< dev->getMacAddress() << std::endl 
		<< "   Default gateway:       " 
		<< dev->getDefaultGateway() << std::endl 
		<< "   Interface MTU:         " 
		<< dev->getMtu() << std::endl; 


	// open the device before start capturing/sending packets
	if (!dev->open())
	{
		std::cerr << "Cannot open device" << std::endl;
		return 1;
	}

	std::cout << std::endl << "Sending " << this->packet_vect.size() << 
			" packets one by one..." << std::endl;

	// go over the vector of packets and send them one by one
	for (auto pkt : this->packet_vect)
	{
		// send the packet. If fails exit the application
		if (!dev->sendPacket(&pkt))
		{
			std::cerr << "Couldn't send packet" << std::endl;
			return -1;
		}
	}
	std::cout << this->packet_vect.size() << " packets sent" << std::endl;
}

int
print_usage()
{
	std::cout << "Usage: dragoon_network <option(s)> SOURCES\n\n"
		<< "Options:\n"
		<< "  -h, --help                   Show this help message\n"
          	<< "  -m, --source-mac <MAC>       Specify the source MAC address\n"
          	<< "  -d, --destination-mac <MAC>  Specify the destination MAC address\n"
          	<< "  -s, --source-ip <IP>         Specify the source IP address\n"
          	<< "  -i, --destination-ip <IP>    Specify the destination IP address\n"
          	<< "  -p, --interface-ip [IP]      Specify the interface IP address (optional)\n"
          	<< std::endl;
}


int
main(int argc, char** argv)
{
	std::cout << "Dragoon Network 0.0.1" << std::endl;
	
    	int c;
    	int digit_optind = 0;


	std::string src_mac, dest_mac, src_ip, dest_ip, inter_ip;

   	for(;;) {
        	int this_option_optind = optind ? optind : 1;
        	int option_index = 0;
        	static struct option long_options[] = {
			{"source-mac",       required_argument, 0,  'm'},
			{"destination-mac",  required_argument, 0,  'b'},
			{"source-ip",  	     required_argument, 0,  'c'},
			{"destination-ip",   required_argument, 0,  'd'},
        	    	{"interface-ip",     optional_argument, 0,  'i'},
        	    	{0,                  0,                 0,    0}
        	};

       		c = getopt_long(argc, argv, "m:d:s:i:p:", long_options, &option_index);
        	if (c == -1){
			print_usage();
            		break;
		}
		
       		switch (c) {
       			case 'm':
				src_mac = optarg;
       			     	break;

       			case 'b':
				dest_mac = optarg;
       			     	break;

       			case 'c':
				src_ip = optarg;
       			     	break;

       			case 'd':
				dest_ip = optarg;
       			     	break;
       			case 'i':
				inter_ip = optarg;
       			     	break;

       			case '?':
       			     	break;

       			default:
				print_usage();
       			     	printf("?? getopt returned character code 0%o ??\n", c);
		}

	}

	return 0;
}
