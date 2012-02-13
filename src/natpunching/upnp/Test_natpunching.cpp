#include <iostream>
#include <sstream>
#include <map>
#include "command.h"
#include "openconnect.h"
#include "../../core/socket.h"
#include "../../core/address.h"
#include "igd.h"
#include "../../core/exception.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#define SOCKET_ERROR (-1)


int main(int argc, char* argv[]){
    std::string endl ="\r\n";
    std::string address, service, path;
    unsigned short port;
    
    std::cout << "Enter Address to connect with : " ;
    std::cin >> address;
    std::cout << "Enter Port : ";
    std::cin >> port;
    std::cout << "Enter Path to rootDesc.xml : " ;
    std::cin >> path;
    
    std::string blah;
    
    std::cout << "The command is being created ..." << std::endl;
    Epyx::UPNP::IGD igd;
    igd.setAddress(address, port);
    igd.setRootDescPath(path);
    igd.getServices();
    std::map<std::string,std::string> services = igd.getServiceList();
    for (std::map<std::string,std::string>::iterator it=services.begin(); it != services.end(); ++it){
        std::cout << it->first << " : " << it->second << std::endl;
    }
    std::cout << "IGD Configured. Adding port map" << std::endl;
    try{
        Epyx::Address addr = igd.addPortMap(22,Epyx::UPNP::TCP,1337);
        std::cout << "Port map added" << std::endl;
        std::cout << "Does it work? ";
        std::cin >> blah;
        std::cout << "Deleting Port Map" << std::endl;
        igd.delPortMap(addr,Epyx::UPNP::TCP);
        std::cout << "Port Map deleted" << std::endl;
    }
    catch(Epyx::FailException e){
        std::cerr << e << std::endl;
    }
    
    return 0;
    
}
