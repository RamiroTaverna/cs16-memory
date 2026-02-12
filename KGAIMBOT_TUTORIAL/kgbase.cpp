#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "KG_VARIABLES_GLOBALES.h"
#include "KGMODULO.h"
#include "KGVENTANA.h"
#include "KG_INJECT.h"
#include "GUI.h"

using namespace std;

using namespace std;

bool aimbotActivo = false;

// Función para manejar la activación del aimbot con la tecla "P"
void toggleAimbot() {
    static bool prev_p_state = false;

    while (true) {
        bool current_p_state = GetAsyncKeyState('P') & 0x8000;

        		if (current_p_state && !prev_p_state) {
            Config.EnableAimbot = !Config.EnableAimbot;
        }

        prev_p_state = current_p_state;
        Sleep(10);
    }
}

// Modulos
// DWORD HL; // Definido en Globals.cpp

// HEADERS
// HEADERS

// Offsets de direcciones de memoria de los jugadores
DWORD NICK_BASE = 0x1B5A6C8;
DWORD MODEL_BASE = 0x1B5A6F4;
DWORD POSS_BASE = 0x1B5A74C;

// Estado del bot (0=muerto, 1=parado, 2=agachado, etc.)
DWORD BOT_STATUS = 0x1B5A98C;

// Control del mouse (ángulos de la mira)
// Control del mouse (ángulos de la mira)
// DWORD MOUSE = 0x19E10C4; // En Globals.cpp
// float mi_mira[3]; // En Globals.cpp

// Mi info 3d (coords)
// DWORD MI_POSS_3D = 0x5EF748; // En Globals.cpp
// float mi_poss_3d[3]; // En Globals.cpp

// DWORD MI_TEAM = 0x5F73E4; // En Globals.cpp
// int mi_team; // En Globals.cpp

DWORD VIDA = 0x5EF5FC;

// Distancia de estructura en estructura
DWORD DISTANCIA_OFFSET = 0x24C;

// Struct KeyGen2009 movido a KG_VARIABLES_GLOBALES.h
// extern KeyGen2009 jugadores[32]; // Definido en Globals.cpp

// Variables para smooth aim
bool moviendoMira = false;
float angulo_destino[3];
float angulo_inicio[3];
int enemigo_actual = -1;
float tiempo_inicio_smooth = 0;
float duracion_smooth = 250.0f;
float ultimo_tiempo_ajuste = 0;

// Funciones
void generar_direcciones();
void leer_mi_poss();
void leer_mi_mira();

// Sensores aimbot
bool ctAim = false;
bool ttAim = false;

void team_sensor();
void tt_aimbot();
void ct_aimbot();
void kg_reader();
void smooth_aim_controller();

// Función calcular el angulo de trackeo
void calcular(float* src, float* dst, float* angulos, int target_status);

// Función para calcular diferencia angular
float calcular_diferencia_angular(float* angulo1, float* angulo2);

// Función para ajustar ángulos dentro de límites
void ajustar_angulos(float* angulos);

// Función para interpolar linealmente
float lerp(float a, float b, float t);

// Listas de skins por equipo
std::vector<std::string> CT_SKINS = { "gign", "sas", "gsg9", "urban" };
std::vector<std::string> TT_SKINS = { "arctic", "guerilla", "terror", "leet" };

void UpdatePlayers();

// Función para verificar si un modelo pertenece al equipo enemigo
bool esEnemigo(const std::string& modelo) {
    bool isTT = std::find(TT_SKINS.begin(), TT_SKINS.end(), modelo) != TT_SKINS.end();
    bool isCT = std::find(CT_SKINS.begin(), CT_SKINS.end(), modelo) != CT_SKINS.end();

    if (Config.TeamTT && isTT) return true;
    if (Config.TeamCT && isCT) return true;

    return false;
}

// Función para leer el estado de los jugadores
void leer_estados_jugadores() {
    while (1) {
        for (int i = 0; i < 31; i++) {
            ReadProcessMemory(hp, (LPBYTE*)(HL + BOT_STATUS + (DISTANCIA_OFFSET * i)),
                &jugadores[i + 1].e_status, sizeof(jugadores[i + 1].e_status), 0);
        }
        Sleep(1); // CAMBIADO DE 10 A 1
    }
}

// Leer la mira actual del jugador
void leer_mi_mira() {
    while (1) {
        ReadProcessMemory(hp, (LPBYTE*)(HL + MOUSE), &mi_mira, sizeof(mi_mira), 0);
        Sleep(1); // CAMBIADO DE 1 A 1 (se queda igual)
    }
}

