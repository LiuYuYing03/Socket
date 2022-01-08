#include "myMsg.h"
#include <string>

std::string myMessage::Encapsulation(int type, std::string ss,int id){
    this->type=type;
    this->clientID=id;
    char flag = '~';
    char Type_ID,final_TI;
    if(type>=0&&type<=5){
        Type_ID=type;
    }
    else{
        Type_ID=7;
        this->content=" ";
    }
    Type_ID=Type_ID<<5;
    if(id>=0&&id<=31){
        final_TI=(Type_ID|(char)id);
    }
    else{
        final_TI=Type_ID;
    }
    std::string temp;
    std::string tmp="~";
    temp=tmp+final_TI+content;
    this->Msg=temp+tmp;
    return Msg;
}

void myMessage::AnalyzeMsg(){
    int length=Msg.length();
    //std::cout<<length<<std::endl;
    //std::cout<<Msg<<std::endl;
    std::string temp=Msg.substr(2,length-3);
    content=temp;
    char T_I=Msg[1];
    char T_res=T_I&0xE0;
    char I_res=T_I&0x1F;
    this->type=T_res>>5;
    this->clientID=I_res;
}

std::string myMessage::AnalyzeTime(){
    //暂时保留
    return "It can do time!";
}
clientList myMessage::AnalyzeList(){
    //暂时保留
    clientList tt=clientList();
    tt.no=1;
    tt.ip="192.168.0.1";
    tt.port_num=3000;
    return tt;
}