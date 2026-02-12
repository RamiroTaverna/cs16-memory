# RAMA AIMBOT & ESP - Counter-Strike 1.6 (v23b) (SOLO TESTEADO CONTRA BOTS)

# DESCARGA DEL CS 1.6: https://www.bkgaming.net/foro/files/file/4-counter-strike-16-no-steam-v23b-full/

# CREDITOS A https://www.youtube.com/@KeyGen2009

Este proyecto es un **Cheat Externo** para Counter-Strike 1.6 (versión No-Steam v23b, protocolo 47/48). Incluye un **Aimbot Smooth** configurable y un sistema de **ESP (Wallhack)** visual con cajas y nombres, todo controlado desde un menú gráfico (GUI) superpuesto.

## 🚀 Características

### 🎯 Aimbot
*   **Smooth Aim**: Movimiento de mira suavizado para parecer humano ("Legit").
*   **Angulo de tracking Configurable**: Campo de visión ajustable.
*   **Selector de Huesos**: Apunta a la cabeza, pecho o cintura.
*   **Team Check**: Configurable para atacar solo Terroristas (TT) o Anti-Terroristas (CT).

### 👁️ ESP (Visuals)
*   **Box ESP**: Dibuja cajas 2D alrededor de los enemigos.
*   **Name ESP**: Muestra el nombre del jugador sobre su cabeza.
*   **Snaplines**: Las cajas se "pegan" al jugador perfectamente gracias a matemáticas World-To-Screen.

### ⚙️ Sistema
*   **Menú GUI**: Presiona `INSERT` para abrir/cerrar el menú.
*   **Configuración Persistente**: Los ajustes se guardan automáticamente en `config.ini`.
*   **Externo**: Lee y escribe memoria desde fuera del proceso del juego (`hl.exe`).

## 🛠️ Tecnologías Usadas
*   **C++**: Lenguaje principal.
*   **DirectX 9**: Para el dibujado del Overlay y el Menú.
*   **ImGui**: Librería para la interfaz gráfica.
*   **WinAPI**: `ReadProcessMemory`, `WriteProcessMemory` para interactuar con el juego.

## 💾 Offsets (Técnico)

Estos offsets son específicos para la versión v23b de CS 1.6 (hw.dll / client.dll).

| Variable | Offset | Módulo Base |
| :--- | :--- | :--- |
| **Local Player** | | |
| `MI_POSS_3D` | `0x5EF748` | `hl.exe` / `hw.dll` |
| `MOUSE` (ViewAngles) | `0x19E10C4` | `hl.exe` / `hw.dll` |
| `MI_TEAM` | `0x5F73E4` | `hl.exe` / `hw.dll` |
| **Entity List (Loop 1-32)** | | |
| `NICK_BASE` | `0x1B5A6C8` | `hl.exe` / `hw.dll` |
| `MODEL_BASE` | `0x1B5A6F4` | `hl.exe` / `hw.dll` |
| `POSS_BASE` | `0x1B5A74C` | `hl.exe` / `hw.dll` |
| `BOT_STATUS` | `0x1B5A98C` | `hl.exe` / `hw.dll` |
| `DISTANCIA_OFFSET` | `0x24C` | (Salto entre entidades) |

## 📦 Instrucciones de Uso

1.  **Compilar**: Abre el proyecto en Visual Studio 2019/2022 y compila en `Release (x86)`.
    *   *Nota*: Necesitas tener el SDK de DirectX 9 instalado y configurado.
2.  **Ejecutar Juego**: Abre Counter-Strike 1.6 en modo Ventan `Windowed Mode` o `Fullscreen Windowed` (recomendado para el Overlay).
3.  **Ejecutar Cheat**: Abre el `.exe` generado como Administrador.
4.  **En el Juego**:
    *   Presiona `INSERT` para mostrar/ocultar el menú.
    *   Configura tu FOV, Smooth y activa "Aimbot" o "ESP".

## ⚠️ Disclaimer
Este software fue creado con fines **educativos** para aprender sobre lectura de memoria, matemáticas vectoriales (World-To-Screen) y renderizado con DirectX.