// Calcula la diferencia angular entre dos vectores de ángulos
float calcular_diferencia_angular(float* angulo1, float* angulo2) {
    float yaw1 = fmodf(angulo1[1] + 360.0f, 360.0f);
    float pitch1 = fmodf(angulo1[0] + 360.0f, 360.0f);
    float yaw2 = fmodf(angulo2[1] + 360.0f, 360.0f);
    float pitch2 = fmodf(angulo2[0] + 360.0f, 360.0f);

    float diff_yaw = fabs(yaw1 - yaw2);
    if (diff_yaw > 180.0f) diff_yaw = 360.0f - diff_yaw;
    float diff_pitch = fabs(pitch1 - pitch2);
    if (diff_pitch > 180.0f) diff_pitch = 360.0f - diff_pitch;

    return sqrtf(diff_yaw * diff_yaw + diff_pitch * diff_pitch);
}

// Interpolación lineal
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Ajusta los ángulos para que estén dentro de límites razonables
void ajustar_angulos(float* angulos) {
    if (angulos[0] > 89.0f) angulos[0] = 89.0f;
    if (angulos[0] < -89.0f) angulos[0] = -89.0f;
    angulos[1] = fmodf(angulos[1], 360.0f);
    if (angulos[1] < 0.0f) angulos[1] += 360.0f;
}

// CALCULAMOS EL ANGULO DEL AIMBOT
// CALCULAMOS EL ANGULO DEL AIMBOT
void calcular(float* src, float* dst, float* angulos, int target_status) {
    float delta[3] = { dst[0] - src[0], dst[1] - src[1], dst[2] - src[2] };
    
    // Altura base configurable
    float height_fix = Config.HeightOffset;
    
    // Correccion automatica de agachado
    if (target_status == 2) { 
        height_fix -= 18.0f; // Bajar la mira si esta agachado
    }
    
    delta[2] += height_fix;
    
    float hyp = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);

    angulos[0] = -atan2f(delta[2], hyp) * 57.295779513082f;
    angulos[1] = atan2f(delta[1], delta[0]) * 57.295779513082f;
    angulos[2] = 0.0f;

    ajustar_angulos(angulos);
}

// Actualiza la informacion de los jugadores (Memoria -> Struct Global)
void UpdatePlayers() {
    for (int i = 1; i < 32; i++) {
        ReadProcessMemory(hp, (LPBYTE*)(HL + NICK_BASE + jugadores[i].E_NICK),
            &jugadores[i].e_nick, sizeof(jugadores[i].e_nick), 0);
        ReadProcessMemory(hp, (LPBYTE*)(HL + MODEL_BASE + jugadores[i].E_MODEL),
            &jugadores[i].e_model, sizeof(jugadores[i].e_model), 0);
        ReadProcessMemory(hp, (LPBYTE*)(HL + POSS_BASE + jugadores[i].E_POSS),
            &jugadores[i].e_poss, sizeof(jugadores[i].e_poss), 0);
    }
}

// Encuentra el enemigo más cercano a la mira
int encontrarEnemigoConveniente() {
    int mejorEnemigo = -1;
    float mejorPuntuacion = 999999.0f;
    const float ANGULO_MAXIMO = 360.0f;

    // Nota: La lectura de memoria se hace ahora en UpdatePlayers() llamdo desde el bucle principal.

    for (int i = 1; i < 32; i++) {
        bool esValido = (strcmp(jugadores[i].e_nick, "") != 0 &&
            (jugadores[i].e_poss[0] != 0 || jugadores[i].e_poss[1] != 0 || jugadores[i].e_poss[2] != 0) &&
            esEnemigo(jugadores[i].e_model) &&
            jugadores[i].e_status != 0);

        if (esValido) {
            float dx = mi_poss_3d[0] - jugadores[i].e_poss[0];
            float dy = mi_poss_3d[1] - jugadores[i].e_poss[1];
            float dz = mi_poss_3d[2] - jugadores[i].e_poss[2];
            jugadores[i].distancia = sqrt(dx * dx + dy * dy + dz * dz);

            calcular(mi_poss_3d, jugadores[i].e_poss, jugadores[i].e_angulo, jugadores[i].e_status);
            jugadores[i].angulo_diferencia = calcular_diferencia_angular(mi_mira, jugadores[i].e_angulo);

            if (jugadores[i].angulo_diferencia <= Config.FOV) {
                float puntuacion = jugadores[i].angulo_diferencia * 0.7f +
                    (jugadores[i].distancia / 1000.0f) * 0.3f;

                puntuacion *= 1.0f + ((rand() % 11) - 5) / 100.0f;

                if (puntuacion < mejorPuntuacion) {
                    mejorPuntuacion = puntuacion;
                    mejorEnemigo = i;
                }
            }
        }
    }

    return mejorEnemigo;
}

