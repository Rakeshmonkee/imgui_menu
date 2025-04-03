#include "gui.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"
#include "../Misc/Button Values.h" // For Variables used in Menu
#include <fstream>
#include <GLFW/glfw3.h>
#include "../Misc/JSONHelper.h" // For JSON Load and Save
#include <windows.h> // For HWND
#include <winuser.h> // For WNDCLASSEXA



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
		}
	} return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU && longParameter == VK_F12)
			return 0;
	} break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter);
	} return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = RECT{};
			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}
	} return 0;
	}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

namespace gui {

	bool exit = true;
	HWND window = nullptr;
	WNDCLASSEXW windowClass = {};  // Unicode version
	IDirect3D9* d3d = nullptr;
	IDirect3DDevice9* device = nullptr;
	D3DPRESENT_PARAMETERS presentParameters = {};
	POINTS position = {};

	void CreateHWindow(const wchar_t* windowName, const wchar_t* className) noexcept
	{
		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_CLASSDC;
		windowClass.lpfnWndProc = WindowProcess;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = GetModuleHandleW(nullptr);
		windowClass.hIcon = 0;
		windowClass.hbrBackground = 0;
		windowClass.lpszMenuName = 0;
		windowClass.lpszClassName = className;
		windowClass.hIconSm = 0;

		// Register the window class (Unicode)
		RegisterClassExW(&windowClass);

		// Create the window (Unicode)
		window = CreateWindowW(
			className,    // Class name
			windowName,   // Window title
			WS_POPUP,
			100,
			100,
			WIDTH,
			HEIGHT,
			nullptr,
			nullptr,
			windowClass.hInstance,
			nullptr
		);

		ShowWindow(window, SW_SHOWDEFAULT);
		UpdateWindow(window);
	}


	void DestroyHWindow() noexcept
	{
		DestroyWindow(window);
		// Call the Unicode version of UnregisterClass
		UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
	}
}



bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;

};

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();

}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;

	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;

	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();

}



void gui::Render() noexcept
{

		// Show or hide the window based on the value of the showWindow flag

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.070f, 0.078f, 0.086f, 1.0f); // Change the inactive title bar color
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.070f, 0.078f, 0.086f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.070f, 0.078f, 0.086f, 1.0f);       // Background color of windows
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.117f, 0.121f, 0.133f, 1.0f);      // Background color of the menu bar
		style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);         // Background color of headers
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // Background color of hovered headers
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);   // Background color of active headers

		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.094f, 0.101f, 0.109f, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.086f, 0.098f, 0.101f, 1.0f);



		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.772f, 0.223f, 0.705f, 1.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.137f, 0.149f, 0.180f, 1.0f);     // Checkbox background color
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // Checkbox background color when hovered




		ImGui::Begin(
			"Name Here", // Title Bar Name
			&exit, // close button
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse
		);


		
  //IMGUI code goes here


  
			ImGui::End();
}
