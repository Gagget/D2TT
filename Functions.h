#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

extern Control *control_list,*control_image,*control_backbutton,*control_emulatebutton;

bool Text(signed int xpos,signed int ypos,unsigned long Color,unsigned long Size, char *szText, ...);
void TextCenter(signed long xpos,signed long ypos,unsigned long Color,unsigned long Size, char *szText, ...);
void AdjustAngle(float *pangle);

unsigned short GetCurrentSkill(bool bLeft);
void RecvCastOnUnit(unsigned char skill,unsigned char type,unsigned long ID);
void RecvCastOnPos(unsigned char skill,unsigned short xpos,unsigned short ypos);
void RecvWalk(bool run,unsigned short xpos,unsigned short ypos);
void RecvWalkToUnit(bool run,unsigned char type,unsigned long TargetId);

void __fastcall JoinGame(char *Gamename,char *password);
void DrawStatusBar(float relative,signed int xpos, signed int ypos, signed int x_r, signed int y_r);

#endif