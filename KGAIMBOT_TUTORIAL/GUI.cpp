#include "GUI.h"
#include <iostream>
#include "KG_VARIABLES_GLOBALES.h"
#include "KGVENTANA.h"

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <fstream>

namespace GUI {

    LPDIRECT3D9 pD3D = NULL;
    LPDIRECT3DDEVICE9 pDevice = NULL;
    D3DPRESENT_PARAMETERS d3dpp = {};
    HWND overlayHwnd = NULL;
    bool menuOpen = true; // Empieza abierto para configurar

    void SaveConfig() {
        std::ofstream file("config.ini", std::ios::binary);
        if (file.is_open()) {
            file.write((char*)&Config, sizeof(AimbotConfig));
            file.close();
        }
    }

    void LoadConfig() {
        std::ifstream file("config.ini", std::ios::binary);
        if (file.is_open()) {
            file.read((char*)&Config, sizeof(AimbotConfig));
            file.close();
        }
    }

    // Forward declarations
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    bool InitD3D(HWND hWnd);
    void CleanupD3D();
    void SetClickthrough(bool clickthrough);

    // Helper for ESP
    struct Vec3 { float x, y, z; };
    struct Vec2 { float x, y; };

    bool WorldToScreen(float* from, float* to, int width, int height) {
        // Obtenemos el rectangulo del AREA CLIENTE (sin bordes ni barra de titulo)
        RECT clientRect;
        POINT topLeft = { 0, 0 };
        
        if (g_hwnd && GetClientRect(g_hwnd, &clientRect)) {
            ClientToScreen(g_hwnd, &topLeft);
            width = clientRect.right - clientRect.left;
            height = clientRect.bottom - clientRect.top;
        } else {
             // Fallback
             width = GetSystemMetrics(SM_CXSCREEN);
             height = GetSystemMetrics(SM_CYSCREEN);
        }

        float eyePos[3] = { mi_poss_3d[0], mi_poss_3d[1], mi_poss_3d[2] + 17.0f };
        float vDelta[3] = { from[0] - eyePos[0], from[1] - eyePos[1], from[2] - eyePos[2] };
        
        float yawRad = mi_mira[1] * (3.14159265f / 180.0f);
        float pitchRad = mi_mira[0] * (3.14159265f / 180.0f);

        // 1. Yaw Rotation (Z-Axis)
        float vForwardXY = vDelta[0] * cos(yawRad) + vDelta[1] * sin(yawRad); 
        float vRight = vDelta[0] * sin(yawRad) - vDelta[1] * cos(yawRad);
        
        // If I look down, the world moves UP on screen.
        // My crosshair is low in the world.
        // The object (at eye level) is HIGH.
        // So I want object at TOP of screen (Y < Center).
        // Formula: Center - Z'.
        // We want Center - (Positive).
        // So Z' must be Positive.
        // Current: -70. Wrong.
        // 2. Pitch Rotation (Y-Axis)
        // Correcting sign: Look Down (+Pitch) -> World Moves Up (vUp +). 
        // Previously: - vForwardXY * sin, which gave Negative (Down).
        // New: + vForwardXY * sin, should give Positive (Up).
        float vForward = vForwardXY * cos(pitchRad) + vDelta[2] * sin(pitchRad);
        float vUp = vDelta[2] * cos(pitchRad) + vForwardXY * sin(pitchRad); // Inverted sign based on testing
        
        // Perspective
        float centerX = width / 2.0f;
        float centerY = height / 2.0f;

        if (vForward < 0.1f) return false;

        // Projection
        to[0] = topLeft.x + centerX + (vRight / vForward) * centerX; 
        to[1] = topLeft.y + centerY - (vUp / vForward) * centerX; // Using CenterX for aspect ratio fix assumption

        return true;
    }

    // Checking Skins (Local Helper)
    bool IsEnemySkin(const char* model) {
         // Simple substring check or exact match
         std::string s = model;
         bool isTT = (s.find("arctic") != std::string::npos || s.find("guerilla") != std::string::npos || s.find("terror") != std::string::npos || s.find("leet") != std::string::npos);
         bool isCT = (s.find("gign") != std::string::npos || s.find("sas") != std::string::npos || s.find("gsg9") != std::string::npos || s.find("urban") != std::string::npos);
         
         if (Config.TeamTT && isTT) return true;
         if (Config.TeamCT && isCT) return true;
         return false;
    }

