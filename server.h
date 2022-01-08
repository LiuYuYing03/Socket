#ifndef SERVER_H
#define SERVER_H

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include "myMsg.h"
#include "myMsg.cpp"


// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8000"
#define CLIENT_MAX_NUM 32

struct Client
{
    std::string ip_addr;
    int ip_port;
    SOCKET client_socket;
    int index;
    int clientID;
};
int hasClient[CLIENT_MAX_NUM];
Client ClientList[CLIENT_MAX_NUM];

pthread_mutex_t client_mutex;
int ClientNum=0;


int InsertClient(Client newClient)
{
    while(ClientNum>CLIENT_MAX_NUM);
    pthread_mutex_lock(&client_mutex);
    ClientNum++;
    int insert_index=-1;
    for(int i=0;i<CLIENT_MAX_NUM;i++)
        if(hasClient[i]==0){
            insert_index=i;
            break;
        }
    hasClient[insert_index]=1;
    ClientList[insert_index]=newClient;
    pthread_mutex_unlock(&client_mutex);
    return insert_index;
}
void DeleteClient(Client oldClient)
{
    pthread_mutex_lock(&client_mutex);
    ClientNum--;
    hasClient[oldClient.index]=0;
    pthread_mutex_unlock(&client_mutex);
}
std:: string getLocalTime()
{
    time_t t = time(nullptr);
    char buf[128] = {0};
    strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));
    return buf;
}
std::string getClientList()
{
    char buf[DEFAULT_BUFLEN];
    std::string res;
    for(int i=0;i<CLIENT_MAX_NUM;i++)
    if(hasClient[i]==1){
        sprintf(buf,"ip=%s,port=%d,clientID=%d\n",ClientList[i].ip_addr,ClientList[i].ip_port,ClientList[i].clientID);
        res=res+std::string(buf);
    }
    return res;
}
std:: string ProcessRequest(std::string request,Client newClient)
{
    myMessage mes(request);
    mes.AnalyzeMsg();
    myMessage reply;
    switch (mes.getMsgType())
    {
        case 0:
            break;
        case 1:
            reply.setContent( getLocalTime() );
            reply.setType(1);
            break;
        case 2:
            reply.setContent("Server Name\n");
            reply.setType(2);
            break;
        case 3:
            reply.setContent( getClientList() );
            reply.setType(3);
            break;
        case 4:
            std::cout << "The client's connection closing..." << std::endl;
            std::cout << "ip=" << newClient.ip_addr << ":" << newClient.ip_port << std::endl;
            DeleteClient(newClient);
            reply.setContent("Disconnect....\n");
            reply.setType(4);
            break;
        default:
            break;
    }
    reply.setClientID(newClient.clientID);
    reply.Encapsulation(reply.getMsgType(), reply.getContent(), reply.getClientID() );
    return reply.getMsg();
    //send(newClient.client_socket, reply.getMsg().c_str(), reply.getMsg().size(), 0);
}
void* ThreadRun(void* arg)
{
    pthread_detach(pthread_self()); 
    
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    Client newClient=*(struct Client*)arg;
    int iResult;
    int iSendResult;

    std::string reply;
    while(true){
        iResult = recv(newClient.client_socket, recvbuf, recvbuflen, 0);
        
        if(iResult>0){
            std::string request=recvbuf;
            std::cout<<request<<std::endl;
            reply=ProcessRequest(request, newClient);
            iSendResult = send(newClient.client_socket, reply.c_str(), reply.length(), 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                break ;
            }
        }
        else if(iResult==0){
            std::cout<<"The client's connection closing..."<<std::endl;
            std::cout << "ip=" << newClient.ip_addr << ":" << newClient.ip_port << std::endl;
            DeleteClient(newClient);
            break;
        }
        else{
            printf("recv failed with error: %d\n", WSAGetLastError());
            break ;
        }
    }
    // shutdown the connection since we're done
    iResult = shutdown(newClient.client_socket, SD_SEND);
    if (iResult == SOCKET_ERROR){
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    }
    closesocket(newClient.client_socket);
    WSACleanup();

    pthread_exit(NULL);
    return NULL;
}

#endif // SERVER_H