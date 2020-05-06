#include "blockchain.hpp"
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <thread>
#include <atomic>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <net/if.h>
#include <chrono>
#include <vector>
#include <mutex>

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define IPV4
#define IP_SIZE 16
#define MESSAGE_LENGTH (1<<20)

namespace messageType{
	static const uint32_t min=0,max=3;
	static const uint32_t unknown=0;
	static const uint32_t addBlock=1;
	static const uint32_t askChain=2;
	static const uint32_t replyMsg=3;
};

class Message{
	public:
		uint32_t type;
		std::string content;
	
	public:
		int operator=(const std::string&);
		Message& operator=(const Message&);
		std::string toString();
		void set(uint32_t,const std::string&);
		void setAddBlock(const Branch&,const Block&);
		int getAddBlock(Branch&,Block&)const;
	
		Message();
		~Message();
};

class NetworkNode:public BlockChain{
	private:
		struct nodeAddress{
			std::string ip;unsigned short port;
		};
		std::vector<struct nodeAddress>addressList;
	private:
		static const uint32_t MAX_SERVER_REQUEST=10;
		std::thread serverThread;
		mutable std::atomic<uint32_t> isFinished;
		mutable std::mutex chainLock;
		unsigned short portNum;
		std::string nodeIP;
		time_t updateTime;
		Branch currentCursor;
		
		void serverRoutine();
		void clientRoutine();
		
		int retrieveAddBlock(const Message& msg,std::string& strOut);
		int retrieveAskChain(const Message& msg,std::string& strOut);
		int retrieveReplyMsg(const Message& msg,std::string& strOut);
	public:
		int retrieveMessage(const std::string& strIn,std::string& strOut);
		int operator=(const std::string&);
	public:
		int sendString(std::string ip,unsigned short port_num,const std::string& msg,std::string& strReceive);
		std::string receiveString(unsigned short port_num);
		void receiveStringForMillisecond(unsigned short port_num,uint32_t millisecond);
		
		void addAddress(const std::string& ipIn,unsigned short portIn);
		
		uint32_t nextBlock(uint32_t nextBranch);
		Block& currBlock();
		void run();
		void finish();
		
		static int get_local_ip(const char* eth_inf,char* ip);
		
		NetworkNode(const std::string& ipIn="127.0.0.1",unsigned short portNumIn=3000,uint32_t mineDifficultyIn=2);
		~NetworkNode();
};
#endif