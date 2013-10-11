#include "stdafx.h"

HANDLE HGS = NULL;

bool b_BrowseActivate = false;
bool b_EmulateGS = false;
bool b_Hook = false;
bool b_Buttons = false;
bool b_Record = false;
bool b_ValidGame = false;
unsigned long b_temp_ui[38];

char szGame[16];

int Files = 0;

sFile *SelectedFile = 0;
sFile *EnumFile = 0;
sFile *g_Files = 0;

GameServer GS;

unsigned long GameTick = 0;

//Decompression.. This function handles all incoming packets from the real GameServer, stores the compressed packets and decompresses the packets for the client
unsigned long __fastcall Decompress_Packet( unsigned char *dest,unsigned long memory_size,unsigned char *source, unsigned long src_size )
{
	int siz = src_size;
	int header = ( src_size > 0xEF )? 2:1;

	unsigned char *opacket = new unsigned char [ src_size + header ];

	if( !opacket )
	{
		MessageBoxA(0,"Error allocating memory...\nProgram termination","Error",0);
		TerminateProcess(GetCurrentProcess(),0);
	}

	memcpy( opacket,source - header,src_size + header );

	unsigned long ret = FOG_DecompressPacket( dest,memory_size,source,src_size );

	// List of Server Packets 
	// http://www.blizzhackers.cc/viewtopic.php?f=182&t=444264

	switch( dest[0] ) {
		case 0x00 : // Game Loading
		case 0x02 : // Load Successful
		case 0x05 : // Unload Complete
		case 0x06 : // Game Exit Sucessful
		case 0x8f : // Pong
		case 0xae : // Warden Request
		case 0xaf : // Connection Info
		case 0xB0 : // Game Connection Terminated
			break;
		default : AddLog( opacket,siz + header,true );
	}

	delete[] opacket;
	return ret;
}

// Gameinfo storage, validation AND faking some movements of the actual player
// this means saving some client packets as server packets (
bool __fastcall _SendPacket( BYTE* aPacket, DWORD aLen ) 
{
	if( aPacket[0] == 0x6B ){ // Join Game
		unsigned long **ptr = &(*p_D2CLIENT_ui_table);

		for( unsigned char n = 1; n < 38; n++ )
			b_temp_ui[n] = (unsigned long)ptr[n];

		b_ValidGame = true;
		return true;
	}

	if( aPacket[0] == 0x68 ) // Game Join Request
	{
		g_Info = new LInfo;
		strcpy( g_Info->szGame,szGame );
		strcpy( g_Info->szName,(char*)&aPacket[21] );
		g_Info->nTick = GetTickCount();
		return true;
	}

	if(    aPacket[0] == 0x06 // Left Skill on unit
		|| aPacket[0] == 0x07 // Shift Left Skill on unit
		|| aPacket[0] == 0x09 // Left Skill on unit (hold)
		|| aPacket[0] == 0x0a // Shift Left Skill on unit (hold)
		|| aPacket[0] == 0x0d // Right skill unit
		|| aPacket[0] == 0x0e // Shift right skill unit
		|| aPacket[0] == 0x10 // Right skill unit (hold)
		|| aPacket[0] == 0x11 // Shift Right skill unit (hold)
	){
		bool right = true;
		if( aPacket[0] < 0x0b )
			right = false;
		RecvCastOnUnit( GetCurrentSkill( right ),*(unsigned long*)&aPacket[1],*(unsigned long*)&aPacket[5] );
		return true;
	}

	if(    aPacket[0] == 0x05 // Shift Left Click Skill
		|| aPacket[0] == 0x08 // Shift Left Skill (hold)
		|| aPacket[0] == 0x0c // Right Skill
		|| aPacket[0] == 0x0f // Right Skill (hold)
	){
		bool right = true;
		if( aPacket[0] < 0x09 )
			right = false;
		RecvCastOnPos( GetCurrentSkill( right ),*(unsigned short*)&aPacket[1],*(unsigned short*)&aPacket[3] );
		return true;
	}
	
	if(    aPacket[0] == 0x01 // Walk
		|| aPacket[0] == 0x03 // Run
	){
		bool run = true;
		if( aPacket[0] == 0x01 )
			run = false;
		RecvWalk( run,*(unsigned short*)&aPacket[1],*(unsigned short*)&aPacket[3] );
		return true;
	}
	
	if(    aPacket[0] == 0x02 // Walk to entity
		|| aPacket[0] == 0x04 // Run to entity
	){
		bool run = true;
		if( aPacket[0] == 0x02 )
			run = false;
		RecvWalkToUnit( run,(unsigned char)(*(unsigned long*)&aPacket[1]),*(unsigned long*)&aPacket[5] );
		return true;
	}

	return true;
}

