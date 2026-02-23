#include "KGMODULO.h"
#include <iostream>

HANDLE kg_abrir_proceso(const char* nombre, DWORD acceso){
	PROCESSENTRY32 proceso;
	proceso.dwSize = sizeof(PROCESSENTRY32);
	HANDLE capturar = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if(Process32First(capturar, &proceso) == FALSE){
		CloseHandle(capturar);
		return INVALID_HANDLE_VALUE;
	}
	while(Process32Next(capturar, &proceso) == TRUE){
		if(strcmp(proceso.szExeFile, nombre) == 0){
			HANDLE processHandle = OpenProcess(acceso, FALSE, proceso.th32ProcessID);
			CloseHandle(capturar);
			pid=proceso.th32ProcessID;
			return processHandle;
		}
	}
	
	CloseHandle(capturar);
	return INVALID_HANDLE_VALUE;
}

DWORD kg_obtener_modulo(LPSTR modulo, DWORD kg_pid)
{ 
	MODULEENTRY32 modulo_de_entrada = {0};
	HANDLE kg_capturar = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE,kg_pid);
	if(!kg_capturar)
		return NULL;  
	modulo_de_entrada.dwSize = sizeof(modulo_de_entrada);
	BOOL correr_modulo = Module32First(kg_capturar, &modulo_de_entrada );
	while(correr_modulo)
	{
		if(!strcmp(modulo_de_entrada.szModule, modulo ) )
		{CloseHandle( kg_capturar );
		return (DWORD)modulo_de_entrada.modBaseAddr;
		}
		correr_modulo = Module32Next( kg_capturar, &modulo_de_entrada );
	}
	CloseHandle( kg_capturar );
	return NULL;
}
