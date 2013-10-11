#include "stdafx.h"

Control *control_list,*control_image,*control_backbutton,*control_emulatebutton;
unsigned char CompPacket[0x448]; //D2 uses this size 0x448

VOID MyMultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, INT cbMultiByte, LPWSTR lpWideCHARStr, int cchWideCHAR)
{
	MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCHARStr, cchWideCHAR);
	for(DWORD i = 0; i < wcslen(lpWideCHARStr); i++)
	{
		if(lpWideCHARStr[i] >= 0xFF && lpWideCHARStr[i + 1] == L'c')
		{
			if(lpWideCHARStr[i + 2] >= L'0' && lpWideCHARStr[i + 2] <= L'9')
			{
				lpWideCHARStr[i] = 0xFF;
			}
		}

	}
}
bool Text(signed int xpos,signed int ypos,unsigned long Color,unsigned long Size, char *szText, ...) //d2bs?
{
	WCHAR wTemp[0x400] = L"";
	CHAR szTemp[0x400] = "";
	DWORD dwOldSize;
	va_list Args;

	va_start(Args, szText);
	vsprintf(szTemp, szText, Args);
	va_end(Args);

	MyMultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szTemp, (INT)strlen(szTemp), wTemp, (INT)strlen(szTemp));

	dwOldSize = D2WIN_SetTextSize(Size);
	D2WIN_DrawText(wTemp, xpos, ypos, Color, -1);
	D2WIN_SetTextSize(dwOldSize);

	return true;
}

void TextCenter(signed long xpos,signed long ypos,unsigned long Color,unsigned long Size, char *szText, ...)
{
	char szTemp[0x400] = "";
	va_list Args;

	va_start(Args, szText);
	vsprintf(szTemp, szText, Args);
	va_end(Args);

	float magnitude = 0.0f;
	float ydiff = 0.0f;

	for(unsigned int n = 0; n < strlen(szText); n++)
	{
		Fontx *fx = D2WIN_GetFontChar(szText[n]);
		magnitude += fx->Width;
		ydiff = fx->Height;
	}

	xpos -= (signed long)(magnitude * 0.5);
	ypos += (signed long)(ydiff * 0.5);

	Text(xpos,ypos,Color,Size,szTemp);
}

int CreateHeader(int contentLength, void* header) //Redvex
{
	unsigned char* bytes = static_cast<unsigned char*>(header);
	int dataLength, headerLength;

	if (contentLength > 0xEF)
	{
		headerLength = 2;

		dataLength = contentLength + headerLength;
		dataLength |= 0xf000;

		bytes[0] = static_cast<unsigned char>(dataLength >> 8);
		bytes[1] = static_cast<unsigned char>(dataLength & 0xff);
	}

	else
	{
		headerLength = 1;

		dataLength = contentLength + headerLength;
		bytes[1] = static_cast<unsigned char>(dataLength);
	}

	return headerLength;
}

