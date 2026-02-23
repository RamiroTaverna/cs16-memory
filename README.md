# RAMA AIM & ESP - Counter-Strike 1.6 (v23b) (SOLO TESTEADO CONTRA BOTS)

# CREDITOS A https://www.youtube.com/@KeyGen2009

## ⚠️ Disclaimer
Este software fue creado con fines **educativos** para aprender sobre lectura de memoria, matemáticas vectoriales (World-To-Screen) y renderizado con DirectX.

Este proyecto es un **Cheat Externo** para Counter-Strike 1.6 (versión No-Steam v23b, protocolo 47/48). Incluye un **Aim Smooth** configurable y un sistema de **ESP (WH)** visual con cajas y nombres, todo controlado desde un menú gráfico (GUI) superpuesto.

## 🚀 Características

### 🎯 Aim
*   **Smooth Aim**: Movimiento de mira suavizado para parecer humano ("Legit").
*   **Angulo de tracking Configurable**: Campo de visión ajustable.
*   **Selector del model**: Apunta a la cabeza, pecho o cintura.
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

# DESCARGA DEL CS 1.6: https://www.bkgaming.net/foro/files/file/4-counter-strike-16-no-steam-v23b-full/

## 🛠️ Compilación paso a paso

### Requisitos previos

| Herramienta | Versión mínima | Descarga |
| :--- | :--- | :--- |
| **Visual Studio** | 2019 o 2022 | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) |
| **Workload de C++** | Desktop development with C++ | (seleccionar durante instalación de VS) |
| **DirectX SDK** | June 2010 (legacy) | [Microsoft Download Center](https://www.microsoft.com/en-us/download/details.aspx?id=6812) |

> ⚠️ El proyecto usa el **Platform Toolset v145** (Visual Studio 2022). Si tenés VS2019, podés cambiar el toolset en las propiedades del proyecto a `v142`.

---

### Pasos

**1. Clonar el repositorio**
```bash
git clone https://github.com/RamiroTaverna/cs16-memory.git
cd cs16-memory
```

**2. Instalar el DirectX SDK (June 2010)**
- Descargarlo e instalarlo desde el link de arriba.
- Se instala por defecto en `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\`.
- Visual Studio lo detecta automáticamente vía la variable de entorno `DXSDK_DIR`.

**3. Abrir la solución**
- Abrir el archivo `KGAIMBOT_TUTORIAL.sln` con Visual Studio.

**4. Configurar la build**
- En la barra superior, seleccionar:
  - Configuración: **`Release`**
  - Plataforma: **`Win32`** ← ⚠️ importante, **no** x64

**5. Compilar**
- Menú `Build` → `Build Solution` (o `Ctrl + Shift + B`).
- El ejecutable se genera en: `Debug\KGAIMBOT_TUTORIAL.exe`

---

### Solución de errores comunes

| Error | Causa | Solución |
| :--- | :--- | :--- |
| `d3d9.h: No such file` | DirectX SDK no instalado | Instalar DirectX SDK June 2010 |
| `Toolset v145 not found` | VS2019 en lugar de VS2022 | Cambiar toolset a `v142` en propiedades del proyecto |
| `LNK2019: unresolved external` en d3d9 | SDK no en el path | Verificar que `DXSDK_DIR` esté definido como variable de entorno |

---

## 🚀 Uso

1. **Ejecutar el juego**: Abrir Counter-Strike 1.6 en modo `Windowed` o `Fullscreen Windowed` (necesario para el overlay).
2. **Ejecutar el cheat**: Correr `KGAIMBOT_TUTORIAL.exe` **como Administrador**.
3. **En el juego**:
   - Presionar `INSERT` para mostrar/ocultar el menú.
   - Configurar FOV, Smooth y activar **Aimbot** o **ESP** desde el menú.
