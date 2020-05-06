#include "network.hpp"

int NetworkNode::retrieveAskChain(const Message& msg,std::string& strOut){
	chainLock.lock();
	strOut=toString();
	std::cout<<"chain=\""<<strOut<<"\"\n";
	return 1;
}

int NetworkNode::retrieveReplyMsg(const Message& msg,std::string& strOut){
	if(msg.type!=messageType::replyMsg)return 0;
	strOut="";
	return 1;
}

int NetworkNode::retrieveAddBlock(const Message& msg,std::string& strOut){
	Branch brch;
	Block b(0);
	
	if((msg.type!=messageType::addBlock)||(msg.getAddBlock(brch,b))==0)return 0;
	
	chainLock.lock();
	if((addBlockWithCheckAfter(getNode(brch),b))==0){
		chainLock.unlock();
		strOut="0";
		return 0;
	}
	updateTime=time(nullptr);
	chainLock.unlock();
	strOut="1";
//std::cout<<"add block=\""<<b.toString()<<"\""<<std::endl;
	
	return 1;
}

int Message::getAddBlock(Branch& branch,Block& blockOut)const{
	Tag tgBranchNum,tgBranch,tgBlock;
	uint32_t branchNum;
	std::stringstream ssBranch;
	
	tgBlock<<(tgBranch<<(tgBranchNum<<content));
	if(!isNum(tgBranchNum.content.c_str())||((tgBranch.name.length())==0)||((tgBlock.name.length())==0))
		return 0;
	
	ssBranch.str("");ssBranch.clear();
	branch.clear();
	branchNum=atoi(tgBranchNum.content.c_str());
	ssBranch<<tgBranch.content;
	for(uint32_t i=0;i<branchNum;i++){
		uint32_t b;
		ssBranch>>b;
		branch.push_back(b);
	}
//std::cout<<"branch>>";for(int i=0;i<branch.size();i++)std::cout<<" "<<branch.at(i);std::cout<<std::endl<<"tgBlock.content=\""<<tgBlock.content<<"\""<<std::endl;

	blockOut=tgBlock.content;
	
	return 1;
}

/*
std::string NetworkNode::toString(const Block& block)const{
	std::string strOut="",strData,strTime;
	std::stringstream ss;ss.str("");ss.clear();
	
	//ss<<block.time
	
	//strData="<BlockData length="+block.getData().toString().length()+">"+block.data.toString()+"<\\BlockData>";
	//strData=std::to_string((block.getData()).toString().length());
	strTime="<time length=";
}
*/

void Message::setAddBlock(const Branch& branch,const Block& block){
	std::stringstream ss;ss.str("");ss.clear();
	
	for(int i=0;i<branch.size();i++){
		ss<<branch.at(i);
		if(i!=(branch.size()-1))ss<<" ";
	}
	Tag tgBranchNum("branchNum",std::to_string(branch.size()));
	Tag tgBranch("branch",ss.str());
	Tag tgBlock("block",block.toString());
	
	type=messageType::addBlock;
	content=tgBranchNum.toString()+tgBranch.toString()+tgBlock.toString();
//std::cout<<"content>>\""<<content<<"\""<<std::endl;
}

int NetworkNode::retrieveMessage(const std::string& strIn,std::string& strOut){
	Message msg;
	if(!(msg=strIn))return 0;
	switch(msg.type){
		case messageType::addBlock:
			return retrieveAddBlock(msg,strOut);
		case messageType::askChain:
			return retrieveAskChain(msg,strOut);
		case messageType::replyMsg:
			return retrieveReplyMsg(msg,strOut);
		default:
			return 0;
	}
	return 0;
}

void Message::set(uint32_t typeIn,const std::string& contentIn){
	type=typeIn;
	content=contentIn;
}

