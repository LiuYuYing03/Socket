#include "myMsg.h"
#include "myMsg.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <pthread.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>

using namespace std;
 
#pragma comment(lib, "ws2_32.lib")			//add ws2_32.lib
 
//Macro variables
const int DEFAULT_PORT = 3000;
const string DEFAULT_IP="127.0.0.1";
SOCKET clientSocket;
bool alreadyConnected = false;
pthread_mutex_t mutex;
bool waitingStatus=false;
pthread_t myThread;


void * recieveMsg(void *args);

void releWaitState(){
    pthread_mutex_lock(&mutex);
    waitingStatus = false;
    pthread_mutex_unlock(&mutex);
}
void setWaitState(){
    pthread_mutex_lock(&mutex);
    waitingStatus = true;
    pthread_mutex_unlock(&mutex);
}

int connect2Server(){
    if(alreadyConnected){
        cout << "You've already connected!"<<endl;
        return -1;
    }
    static char ip[1024];
    int port;
    cout << "please enter ip address:"<<endl;
    cin >> ip;
    cout << "please enter the port:"<<endl;
    cin >>port;

    SOCKADDR_IN addrServe;
    addrServe.sin_family=AF_INET;
    addrServe.sin_addr.s_addr = inet_addr(ip);
    addrServe.sin_port = htons(port);

    int ret;
    ret = connect(clientSocket,(SOCKADDR*)&addrServe,sizeof(SOCKADDR));
    if(ret == INVALID_SOCKET){
        cout << "connect() fail" <<WSAGetLastError()<<endl;
        return -1;
    }
    else{
        cout << "Successfully connected!"<<endl;
    }
    alreadyConnected = true;

    //send a package for connecting successfully
    //暂定，待修改
    // myMessage pack = myMessage(0," ",1); //connect
    // string pstr = pack.Encapsulation(0," ",1); //
    // send(clientSocket,pstr.c_str(), pstr.size(), 0);
    return 0;

}


void closeClient(){
    //send a package for closing the connection
    //暂定
    int ret;
    myMessage pack = myMessage(4," ",1); //close
    string pstr = pack.Encapsulation(4," ",1); //
    send(clientSocket, pstr.c_str(), pstr.size(), 0);

    ret=closesocket(clientSocket);//关闭socket
    alreadyConnected = false;
    if(ret!=0){
        cout<<"Error! Can't close the socket!"<<endl;
        //暂不处理
        return;
    }
    cout<<"Close the client successfully!"<<endl;
    return;
}

void request2Server(int operation){

    //将request信息装包
    string packMessage;
    if(operation==3){
        packMessage="time";
    }
    else if(operation == 4){
       // packMessage="request name";
       packMessage="ask for server name";
    }
    else if(operation == 5){
        packMessage ="request client list";
    }

    myMessage pack = myMessage(operation-2,packMessage,1); 
    string pstr = pack.Encapsulation(operation-2,"request for the server",1); 
    send(clientSocket, pstr.c_str(), pstr.size(), 0);
    while(waitingStatus){

    };
    setWaitState();//wait4Msg==true
    //不断地等recieve处理完消息
    int ret = pthread_create(&myThread, NULL,recieveMsg,&clientSocket);
    if(ret !=0){
        cout<<"ERROR!Create pthread failed!"<<endl;
        return;
    }
    //cout<<"sssss"<<endl;
    pthread_join(myThread,NULL);
    //cout<<"ttttt";
    //
    while(true){
        pthread_mutex_lock(&mutex);
        if(!waitingStatus){
            //releWaitState();//代表已经接收到消息了
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);//实现访问alreadyRsv时操作互斥
    }


}

//基本功能保持与request一致一致
void send2Server(){
    string Msg;
    int list_id;
    cout << "please enter your client id:"<<endl;
    cin >> list_id;
    cout << "Please enter your message to send:"<<endl;
    cin >> Msg;//看看读的对不对
    cout << Msg;
    //组装Msg
    myMessage pack = myMessage(5,Msg,list_id); 
    string pstr = pack.Encapsulation(5,Msg,list_id); 
    send(clientSocket, pstr.c_str(), pstr.size(), 0);

    //互斥接收信息
    while(waitingStatus){

    };
    setWaitState();//wait4Msg==true
    cout<<"xxxx";
    int ret = pthread_create(&myThread, NULL,recieveMsg,&clientSocket);
    cout<<"yyyy";
    if(ret !=0){
        cout<<"ERROR!Create pthread failed!"<<endl;
        return;
    }
    //cout<<"ssss";
    pthread_join(myThread,NULL);
    //不断地等recieve处理完消息
    cout<<"ttttt";
    while(true){
        pthread_mutex_lock(&mutex);
        if(waitingStatus){
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);//实现访问alreadyRsv时操作互斥
        cout<<"jump out";
    }
}

