#ifndef _HANDLER_H
#define _HANDLER_H

extern bool b_BrowseActivate;
extern sFile *g_Files;
extern sFile *SelectedFile;

unsigned long __fastcall Decompress_Packet( unsigned char *dest,unsigned long memory_size,unsigned char *source, unsigned long src_size );
bool __fastcall _SendPacket(BYTE* aPacket, DWORD aLen);
bool __fastcall MCP_PacketHandler(BYTE* pPacket);
void __stdcall RealmSendSTUB(SOCKET s,const char* buf,int len,int flags);
void __stdcall ChatSendSTUB(SOCKET s,const char* buf,int len,int flags);

void __stdcall LobbyDrawing();
void __stdcall EmuDrawing();

bool __fastcall BrowseCallback(unsigned long arg,unsigned long arg2,unsigned long arg3);
unsigned long __stdcall MainActivation(void *lp);

#endif