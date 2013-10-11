#ifndef _GAMESERVER_H
#define _GAMESERVER_H

class GameServer{
public:
	GameServer();
	virtual ~GameServer();

	int StartServer();
	bool TerminateServer();

	bool SendPacket(unsigned char *pPacket,unsigned long size);
	void WaitForClient();
	unsigned char ReceivePacketID();
private:

	bool IsInitialized;
	SOCKET g_GS;
	SOCKET g_Player;
	bool IsConnected;
};


#endif