    void DrawESP(int screenW, int screenH) {
         for (int i=1; i<32; i++) {
             if (jugadores[i].e_status != 0 && (jugadores[i].e_poss[0] != 0)) { // Alive and valid
                 if (IsEnemySkin(jugadores[i].e_model)) {
                     // Correction: User requested lowering by 20 units (Total -20 from base).
                     // Likely e_poss is not exactly feet.
                     float zOffset = -40.0f; 
                     
                     float footPos[3] = { jugadores[i].e_poss[0], jugadores[i].e_poss[1], jugadores[i].e_poss[2] + zOffset };
                     
                     // Crouch adjustment: Standing ~62 height (visual), Crouch ~50
                     float height = (jugadores[i].e_status == 2) ? 45.0f : 65.0f; // Adjusted height shorter as we moved down
                     
                     float headPos[3] = { footPos[0], footPos[1], footPos[2] + height };

                     float screenHead[2];
                     float screenFoot[2];

                     if (WorldToScreen(headPos, screenHead, screenW, screenH) && 
                         WorldToScreen(footPos, screenFoot, screenW, screenH)) {
                         
                         float h = screenFoot[1] - screenHead[1];
                         float w = h / 2.0f; // Aspect ratio
                         float x = screenFoot[0] - w / 2.0f;
                         float y = screenHead[1];

                         // Draw Box
                         ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
                         
                         // Draw Name
                         ImVec2 textSize = ImGui::CalcTextSize(jugadores[i].e_nick);
                         ImGui::GetBackgroundDrawList()->AddText(ImVec2(x + w/2 - textSize.x/2, y - textSize.y - 2), IM_COL32(255, 255, 255, 255), jugadores[i].e_nick);
                     }
                 }
             }
         }
    }