//Check if the requested game got successfully received + remove hooks and delete logging info if necessary
bool __fastcall MCP_PacketHandler( BYTE* pPacket )
{
	if( pPacket[0] == 0x04 ) // Load Complete
	{
		if( *(unsigned long*)&pPacket[15] == 0x00 )
			if( b_Record && !b_EmulateGS ){
				delete g_Info;
				g_Info = NULL;
				TerminateLogs();
				pRec.Install();
				pEmu.Remove();
			}
	}
	return true;
}
//To avoid any send calls to battle.net while joining our fake game. + store the gamename for saving purpose
void __stdcall RealmSendSTUB( SOCKET s,const char* buf,int len,int flags )
{
	if( buf[2] == 0x04 )
		strcpy( szGame,(char*)&buf[5] );

	if( !b_EmulateGS )
		send( s,buf,len,0 );
}

//To avoid any send calls to battle.net while joining our fake game.
void __stdcall ChatSendSTUB( SOCKET s,const char* buf,int len,int flags )
{
	if( !b_EmulateGS )
		send( s,buf,len,0 );
}

//Channel drawing...
void __stdcall LobbyDrawing()
{
	if( *p_D2WIN_LobbyUnk ){
		Text( 144,20,3,6,"D2TT 0.1 by Gnarmock" );
		Text( 164,66,4,1,"Record gameplay?" );

		Text( 526,27,4,1,"Browse Files" );

		if( b_BrowseActivate ){

			if( SelectedFile ){
				Text( 425,120,4,0,"Selected file size: %d KB ( %d MB )",SelectedFile->Size / 1000,SelectedFile->Size / 1000000 );

				Text( 425,110,4,0,"GameTime: %f seconds",(float)SelectedFile->GameTime * 0.001f );
			}

			Text( 425,130,4,0,"Files: %d",Files );

			int size = 0;
			for( sFile *pf = g_Files; pf; pf = pf->pNext )
				size += pf->Size;

			Text( 425,140,4,0,"Total size: %d KB ( %d MB )",size / 1000,size / 1000000 );
			Text( 430,165,4,0,"CharName" );
			Text( 430+100,165,4,0,"GameName" );
			Text( 430+200,165,4,0,"Date\\Time" );
		}
	}
	D2WIN_MouseSprite();
}

void __stdcall EmuDrawing()
{
	if(EnumFile){
		float second = GetTickCount() - GameTick;
		float relative = second / (float)EnumFile->GameTime;

		if(relative > 1.0f)
			relative = 1.0f;

		DrawStatusBar(relative,400,530,280,10);
	}

	D2CLIENT_old_drawing_code();
}

//Click box registry stuff, so it will remember the users last choice.
bool __fastcall RecordCallback( unsigned long arg,unsigned long arg2,unsigned long arg3 )
{
	unsigned long bTrue;
	STORM_ReadRegistryInt( "Diablo II","D2TT RC",0x00,&bTrue );

	if(!bTrue)
		STORM_SetRegistryInt( "Diablo II","D2TT RC",0x00,0x01 );
	else
		STORM_SetRegistryInt( "Diablo II","D2TT RC",0x00,0x00 );

	b_Record = !bTrue;

	return true;
}

unsigned long __stdcall GameServerThread( void *param );

bool __fastcall EmulateButtonCallback( unsigned long arg,unsigned long arg2,unsigned long arg3 )
{
	if( !SelectedFile )
		return false;

	b_EmulateGS = true;
	pRec.Remove();
	pEmu.Install();

	EnumFile = SelectedFile;

	JoinGame( SelectedFile->szGame,"" ); //Prepare the client for the game joining sequence.

	if(HGS != NULL){
		TerminateThread(HGS,0);
		HGS = NULL;
	}

	HGS = CreateThread( 0,0,GameServerThread,0,0,0 ); //Create the GS thread.

	unsigned char aPacket[13];
	memset( aPacket,0x00,13 );

	aPacket[0] = 0x04;
	*(unsigned short*)&aPacket[1] = (*p_D2MCPCLIENT_RequestId);
	*(unsigned long*)&aPacket[7] = inet_addr( "127.0.0.1" );
	D2MCPCLIENT_RecvMCP( aPacket,13 ); //With this packet we're telling our client, that it should connect to the localhost. ||REQUEST ID MUST MATCH||

	return true;
}

