#ifndef _FILE_H
#define _FILE_H

extern char szPath[MAX_PATH];

struct sFile{
	char Path[MAX_PATH];
	char szGame[16];
	char szChar[16];
	char szTime[20];
	unsigned long GameTime;
	unsigned long Size;
	sFile *pNext;
};

int InitFiles();
bool SaveReplay(LInfo *pInfo, Log *pLog, unsigned long Packets);

#endif