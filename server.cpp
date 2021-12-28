#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "server.h"
#include "myMsg.h"

int main(void)
{

    pthread_mutex_init(&client_mutex, NULL);

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;

//(1) Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
//(2) Resolve the server address and port
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0; //-------changed IPPROTO_TCP
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

//(3) Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

//(4) Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
//(5)
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Start the Server" << std::endl;
//(6) Accept a client socket
    while (true)
    {
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        SOCKET ClientSocket = INVALID_SOCKET;
        ClientSocket = accept(ListenSocket, (sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        std::cout<<"A client request connection"<<std::endl;

        Client newClient;
        newClient.ip_addr=inet_ntoa(client_addr.sin_addr);
        newClient.ip_port=ntohs(client_addr.sin_port);
        newClient.client_socket=ClientSocket;

        newClient.index = InsertClient(newClient);
        newClient.clientID = newClient.index;
        ClientList[newClient.index]=newClient;
        std::cout<<"ip="<<newClient.ip_addr<<":"<<newClient.ip_port<<std::endl;

        pthread_t thread;
        pthread_create(&ClientSocket, NULL, ThreadRun, &newClient);
    }

    // cleanup
    closesocket(ListenSocket);
//    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}