void AdjustAngle(float *pangle)
{
	if(*pangle < 0.0f)
		*pangle += 360.0f;
	else if(*pangle > 360.0f)
		*pangle -= 360.0f;
}
unsigned short GetCurrentSkill(bool bLeft) //d2bs?
{
	if(!bLeft)
		return *(*p_D2CLIENT_PlayerUnit)->pSkillInfo->pLeftSkill->pSkillId;
	else 
		return *(*p_D2CLIENT_PlayerUnit)->pSkillInfo->pRightSkill->pSkillId;
}
void RecvCastOnUnit(unsigned char skill,unsigned char type,unsigned long ID)
{
	unsigned char Packet[16];
	memset(Packet,0x00,16);

	Packet[0] = 0x4C;
	*(unsigned long*)&Packet[2] = (*p_D2CLIENT_PlayerUnit)->dwUnitId;
	Packet[6] = skill;
	Packet[8] = 0x02;
	Packet[9] = type;
	*(unsigned long*)&Packet[10] = ID;

	int size = FOG_CompressPacket(CompPacket+2,0x448,Packet,16);
	int header = CreateHeader(size,CompPacket);

	AddLog(CompPacket +( 2 - header ),size + header,true);
}
void RecvCastOnPos(unsigned char skill,unsigned short xpos,unsigned short ypos)
{
	unsigned char Packet[17];
	memset(Packet,0x00,17);

	Packet[0] = 0x4d;
	*(unsigned long*)&Packet[2] = (*p_D2CLIENT_PlayerUnit)->dwUnitId;
	Packet[6] = skill;
	Packet[10] = 0x02;
	*(unsigned short*)&Packet[11] = xpos;
	*(unsigned short*)&Packet[13] = ypos;

	int size = FOG_CompressPacket(CompPacket+2,0x448,Packet,17);
	int header = CreateHeader(size,CompPacket);

	AddLog(CompPacket +( 2 - header ),size + header,true);
}
void RecvWalk(bool run,unsigned short xpos,unsigned short ypos)
{
	unsigned char Packet[16];
	memset(&Packet[0], 0, sizeof(Packet));

	Packet[0] = 0x0f;
	*(unsigned long*)&Packet[2] = (*p_D2CLIENT_PlayerUnit)->dwUnitId;
	Packet[6] = (run)? 0x17:0x01;
	*(unsigned short*)&Packet[7] = xpos;
	*(unsigned short*)&Packet[9] = ypos;
	*(unsigned short*)&Packet[12] = (*p_D2CLIENT_PlayerUnit)->pPath->xPos;
	*(unsigned short*)&Packet[14] = (*p_D2CLIENT_PlayerUnit)->pPath->yPos;

	int size = FOG_CompressPacket(CompPacket+2,0x448,Packet,16);
	int header = CreateHeader(size,CompPacket);

	AddLog(CompPacket +( 2 - header ),size + header,true);
}
void RecvWalkToUnit(bool run,unsigned char type,unsigned long TargetId)
{
	unsigned char Packet[16];
	memset(&Packet[0], 0, sizeof(Packet));

	Packet[0] = 0x10;
	*(unsigned long*)&Packet[2] = (*p_D2CLIENT_PlayerUnit)->dwUnitId;
	Packet[6] = (run)? 0x18:0x00;
	Packet[7] = type;
	*(unsigned long*)&Packet[8] = TargetId;
	*(unsigned short*)&Packet[12] = (*p_D2CLIENT_PlayerUnit)->pPath->xPos;
	*(unsigned short*)&Packet[14] = (*p_D2CLIENT_PlayerUnit)->pPath->yPos;

	int size = FOG_CompressPacket(CompPacket+2,0x448,Packet,16);
	int header = CreateHeader(size,CompPacket);

	AddLog(CompPacket + ( 2 - header ),size + header,true);
}
void __declspec(naked) __fastcall JoinGame(char *Gamename,char *password)
{
	__asm{
		push ebx
		push edi

		push 0
		push 0
		lea ebx,dword ptr ss:[edx]
		lea edi,dword ptr ss:[ecx]
		call [D2MULTI_JoinGame_I]

		call [D2MULTI_JoinGame_II]

		//mov ecx,0x147b
		//call D2LANG_GetLocaleText // Please wait message...

		//push eax
		//xor ebx,ebx
		//call [D2MULTI_WaitBox] //not necessary for localhost game and the waitbox sometimes keeps alive after finishing the game.

		pop edi
		pop ebx
		retn
	}
}

void DrawStatusBar(float relative,signed int xpos, signed int ypos, signed int x_r, signed int y_r)
{
	RECT pRect;
	SetRect(&pRect,xpos - x_r,ypos - y_r,xpos + x_r,ypos + y_r);

	D2GFX_DrawRectangle(pRect.left+2, pRect.top, pRect.right, pRect.bottom, 0x00, 2);

	float sbp = (x_r * 2.0f) * relative;

	D2GFX_DrawRectangle(
		pRect.left + 2,
		pRect.top,
		pRect.left + sbp,
		pRect.bottom,
		0x0a,
		2);

	TextCenter(xpos,ypos,0,0,"%f%s",(relative * 100.0f),"%");

	DrawMRect(&pRect);
}