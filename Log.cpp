#include "stdafx.h"

Log *g_Log = 0;
LInfo *g_Info = 0;
unsigned long g_Packets = 0;

void AddLog(unsigned char *pPacket,signed long size, bool IsRecv)
{
	static unsigned long nTick = GetTickCount();

	if(size > 3000)
		return;

	Log *lg = new Log;

	lg->aPacket = new unsigned char [ size ];
	memset(lg->aPacket,0x00,size);

	for(int x = 0; x < size; x++)
		lg->aPacket[x] = pPacket[x];

	lg->pRecv = IsRecv;
	lg->size = size;
	lg->Interval = (g_Log)? (GetTickCount() - nTick): NULL;
	lg->pNext = 0;

	if(!g_Log)
		g_Log = lg;
	else
	{
		for(Log *l = g_Log; l; l = l->pNext)
			if(l->pNext == NULL)
			{
				l->pNext = lg;
				break;
			}
	}

	g_Packets++;
	nTick = GetTickCount();
}

void TerminateLogs()
{
	if(!g_Log)
		return;

	g_Packets = 0;
	Log *ptr = 0;
	for(Log *l = g_Log; l; l = ptr)
	{
		ptr = l->pNext;
		delete[] l;
	}
	g_Log = NULL;
}