void NetworkNode::clientRoutine(){
	std::string strSend,strReceive;char buf;unsigned short port;
	int32_t move1,move2;
	
	for(int terminal=0;terminal==0;){
		std::cout<<"what do you want to do?(leave:0/add block:1/browse block:2)>>"<<std::endl;
		std::cin>>move1;fflush(stdin);
		switch(move1){
			case 0:
				terminal=1;
				break;
			case 1:{
				std::string strBlock;
				
				std::cout<<"how do you input content of block?(file:0/cmd:1)>>"<<std::endl;
				move2=0;std::cin>>move2;fflush(stdin);
				if(move2==0){//Input from file
					std::string path="";
					std::cout<<"file path>>"<<std::endl;
					std::cin>>path;fflush(stdin);
					//std::cout<<"opening \""<<path<<"\""<<std::endl;
					std::ifstream inFile(path);char ch;
					if(!inFile)
						std::cout<<"Error:Cannot openning file:\""<<path<<"\".please redo or check..."<<std::endl;
					strBlock="";
					while(inFile.get(ch))strBlock+=ch;inFile.close();
				}
				else{//Input from command line
					std::string contentIn="";uint32_t contentLine=0;
					std::cout<<"number of input line>>"<<std::endl;
					std::cin>>contentLine;fflush(stdin);
					//std::cout<<"content length="<<contentLength<<std::endl;
					char ch;strBlock="";getchar();
					for(uint32_t i=0;i<contentLine;i++){
						while(true){
							ch=getchar();fflush(stdin);
							if(ch=='\n'){
								if(i<(contentLine-1))strBlock+=ch;
								break;
							}
							strBlock+=ch;
						}
					}
				}
std::cout<<"get string=\""<<strBlock<<"\""<<std::endl;
				
				BlockData bd(strBlock);
				Branch oriBranch=currentCursor;
				uint32_t brch=getNode(currentCursor)->next.size();
				addBlock(bd,currentCursor);
				currentCursor.push_back(brch);
				currBlock().mineBlock(BlockChain::mineDifficulty);
				
				Message msgSend;msgSend.setAddBlock(oriBranch,currBlock());
				Message msgReceive;
				for(int i=0;i<addressList.size();i++){
					std::string strOut="";
					
					sendString(addressList.at(i).ip,addressList.at(i).port,msgSend.toString(),strOut);
					msgReceive=strOut;
					std::cout<<"add result="<<msgReceive.content<<std::endl;
				}
				
				//Not done yet...
				
				break;
			}
			case 2:{
				uint32_t move;
				std::cout<<"Which block?(first:0/current:1/next:2/previous:3)"<<std::endl<<">>";
				std::cin>>move;fflush(stdin);
				switch(move){
					case 0:{
						currentCursor.clear();
						std::cout<<"return first block>>"<<std::endl;
						std::cout<<currBlock().getData().getContent()<<std::endl;
						break;
					}
					case 1:{
						std::cout<<currBlock().getData().getContent()<<std::endl;
						break;
					}
					case 2:{
						uint32_t branchNum;
						if((getNode(currentCursor)->next.size())==0){
							std::cout<<"No next branch..."<<std::endl;
							break;
						}
						std::cout<<"Enter branch num?(0-"<<(getNode(currentCursor)->next.size())-1<<")";
						std::cout<<std::endl<<">>";
						std::cin>>branchNum;fflush(stdin);
						
						if(branchNum<0||branchNum>=(getNode(currentCursor)->next.size()))break;
						currentCursor.push_back(branchNum);
						
						std::cout<<"go next"<<branchNum<<"...\tcontent>>"<<std::endl;
						std::cout<<currBlock().getData().getContent()<<std::endl;
						
						break;
					}
					case 3:{
						currentCursor.pop_back();
						
						std::cout<<"go previous...\tcontent>>"<<std::endl;
						std::cout<<currBlock().getData().getContent()<<std::endl;
						break;
					}
					default:
						break;
				}
				break;
			}
			default:{
				//std::cout<<"Invalid input.Please iput again..."<<std::endl
				std::cout<<"input string to send>>";
				strSend="";
				if((buf=getchar())!='\n')strSend+=buf;
				while((buf=getchar())!='\n'){
					strSend+=buf;
				}
				
				std::cout<<"sending port>>";
				std::cin>>port;fflush(stdin);
				//std::cout<<"start send>>\""<<strSend<<"\""<<std::endl;
				
				sendString("127.0.0.1",port,strSend,strReceive);
				
				break;
			}
		}
/*
		std::cout<<"input string to send or input 'X' to terminal>>";
		strSend="";
		if((buf=getchar())!='\n')strSend+=buf;
		while((buf=getchar())!='\n'){
			strSend+=buf;
		}
		if(strSend[0]=='X')break;
		
		std::cout<<"sending port>>";
		std::cin>>port;fflush(stdin);
		std::cout<<"start send>>\""<<strSend<<"\""<<std::endl;
		
		sendString("127.0.0.1",port,strSend,strReceive);
*/
	}
}