void * recieveMsg(void *args){
    char recvBuf[1024]="\0";
    int ret;
    while(true){
        ret=recv(clientSocket,recvBuf,1024,0);
        if(ret==SOCKET_ERROR || ret==0){
            continue;
        }
       // system("pause");
        //解析字符串
        int no,port_num;
        string ip;
        string pstr =recvBuf;
        myMessage raw;
        raw.setMsg(pstr);
        raw.AnalyzeMsg();
        string tString;
        int type=raw.getMsgType();
        if(type<0||type>7){
            cout<<"Recieve incorrect reply!"<<endl;
            continue;
        }
        if(type==1){
            //处理时间响应
            tString = raw.getContent();
            cout<<"Current Server time is:"<<tString<<endl;
            releWaitState();//代表已经接收到消息了
            break;
        }
        else if(type==2){
            //处理获取客户端名称
            tString = raw.getContent();
            //cout<<"Current client name is:"<<tString<<endl;
            cout<<tString<<endl;
            releWaitState();
            break;
        }
        else if(type==3){
            //处理获取客户端列表:编号、IP地址、端口
            clientList st;
            st = raw.AnalyzeList();
            no = st.no;
            ip = st.ip;
            port_num = st.port_num;
            cout << "Current clientList is"<<endl;
            cout <<raw.getContent() <<endl;
            releWaitState();
            break;
        }
        else if(type==5){
            //处理获取客户端
            tString = raw.getContent();
            cout<< "Receive sending reply!content is "<<tString <<endl;
            releWaitState();
            break;
        }
        else if(type==0){
            cout<< "Connect successfully!"<<endl;
            break;
        }

    }
    cout << "goend";
    //pthread_exit((void*)2);
    return args;
}



int main()
{

    WORD	wVersionRequested;
    WSADATA wsaData;//建立一个wsdata结构
    int	ret,iLen;
    wVersionRequested =	MAKEWORD(2,2);//create 16bit data
    int op,myRet;
    pthread_t rsvP;

//(1)Load WinSock
    ret	=WSAStartup(wVersionRequested,&wsaData);	//load win socket
    if(ret!=0)
    {
        cout<<"Load WinSock Failed!";
        return -1;
    }
//confirm version 2.2 is supported,查看版本信息
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        printf("Invalid Winsock version!\n");
        return 0;
    }

//(2)create socket ：声明为全局变量
    clientSocket = socket(AF_INET,SOCK_STREAM,0);
//domain:AF_INET:ipv4协议
//SOCK_STREAM:Tcp连接，提供序列化的、可靠的、双向连接的字节流。支持带外数据传输
//最后TPPROTO_TCP是默认协议，但是最好不要省略

    if(clientSocket == INVALID_SOCKET){
        cout<<"socket() fail:"<<WSAGetLastError()<<endl;
        return -2;
    }
    while(true){
        //output menu
        cout << "Please choose your operation in the following menu:"<<endl;
        cout << "(1) Connect"<<endl;
        fflush(stdin);
        if(alreadyConnected){
            cout << "(2) Close \n";
			cout << "(3) Request time" << endl;
			cout << "(4) Request name" << endl;
			cout << "(5) Request client list" << endl;
			cout << "(6) Send message" << endl;
        }
        cout << "(7) Exit" << endl;
        cout << "please enter your choice:"<<endl;
		cout << ">>";
        cin >> op;
        if(op<1||op>7){
            cout << "Invalid operation!!! please try again!"<<endl;
            continue;
        }
        if(op==1){
            myRet=connect2Server();
            if(myRet<0){
                continue;
            }
        }
        else if(op==2){
            closeClient();
        }
        else if(op==3||op==4||op==5){
            for(int i=0;i<1;i++){
                cout<<"try send"<<i<<" times"<<endl;
                request2Server(op);
            }
            
        }
        else if(op==6){
            send2Server();
        }
        else if(op==7){
            if(alreadyConnected){
                closeClient();
            }
            break;//退出循环
        }

    }
    return 0;

}