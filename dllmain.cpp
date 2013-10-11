/* 
	Diablo II recording tool by Gnarmock.

	Credits: 
			-- D2BS
			-- Redvex for header function.
*/

#include "stdafx.h"

char szPath[MAX_PATH];

PatchHook RecHook[] = {
   {PatchCall,GetDllOffset("D2Net.dll",0x6CC5),(unsigned long)Decompress_Packet,5 },
   {PatchJmp,GetDllOffset("D2Client.dll",0xD13C),(unsigned long)SendPacketIntercept_STUB, 6},
};

PatchHook MainHook[] = {
   {PatchCall,GetDllOffset("D2Mcpclient.dll", 0x6297),(unsigned long)(D2MCP_PacketHandler_Interception), 5},
   {PatchBytes,GetDllOffset("D2Multi.dll",0x1219C),0x90, 3},
   {PatchBytes,GetDllOffset("D2Win.dll",0xC847),0xeb, 1},
   {PatchBytes,GetDllOffset("D2Multi.dll",0x11DC9),0xeb, 1},
   {PatchCall,GetDllOffset("D2Win.dll",0xECB7),(unsigned long)LobbyDrawing, 5},
   {PatchCall,GetDllOffset("D2Multi.dll",0xFBB0),(unsigned long)TerminateLobby,5},
   {PatchCall,GetDllOffset("Fog.dll",0x10B65),(unsigned long)RealmSendSTUB,5},
   {PatchCall,GetDllOffset("Fog.dll",0x1A76D),(unsigned long)ChatSendSTUB,5},
};

PatchHook EmuHook[] = {
   {PatchCall,GetDllOffset("D2Client.dll",0x6F371),(unsigned long)EmuDrawing,5},
};

Offset pRec;
Offset pMain;
Offset pEmu;

HANDLE HMA;

BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved )
{
	if(ul_reason_for_call == 0x01)
	{
		GetModuleFileNameA((HMODULE)hModule,szPath,MAX_PATH);
	    PathRemoveFileSpecA(szPath);

		//Initialize the hooks.
		pRec.pHooks = RecHook;
		pRec.Hooks = ArraySize(RecHook);

		pMain.pHooks = MainHook;
		pMain.Hooks = ArraySize(MainHook);

		pEmu.pHooks = EmuHook;
		pEmu.Hooks = ArraySize(EmuHook);

		pMain.Install();

		HMA = CreateThread(0,0,MainActivation,0,0,0);

	}

	else if(ul_reason_for_call == 0x00)
	{
		TerminateThread(HMA,0);
	}

	return TRUE;
}