std::string Message::toString(){
	Tag tgType("type",std::to_string(type)),tgContent("content",content);
	
	std::string strOut=(tgType.toString()+tgContent.toString());
	
	return strOut;
}

int Message::operator=(const std::string& strIn){
	Tag tgType,tgContent;
	uint32_t typeIn;std::string contentIn;
	
	tgContent<<(tgType<<strIn);
	if(!isNum(tgType.content)||!tgType.name.length()||!tgContent.name.length())return 0;
	typeIn=atoi(tgType.content.c_str());
	if(typeIn<messageType::min||typeIn>messageType::max)return 0;
	type=typeIn;
	content=tgContent.content;
	return 1;
}
/*
std::string Message::toString(){
	std::string strOut="";
	strOut+="type = ";
	strOut+=std::to_string(type);
	strOut+="\ncontent_length = ";
	strOut+=std::to_string(content.size());
	strOut+="\ncontent>>\n";
	strOut+=content;
	//strOut+="\n";
	return strOut;
}

int Message::operator=(const std::string& strIn){
	uint32_t srch;uint32_t content_num=0;
	std::string tmp_str="",tmp_str1;
	
	uint32_t typeIn;
	uint32_t contentNumIn;
	std::string contentIn="";
	std::stringstream ss;
	ss.str(strIn);
	
	std::getline(ss,tmp_str,'\n');
//std::cout<<"getline>>\""<<tmp_str<<"\""<<std::endl;
	tmp_str1="";
	for(srch=0;srch<tmp_str.length()&&tmp_str[srch]!='=';srch++){
		if(tmp_str[srch]==' ')continue;
		tmp_str1+=tmp_str[srch];
	}
//std::cout<<"getline>>\""<<tmp_str1<<"\""<<std::endl;
	if(tmp_str1!="type"||srch>=tmp_str.length()||tmp_str[srch]!='=')return 0;
	tmp_str1="";
	for(srch++;srch<tmp_str.length();srch++)tmp_str1+=tmp_str[srch];
//std::cout<<"getline>>\""<<tmp_str1<<"\""<<std::endl;
	typeIn=(uint32_t)atoi(tmp_str1.c_str());
//std::cout<<"typeIn>>"<<typeIn<<std::endl;
		
	std::getline(ss,tmp_str,'\n');
//std::cout<<"getline>>\""<<tmp_str<<"\""<<std::endl;
	tmp_str1="";
	for(srch=0;srch<tmp_str.length()&&tmp_str[srch]!='=';srch++){
		if(tmp_str[srch]==' '||tmp_str[srch]=='_')continue;
		tmp_str1+=tmp_str[srch];
	}
//std::cout<<"getline>>\""<<tmp_str1<<"\""<<std::endl;
	if(tmp_str1!="contentlength"||srch>=tmp_str.length()||tmp_str[srch]!='=')return 0;
	tmp_str1="";
	for(srch++;srch<tmp_str.length();srch++)tmp_str1+=tmp_str[srch];
//std::cout<<"getline>>\""<<tmp_str1<<"\""<<std::endl;
	contentNumIn=(uint32_t)atoi(tmp_str1.c_str());
//std::cout<<"contentNumIn>>"<<contentNumIn<<std::endl;
	
	std::getline(ss,tmp_str,'\n');
//std::cout<<"getline>>\""<<tmp_str<<"\""<<std::endl;
	tmp_str1="";
	for(srch=0;srch<tmp_str.length()&&tmp_str[srch]!='>'&&tmp_str[srch]!='=';srch++)tmp_str1+=tmp_str[srch];
//std::cout<<"getline>>\""<<tmp_str1<<"\""<<std::endl;
	if(tmp_str1!="content")return 0;
	
	for(srch=0;srch<strIn.length()&&strIn[srch]!='\n';srch++);
	for(srch++;srch<strIn.length()&&strIn[srch]!='\n';srch++);
	for(srch++;srch<strIn.length()&&strIn[srch]!='\n';srch++);
//std::cout<<"srch>>"<<srch<<"\tcontentNumIn>>"<<contentNumIn<<std::endl;
//std::cout<<"strIn.length()>>"<<strIn.length()<<std::endl;
	contentIn="";
	if(srch+contentNumIn>strIn.length())return 0;
	for(srch++;srch<strIn.length();srch++)contentIn+=strIn[srch];
//std::cout<<"contentIn>>\""<<contentIn<<"\""<<std::endl;
	
	type=typeIn;
	content=contentIn;
	
	return 1;
}
*/

