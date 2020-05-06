#include "blockchain.hpp"

int BlockChain::readFromFile(const std::string& path){
	std::ifstream inFile(path);
	if(!inFile)return 0;
	std::string strIn="";
	char ch;
	while(inFile.get(ch))strIn+=ch;
	inFile.close();
	if(((*this)=strIn)==0)return 0;
	return 1;
}

int BlockChain::writeToFile(const std::string& path)const{
	std::ofstream outFile(path);
	if(!outFile)return 0;
	outFile<<toString();
	outFile.close();
	
	return 1;
}

int BlockChain::node::operator=(const std::string& strIn){
	Tag tgNode,tgBlock,tgChildNum,tgChild,tgChildNode;
	std::string strRemain;
	struct node* newNode=NULL;struct node* newChildNode=NULL;
	
	if((tgNode=strIn)==0)return 0;
	tgChild<<(tgChildNum<<(tgBlock<<tgNode.content));
	if((tgBlock.name.length()==0)||!isNum(tgChildNum.content.c_str())||(tgChild.name.length()==0))return 0;
	uint32_t childNum=atoi(tgChildNum.content.c_str());

	newNode=new struct node;newNode->next.clear();newNode->block=new Block(0);
	if(!((*(newNode->block))=tgBlock.content)){
		delete newNode->block;
		delete newNode;
		return 0;
	}

	strRemain=tgChild.content;
	for(int i=0;i<childNum;i++){
		tgChildNode.clear();
		strRemain=tgChildNode<<strRemain;
		if(tgChildNode.name.length()==0){
			BlockChain::deleteNode(newNode);
			
			return 0;
		}
		newChildNode=new struct node;newChildNode->next.clear();newChildNode->block=new Block(0);
		if(((*newChildNode)=tgChildNode.toString())==0){
			BlockChain::deleteNode(newChildNode);
			BlockChain::deleteNode(newNode);
			
			return 0;
		}
		newNode->next.push_back(newChildNode);
	}
	
	//Success...
	if((block!=NULL)&&(block!=nullptr))delete block;
	block=newNode->block;
	for(int i=0;i<next.size();i++)
		BlockChain::deleteNode(next.at(i));
	next.clear();
	for(int i=0;i<newNode->next.size();i++)
		next.push_back(newNode->next.at(i));
	delete newNode;
	
	return 1;
}

int BlockChain::operator=(const std::string& strIn){
	Tag tgBlockNum,tgMineDifficulty,tgBlockChain;
	
	tgBlockChain<<(tgMineDifficulty<<(tgBlockNum<<strIn));
	if(!isNum(tgMineDifficulty.content.c_str())||!isNum(tgBlockNum.content.c_str())||
		(tgBlockChain.content.length()==0))return 0;
	
	struct node* newChain=new struct node;
	newChain->next.clear();
	newChain->block=new Block(0);
//std::cout<<std::endl<<"assign to string=\""<<tgBlockChain.content<<"\""<<std::endl;
	
	if((*newChain)=tgBlockChain.content){//Success...
		deleteNode(chain);
		chain=newChain;
		
		return 1;
	}
	else{//Fialure...
		deleteNode(newChain);
		return 0;
	}
	return 1;
}

void BlockChain::deleteNode(struct node* chainIn){
	if(chainIn==NULL||chainIn==nullptr)return;
	if(chainIn->block==NULL||chainIn->block==nullptr)delete chainIn->block;
	for(int i=0;i<chainIn->next.size();i++){
		deleteNode(chainIn->next.at(i));
	}
}

std::string BlockChain::node::toString()const{
	std::string strChild;
	
	if(!block)return "";
	Tag tgBlock("block",block->toString());
	
	Tag tgChildNum("childNum",std::to_string(next.size()));
	strChild="";
	for(int i=0;i<next.size();i++){
		strChild+=next.at(i)->toString();
	}
	Tag tgChild("child",strChild);
	Tag tgNode("node",tgBlock.toString()+tgChildNum.toString()+tgChild.toString());
	
	return tgNode.toString();
}

std::string BlockChain::toString()const{
	std::string strOut="",strChain="";
	Tag tgBlockNum("blockNum",std::to_string(blockNum));
	Tag tgMineDifficulty("mineDifficulty",std::to_string(mineDifficulty));
	strOut=(tgBlockNum.toString()+tgMineDifficulty.toString());
	
	if(chain)strChain=chain->toString();
	else strChain="";
	Tag tgBlockChain("blockChain",strChain);
	strOut+=tgBlockChain.toString();
	//std::cout<<"chain=\""<<strOut<<"\""<<std::endl;
	
	return strOut;
}

