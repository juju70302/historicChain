#include <sys/socket.h>
#include "blockchain.hpp"
#include "network.hpp"

using namespace std;

void testBlockchain();
void testSocket(int port);
void testThread();
void testThread1();
void testMsg();
void testTag();
void testReadWite();
void testGetIp();


int main(int argc,char* argv[]){
	//for(int i=0;i<argc;i++)cout<<i<<"th argv>>\""<<argv[i]<<"\""<<endl;
	if(argc>1)testSocket(atoi(argv[1]));else testSocket(3000);
	//testGetIp();
	//testBlockchain();
	//testReadWite();
	
	//testMsg();
	//testTag();
	
	return 0;
}
/*
string getLocalIP(){
    int inet_sock;  
    struct ifreq ifr;  
	char ip[32]={NULL};  

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);  
    strcpy(ifr.ifr_name, "eth0");  
    ioctl(inet_sock, SIOCGIFADDR, &ifr);  
    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));  
    return string(ip);
}
*/
void testGetIp(){
	//cout<<"local ip="<<getLocalIP()<<endl;
}

void testReadWite(){
	BlockChain bc1,bc2;
	BlockData b_d1("block1");
	BlockData b_d2("block2");
	BlockData b_d3("block3");
	BlockData b_d4("block4");
	
	Branch brch1;
	brch1.clear();
	if(bc1.addBlock(b_d1,brch1))cout<<"Successfully add b_d1..."<<endl;else cout<<"Failure on adding b_d1..."<<endl;
	if(bc2.addBlock(b_d2,brch1))cout<<"Successfully add b_d2..."<<endl; else cout<<"Failure on adding b_d2..."<<endl;
	brch1={0};
	if(bc1.addBlock(b_d3,brch1))cout<<"Successfully add b_d3..."<<endl;else cout<<"Failure on adding b_d3..."<<endl;
	
	if(bc1.writeToFile("data/chain1"))
		cout<<"write successful..."<<endl;
	else
		cout<<"write fail..."<<endl;
	if(bc2.readFromFile("data/chain1"))
		cout<<"read successful..."<<endl;
	else
		cout<<"read fail..."<<endl;
}

void testThread(){
	cout<<"testThread start..."<<endl;
	thread thread1(testThread1);
	thread1.join();
	cout<<"testThread finished..."<<endl;
}

void testThread1(){
	cout<<"testThread1 start..."<<endl;
	for(int i=0;i<(1<<30);i++);
	cout<<"testThread1 finished..."<<endl;
}

void testTag(){
	Tag tg;
	std::string str1="<tg1 length=0>< \\  tg1 >";
	std::string str2="     <  tg1 length = 0   >< \\ tg1  > < tg1  length = 1 >h< \\  tg1 >    ";
	if(tg=str1)cout<<"successful!!"<<endl;
	else cout<<"assign failure..."<<endl;
	Tag tg1("tag1","block1...");
	cout<<"tg1=\""<<tg1.toString()<<"\""<<endl;
	
	Tag tg2,tg3,tg4;
	cout<<"original string=\""<<str2<<"\""<<endl;
	cout<<"remain string=\""<<(tg2<<str2)<<"\""<<endl;
	tg4<<(tg3<<str2);
	
	cout<<"tg2=\""<<tg2.toString()<<"\""<<endl;
	cout<<"tg3=\""<<tg3.toString()<<"\""<<endl;
	cout<<"tg4=\""<<tg4.toString()<<"\""<<endl;
}

void testMsg(){
	NetworkNode nNode;
	Message msg,msg1;
	//msg.set(messageType::addBlock,"block1...block1...block1...block1...block1...\n");
	vector<uint32_t> branch={};
	BlockData bd1("block1");Block b1(0,bd1,(nNode.getNode(branch))->block->hash());
	b1.mineBlock(2);
	msg.setAddBlock(branch,b1);
	cout<<"**********Message**********"<<endl<<msg.toString()<<endl<<"********************"<<endl;
	if((msg1=msg.toString()))cout<<"assign successful!!"<<endl;
	else	cout<<"assign fail..."<<endl;
	cout<<"**********Message1**********"<<endl<<msg1.toString()<<endl<<"********************"<<endl;
	
	std::string strOut="";
	if((nNode.retrieveMessage(msg1.toString(),strOut))!=0)cout<<"retrieve success..."<<endl;
	else cout<<"retrieve fail..."<<endl;
	cout<<"return message=\""<<strOut<<"\""<<endl;
	msg.type=messageType::askChain;
	if((nNode.retrieveMessage(msg.toString(),strOut))!=0)cout<<"retrieve success..."<<endl;
	else cout<<"retrieve fail..."<<endl;
	cout<<"return message=\""<<strOut<<"\""<<endl;
	
		
}

