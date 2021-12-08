#include <winsock2.h>
#include <iostream>
 
#include <string.h>
using namespace std;
 
#pragma comment(lib, "ws2_32.lib")			//add ws2_32.lib
 
 
const int DEFAULT_PORT = 8000;
int main(int argc,char* argv[])
{
 
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err,iLen;
	wVersionRequested	=	MAKEWORD(2,2);//create 16bit data
//(1)Load WinSock
	err	=	WSAStartup(wVersionRequested,&wsaData);	//load win socket
	if(err!=0)
	{
		cout<<"Load WinSock Failed!";
		return -1;
	}
//(2)create socket
	SOCKET sockSrv = socket(AF_INET,SOCK_STREAM,0);
	if(sockSrv == INVALID_SOCKET){
		cout<<"socket() fail:"<<WSAGetLastError()<<endl;
		return -2;
	}
//(3)server IP
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);//Auto IP, byte sequence change
	addrSrv.sin_port = htons(DEFAULT_PORT);
//(4)bind
	err = bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if(err!=0)
	{
 
		cout<<"bind()fail"<<WSAGetLastError()<<endl;
		return -3;
	}
	//
//(5)listen
	err = listen(sockSrv,5);
	if(err!=0)
	{
 
		cout<<"listen()fail"<<WSAGetLastError()<<endl;
		return -4;
	}
	cout<<"Server waitting...:"<<endl;
//(6)client ip
	SOCKADDR_IN addrClt;
	int len = sizeof(SOCKADDR);
 
	while(1)
	{
//(7)accept
		SOCKET sockConn = accept(sockSrv,(SOCKADDR*)&addrClt,&len);
		char sendBuf[1024],hostname[100];
		if(gethostname(hostname,100)!=0)	//get host name
			strcpy(hostname,"None");
		sprintf(sendBuf,"Welecome %s connected to %s!",inet_ntoa(addrClt.sin_addr),hostname);
//(8)send recv
		err = send(sockConn,sendBuf,strlen(sendBuf)+1,0);
 
		char recvBuf[1024]="\0";
		iLen = recv(sockConn,recvBuf,1024,0);
 
		recvBuf[iLen]='\0';
		cout <<recvBuf<<endl;
//(9)close connected sock
		closesocket(sockConn);
	}
//(10)close server sock
	closesocket(sockSrv);
//(11)clean up winsock
	WSACleanup();
	return 0;
}