bool __fastcall BrowseCallback2( unsigned long arg,unsigned long arg2,unsigned long arg3 )
{
	static int lastselection = -1; //Doubleclick check
	static unsigned long Tick = 0; //Doubleclick time tick.

	int cc = 0;
	for( sFile *sf = g_Files; sf; sf = sf->pNext ){
		if( cc == D2WIN_GetCurrentListPosition( control_list ) ){
			SelectedFile = sf;
			break;
		}
		cc++;
	}

	if( lastselection == cc && GetTickCount() - Tick < 200 ) // Doubleclick check
		EmulateButtonCallback(0,0,0); //Lets join some fake game :d
	else
		lastselection = cc;

	Tick = GetTickCount();

	return true;
}

bool __fastcall BrowseCallback( unsigned long arg,unsigned long arg2,unsigned long arg3 )
{
	D2MULTI_TerminateLobby(); //Terminate the upper right controls
	b_BrowseActivate = !b_BrowseActivate;

	if( !b_BrowseActivate )
		return true;

	Files = InitFiles(); //Intialize the files and load the headers.

	static unsigned long CF = D2WIN_LoadCellFile("DATA\\GLOBAL\\UI\\FRONTEND\\joingamescrollbars",0);
	static unsigned long CF2 = D2WIN_LoadCellFile("DATA\\GLOBAL\\UI\\BIGMENU\\GameButtonBlank",0);
		
	ControlFont cf;		cf.FontSize = 4;		cf.Unk = 0;		cf.FontSize2 = 3;
	ListScroll ls;		ls.distanceUp = 238;	ls.xpos = 756;	ls.ypos = 391;

	control_image = D2WIN_CreateControl( (unsigned long*)p_D2MULTI_Background );
	control_backbutton = D2WIN_CreateControl( (unsigned long*)p_D2MULTI_BackButton );
	control_list = D2WIN_CreateList( 434,391,323,218,0,0,CF,BrowseCallback2,5,&cf,&ls );
	control_emulatebutton = D2WIN_CreateButton( 0x252,0x1b1,0xac,0x20,CF2,EmulateButtonCallback,0x0d,0x00,0x00,0x141f,0x00 );
	
	D2WIN_SetListChilds( control_list,3 );
	D2WIN_SetListChildTextDistance( control_list,1,100 );
	D2WIN_SetListChildTextDistance( control_list,2,200 );

	int cc = 0;
	for( sFile *sf = g_Files; sf; sf = sf->pNext ){

		D2WIN_AddListText( control_list,sf->szChar );
		D2WIN_AddListChildText( control_list,sf->szGame,cc,1 );
		D2WIN_AddListChildText( control_list,sf->szTime,cc,2 );

		cc++;
	}

	D2WIN_SetListPosition( control_list,0 );

	return true;
}

unsigned long __stdcall MainActivation( void *lp )
{
	Control *n1 = 0;
	Control *n2 = 0;

	while( true )
	{
		if( *p_D2WIN_LobbyUnk ){
			if( !pMain.GetHookState() && b_Hook ){
				pMain.Install();
				pEmu.Remove();

				if( b_ValidGame && b_Record )
					SaveReplay( g_Info,g_Log,g_Packets );

				b_ValidGame = false;
				b_Hook = false;
			}

			if( !b_Buttons )
			{
				static unsigned long CellMButton = D2WIN_LoadCellFile( "DATA\\GLOBAL\\UI\\FRONTEND\\mediumbuttonblank",0 );
				static unsigned long CellClickBox = D2WIN_LoadCellFile( "DATA\\GLOBAL\\UI\\BIGMENU\\joingameclickbox",0 );

				n1 = D2WIN_CreateButton( 526,66,128,35,CellMButton,BrowseCallback,0,0,0,0,0 );
				n2 = D2WIN_CreateButton( 150,66,100,16,CellClickBox,RecordCallback,0,0,2,0,0 );

				unsigned long bTrue;
				STORM_ReadRegistryInt( "Diablo II","D2TT RC",0x00,&bTrue );

				if(bTrue){
					n2->ButtonFlag_I = true;
					n2->ButtonFlag_II = true;
					b_Record = true;
				}
				else
					STORM_SetRegistryInt( "Diablo II","D2TT RC",0x00,0x00 );

				b_Buttons = true;
			}
		}
		else if(b_Buttons){
			D2WIN_DestroyButton( n1 );
			D2WIN_DestroyButton( n2 );
			pMain.Remove();
			b_Hook = true;
			b_Buttons = false;
		}
		else
		{
			if(b_ValidGame)
			{
				unsigned long **ptr = &(*p_D2CLIENT_ui_table);

				for( unsigned char n = 1; n < 38; n++ )
					if( b_temp_ui[n] != (unsigned long)ptr[n] ){
						unsigned char packet[4];

						packet[0] = 'N';		packet[1] = 'P';		packet[2] = n;			packet[3] = (b_temp_ui[n]) ? 0x01:0x00;
						AddLog(packet,4,true);

						b_temp_ui[n] = !b_temp_ui[n];
					}

			}
		}

		Sleep(40);
	}
	return 0;
}