void NetworkNode::serverRoutine(){
	//std::cout<<"serverRoutine start..."<<std::endl;
	int sockfd=-1,forClientSockfd=0,maxfd;
	char inputBuffer[MESSAGE_LENGTH];
	std::string recvMsg;
	fd_set read_fds,all_fd;
	struct sockaddr_in serverInfo,clientInfo;
	socklen_t addrlen=sizeof(clientInfo);
	//timeval timeout={millisecond/1000,1000*(millisecond%1000)};
	timeval timeout={0,1000};
	
	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();
	
	FD_ZERO(&read_fds);	FD_ZERO(&all_fd);
	
	//Create socket...
#ifdef IPV4
	while((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
#else
	while((sockfd=socket(AF_INET6,SOCK_STREAM,0))==-1)
#endif
	{
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart socket..."<<std::endl;
#endif
	}
	
	//To avoid erro "Reuse same address" after server shut down...
	int setSocketOpt=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&setSocketOpt,sizeof(int));
	
	//Set server(current) address...
	memset(&serverInfo,0,sizeof(serverInfo));
#ifdef IPV4
	serverInfo.sin_family=AF_INET;
#else
	serverInfo.sin_family=AF_INET6;
#endif
	serverInfo.sin_addr.s_addr=INADDR_ANY;//recieve from every ip
	serverInfo.sin_port=htons(portNum);
	
	//Bind...
	while(bind(sockfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo))==-1){
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart bind..."<<std::endl;
#endif
	}
	
	//Listen to connect request...
	while((listen(sockfd,MAX_SERVER_REQUEST))==-1){
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart listening..."<<std::endl;
#endif
	}
	
	maxfd=sockfd;
	
	int client[MAX_SERVER_REQUEST];
	sockaddr_in client_info[MAX_SERVER_REQUEST];
	for(int i=0;i<MAX_SERVER_REQUEST;i++)client[i]=-1;
	
	FD_SET(sockfd,&all_fd);
	
	int read_num,maxIndex,connfd;
	std::string strReceive,strReply;
	stop = std::chrono::high_resolution_clock::now();
	do{
		read_fds=all_fd;

		timeout.tv_sec=0;timeout.tv_usec=1000;
		while((read_num=select(maxfd+1,&read_fds,NULL,NULL,&timeout))==-1){
#ifdef _WARNING_MESSAGE_
			std::cout<<"Restart select..."<<std::endl;
#endif
		}
		
		if(FD_ISSET(sockfd,&read_fds)){
			maxIndex=0;
			if((connfd=accept(sockfd,(sockaddr*)&clientInfo,&addrlen))!=-1){
				FD_SET(connfd,&all_fd);
				int i;
				for(i=0;i<MAX_SERVER_REQUEST;i++){
					if(client[i]==-1){
						client[i]=connfd;
						client_info[i]=clientInfo;
						break;
					}
				}
						
				if(connfd>maxfd)maxfd=connfd;
				if(i>maxIndex)maxIndex=i;
				
				read_num--;
				if(read_num<=0)continue;
			}
			else{
				continue;
			}
		}
		for(int i=0;i<=maxIndex;i++){
			if(client[i]==-1)continue;
			if(FD_ISSET(client[i],&read_fds)){
				int readCount=read(client[i],inputBuffer,sizeof(inputBuffer));
				if(readCount==0){
					close(client[i]);
					FD_CLR(client[i],&all_fd);
					client[i]=-1;
				}
				else if(readCount!=-1){//Success to receive...
					strReceive=inputBuffer;
					std::cout<<"receive message\""<<strReceive<<"\""<<std::endl;
					if((retrieveMessage(strReceive,strReply))!=0){
						if((strReply.length())>=0){
							Tag tgType("type",std::to_string(messageType::replyMsg));
							Tag tgContent("content",strReply);
							strReply=tgType.toString()+tgContent.toString();
							write(client[i],strReply.c_str(),strReply.length());
						}
					}
					
					write(client[i],"message received...",19);
				}
					
				read_num--;
				if(read_num<=0)break;
			}
		}
		stop = std::chrono::high_resolution_clock::now();
		//if((std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count())%1000==0)std::cout<<"passing time>>"<<std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count()<<"ms"<<std::endl;
	}while(!isFinished);

	close(sockfd);
	//std::cout<<"serverRoutine finished..."<<std::endl;
}