    void RenderLoop() {
        std::cout << "[GUI] Iniciando RenderLoop..." << std::endl;
        
        // Crear Ventana de Overlay
        WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "KGAimbotOverlay", NULL };
        RegisterClassEx(&wc);
        
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);

        overlayHwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, "KGAimbotOverlay", "KGAimbot Overlay", WS_POPUP, 0, 0, screenW, screenH, NULL, NULL, wc.hInstance, NULL);
        
        if (!overlayHwnd) {
             std::cout << "[GUI] ERROR: Fallo al crear ventana de overlay." << std::endl;
             return;
        }

        // Hacer la ventana transparente pero visible
        SetLayeredWindowAttributes(overlayHwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(overlayHwnd, SW_SHOWDEFAULT);
        UpdateWindow(overlayHwnd);

        // Inicializar D3D
        if (!InitD3D(overlayHwnd)) {
            std::cout << "[GUI] ERROR: Fallo al inicializar DirectX 9." << std::endl;
            CleanupD3D();
            UnregisterClass("KGAimbotOverlay", wc.hInstance);
            return;
        }
        std::cout << "[GUI] Overlay e ImGui inicializados correctamente." << std::endl;

        // Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(overlayHwnd);
        ImGui_ImplDX9_Init(pDevice);

        bool exit = false;
        while (!exit) {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    exit = true;
            }
            if (exit) break;

            // Toggle Menu con INSERT
            if (GetAsyncKeyState(VK_INSERT) & 1) {
                menuOpen = !menuOpen;
                SetClickthrough(!menuOpen);
            }

            // Start Frame
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // 1. Dibujar Menu de Configuracion
            if (menuOpen) {
                ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);
                ImGui::Begin("RAMA AIMBOT & ESP - cs16full_v23b_v2_5", &menuOpen);
                
                ImGui::Checkbox("Activar Aimbot 'P'", &Config.EnableAimbot);
                ImGui::Checkbox("Activar ESP (Visuals)", &Config.EnableESP);
                
                if (ImGui::CollapsingHeader("Equipos", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Checkbox("Apuntar a TERRORISTAS", &Config.TeamTT);
                    ImGui::Checkbox("Apuntar a POLICIAS", &Config.TeamCT);
                }

                if (ImGui::CollapsingHeader("Aimbot Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::SliderFloat("FOV", &Config.FOV, 1.0f, 360.0f, "%.1f deg");
                    
                    ImGui::Text("Smooth settings:");
                    ImGui::SliderFloat("Base Time (ms)", &Config.SmoothBase, 1.0f, 500.0f);
                    ImGui::SliderFloat("Multiplier", &Config.SmoothMultiplier, 0.1f, 10.0f);
                    ImGui::SliderFloat("Max Time (ms)", &Config.SmoothMax, 100.0f, 1000.0f);
                    
                    ImGui::Separator();
                    ImGui::Text("Hitbox Offset:");
                    ImGui::Checkbox("Manual Height Adjustment", &Config.AimHeightManual);
                    if (Config.AimHeightManual) {
                        ImGui::SliderFloat("Height Offset", &Config.HeightOffset, -30.0f, 10.0f, "%.1f");
                    } else {
                        // Presets
                        const char* items[] = { "Cabeza (+1.0)", "Cuello (0.0)", "Pecho (-10.0)", "Panza (-15.0)", "Pies (-20.0)" };
                        static int current_item = 0;
                        if (ImGui::Combo("Body Part", &current_item, items, IM_ARRAYSIZE(items))) {
                             switch (current_item) {
                                case 0: Config.HeightOffset = 1.0f; break;
                                case 1: Config.HeightOffset = 0.0f; break;
                                case 2: Config.HeightOffset = -10.0f; break;
                                case 3: Config.HeightOffset = -15.0f; break;
                                case 4: Config.HeightOffset = -20.0f; break;
                             }
                        }
                    }
                }
                if (ImGui::Button("Save Config")) SaveConfig();
                ImGui::SameLine();
                if (ImGui::Button("Load Config")) LoadConfig();
                
                ImGui::End();
            }

            // 2. Dibujar FOV Circle (Visuals)
            if (Config.EnableAimbot) {
                // Dibujar circulo de FOV en el centro de la pantalla
                // Nota: El FOV en el juego es 3D y cónico, dibujar un circulo 2D perfecto requiere calcular el radio basado en la proyección.
                // Por simplicidad para el tutorial, dibujaremos un circulo fijo o proporcional si se desea, 
                // pero un FOV de 360 cubre todo. Un FOV pequeño se puede visualizar.
                // Calculo aproximado: FOV de 90 grados es toda la pantalla de ancho aprox.
                
                if (Config.FOV < 180.0f) {
                   // Radio aproximado visual
                   float radius = (screenH / 2.0f) * tanf((Config.FOV * 3.14159f / 180.0f) / 2.0f); // Muy aproximado
                   ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(screenW/2, screenH/2), radius, IM_COL32(255, 255, 255, 50), 64);
                }
                
            } // End of EnableAimbot block

            // 3. Draw ESP (Independent)
            if (Config.EnableESP) {
                DrawESP(screenW, screenH);
            }

            ImGui::EndFrame();

            pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

            // Clear con color transparente (key color)
            pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

            if (pDevice->BeginScene() >= 0) {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                pDevice->EndScene();
            }

            HRESULT result = pDevice->Present(NULL, NULL, NULL, NULL);
            if (result == D3DERR_DEVICELOST && pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
                 ImGui_ImplDX9_InvalidateDeviceObjects();
                 pDevice->Reset(&d3dpp);
                 ImGui_ImplDX9_CreateDeviceObjects();
            }
            
            Sleep(10);
        }

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupD3D();
        DestroyWindow(overlayHwnd);
        UnregisterClass("KGAimbotOverlay", wc.hInstance);
    }

    bool InitD3D(HWND hWnd) {
        if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
            std::cout << "[GUI] ERROR: Direct3DCreate9 fallo." << std::endl;
            return false;
        }

        ZeroMemory(&d3dpp, sizeof(d3dpp));
        d3dpp.Windowed = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Usa el modo de escritorio actual
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // VSync

        // Try Hardware Vertex Processing
        if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pDevice) < 0) {
            std::cout << "[GUI] Advertencia: Hardware Vertex Processing fallo. Intentando Software..." << std::endl;
            
            // Try Software Vertex Processing (Fallback)
            if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice) < 0) {
                std::cout << "[GUI] ERROR: CreateDevice fallo completamente." << std::endl;
                return false;
            }
        }

        return true;
    }

    void CleanupD3D() {
        if (pDevice) { pDevice->Release(); pDevice = NULL; }
        if (pD3D) { pD3D->Release(); pD3D = NULL; }
    }

    void SetClickthrough(bool clickthrough) {
        LONG style = GetWindowLong(overlayHwnd, GWL_EXSTYLE);
        if (clickthrough) {
            style |= WS_EX_TRANSPARENT; // Click a traves
            // Aseguramos que sea top most y layered
             SetWindowPos(overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        } else {
            style &= ~WS_EX_TRANSPARENT; // Captura clicks
            SetWindowPos(overlayHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        SetWindowLong(overlayHwnd, GWL_EXSTYLE, style);
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg) {
        case WM_SIZE:
            if (pDevice != NULL && wParam != SIZE_MINIMIZED) {
                d3dpp.BackBufferWidth = LOWORD(lParam);
                d3dpp.BackBufferHeight = HIWORD(lParam);
                // Reset device logic if needed
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
