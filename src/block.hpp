#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include "sha256.hpp"
#include "tag.hpp"

#define _WARNING_MESSAGE_

#ifndef _BLOCK_H_
#define _BLOCK_H_
class BlockData{
	private:
		std::string content;
	public:
		std::string getContent()const;
		std::string toString()const;
		void set(std::string);
		BlockData& operator=(const BlockData&);
		BlockData& operator=(const std::string&);
	
		BlockData(std::string dataIn="");
		~BlockData();
};

class Block{
	public:
		std::string prevHash;
	private:
		BlockData data;
		time_t create_time;
		uint32_t nIndex;
		uint64_t nonce;
		
	public:
		std::string hash()const;
		std::string hash(std::string)const;
		void setData(std::string);
		BlockData& getData();
		uint32_t getId()const;
		void mineBlock(uint32_t);
		uint32_t checkKey(uint32_t difficulty)const;
		std::string toString()const;
		int operator=(const std::string&);
		Block& operator=(const Block&);
		
		Block(uint32_t nIndexIn,std::string dataIn="",std::string prevHashIn="");
		Block(uint32_t nIndexIn,const BlockData& dataIn,std::string prevHashIn="");
		~Block();
};
#endif