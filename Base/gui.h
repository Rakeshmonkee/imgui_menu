#pragma once
#include <d3d9.h>
#include <windows.h> // For HWND
#include <winuser.h> // For WNDCLASSEXW


namespace gui {
    constexpr int WIDTH = 500;
    constexpr int HEIGHT = 490;

    extern bool exit;
    extern HWND window;
    extern WNDCLASSEXW windowClass;
    extern IDirect3D9* d3d;
    extern IDirect3DDevice9* device;
    extern D3DPRESENT_PARAMETERS presentParameters;
    extern POINTS position;

    void CreateHWindow(const wchar_t* windowName, const wchar_t* className) noexcept;
    void DestroyHWindow() noexcept;
    bool CreateDevice() noexcept;
    void ResetDevice() noexcept;
    void DestroyDevice() noexcept;
    void CreateImGui() noexcept;
    void DestroyImGui() noexcept;
    void BeginRender() noexcept;
    void EndRender() noexcept;
    void Render() noexcept;
}