uint32_t BlockChain::branchNum(struct node* nd)const{
	if(nd==NULL||nd==nullptr)return 0;
	return nd->next.size();
}

int BlockChain::addBlockWithCheckAfter(struct node* leaf,const Block& blockIn){
	if(leaf==NULL||leaf==nullptr)return 0;
	if(!blockIn.checkKey(mineDifficulty))return 0;
	
	struct node* next_node=new struct node;
	Block b((leaf->block->getId())+1);b=blockIn;
	next_node->block=new Block((leaf->block->getId())+1,b.getData(),leaf->block->hash());
	//next_node->block=new Block((leaf->block->getId())+1,blockIn.data,leaf->block->hash());
	next_node->next.clear();
	*(next_node->block)=blockIn;
	leaf->next.push_back(next_node);
	blockNum++;
	return 1;
}

uint32_t BlockChain::addBlockAfter(std::string target,const BlockData&dataIn){
	struct node* leaf=findData(target);
	if(leaf==NULL||leaf==nullptr)return 0;
	
	struct node* next_node=new struct node;
	next_node->block=new Block((leaf->block->getId())+1,dataIn,leaf->block->hash());
	next_node->next.clear();
	next_node->block->mineBlock(mineDifficulty);
	leaf->next.push_back(next_node);
	blockNum++;
	return 1;
}

struct BlockChain::node* BlockChain::getNode(const Branch& branch){
	struct node* leaf=chain;

	for(int i=0;i<branch.size();i++){
		if(branch.at(i)>=leaf->next.size())return NULL;
		leaf=leaf->next.at(branch.at(i));
	}
	return leaf;
}

int BlockChain::addBlock(const BlockData& dataIn,const Branch& branch){
/*
	struct node* leaf=chain;

	for(int i=0;i<branch.size();i++){
		if(branch.at(i)>=leaf->next.size())return 0;
		leaf=leaf->next.at(branch.at(i));
	}
*/
	struct node* leaf=getNode(branch);
	if(leaf==NULL||leaf==nullptr)return 0;
	
	struct node* next_node=new struct node;
	next_node->block=new Block((leaf->block->getId())+1,dataIn,leaf->block->hash());
	next_node->next.clear();
	next_node->block->mineBlock(mineDifficulty);
	leaf->next.push_back(next_node);
	
	return 1;
}

int BlockChain::addBlock(const BlockData& dataIn){
	struct node* leaf=getLastBlock();
	struct node* next_node=new struct node;
	next_node->block=new Block((leaf->block->getId())+1,dataIn,leaf->block->hash());
	next_node->next.clear();
	next_node->block->mineBlock(mineDifficulty);
	leaf->next.push_back(next_node);
	blockNum++;
	return 1;
}

struct BlockChain::node* BlockChain::findData(std::string target)const{
	std::stack<struct node*>stk;
	struct node* curr;
	
	stk.push(chain);
	while(!stk.empty()){
		curr=stk.top();	stk.pop();
		if(curr->block->getData().toString()==target)return curr;
		for(uint32_t i=0;i<(uint32_t)curr->next.size();i++){
			stk.push(curr->next.at(i));
		}
	}
	return NULL;
}

struct BlockChain::node* BlockChain::getLastBlock()const{
	uint32_t length;
	struct node *leaf;
	
	getLengthAndLeaf(chain,leaf,length);
	return leaf;
}

uint32_t BlockChain::checkChain()const{
	std::stack<struct node*>stk;
	struct node* curr;
	
	stk.push(chain);
	while(!stk.empty()){
		curr=stk.top();	stk.pop();
		for(uint32_t i=0;i<(uint32_t)curr->next.size();i++){
			if(curr->block->hash()!=curr->next.at(i)->block->prevHash)return 0;
			if(!curr->next.at(i)->block->checkKey(mineDifficulty))return 0;
			stk.push(curr->next.at(i));
		}
	}
	return 1;
}

BlockChain::BlockChain(uint32_t mineDifficultyIn):chain(NULL),blockNum(1),mineDifficulty(mineDifficultyIn){
	chain=new struct node;
	chain->next.clear();
	chain->block=new Block(0,"Genesis Block");
}

void BlockChain::getLengthAndLeaf(struct node* root,struct node* &leaf,uint32_t& length)const{
	uint32_t max_length,this_length;
	struct node *this_leaf;
	
	leaf=root;	max_length=0;
	for(uint32_t i=0;i<root->next.size();i++){
		getLengthAndLeaf(root->next.at(i),this_leaf,this_length);
		if(max_length<this_length){
			max_length=this_length;
			leaf=this_leaf;
		}
	}
	length=max_length+1;
}

BlockChain::~BlockChain(){
	deleteNode(chain);
}