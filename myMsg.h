#ifndef _MYMSG_H_
#define _MYMSG_H_
#include <iostream>
#include <stdio.h>
#include "clientList.h"

class myMessage{
    int type;
    std::string Msg;
    std::string content;
    int clientID;//区分服务器和客户端---有一个问题，这个listid是怎么分配的ying

public:
    myMessage(int type,std::string content,int ID){
        this->type=type;
        this->content=content;
        this->clientID=ID;
    }
    myMessage(){
        this->type=0;
        this->content=" ";
        this->clientID=1;
    }
    std::string Encapsulation(int type, std::string ss,int id);
    void AnalyzeMsg();
    int getMsgType(){
        return type;
    }
    std::string getMsgContent(){
        return Msg;
    }
    std::string AnalyzeTime();//分析时
    void setMsg(std::string ss){
        Msg=ss;
    }
    std::string getContent(){
        std::string ss=content;
        return ss;
    }
    clientList AnalyzeList();//返回列表

};
#endif