void testSocket(int portIn){
	NetworkNode* nNode=NULL;
	
	
	unsigned short port;std::string ip;
	//Start an new chain...
	//Input IP...
	ip="";
	std::cout<<"Your IP"<<std::endl<<">>";
	std::cin>>ip;fflush(stdin);
	
	while(true){//Input port...
		std::string strPort="";
		std::cout<<"Using port"<<std::endl<<">>";
		std::cin>>strPort;fflush(stdin);
		if(isNum(strPort)){
			port=atoi(strPort.c_str());
			break;
		}
	}
//std::cout<<"ip=\""<<ip<<"\tport=\""<<port<<"\""<<std::endl;
	nNode=new NetworkNode(ip,port);
	
	
	nNode->run();
	delete nNode;
/*
	NetworkNode* nNode=NULL;unsigned short port;std::string ip;

	//Start an new chain...
	//Input IP...
	ip="";
	std::cout<<"Your IP"<<std::endl<<">>";
	std::cin>>ip;fflush(stdin);
	
	while(true){//Input port...
		std::string strPort="";
		std::cout<<"Using port"<<std::endl<<">>";
		std::cin>>strPort;fflush(stdin);
		if(isNum(strPort)){
			port=atoi(strPort.c_str());
			break;
		}
	}
//std::cout<<"ip=\""<<ip<<"\tport=\""<<port<<"\""<<std::endl;
	for(uint32_t terminal=0;terminal==0;){//Import port
		uint32_t num;
		std::cout<<"where does your chain come from?(Internet:0/File:1/New chain:2)"<<std::endl<<">>";
		std::cin>>num;fflush(stdin);
		switch(num){
			case 0:{
				nNode=new NetworkNode(ip,portIn);
				std::cout<<"another node ip"<<std::endl<<">>";
				std::cin>>ip;fflush(stdin);
				std::cout<<"another node port"<<std::endl<<">>";
				std::cin>>port;fflush(stdin);
				Message askMsg;askMsg.set(messageType::askChain,"");
				
				std::string strBlockChain="";
				do{
					nNode->sendString(ip,port,askMsg.toString(),strBlockChain);
				}while(((*nNode)=strBlockChain)==0);
				
				terminal=1;
				break;
			}
			case 1:{
				std::string path="",strBlockChain;
				std::cout<<"file path>>"<<std::endl;
				std::cin>>path;fflush(stdin);
				//std::cout<<"opening \""<<path<<"\""<<std::endl;
				std::ifstream inFile(path);char ch;
				if(!inFile){
					std::cout<<"Error:Cannot openning file:\""<<path<<"\".please redo or check..."<<std::endl;
					break;
				}
				strBlockChain="";
				while(inFile.get(ch))strBlockChain+=ch;inFile.close();
				
				nNode=new NetworkNode(ip,portIn);
				if((*nNode=strBlockChain)==0){
					delete nNode;
					break;
				}
				
				terminal=1;
				break;
			}
			case 2:{
				nNode=new NetworkNode(ip,portIn);
				terminal=1;
				break;
			}
			default:
				break;
		}
	}

	nNode->run();
	delete nNode;
*/
}

void testBlockchain(){
	BlockChain c(2);
	BlockData b_d1("block1");
	BlockData b_d2("block2");
	BlockData b_d3("block3");
	BlockData b_d4("block4");
	
	Branch brch1;
	brch1.clear();
	if(c.addBlock(b_d1,brch1))cout<<"Successfully add b_d1..."<<endl;else cout<<"Failure on adding b_d1..."<<endl;
	if(c.addBlock(b_d2,brch1))cout<<"Successfully add b_d2..."<<endl; else cout<<"Failure on adding b_d2..."<<endl;
	brch1={0};
	if(c.addBlock(b_d3,brch1))cout<<"Successfully add b_d3..."<<endl;else cout<<"Failure on adding b_d3..."<<endl;
	//c.addBlock(b_d3);
	if(c.findData("block3"))
		cout<<"Found block3"<<endl;
	else
		cout<<"No block3..."<<endl;
	
	//c.addBlockAfter("block5",b_d4);
	
	if(c.checkChain())
		cout<<"Valid chain"<<endl;
	else
		cout<<"Bad chain..."<<endl;
	if(c.findData("block4"))
		cout<<"Found block4"<<endl;
	else
		cout<<"No block4..."<<endl;
	
	Block b1(0,b_d1,sha256("block0"));
	b1.toString();
	//cout<<"block1=\""<<b1.toString()<<"\""<<endl;
	Block b2(0,"block2");
	//if(b2=b1.toString()) cout<<"assign successful..."<<std::endl;
	//else cout<<"failure on assignning..."<<endl;
	//cout<<"block2=\""<<b2.toString()<<"\""<<endl;
	
	BlockChain bc1(3);
	cout<<endl;
	cout<<c.toString()<<endl<<endl;
	if(bc1=c.toString())
		cout<<"assign successful..."<<std::endl;
	else
		cout<<"failure on assignning..."<<endl;
}