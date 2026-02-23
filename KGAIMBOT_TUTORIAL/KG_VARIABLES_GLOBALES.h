#pragma once
#include <Windows.h>

//VARIABLES GLOBALES para la busqueda de modulos!
extern char g_acTitle[255];
extern HANDLE hp;
extern HWND hwnd;
extern DWORD pid;

//Definiciones
#define sec1 Sleep(1);
#define borrar system("CLS");
#define VK_F2 0x3C0000

//DIRECCIONES DE MEMORIA
extern DWORD CLIENT;
extern DWORD HW;
extern DWORD HL;

		//Direccion del menu
extern DWORD KG_SPEED;				//CLIENT
extern float kg_speed;

extern DWORD MENU_ON;					//CLIENT
extern DWORD MENU_TEXTO;
extern int prender_menu;
extern int apagar_menu;
extern char kg_menu_p1[];
extern char kg_menu_final[255];


		//DIRECCION DE 3D HACK CAM
extern DWORD KG3D_CAM; //HW
extern float kg3d_on;
extern float kg3d_off;



//DIRECCION DEL JUGADOR TOCANDO EL SUELO
extern DWORD FL_ONGROUND; //OFFSET
extern int ground; 
	

//Direccion de r_fullbright
extern DWORD KG_FULLBRIGHT;// hw!
extern float kg_fullbright_on;
extern float kg_fullbright_off;
extern float mi_poss_3d[3];
extern float mi_mira[3];
extern int mi_team;
extern DWORD MI_POSS_3D;
extern DWORD MOUSE;
extern DWORD MI_TEAM;

// CONFIGURACION GLOBAL DEL AIMBOT
struct KeyGen2009 {
    DWORD E_NICK;
    DWORD E_MODEL;
    DWORD E_POSS;
    DWORD E_STATUS;

    char e_nick[255];
    char e_model[255];
    float e_poss[3];
    int e_status;

    // Aimbot angulo
    float e_angulo[3];
    float distancia;
    float angulo_diferencia;
};

extern KeyGen2009 jugadores[32];

struct AimbotConfig {
    bool EnableAimbot = false; // Default false
    bool EnableESP = true;     // Default true for visual feedback
    bool TeamCT = true;
    bool TeamTT = true;
    float FOV = 360.0f;
    float SmoothBase = 100.0f;
    float SmoothMultiplier = 2.5f;
    float SmoothMax = 550.0f;
    bool AimHeightManual = false; // si es true usa HeightOffset, si no usa logica default
    float HeightOffset = 1.0f; // cabeza por defecto
    int BoneID = 8; // 8 = cabeza aprox en hitbox index (pero aqui usamos offset)
    bool DynamicSmooth = true;
};

extern AimbotConfig Config;
