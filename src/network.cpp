#include "network.hpp"

int
Network::create_dns_query(bool save, std::string fname, 
				std::string srcip, std::string destip)
{


	// create a new Ethernet layer
	pcpp::EthLayer newEthernetLayer(pcpp::MacAddress("00:50:43:11:22:33"), pcpp::MacAddress("aa:bb:cc:dd:ee:ff"));

	// create a new IPv4 layer
	pcpp::IPv4Layer newIPLayer(pcpp::IPv4Address(srcip.c_str()), pcpp::IPv4Address(destip.c_str()));
	newIPLayer.getIPv4Header()->ipId = pcpp::hostToNet16(2000);
	newIPLayer.getIPv4Header()->timeToLive = 64;

	// create a new UDP layer
	pcpp::UdpLayer newUdpLayer(12345, 53);

	// create a new DNS layer
	pcpp::DnsLayer newDnsLayer;
	newDnsLayer.addQuery("www.ebay.com", pcpp::DNS_TYPE_A, pcpp::DNS_CLASS_IN);

	// create a packet with initial capacity of 100 bytes (will grow automatically if needed)
	pcpp::Packet newPacket(100);

	// add all the layers we created
	newPacket.addLayer(&newEthernetLayer);
	newPacket.addLayer(&newIPLayer);
	newPacket.addLayer(&newUdpLayer);
	newPacket.addLayer(&newDnsLayer);

	// compute all calculated fields
	newPacket.computeCalculateFields();

	packetvect.push_back(newPacket);

	if(save){
		// write the new packet to a pcap file
		pcpp::PcapFileWriterDevice writer2(fname);
		writer2.open();
		writer2.writePacket(*(newPacket.getRawPacket()));
		writer2.close();
	}
}
