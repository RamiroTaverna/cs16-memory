#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "KG_VARIABLES_GLOBALES.h"

HANDLE kg_abrir_proceso(const char* nombre, DWORD acceso);
DWORD kg_obtener_modulo(LPSTR modulo, DWORD kg_pid);
