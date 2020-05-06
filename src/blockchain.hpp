#include <vector>
#include <stack>
#include <fstream>
#include "block.hpp"

#ifndef _BLOCKCHAIN_H_
#define _BLOCKCHAIN_H_

typedef std::vector<uint32_t> Branch;

class BlockChain{
	public:
		struct node{
			std::vector<struct node*>next;
			Block* block;
			
			std::string toString()const;
			int operator=(const std::string&);
		};
	private:
		struct node* chain;
		uint64_t blockNum;
	public:
		uint32_t mineDifficulty;
		
	public:
		int addBlock(const BlockData&);
		int addBlock(const BlockData&,const Branch&);
		uint32_t checkChain()const;
		struct node* getNode(const Branch&);
		struct node* findData(std::string)const;
		uint32_t addBlockAfter(std::string,const BlockData&);
		int addBlockWithCheckAfter(struct node*,const Block&);
		uint32_t branchNum(struct node*)const;
		
		std::string toString()const;
		int operator=(const std::string&);
		
		static void deleteNode(struct node*);
		
		int writeToFile(const std::string& path)const;
		int readFromFile(const std::string& path);
		
		BlockChain(uint32_t mineDifficultyIn=2);
		~BlockChain();
	private:
		struct node* getLastBlock()const;
		void getLengthAndLeaf(struct node* root,struct node* &leaf,uint32_t& length)const;
};
#endif