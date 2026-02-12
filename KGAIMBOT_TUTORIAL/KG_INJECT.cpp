#include "KG_INJECT.h"
#include "KG_VARIABLES_GLOBALES.h"
#include "KGMODULO.h"
#include "KGVENTANA.h"
#include <iostream>

using namespace std;

void injeccion()
{
	Sleep(5000);

	hp=kg_abrir_proceso("hl.exe",PROCESS_VM_READ);
	if(hp==INVALID_HANDLE_VALUE){
		// Process not found
		
	}else{
		// Load game modules
		CLIENT=kg_obtener_modulo("client.dll", pid);
		HL=kg_obtener_modulo("hl.exe", pid);
		HW=kg_obtener_modulo("hw.dll", pid);
		hp= OpenProcess(PROCESS_ALL_ACCESS, true, pid);
		kg_obtener_titulo_de_ventana_con_pid(pid);
		hwnd=FindWindow(0,g_acTitle);
		
		// Hide console window after successful initialization
		HWND consoleWindow = GetConsoleWindow();
		if (consoleWindow != NULL) {
			ShowWindow(consoleWindow, SW_HIDE);
		}
	}
}
