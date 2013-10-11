#include "Structs.h"

//credits: d2bs for macros
#ifdef _DEFINE_VARS

static char *wDlls[] = {"D2Client.DLL", "D2Common.DLL", "D2Gfx.DLL", "D2Lang.DLL", 
			       "D2Win.DLL", "D2Net.DLL", "D2Game.DLL", "D2Launch.DLL", "Fog.DLL", "BNClient.DLL",
					"Storm.DLL", "D2Cmp.DLL", "D2Multi.DLL", "D2MCPCLIENT.DLL", "D2GDI.DLL"};

enum {DLLNO_D2CLIENT, DLLNO_D2COMMON, DLLNO_D2GFX, DLLNO_D2LANG, DLLNO_D2WIN, DLLNO_D2NET, DLLNO_D2GAME, DLLNO_D2LAUNCH, DLLNO_FOG, DLLNO_BNCLIENT, DLLNO_STORM, DLLNO_D2CMP, DLLNO_D2MULTI, DLLNO_D2MCPCLIENT,DLLNO_D2GDI};



#define FUNCPTR(dll, name, callingret, args, address) \
	__declspec(naked) callingret dll##_##name##args \
	{ \
		static DWORD f##dll##_##name = NULL; \
		if(f##dll##_##name == NULL) \
		{ \
		__asm { pushad } \
			f##dll##_##name = GetDllOffset(wDlls[DLLNO##_##dll], address); \
		__asm { popad } \
		} \
		__asm jmp [f##dll##_##name] \
	}

#define ASMPTR(dll, name, address) \
	DWORD* Asm_##dll##_##name##(VOID) \
	{ \
		static DWORD f##Asm_##dll##_##name = NULL; \
		if(f##Asm_##dll##_##name## == NULL) \
		{ \
			f##Asm_##dll##_##name## = GetDllOffset(wDlls[DLLNO##_##dll], address); \
		} \
		return &##f##Asm_##dll##_##name; \
	} 

#define VARPTR(dll, name, type, address) \
	type** Var_##dll##_##name##(VOID) \
	{ \
		static DWORD f##Var_##dll##_##name = NULL; \
		if(f##Var_##dll##_##name## == NULL) \
		{ \
			f##Var_##dll##_##name## = GetDllOffset(wDlls[DLLNO##_##dll], address); \
		} \
		return (type**)&##f##Var_##dll##_##name; \
	} 

#else
#define FUNCPTR(dll, name, callingret, args, address) extern callingret dll##_##name##args;
#define ASMPTR(dll, name, address) extern DWORD* Asm_##dll##_##name##(VOID); static DWORD dll##_##name = *Asm_##dll##_##name##();
#define VARPTR(dll, name, type, address) extern type** Var_##dll##_##name##(VOID); static type* p##_##dll##_##name = (type*)*Var_##dll##_##name##();
#endif

FUNCPTR(D2CLIENT, Print, void __stdcall, (wchar_t *wMessage, int nColor), 0x75EB0) 
FUNCPTR(D2CLIENT, SetUIVar, unsigned long __fastcall, (unsigned long varno, unsigned long howset, unsigned long unknown1), 0x1C190)
FUNCPTR(D2CLIENT, old_drawing_code, void __stdcall, ( ) , 0x6E670)
FUNCPTR(D2CLIENT, DrawRectFrame, void __fastcall, (unsigned long Rect), 0x17D10)
FUNCPTR(D2CLIENT, ExitGame, void __fastcall, (void), 0x43870)

VARPTR(D2CLIENT, PlayerUnit, UnitAny *, 0x11D050)
VARPTR(D2CLIENT, ui_table, unsigned long *, 0x11C890)

FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwTrans), 0x8210)

FUNCPTR(D2NET, SendPacket, void __stdcall, (unsigned long aLen, unsigned long unk, unsigned char *Packet), 0x6F20)

FUNCPTR(D2WIN, CreateButton,Control * __fastcall, (unsigned long xPos,unsigned long yPos,unsigned long width,unsigned long height, unsigned long cell,bool (__fastcall *pCallback)(unsigned long arg,unsigned long arg2,unsigned long arg3),unsigned long Unk1,unsigned long Unk2, unsigned long clickbox,unsigned long Unk4,unsigned long Unk5), -10192)
FUNCPTR(D2WIN, CreateList,Control * __fastcall, ( unsigned long xPos,unsigned long yPos,unsigned long width,unsigned long height,unsigned long unk1,unsigned long unk2, unsigned long cell,bool (__fastcall *pCallback)(unsigned long arg,unsigned long arg2,unsigned long arg3),unsigned long type,ControlFont *cp, ListScroll *ls ), -10151)
FUNCPTR(D2WIN, AddListText, void __fastcall, (Control *pCtrl, char *text) , -10135)
FUNCPTR(D2WIN, AddListChildText, void __fastcall, (Control *pCtrl, char *text , unsigned long textposition, unsigned long childid) , -10035)
FUNCPTR(D2WIN, SetListChilds, void __fastcall, (Control *pCtrl, unsigned long childs) , -10176)
FUNCPTR(D2WIN, SetListChildTextDistance, void __fastcall, (Control *pCtrl, unsigned long child,unsigned long width) , -10106)
FUNCPTR(D2WIN, SetListPosition, void __fastcall, (Control *pCtrl, unsigned long pos) , -10068)
FUNCPTR(D2WIN, GetCurrentListPosition, unsigned long __fastcall ,( Control *pCtrl ), -10122)
FUNCPTR(D2WIN, GetCurrentListText, wchar_t * __fastcall, (Control *pCtrl) , -10082)
FUNCPTR(D2WIN, TerminateControl, void __stdcall, (Control **pCtrl), -10123)
FUNCPTR(D2WIN, CreateControl, Control * __stdcall, (unsigned long *pArgs),-10164)
FUNCPTR(D2WIN, DestroyButton, void __fastcall, (Control *pCtrl), -10158)
FUNCPTR(D2WIN, LoadCellFile, unsigned long __fastcall, (char *Path,unsigned long unk), -10023)
FUNCPTR(D2WIN, SetTextSize, unsigned long __fastcall, (unsigned long Size), -10047)
FUNCPTR(D2WIN, DrawText, void __fastcall, (const wchar_t *wStr, int xPos, int yPos, unsigned long dwColor, unsigned long dwUnk), 0x13B30)
FUNCPTR(D2WIN, MouseSprite, void __stdcall, () , 0xE0C0)
FUNCPTR(D2WIN, GetFontChar, Fontx* __fastcall,(int Char),0x12D60)

VARPTR(D2WIN, FirstControl, Control *, 0x8DB34)
VARPTR(D2WIN, LobbyUnk,unsigned long, 0x8DB28)

VARPTR(D2MULTI,Background,unsigned long *,0x1B020)
VARPTR(D2MULTI,BackButton,unsigned long *,0x1AFF0)
ASMPTR(D2MULTI,JoinGame_I,0x10D90)
ASMPTR(D2MULTI,JoinGame_II,0x83F0)
//ASMPTR(D2MULTI,WaitBox, 0xE8C0)

FUNCPTR(D2MULTI, TerminateLobby_I, void __stdcall, ( ), 0x8AB0)
FUNCPTR(D2MULTI, TerminateLobby, void __stdcall, ( ) , 0xFBB0)

FUNCPTR(D2MCPCLIENT, RecvMCP, void __fastcall, (void *pPacket,unsigned long nSize), 0x7590) // Packets must have the correct request id as there's a check inside this function, otherwise it ignores the call.
VARPTR(D2MCPCLIENT, RequestId, unsigned short, 0xAEBC)

FUNCPTR(FOG, CompressPacket, unsigned long __fastcall, ( unsigned char *dest,unsigned long memory_size,unsigned char *source, unsigned long src_size) , -10223)
FUNCPTR(FOG, DecompressPacket, unsigned long __fastcall, ( unsigned char *dest,unsigned long memory_size,unsigned char *source, unsigned long src_size) , -10224)

FUNCPTR(STORM, ReadRegistryString,void __stdcall,(char *Directory,char *File,unsigned long Unk /* 0x00 */,char *Buffer,int nSize),-422)
FUNCPTR(STORM, ReadRegistryInt,void __stdcall,(char *Directory,char *File,unsigned long Unk /* 0x00 */,unsigned long *Buffer),-423)
FUNCPTR(STORM, SetRegistryString,void __stdcall,(char *Directory,char* File,unsigned long Unk /* 0x00 */,char *NewString),-425)
FUNCPTR(STORM, SetRegistryInt,void __stdcall,(char *Directory,char *File,unsigned long Unk /* 0x00 */,int Newstate),-426)

#define DrawMRect(x)                        (D2CLIENT_DrawRectFrameSTUB(x))

#undef FUNCPTR
#undef VARPTR
#undef ASMPTR