#include "block.hpp"

int Block::operator=(const std::string& strIn){
	Tag tgTime,tgData,tgNIndex,tgNonce,tgPrevHash;
	tgPrevHash<<(tgNonce<<(tgNIndex<<(tgData<<(tgTime<<strIn))));
	if((!isNum(tgTime.content))||(!isNum(tgNIndex.content))||(!isNum(tgNonce.content))||
		(tgData.name.length()==0)||(tgPrevHash.name.length()==0))return 0;
	
	std::stringstream ss;ss.str("");ss.clear();
	ss<<tgTime.content;ss>>create_time;
	//std::cout<<"get time=\""<<create_time<<"\""<<std::endl;
	
	data=tgData.content;
	nIndex=atoi(tgNIndex.content.c_str());
	nonce=atoi(tgNonce.content.c_str());
	prevHash=tgPrevHash.content;
	
	return 1;
}

std::string Block::toString()const{
	std::string strOut;
	std::stringstream ss;ss.str("");ss.clear();
	
	ss<<create_time;
	Tag tgTime("time",ss.str());
	Tag tgData("BlockData",data.toString());
	Tag tgNIndex("nIndex",std::to_string(nIndex));
	Tag tgNonce("nonce",std::to_string(nonce));
	Tag tgPrevHash("prevHash",prevHash);
	
	strOut="";
	strOut+=tgTime.toString();
	strOut+=tgData.toString();
	strOut+=tgNIndex.toString();
	strOut+=tgNonce.toString();
	strOut+=tgPrevHash.toString();
	return strOut;
}

uint32_t Block::getId()const{
	return nIndex;
}

void Block::mineBlock(uint32_t difficulty){
	std::string str="";
	for(uint32_t i=0;i<difficulty;i++)str+="0";
	std::string hash_value;
	do{
		nonce++;
		hash_value=hash();
	}while(hash_value.substr(0,difficulty)!=str);
}

uint32_t Block::checkKey(uint32_t difficulty)const{
	std::string str="";std::string hash_value=hash();
	for(uint32_t i=0;i<difficulty;i++)
		str+="0";
	if(hash_value.substr(0,difficulty)==str)return 1;
	return 0;
}

Block& Block::operator=(const Block& blockIn){
	prevHash=blockIn.prevHash;
	data=blockIn.data;
	create_time=blockIn.create_time;
	nIndex=blockIn.nIndex;
	nonce=blockIn.nonce;
	
	return *this;
}

std::string Block::hash(std::string str)const{
	return sha256(str);
}

std::string Block::hash()const{
	std::stringstream ss;
	ss<<nIndex<<create_time<<nonce<<data.toString()<<prevHash;
	return sha256(ss.str());
}

void Block::setData(std::string str){
	data.set(str);
}

std::string BlockData::toString()const{
	return content;
}

std::string BlockData::getContent()const{
	return content;
}

void BlockData::set(std::string str){
	content=str;
}

BlockData& BlockData::operator=(const BlockData& dataIn){
	content=dataIn.content;
	return *this;
}

BlockData& BlockData::operator=(const std::string& strIn){
	content=strIn;
	return *this;
}

Block::Block(uint32_t nIndexIn,const BlockData& dataIn,std::string prevHashIn):prevHash(prevHashIn),
  nIndex(nIndexIn),nonce(0){
	data=dataIn;
	create_time=time(nullptr);
}

Block::Block(uint32_t nIndexIn,std::string dataIn,std::string prevHashIn):data(dataIn),prevHash(prevHashIn),
  nIndex(nIndexIn),nonce(0){
	create_time=time(nullptr);
}

BlockData& Block::getData(){
	return data;
}

BlockData::~BlockData(){
	
}

BlockData::BlockData(std::string dataIn):content(dataIn){
	
}

Block::~Block(){
	
}