void NetworkNode::receiveStringForMillisecond(unsigned short port_num,uint32_t millisecond){
	int sockfd=-1,forClientSockfd=0,maxfd;
	char inputBuffer[MESSAGE_LENGTH];
	std::string recvMsg;
	fd_set read_fds,all_fd;
	struct sockaddr_in serverInfo,clientInfo;
	socklen_t addrlen=sizeof(clientInfo);
	//timeval timeout={millisecond/1000,1000*(millisecond%1000)};
	timeval timeout={0,1000};
	
	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();
	
	FD_ZERO(&read_fds);	FD_ZERO(&all_fd);
	
	//Create socket...
#ifdef IPV4
	while((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
#else
	while((sockfd=socket(AF_INET6,SOCK_STREAM,0))==-1)
#endif
	{
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart socket..."<<std::endl;
#endif
	}
	
	//To avoid erro "Reuse same address" after server shut down...
	int setSocketOpt=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&setSocketOpt,sizeof(int));
	
	//Set server(current) address...
	memset(&serverInfo,0,sizeof(serverInfo));
#ifdef IPV4
	serverInfo.sin_family=AF_INET;
#else
	serverInfo.sin_family=AF_INET6;
#endif
	serverInfo.sin_addr.s_addr=INADDR_ANY;//recieve from every ip
	serverInfo.sin_port=htons(port_num);
	
	//Bind...
	while(bind(sockfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo))==-1){
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart bind..."<<std::endl;
#endif
	}
	
	//Listen to connect request...
	while((listen(sockfd,MAX_SERVER_REQUEST))==-1){
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart listening..."<<std::endl;
#endif
	}
	
	maxfd=sockfd;
	
	int client[MAX_SERVER_REQUEST];
	sockaddr_in client_info[MAX_SERVER_REQUEST];
	for(int i=0;i<MAX_SERVER_REQUEST;i++)client[i]=-1;
	
	FD_SET(sockfd,&all_fd);
	
	int read_num,maxIndex,connfd;
	stop = std::chrono::high_resolution_clock::now();
	do{
		read_fds=all_fd;

		timeout.tv_sec=0;timeout.tv_usec=1000;
		while((read_num=select(maxfd+1,&read_fds,NULL,NULL,&timeout))==-1){
#ifdef _WARNING_MESSAGE_
			std::cout<<"Restart select..."<<std::endl;
#endif
		}
		
		if(FD_ISSET(sockfd,&read_fds)){
			maxIndex=0;
			if((connfd=accept(sockfd,(sockaddr*)&clientInfo,&addrlen))!=-1){
				FD_SET(connfd,&all_fd);
				int i;
				for(i=0;i<MAX_SERVER_REQUEST;i++){
					if(client[i]==-1){
						client[i]=connfd;
						client_info[i]=clientInfo;
						break;
					}
				}
						
				if(connfd>maxfd)maxfd=connfd;
				if(i>maxIndex)maxIndex=i;
				
				read_num--;
				if(read_num<=0)continue;
			}
			else{
				continue;
			}
		}
		for(int i=0;i<=maxIndex;i++){
			if(client[i]==-1)continue;
			if(FD_ISSET(client[i],&read_fds)){
				int readCount=read(client[i],inputBuffer,sizeof(inputBuffer));
				if(readCount==0){
					close(client[i]);
					FD_CLR(client[i],&all_fd);
					client[i]=-1;
				}
				else if(readCount!=-1){
					std::cout<<"receive message\""<<inputBuffer<<"\""<<std::endl;
					write(client[i],"message received...",19);
				}
					
				read_num--;
				if(read_num<=0)break;
			}
		}
		stop = std::chrono::high_resolution_clock::now();
		//if((std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count())%1000==0)std::cout<<"passing time>>"<<std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count()<<"ms"<<std::endl;
	}while(millisecond>std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count());
/*
	forClientSockfd=accept(sockfd,(struct sockaddr*)&clientInfo,&addrlen);
	recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
	recvMsg=inputBuffer;
*/
	close(sockfd);
}