// CONTROLADOR PRINCIPAL DEL SMOOTH AIM
void smooth_aim_controller() {
    while (true) {
        DWORD tiempo_actual = GetTickCount();

        if (moviendoMira) {
            float tiempo_transcurrido = tiempo_actual - tiempo_inicio_smooth;
            float t = tiempo_transcurrido / duracion_smooth;

            if (t >= 1.0f) {
                WriteProcessMemory(hp, (LPBYTE*)(HL + MOUSE), angulo_destino, 12, 0);
                moviendoMira = false;
                enemigo_actual = -1;
            }
            else {
                float t_suave = t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;

                float angulos_actuales[3];

                angulos_actuales[0] = lerp(angulo_inicio[0], angulo_destino[0], t_suave);

                float diff_yaw = angulo_destino[1] - angulo_inicio[1];
                if (diff_yaw > 180.0f) diff_yaw -= 360.0f;
                if (diff_yaw < -180.0f) diff_yaw += 360.0f;
                angulos_actuales[1] = angulo_inicio[1] + diff_yaw * t_suave;

                angulos_actuales[2] = 0.0f;

                ajustar_angulos(angulos_actuales);
                WriteProcessMemory(hp, (LPBYTE*)(HL + MOUSE), angulos_actuales, 12, 0);
            }
        }
        Sleep(1); // CAMBIADO DE 5 A 1
    }
}

// INICIAR MOVIMIENTO SMOOTH HACIA UN ENEMIGO
void iniciarAimbotSmooth(int indiceEnemigo) {
    if (indiceEnemigo == -1) return;

    if (enemigo_actual == indiceEnemigo && moviendoMira) return;

    ReadProcessMemory(hp, (LPBYTE*)(HL + MOUSE), &angulo_inicio, sizeof(angulo_inicio), 0);
    memcpy(angulo_destino, jugadores[indiceEnemigo].e_angulo, sizeof(angulo_destino));

    float diff_angular = calcular_diferencia_angular(angulo_inicio, angulo_destino);
    duracion_smooth = Config.SmoothBase + (diff_angular * Config.SmoothMultiplier); 
    if (duracion_smooth > Config.SmoothMax) duracion_smooth = Config.SmoothMax;

    tiempo_inicio_smooth = GetTickCount();
    enemigo_actual = indiceEnemigo;
    moviendoMira = true;
}

// Hilo UNIFICADO del Aimbot
void aimbot_logic() {
    const int INTERVALO_ACTUALIZACION = 1;

    while (true) {
        // Actualizar datos SIEMPRE (para ESP y Aimbot)
        UpdatePlayers();

        // Ejecutar si el Aimbot esta activo y no estamos ya moviendo la mira
        if (Config.EnableAimbot && !moviendoMira) {
            DWORD tiempo_actual = GetTickCount();
            if (tiempo_actual - ultimo_tiempo_ajuste > INTERVALO_ACTUALIZACION) {
                // encontrarEnemigoConveniente ya usa esEnemigo, que ahora chequea Config.TeamCT/TT
                int enemigo = encontrarEnemigoConveniente();
                if (enemigo != -1) {
                    iniciarAimbotSmooth(enemigo);
                    ultimo_tiempo_ajuste = tiempo_actual;
                }
            }
        }
        Sleep(1);
    }
}

void kg_reader() {
    while (1) {
        Sleep(100);
    }
}

void team_sensor() {
    while (1) {
        ReadProcessMemory(hp, (LPBYTE*)(HL + MI_TEAM), &mi_team, sizeof(mi_team), 0);

// Sensor de equipo desactivado en favor de GUI manual
/*
        switch (mi_team) {
        case 0:
            ctAim = false;
            ttAim = false;
            break;
        case 1:
            ctAim = false;
            ttAim = true;
            break;
        case 2:
            ctAim = true;
            ttAim = false;
            break;
        }
*/
        Sleep(1); // CAMBIADO DE 100 A 1
    }
}

void generar_direcciones() {
    for (int i = 0; i < 32; i++) {
        jugadores[i].E_NICK = DISTANCIA_OFFSET * i;
        jugadores[i].E_MODEL = DISTANCIA_OFFSET * i;
        jugadores[i].E_POSS = DISTANCIA_OFFSET * i;
        jugadores[i].E_STATUS = DISTANCIA_OFFSET * (i - 1);
    }
    jugadores[0].e_status = 1;
}

void leer_mi_poss() {
    while (1) {
        ReadProcessMemory(hp, (LPBYTE*)(HL + MI_POSS_3D), &mi_poss_3d, sizeof(mi_poss_3d), 0);
        Sleep(1); // CAMBIADO DE 10 A 1
    }
}

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    injeccion();
    generar_direcciones();

    // Creación de hilos
    thread t1(leer_mi_poss);
    thread t2(team_sensor);
    thread tAimbot(aimbot_logic); // Hilo unificado
    // thread t4(ct_aimbot); // Eliminado
    thread t5(smooth_aim_controller);
    thread t6(toggleAimbot);
    thread t7(leer_estados_jugadores);
    thread t8(leer_mi_mira);
    thread t9(GUI::RenderLoop);

    t1.join();
    t2.join();
    tAimbot.join();
    // t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

    return 0;
}