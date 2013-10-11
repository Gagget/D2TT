#ifndef _LOG_H
#define _LOG_H

struct Log{
	unsigned long size;
	unsigned char *aPacket;
	unsigned long Interval;
	unsigned char pRecv;
	Log *pNext;
};

struct LInfo{
	char szName[16];
	char szGame[16];
	unsigned long nTick;
};

extern Log *g_Log;
extern LInfo *g_Info;
extern unsigned long g_Packets;

void AddLog(unsigned char *pPacket,signed long size, bool IsRecv);
void TerminateLogs();
extern int LogSize;

#endif