int NetworkNode::sendString(std::string ip,unsigned short port_num,const std::string& msg,std::string& strReceive){
	int sockfd=-1,forClientSockfd=0;
	char buffer[MESSAGE_LENGTH];
	
#ifdef IPV4
	while((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
#else
	while((sockfd=socket(AF_INET6,SOCK_STREAM,0))==-1)
#endif
	{
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart socket..."<<std::endl;
#endif
	}
	
	struct sockaddr_in serverInfo;
	memset(&serverInfo,0,sizeof(serverInfo));
#ifdef IPV4
	serverInfo.sin_family=AF_INET;
#else
	serverInfo.sin_family=AF_INET6;
#endif
	serverInfo.sin_addr.s_addr=inet_addr(ip.c_str());
	serverInfo.sin_port=htons(port_num);
	connect(sockfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo));
	
	for(int i=0;i<msg.size();i++)buffer[i]=msg[i];
	int sendResult=send(sockfd,buffer,msg.size()*sizeof(char),0);
	
	recv(sockfd,buffer,sizeof(buffer),0);
	strReceive=buffer;
	
	
	close(sockfd);
	if(sendResult==-1){
#ifdef _WARNING_MESSAGE_
		std::cout<<"Sending failure..."<<std::endl;
#endif
		return 0;
	}
	else{//Success...
//std::cout<<"message send..."<<std::endl<<"message received>>"<<buffer<<std::endl;
		Tag tgReceive;
		tgReceive=strReceive;
		if(tgReceive.name.length()<=0){
			strReceive="";
			return 0;
		}
		strReceive=tgReceive.content;
		return 1;
	}
}

std::string NetworkNode::receiveString(unsigned short port_num){
	int sockfd=-1,forClientSockfd=0;
	char inputBuffer[MESSAGE_LENGTH];
	std::string recvMsg;
	
#ifdef IPV4
	while((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
#else
	while((sockfd=socket(AF_INET6,SOCK_STREAM,0))==-1)
#endif
	{
#ifdef _WARNING_MESSAGE_
		std::cout<<"Restart socket..."<<std::endl;
#endif
	}
	
	struct sockaddr_in serverInfo,clientInfo;
	int addrlen=sizeof(clientInfo);
	memset(&serverInfo,0,sizeof(serverInfo));
#ifdef IPV4
	serverInfo.sin_family=AF_INET;
#else
	serverInfo.sin_family=AF_INET6;
#endif
	serverInfo.sin_addr.s_addr=INADDR_ANY;//recieve from every ip
	serverInfo.sin_port=htons(port_num);
	bind(sockfd,(struct sockaddr*)&serverInfo,sizeof(serverInfo));
	listen(sockfd,MAX_SERVER_REQUEST);//second parameter mean how many client can connect at the same time
	
	forClientSockfd=accept(sockfd,(struct sockaddr*)&clientInfo,&addrlen);
	recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
	recvMsg=inputBuffer;
	
	close(sockfd);
	return recvMsg;
}

Message& Message::operator=(const Message& msgIn){
	type=msgIn.type;
	content=msgIn.content;
}

int NetworkNode::operator=(const std::string& strIn){
	chainLock.lock();
	if((BlockChain::operator=(strIn))==0){
		return 0;
	}
	updateTime=time(nullptr);
	currentCursor.clear();
	chainLock.unlock();
	return 1;
}

void NetworkNode::run(){

	unsigned short port;std::string ip;
/*
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
*/
//std::cout<<"ip=\""<<ip<<"\tport=\""<<port<<"\""<<std::endl;
	for(uint32_t terminal=0;terminal==0;){//Import port
		uint32_t num;
		std::cout<<"where does your chain come from?(Internet:0/File:1/New chain:2)"<<std::endl<<">>";
		std::cin>>num;fflush(stdin);
		switch(num){
			case 0:{
				std::cout<<"another node ip"<<std::endl<<">>";
				std::cin>>ip;fflush(stdin);
				std::cout<<"another node port"<<std::endl<<">>";
				std::cin>>port;fflush(stdin);
				Message askMsg;askMsg.set(messageType::askChain,"");
				
				std::string strBlockChain="";
				sendString(ip,port,askMsg.toString(),strBlockChain);
std::cout<<"get chain=\""<<strBlockChain<<"\""<<std::endl;
				if(((*this)=strBlockChain)==0)break;
std::cout<<"success input chain...\n";
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
				
				if(((*this)=strBlockChain)==0){
					break;
				}
				
				terminal=1;
				break;
			}
			case 2:{
				terminal=1;
				break;
			}
			default:
				break;
		}
	}

	clientRoutine();
}

Block& NetworkNode::currBlock(){
	return *((getNode(currentCursor))->block);
}

uint32_t NetworkNode::nextBlock(uint32_t nextBranch){
	struct node* leaf=getNode(currentCursor);
	if(leaf==NULL||leaf==nullptr)return 0;
	if(leaf->next.size()<=nextBranch)return 0;
	currentCursor.push_back(nextBranch);
	return 1;
}

NetworkNode::NetworkNode(const std::string& ipIn,unsigned short portNumIn,uint32_t mineDifficultyIn):isFinished(0),
  serverThread(NetworkNode::serverRoutine,this),portNum(portNumIn),nodeIP(ipIn),updateTime(time(nullptr)),
  BlockChain(mineDifficultyIn){
	currentCursor.clear();
	addressList.clear();
	//serverThread=new std::thread(&NetworkNode::serverRoutine,this);
}

void NetworkNode::finish(){
	isFinished=1;
}

NetworkNode::~NetworkNode(){
	finish();
	serverThread.join();
//std::cout<<"success finish"<<std::endl;
}

Message::Message():type(messageType::unknown){
	
}

Message::~Message(){
	
}

void NetworkNode::addAddress(const std::string& ipIn,unsigned short portIn){
	struct nodeAddress n_ad;
	n_ad.ip=ipIn;
	n_ad.port=portIn;
	addressList.push_back(n_ad);
}

int NetworkNode::get_local_ip(const char* eth_inf,char* ip){
	int sd;
	struct sockaddr_in sin;
	struct ifreq ifr;
#ifdef IPV4
	sd=socket(AF_INET,SOCK_DGRAM,0);
#else
	sd=socket(AF_INET6,SOCK_DGRAM,0);
#endif
	if(sd==-1){
		std::cout<<"socket error:"<<strerror(errno)<<std::endl;
		return -1;
	}
	strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	
	if (ioctl(sd, SIOCGIFADDR, &ifr) < 0){
		std::cout<<"ioctl error:"<<strerror(errno)<<std::endl;
		close(sd);
		return -1;
	}
	
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	snprintf(ip, IP_SIZE, "%s", inet_ntoa(sin.sin_addr));
	
	close(sd);
	return 0;
}