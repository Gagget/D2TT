#include "stdafx.h"

void __declspec(naked) SendPacketIntercept_STUB() //Forgot where I got this code from :x
{
   __asm
   {
      pushad;

      mov ecx, [esp + 0x20 + 0xC];
      mov edx, [esp + 0x20 + 0x4];
      call _SendPacket;
      test eax, eax;

      popad;
      jnz OldCode;

      mov [esp + 0x4], 0;

OldCode:
      jmp D2NET_SendPacket;
   }
}

VOID __declspec(naked) D2MCP_PacketHandler_Interception(VOID) //Sheppo
{
	__asm
	{
		LEA ECX,DWORD PTR SS:[ESP+4]
		PUSHAD
		CALL MCP_PacketHandler
		CMP EAX, 0
		POPAD
		JE Block
		CALL EAX
Block:
		RETN
	}
}

VOID __declspec(naked) __fastcall D2CLIENT_DrawRectFrameSTUB(RECT* pRect) //from mMap I think
{
	__asm{
		mov eax, ecx;
		jmp D2CLIENT_DrawRectFrame
	}
}

void __stdcall TerminateLobby()
{
	D2WIN_TerminateControl(&control_list);
	D2WIN_TerminateControl(&control_image);
	D2WIN_TerminateControl(&control_backbutton);
	D2WIN_TerminateControl(&control_emulatebutton);

	D2MULTI_TerminateLobby_I();
	b_BrowseActivate = false;
	SelectedFile = 0;
}