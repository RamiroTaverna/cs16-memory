#pragma once
#include <Windows.h>
#include "KG_VARIABLES_GLOBALES.h"

extern HWND  g_hwnd;
extern int  g_nFound;


BOOL CALLBACK kg_buscar_ventana_con_pid( HWND hwnd, LPARAM lParam);
LPSTR kg_obtener_titulo_de_ventana_con_pid (DWORD dwProcessId);
