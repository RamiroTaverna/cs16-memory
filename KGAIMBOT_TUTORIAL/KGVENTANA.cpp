#include "KGVENTANA.h"
#include <iostream>

BOOL CALLBACK kg_buscar_ventana_con_pid( HWND hwnd, LPARAM lParam);

LPSTR kg_obtener_titulo_de_ventana_con_pid (DWORD dwProcessId){
	g_hwnd = NULL;
	g_nFound = 0;
	EnumWindows (kg_buscar_ventana_con_pid,( LPARAM)dwProcessId);
	if (g_nFound)
	
		Sleep(2222);
	return ( g_acTitle);
	return (NULL);
}

BOOL CALLBACK kg_buscar_ventana_con_pid( HWND hwnd, LPARAM lParam){
	DWORD dwPID2Find=(DWORD)lParam;
	DWORD dwPID =0;
	
	if(GetWindowThreadProcessId(hwnd,&dwPID))
	{
		if(dwPID==dwPID2Find)
		{
			g_hwnd=hwnd;
			g_nFound=GetWindowText ( g_hwnd, g_acTitle, 222);
			return(FALSE);
		}
	}
	return  (TRUE);
}
