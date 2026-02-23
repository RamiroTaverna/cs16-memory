#include <Windows.h>
#include "KG_VARIABLES_GLOBALES.h"

// Variable definitions (allocated storage)
char g_acTitle[255];
HANDLE hp;
HWND hwnd;
HWND g_hwnd;
int g_nFound;
DWORD pid;

DWORD CLIENT;
DWORD HW;
DWORD HL;

DWORD KG_SPEED = 0x11D2E4;
float kg_speed;

DWORD MENU_ON=0x12126C;
DWORD MENU_TEXTO=0x12D2F0;
int prender_menu=1;
int apagar_menu=0;
char kg_menu_p1[]="\\rKeyGen2009 \\wTutorial\n" \
				  "\\ySpeed: ";
char kg_menu_final[255];

DWORD KG3D_CAM = 0x167A1C;
float kg3d_on=1;
float kg3d_off=0;

DWORD FL_ONGROUND = 0x11FDAB4;
int ground; 

DWORD KG_FULLBRIGHT=0x176AB4;
float kg_fullbright_on=1;
const float PI = 3.14159265f;

// Direcciones Globales
DWORD MI_POSS_3D = 0x5EF748;
DWORD MOUSE = 0x19E10C4;
DWORD MI_TEAM = 0x5F73E4;

// Variables Globales
float kg_fullbright_off=0;
float mi_poss_3d[3];
float mi_mira[3];
int mi_team;

AimbotConfig Config;
KeyGen2009 jugadores[32];
