#include "stdafx.h"

int InitFiles()
{
	sFile *bu = 0;

	for(sFile *pf = g_Files; pf; pf = bu)
	{
		bu = pf->pNext;
		delete[] pf;
	}
	g_Files = 0;

	CHAR path[MAX_PATH];
	strcpy(path,szPath);

	strcat((char*)path,"\\Records");
	char searchPath[MAX_PATH];
	strcpy_s(searchPath, sizeof(searchPath), path);
	PathAppendA(searchPath, "\\*.gpl");

	WIN32_FIND_DATAA data;
	int Count = 0;
	HANDLE search = FindFirstFileA(searchPath, &data);

	if (search != INVALID_HANDLE_VALUE)
	{
		do
		{
			sFile *pf = new sFile;

			char filePath[MAX_PATH];
			strcpy(filePath, path);
			PathAppendA(filePath, data.cFileName);

			strcpy(pf->Path,filePath);
			pf->Size = data.nFileSizeLow;
			pf->pNext = NULL;

			FILE *fp = fopen(pf->Path,"rb");
			if(fp){
				fread(pf->szChar,16,1,fp);
				fread(pf->szGame,16,1,fp);
				fread(pf->szTime,20,1,fp);
				fread(&pf->GameTime,4,1,fp);
				fclose(fp);
			}
			
			if(Count == 0)
				g_Files = pf;
			else
				for(sFile *file = g_Files; file; file = file->pNext)
					if(file->pNext == NULL)
					{
						file->pNext = pf;
						break;
					}

			
			Count++;

		}
		while (FindNextFileA(search, &data));

		FindClose(search);
	}

	return Count;
}

bool SaveReplay(LInfo *pInfo, Log *pLog, unsigned long Packets)
{
	if(!pInfo || !pInfo->szGame || !pInfo->szName
		|| !pLog || !pLog->aPacket)
		return false;

    SYSTEMTIME st;
    GetSystemTime(&st);
	
	char sz[MAX_PATH] = "";
	char szTime[20] = "";

	sprintf(szTime,"%.2d%.2d%.2d%.2d%.2d%.2d%.2d",st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
	sprintf(sz,"%s\\records\\%s.gpl",szPath,szTime);

	FILE *fp = fopen(sz,"wb");
	if(!fp)
		return false;

	//Write the header..
	fwrite(pInfo->szName,16,1,fp);
	fwrite(pInfo->szGame,16,1,fp);
	sprintf(szTime,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d",st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond);
	fwrite(szTime,20,1,fp);
	unsigned long GameTime = GetTickCount() - pInfo->nTick;
	fwrite(&GameTime,4,1,fp);

	fwrite(&Packets,4,1,fp);

	//Write the actual content.
	for(Log *lg = pLog; lg; lg = lg->pNext)
	{
		fwrite(&lg->size,4,1,fp);
		fwrite(&lg->Interval,4,1,fp);
		fwrite(lg->aPacket,lg->size,1,fp);
	}

	fclose(fp);

	TerminateLogs();

	return true;
}