#include "stdafx.h"

GameServer::GameServer(){
	IsInitialized = false;
	g_Player = SOCKET_ERROR;
	g_GS = NULL;
	IsConnected = false;

	WSADATA wsaData; 
	if(WSAStartup(MAKEWORD(2,0),&wsaData) != SOCKET_ERROR)
		IsInitialized = true;
}

GameServer::~GameServer(){
	IsInitialized = false;
	g_Player = NULL;
	g_GS = NULL;
	IsConnected = false;
}

int GameServer::StartServer()
{
	if(IsConnected)
		return 0;

	if(!IsInitialized)
		return 4;

	SOCKADDR_IN addr; 

	memset(&addr,0,sizeof(SOCKADDR_IN)); 
	addr.sin_family=AF_INET; 
	addr.sin_port=htons(4000); 
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	g_GS=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP); 

	if(g_GS==INVALID_SOCKET) { 
		MessageBoxA( 0,"Invalid socket.\n",0,0);
		return 1; 
	} 
 
	int rc = bind(g_GS,(SOCKADDR*)&addr,sizeof(SOCKADDR)); 
	if(rc == SOCKET_ERROR) {  
		MessageBoxA( 0,"Error bind on socket.\n",0,0);
		return 2; 
	}

	if (listen(g_GS, 1 ) == SOCKET_ERROR){
		MessageBoxA( 0,"Error listening on socket.\n",0,0);
		return 3;
	}

	IsConnected = true;

	return 0;
}

bool GameServer::TerminateServer()
{
	if(!IsInitialized)
		return false;
	
	closesocket(g_Player);
	g_Player = SOCKET_ERROR;
	
	return true;
}

bool GameServer::SendPacket(unsigned char *pPacket,unsigned long size)
{
	if(g_Player == SOCKET_ERROR)
		return false;

	send(g_Player,(const char*)pPacket,size,0);

	return true;
}

void GameServer::WaitForClient()
{
	sockaddr_in pdr;
	int pdrlen=sizeof(pdr);

	while ( g_Player == SOCKET_ERROR ) {
		g_Player = accept( g_GS,(sockaddr*)&pdr, &pdrlen );
	}
}

unsigned char GameServer::ReceivePacketID()
{
	if(g_Player == SOCKET_ERROR)
		return 0;

	static char buffer[1024] = "";
	recv(g_Player,buffer,1024,0);

	return buffer[0];
}