unsigned long __stdcall GameServerThread( void *param )
{
	if(!EnumFile)
		return 0;

	TerminateLogs();

	FILE *fp = fopen( EnumFile->Path,"rb" );
	if(!fp)
		return 0;

	fseek( fp,56,SEEK_SET ); // (szChar)16 + (szGame)16 + (szTime)20 + (GameTime)4 = :D; Skipping most of the header.

	int packs = 0;
	fread( &packs,4,1,fp ); // read the packet count.
	
	for( int x = 0; x < packs; x++ ) // loop through all packets and create a new linkedlist.
	{
		Log *lg = new Log;
		fread( &lg->size,4,1,fp );

		lg->aPacket = (unsigned char*)malloc( lg->size );
		memset( lg->aPacket,0,lg->size );

		fread( &lg->Interval,4,1,fp );
		fread( lg->aPacket,lg->size,1,fp );
		lg->pNext = NULL;

		if( !g_Log )
			g_Log = lg;
		else
		{
			for( Log *l = g_Log; l; l = l->pNext )
				if( l->pNext == NULL )
				{
					l->pNext = lg;
					break;
				}
		}
	}
	fclose( fp );

	if(GS.StartServer() != 0)
		goto exit;

	GS.WaitForClient(); //Wait till someone connects to localhost:4000

	unsigned char pack[2] = {0xAF,0x01}; //Handshaking packet?
	GS.SendPacket( pack,2 ); //Non compressed.

	//Every packet from now on, needs to be compressed.
	if( GS.ReceivePacketID() == 0x68 )//Enter game information received?
	{
		GameTick = GetTickCount();
		GS.SendPacket( g_Log->aPacket,g_Log->size ); //Send back the very first logged packet aka 0x01 with its information

		GS.ReceivePacketID(); // wait for response

		pack[0] = 0x02;		pack[1] = 0x80;
		GS.SendPacket( pack,2 ); //Send 'OK' packet back

		pack[0] = 0x02;		pack[1] = 0x5C;
		GS.SendPacket( pack,2 ); //Send loading successful packet
	}
	if( GS.ReceivePacketID() == 0x6b ) //if the clients sends the loading done packet,flood the client with compressed packets and with the right interval between each one.
	{
		unsigned long tick = GetTickCount();
		for( Log *lg = g_Log->pNext ; lg; lg = lg->pNext )
		{
			while( GetTickCount() - tick < lg->Interval ) {Sleep(1);}
			tick = GetTickCount();

			if( lg->size == 4 && lg->aPacket[0] == 'N' && lg->aPacket[1] == 'P' )
				D2CLIENT_SetUIVar( lg->aPacket[2],lg->aPacket[3],1 );
			else
				GS.SendPacket(lg->aPacket ,lg->size);
/*
			if(    lg->aPacket[0] == 0x03 //  Load Act 
				|| lg->aPacket[0] == 0x09 // Assign Lvl Warp (Door)
			){
				GS.ReceivePacketID(); // wait for response
			}*/
		}
		Sleep(1000); D2CLIENT_ExitGame();
	}

	while( GS.ReceivePacketID() != 0x69 ) // Wait for Client Leave Game Packet
		{Sleep(1);}
	
	pack[0] = 0x05;		pack[1] = 0x5C;
	GS.SendPacket( pack,2 ); //Send Server unload complete packet
	pack[0] = 0x06;		pack[1] = 0x5C;
	GS.SendPacket( pack,2 ); //Send Server game exit successful packet

	
	EnumFile = 0;
	b_EmulateGS = false;
	TerminateLogs();


exit:

	GS.TerminateServer();
	TerminateThread( GetCurrentThread(),0 );

	return 0;
}