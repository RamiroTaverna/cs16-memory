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
		cout << "KG --> El proceso no esta abierto!" << endl;
		
	}else{
		cout << "KG --> Cargando modulos del juego" << endl;
		CLIENT=kg_obtener_modulo("client.dll", pid);   //CONSIGUE LOS MODULOS DEL JUEGO (PODES CAMBIARLO POR EL Q VOS QUIERAS.)
		HL=kg_obtener_modulo("hl.exe", pid);   //CONSIGUE LOS MODULOS DEL JUEGO (PODES CAMBIARLO POR EL Q VOS QUIERAS.)
		HW=kg_obtener_modulo("hw.dll", pid);           //IDEM XD
		hp= OpenProcess(PROCESS_ALL_ACCESS, true, pid);    //DAMOS ACCESO AL PROCESO
		kg_obtener_titulo_de_ventana_con_pid(pid);                        //OBTENEMOS EL TITULO DE LA VENTANA MEDIANTE UNA FUNCION CON EL PID.
		hwnd=FindWindow(0,g_acTitle);                //BUSCAMOS LA VENTANA OBTENIDA MEDIANTE LA FUNC ANTERIOR.
		cout << g_acTitle << endl;                       //LA VENTANITA ENCONTRADA.
		
	}
}
