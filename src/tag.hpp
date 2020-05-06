#ifndef _TAG_H_
#define _TAG_H_

class Tag{
	public:
		std::string name;
		std::string content;
		uint32_t length;
		
		inline int operator=(const std::string&);
		inline std::string operator<<(const std::string&);
		inline std::string toString()const;
		
		inline void clear();
		
		inline Tag();
		inline Tag(const std::string& nameIn,const std::string& contentIn);
};

inline void Tag::clear(){
	name="";content="";length=0;
}

inline Tag::Tag(const std::string& nameIn,const std::string& contentIn):name(nameIn),content(contentIn),
  length(contentIn.length()){
	
}

inline int Tag::operator=(const std::string& strIn){
	int srch;
	std::string nameIn,contentIn,tmp_str;
	uint32_t lengthIn;
	
	for(srch=0;srch<strIn.length()&&strIn[srch]!='<';srch++);
	if(srch>=strIn.length())return 0;
	
	for(srch++;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	nameIn="";
	for(;srch<strIn.length()&&strIn[srch]!=' ';srch++)nameIn+=strIn[srch];
	if(srch>=strIn.length())return 0;
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	tmp_str="";
	for(;srch<strIn.length()&&strIn[srch]!=' '&&strIn[srch]!='=';srch++)tmp_str+=strIn[srch];
	if(srch>=strIn.length()||!(tmp_str=="length"))return 0;
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	for(;srch<strIn.length()&&strIn[srch]=='=';srch++);
	if(srch>=strIn.length())return 0;
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	tmp_str="";
	for(;srch<strIn.length()&&strIn[srch]!=' '&&strIn[srch]!='>';srch++)tmp_str+=strIn[srch];
	if(srch>=strIn.length())return 0;
	lengthIn=atoi(tmp_str.c_str());
	
	for(;srch<strIn.length()&&strIn[srch]!='>';srch++);
	if(srch>=strIn.length()||(srch+lengthIn>=strIn.length()))return 0;
	
	contentIn="";
	for(int i=0;i<lengthIn;i++)contentIn+=strIn[++srch];
	
	for(srch++;srch<strIn.length()&&strIn[srch]!='<';srch++);
	if(srch>=strIn.length())return 0;
	
	for(srch++;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	for(;srch<strIn.length()&&strIn[srch]=='\\';srch++);
	if(srch>=strIn.length())return 0;
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	tmp_str="";
	for(;srch<strIn.length()&&strIn[srch]!=' '&&strIn[srch]!='>';srch++)tmp_str+=strIn[srch];
	if(srch>=strIn.length()||tmp_str!=nameIn)return 0;
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return 0;
	
	for(;srch<strIn.length()&&strIn[srch]!='>';srch++);
	if(srch>=strIn.length()||strIn[srch]!='>')return 0;
	
	name=nameIn;
	length=lengthIn;
	content=contentIn;
	//std::cout<<"original string=\""<<strIn<<"\""<<std::endl<<"name=\""<<nameIn<<"\"\tlengthIn=\""<<lengthIn<<"\""<<std::endl;
	//std::cout<<"tmp_str=\""<<tmp_str<<"\""<<std::endl<<"contentIn=\""<<contentIn<<"\""<<std::endl;
	
	return 1;
}

inline std::string Tag::operator<<(const std::string& strIn){
	std::string strOut="";
	int srch;
	std::string nameIn,contentIn,tmp_str;
	uint32_t lengthIn;
	
	for(srch=0;srch<strIn.length()&&strIn[srch]!='<';srch++);
	if(srch>=strIn.length())return "";
	
	for(srch++;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	nameIn="";
	for(;srch<strIn.length()&&strIn[srch]!=' ';srch++)nameIn+=strIn[srch];
	if(srch>=strIn.length())return "";
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	tmp_str="";
	for(;srch<strIn.length()&&strIn[srch]!=' '&&strIn[srch]!='=';srch++)tmp_str+=strIn[srch];
	if(srch>=strIn.length()||!(tmp_str=="length"))return "";
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	for(;srch<strIn.length()&&strIn[srch]=='=';srch++);
	if(srch>=strIn.length())return "";
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	tmp_str="";
	for(;srch<strIn.length()&&strIn[srch]!=' '&&strIn[srch]!='>';srch++)tmp_str+=strIn[srch];
	if(srch>=strIn.length())return "";
	lengthIn=atoi(tmp_str.c_str());
	
	for(;srch<strIn.length()&&strIn[srch]!='>';srch++);
	if(srch>=strIn.length()||(srch+lengthIn>=strIn.length()))return "";
	
	contentIn="";
	for(int i=0;i<lengthIn;i++)contentIn+=strIn[++srch];
	
	for(srch++;srch<strIn.length()&&strIn[srch]!='<';srch++);
	if(srch>=strIn.length())return "";
	
	for(srch++;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	for(;srch<strIn.length()&&strIn[srch]=='\\';srch++);
	if(srch>=strIn.length())return "";
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	tmp_str="";
	for(;srch<strIn.length()&&strIn[srch]!=' '&&strIn[srch]!='>';srch++)tmp_str+=strIn[srch];
	if(srch>=strIn.length()||tmp_str!=nameIn)return "";
	
	for(;srch<strIn.length()&&strIn[srch]==' ';srch++);
	if(srch>=strIn.length())return "";
	
	for(;srch<strIn.length()&&strIn[srch]!='>';srch++);
	if(srch>=strIn.length()||strIn[srch]!='>')return "";
	
	//std::cout<<"original string=\""<<strIn<<"\""<<std::endl<<"name=\""<<nameIn<<"\"\tlengthIn=\""<<lengthIn<<"\""<<std::endl;
	//std::cout<<"tmp_str=\""<<tmp_str<<"\""<<std::endl<<"contentIn=\""<<contentIn<<"\""<<std::endl;
	
	name=nameIn;
	length=lengthIn;
	content=contentIn;
	
	strOut="";
	for(srch++;srch<strIn.length();srch++)strOut+=strIn[srch];
	
	return strOut;
}

inline std::string Tag::toString()const{
	std::string strOut="";
	strOut=("<"+name+" length="+std::to_string(length)+">"+content+"<\\"+name+">");
	return strOut;
}

inline Tag::Tag():name(""),content(""),length(0){
	
}

inline int isNum(const std::string& str){  
	std::stringstream sin(str);  
	double d;  
	char c;  
	if(!(sin >> d))  
	{
		return 0;
	}
	if(sin >> c) 
	{
		return 0;
	}  
	return 1;  
}
#endif