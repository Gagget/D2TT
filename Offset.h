#ifndef _OFFSET_H
#define _OFFSET_H


#define INST_INT3	0xCC
#define INST_CALL	0xE8
#define INST_NOP	0x90
#define INST_JMP	0xE9
#define INST_RET	0xC3

typedef struct PatchHook_t
{
	void (*pFunc)(DWORD, DWORD, DWORD);
	DWORD dwAddr;
	DWORD dwFunc;
	DWORD dwLen;
	BYTE *bOldCode;
} PatchHook;

DWORD GetDllOffset(int num);
DWORD GetDllOffset(char *DllName, int Offset);
void PatchBytes(DWORD dwAddr, DWORD dwValue, DWORD dwLen);
void PatchJmp(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
void PatchCall(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
void InterceptLocalCode(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen);
void FillBytes(void *pAddr, BYTE bFill, DWORD dwLen);
bool WriteBytes(void *pAddr, void *pData, DWORD dwLen);

class Offset{
public:
	
	Offset();
	virtual ~Offset();

	PatchHook *RetrievePatchHooks(PINT pBuffer);
	void Remove();
	void Install();

	PatchHook *pHooks;
	unsigned long Hooks;

	bool GetHookState() { return bInstalled; }
private:
	bool bInstalled;
};

extern Offset pRec;
extern Offset pMain;
extern Offset pEmu;

#endif