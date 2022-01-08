#ifndef _MYMSG_H_
#define _MYMSG_H_
#include <iostream>
#include <stdio.h>
#include "clientList.h"

class myMessage{
    int type;
    std::string Msg;//打包好的message
    std::string content;//解析后的内容
    int clientID;//区分服务器和客户端--- 0-server
    int dst_clientID;

public:
    myMessage(int type,std::string content,int ID){
        this->type=type;
        this->content=content;
        this->Msg=content;
        this->clientID=ID;
    }
    myMessage(){
        this->type=0;
        this->content=" ";
        this->clientID=1;
    }
    myMessage(std::string msg){
        this->Msg=msg;
    }
    std::string Encapsulation(int type, std::string ss,int id);//封装为数据包

    void AnalyzeMsg();//解析数据包
    int getMsgType(){
        return type;
    }
    std::string getMsgContent(){
        return Msg;
    }
    std::string AnalyzeTime();//分析时间
    
    std::string getContent(){
        std::string ss=content;
        return ss;
    }
    int getClientID(){
        return clientID;
    }
    std::string getMsg(){
        return Msg;
    }

    void setMsg(std::string ss){
        Msg=ss;
    }
    void setContent(std::string ss){
        content=ss;
    }
    void setType(int ty){
        type=ty;
    }
    void setClientID(int id){
        clientID=id;
    }
    void setdst_ClientID(int id){
        dst_clientID=id;
    }
    int  getdst_ClientID()
    {
        return dst_clientID;
    }
    clientList AnalyzeList();//返回列表

};
#endif