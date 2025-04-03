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










float PI = 3.14159265358f;



void PlayerSkeleton() noexcept {
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Get the position and size of the child window
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	// Calculate the center point of the child window
	ImVec2 center = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

	// Calculate the length and width of the body parts
	float skeletonLength = windowSize.y * 0.4f;
	float headRadius = skeletonLength * 0.0f;
	float spineLength = skeletonLength * 0.3f;
	float shoulderLength = skeletonLength * 0.15f;
	float shoulderWidth = skeletonLength * 0.1f;
	float armLength = skeletonLength * 0.15f;
	float torsoLength = skeletonLength * 0.05f;
	float torsoWidth = skeletonLength * 0.1f;
	float legLength = skeletonLength * 0.25f;

	// Adjust center position to center the skeleton vertically

	center.y -= (headRadius + spineLength + legLength + legLength * cos(15 * PI / 180)) / 2;
	center.y += 10.0f;


	// Draw head
	drawList->AddCircle(center, headRadius, IM_COL32(255, 255, 255, 255), 25, 2.0f);

	// Draw spine
	drawList->AddLine(ImVec2(center.x, center.y + headRadius), ImVec2(center.x, center.y + headRadius + spineLength), IM_COL32(255, 255, 255, 255), 2.0f);

	// Draw shoulders
	float shoulderPosY = center.y + headRadius + spineLength / 2;
	drawList->AddLine(ImVec2(center.x - shoulderWidth, shoulderPosY), ImVec2(center.x + shoulderWidth, shoulderPosY), IM_COL32(255, 255, 255, 255), 2.0f);

	// Calculate left arm positions
	float leftArmPosY = shoulderPosY;
	float leftArmEndpointX = center.x - shoulderWidth - cos(45 * PI / 180) * armLength;
	float leftArmEndpointY = leftArmPosY + sin(45 * PI / 180) * armLength;

	// Draw left arm connected to the left side of the shoulder
	drawList->AddLine(ImVec2(center.x - shoulderWidth, shoulderPosY), ImVec2(leftArmEndpointX, leftArmEndpointY), IM_COL32(255, 255, 255, 255), 2.0f);

	// Calculate right arm positions
	float rightArmPosY = shoulderPosY;
	float rightArmEndpointX = center.x + shoulderWidth + cos(45 * PI / 180) * armLength;
	float rightArmEndpointY = rightArmPosY + sin(45 * PI / 180) * armLength;

	// Draw right arm connected to the right side of the shoulder
	drawList->AddLine(ImVec2(center.x + shoulderWidth, shoulderPosY), ImVec2(rightArmEndpointX, rightArmEndpointY), IM_COL32(255, 255, 255, 255), 2.0f);


	// Calculate left torso positions
	float lefttorsoPosY = center.y + headRadius + spineLength;
	float lefttorsoEndpointX = center.x - torsoWidth - cos(75 * PI / 180) * torsoLength;
	float lefttorsoEndpointY = lefttorsoPosY + sin(75 * PI / 180) * torsoLength;

	// Draw left torso connected to the spine
	drawList->AddLine(ImVec2(center.x, center.y + headRadius + spineLength), ImVec2(lefttorsoEndpointX, lefttorsoEndpointY), IM_COL32(255, 255, 255, 255), 2.0f);

	// Calculate right torso positions
	float righttorsoPosY = center.y + headRadius + spineLength;
	float righttorsoEndpointX = center.x + torsoWidth + cos(75 * PI / 180) * torsoLength;
	float righttorsoEndpointY = righttorsoPosY + sin(75 * PI / 180) * torsoLength;

	// Draw right torso connected to the spine
	drawList->AddLine(ImVec2(center.x, center.y + headRadius + spineLength), ImVec2(righttorsoEndpointX, righttorsoEndpointY), IM_COL32(255, 255, 255, 255), 2.0f);

	// Calculate left leg positions
	float leftLegPosX = lefttorsoEndpointX - sin(15 * PI / 180) * legLength;
	float leftLegPosY = lefttorsoEndpointY + cos(15 * PI / 180) * legLength;

	// Draw left leg connected to the left torso
	drawList->AddLine(ImVec2(lefttorsoEndpointX, lefttorsoEndpointY), ImVec2(leftLegPosX, leftLegPosY), IM_COL32(255, 255, 255, 255), 2.0f);

	// Calculate right leg positions
	float rightLegPosX = righttorsoEndpointX + sin(15 * PI / 180) * legLength;
	float rightLegPosY = righttorsoEndpointY + cos(15 * PI / 180) * legLength;

	// Draw right leg connected to the right torso
	drawList->AddLine(ImVec2(righttorsoEndpointX, righttorsoEndpointY), ImVec2(rightLegPosX, rightLegPosY), IM_COL32(255, 255, 255, 255), 2.0f);
}





void DrawPlayerBox() {
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	float boxWidth = 125.0f;
	float boxHeight = 175.0f;
	ImVec2 center = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);
	ImVec2 boxPos = ImVec2(center.x - boxWidth / 2, center.y - boxHeight / 2);
	ImVec2 boxSize = ImVec2(boxWidth, boxHeight);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddRect(boxPos, ImVec2(boxPos.x + boxSize.x, boxPos.y + boxSize.y), IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

}


void PlayerName() {

	// Get the position and size of the window
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	float boxWidth = 125.0f;
	float boxHeight = 175.0f;
	ImVec2 center = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

	// Calculate the position and size of the player name
	ImVec2 namePos = ImVec2(center.x - boxWidth / 2, center.y - boxHeight / 2 - 20.0f);
	ImVec2 nameSize = ImVec2(boxWidth, 20.0f);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Draw the player name as text
	drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(), namePos, IM_COL32(255, 255, 255, 255), Previewname);
}


void DrawHealthBox() {
	// Get the position and size of the window
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	float boxWidth = 125.0f;
	float boxHeight = 175.0f;
	ImVec2 center = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

	// Calculate the position and size of the player box
	ImVec2 boxPos = ImVec2(center.x - boxWidth / 2, center.y - boxHeight / 2);
	ImVec2 boxSize = ImVec2(boxWidth, boxHeight);

	// Calculate the position and size of the health box
	float healthBoxWidth = 3.0f;
	float healthBoxHeight = boxHeight;
	ImVec2 healthPos = ImVec2(boxPos.x - healthBoxWidth - 3.0f, boxPos.y);
	ImVec2 healthSize = ImVec2(healthBoxWidth, healthBoxHeight);

	// Calculate the health value (example value)
	float healthValue = float(playerHealth);  // Replace with your actual health value

	// Calculate the height of the health bar based on the health value
	float barHeight = healthSize.y * (healthValue / 100);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Draw the health box
	drawList->AddRectFilled(healthPos, ImVec2(healthPos.x + healthSize.x, healthPos.y + healthSize.y), IM_COL32(255, 0, 0, 255), 0.0f);

	// Draw the health bar
	drawList->AddRectFilled(healthPos, ImVec2(healthPos.x + healthSize.x, healthPos.y + barHeight), IM_COL32(0, 255, 0, 255), 0.0f);
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
			"MonkeeWare",
			&exit,
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse
		);


		









				///////////////////////////
				///
				/// PLAYER
				/// 
				//////////////////////////

			static int tabb = 0;




			style.Colors[ImGuiCol_Button] = ImVec4(0.070f, 0.078f, 0.086f, 1.0f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.094f, 0.101f, 0.109f, 1.0f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.772f, 0.223f, 0.705f, 1.0f); // purple underline color




			ImVec2 buttonSize(75, 25);  // Button size
			ImGuiStyle& currentStyle = ImGui::GetStyle();
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImVec2 buttonPos;  // Define buttonPos outside the if statement
			float buttonWidth = 0.0f;  // Initialize buttonWidth with a default value
			static int selectedButton = 0;  // Index of the selected button (-1 means none selected)
			float underlineHeight = 0.0f;
			float underlineThickness = 3.0f;

			float radius = 10.0f;
			const float originalWindowRounding = style.WindowRounding;

		





			if (ImGui::Button(("Player"), buttonSize))
			{
				tabb = 0;
				selectedButton = 0;
			}

			if (selectedButton == 0 || ImGui::IsItemHovered())
			{
				buttonPos = ImGui::GetItemRectMin();  // Assign buttonPos
				buttonWidth = ImGui::GetItemRectSize().x;  // Assign buttonWidth

				// Draw underline
				drawList->AddLine(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y + underlineHeight),
					ImVec2(buttonPos.x + buttonWidth, buttonPos.y + buttonSize.y + underlineHeight),
					ImGui::GetColorU32(ImGuiCol_ButtonActive),
					underlineThickness);
			}

			ImGui::SameLine();

			if (ImGui::Button("Aimbot", buttonSize))
			{
				tabb = 1;
				selectedButton = 1;
			}

			if (selectedButton == 1 || ImGui::IsItemHovered())
			{
				buttonPos = ImGui::GetItemRectMin();  // Assign buttonPos
				buttonWidth = ImGui::GetItemRectSize().x;  // Assign buttonWidth

				// Draw underline
				drawList->AddLine(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y + underlineHeight),
					ImVec2(buttonPos.x + buttonWidth, buttonPos.y + buttonSize.y + underlineHeight),
					ImGui::GetColorU32(ImGuiCol_ButtonActive),
					underlineThickness);
			}

			ImGui::SameLine();

			if (ImGui::Button("ESP", buttonSize))
			{
				tabb = 2;
				selectedButton = 2;
			}

			if (selectedButton == 2 || ImGui::IsItemHovered())
			{
				buttonPos = ImGui::GetItemRectMin();  // Assign buttonPos
				buttonWidth = ImGui::GetItemRectSize().x;  // Assign buttonWidth

				// Draw underline
				drawList->AddLine(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y + underlineHeight),
					ImVec2(buttonPos.x + buttonWidth, buttonPos.y + buttonSize.y + underlineHeight),
					ImGui::GetColorU32(ImGuiCol_ButtonActive),
					underlineThickness);
			}

			ImGui::SameLine();

			if (ImGui::Button("Weapon", ImVec2(75, 25)))
			{
				tabb = 3;
				selectedButton = 3;

			}


			if (selectedButton == 3 || ImGui::IsItemHovered())
			{
				buttonPos = ImGui::GetItemRectMin();  // Assign buttonPos
				buttonWidth = ImGui::GetItemRectSize().x;  // Assign buttonWidth

				// Draw underline
				drawList->AddLine(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y + underlineHeight),
					ImVec2(buttonPos.x + buttonWidth, buttonPos.y + buttonSize.y + underlineHeight),
					ImGui::GetColorU32(ImGuiCol_ButtonActive),
					underlineThickness);
			}
			ImGui::SameLine();

			if (ImGui::Button("Misc", ImVec2(75, 25)))
			{
				tabb = 4;
				selectedButton = 4;

			}



			if (selectedButton == 4 || ImGui::IsItemHovered())
			{
				buttonPos = ImGui::GetItemRectMin();  // Assign buttonPos
				buttonWidth = ImGui::GetItemRectSize().x;  // Assign buttonWidth

				// Draw underline
				drawList->AddLine(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y + underlineHeight),
					ImVec2(buttonPos.x + buttonWidth, buttonPos.y + buttonSize.y + underlineHeight),
					ImGui::GetColorU32(ImGuiCol_ButtonActive),
					underlineThickness);
			}


			ImGui::SameLine();
			if (ImGui::Button("Config", ImVec2(75, 25)))
			{
				tabb = 5;
				selectedButton = 5;

			}





			if (selectedButton == 5 || ImGui::IsItemHovered())
			{
				buttonPos = ImGui::GetItemRectMin();  // Assign buttonPos
				buttonWidth = ImGui::GetItemRectSize().x;  // Assign buttonWidth

				// Draw underline
				drawList->AddLine(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y + underlineHeight),
					ImVec2(buttonPos.x + buttonWidth, buttonPos.y + buttonSize.y + underlineHeight),
					ImGui::GetColorU32(ImGuiCol_ButtonActive),
					underlineThickness);
			}



			if (tabb == 0) {
				ImGui::Columns(2, " ", false);
				ImGui::SetColumnWidth(0, 220);



				// Create a vertical layout
				ImGui::BeginChild("VerticalLayout", ImVec2(200, 195), true, ImGuiWindowFlags_None);

				ImGui::Text("Player ESP Settings");
				ImGui::Spacing();
				ImGui::Checkbox("Player Skeleton", &PlayerSkeletonToggle);
				ImGui::Checkbox("Player Box", &PlayerBoxToggle);
				ImGui::Checkbox("Player Name", &PlayerNameToggle);
				ImGui::Checkbox("Player Health", &PlayerHealthToggle);
				ImGui::Checkbox("Player Distance", &PlayerDistanceToggle);
				ImGui::Checkbox("Player Hotbar", &PlayerHotbarToggle);
				ImGui::Checkbox("Player Weapon", &PlayerWeaponToggle);





				// End the first child window
				ImGui::EndChild();

				// Start a new line
				ImGui::NewLine();

				// Add a box below the first child
				ImGui::BeginChild("Box2", ImVec2(200, 205), true, ImGuiWindowFlags_None);

				ImGui::Text("Preview Modifications");

				ImGui::Spacing();

				ImGui::Spacing();

				ImGui::Text("Modifications are for menu");

				ImGui::Spacing();

			


				ImGui::Spacing();
				if (PlayerNameToggle == true) {
					ImGui::Text("Modify Preview Name");
					if (ImGui::InputText("##ModifyPreviewName", Previewname, sizeof(Previewname)))
					{
						// Handle the input field being edited
					}

				}
				if (PlayerHealthToggle == true) {

					ImGui::Text("Modify Preview Health");
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);  // Set the rounding of the slider frame

					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.137f, 0.149f, 0.180f, 1.0f));    // Set the background color of the slider frame
					ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.52f, 0.57f, 0.61f, 1.0f));    // Set the background color of the slider frame when hovered
					ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.62f, 0.67f, 0.71f, 1.0f));     // Set the background color of the slider frame when active
					ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.772f, 0.223f, 0.705f, 1.0f));        // Set the color of the slider grab
					ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.772f, 0.223f, 0.705f, 1.0f));  // Set the color of the slider grab when active


					style.GrabMinSize = 1.0f;


					ImGui::SliderInt(" ", &playerHealth, 0, 100);



					ImGui::PopStyleColor(5);  // Restore the ImGui style colors
					ImGui::PopStyleVar(2);    // Restore the ImGui style variables
				}





				ImGui::EndChild();


				ImGui::NextColumn();

				ImGui::BeginChild("HorizontalLayout", ImVec2(0, 420), true, ImGuiWindowFlags_None);
				ImGui::Text("Preview");

				if (PlayerSkeletonToggle == true) {

					PlayerSkeleton();

				}



				if (PlayerBoxToggle == true) {

					DrawPlayerBox();
				}

				if (PlayerNameToggle == true) {
					PlayerName();
				}

				if (PlayerHealthToggle == true) {
					DrawHealthBox();
				}

				ImGui::EndChild();

				ImGui::Columns(1);




			}

				///////////////////////////
				///
				/// AIMBOT
				/// 
				//////////////////////////



			if (tabb == 1) {


				ImGui::Columns(2, " ", false);
				ImGui::SetColumnWidth(0, 220);



				// Create a vertical layout
				ImGui::BeginChild("VerticalLayoutaimbot", ImVec2(200, 195), true, ImGuiWindowFlags_None);

				ImGui::Text("Aimbot Settings");
				ImGui::Spacing();

				ImGui::Checkbox("Head", &HeadAimbot);
				ImGui::Checkbox("Arm", &ArmAimbot);
				ImGui::Checkbox("Chest", &ChestAimbot);
				ImGui::Checkbox("Foot", &FootAimbot);
				ImGui::Checkbox("Leg", &LegAimbot);

				ImGui::Spacing();





				// End the first child window
				ImGui::EndChild();

				// Start a new line
				ImGui::NewLine();

				// Add a box below the first child
				ImGui::BeginChild("Box2aimbot", ImVec2(200, 205), true, ImGuiWindowFlags_None);
				ImGui::Text("Other Settings");
				ImGui::Spacing();
				ImGui::Checkbox("Wounded", &WoundedToggle);
				ImGui::Checkbox("Scientist", &ScientistToggle);
				ImGui::Checkbox("Silent", &SilentToggle);
				ImGui::Checkbox("Triggerbot", &TriggerBotToggle);
				ImGui::Checkbox("Smoothness", &SmoothnessToggle);
				ImGui::Checkbox("MagicBullet", &MagicBulletToggle);
				ImGui::Checkbox("NoRecoil", &NoRecoil);


				ImGui::EndChild();


				ImGui::NextColumn();

				ImGui::BeginChild("HorizontalLayoutaimbot", ImVec2(0, 420), true, ImGuiWindowFlags_None);


				ImGui::Text("Hotkeys:");

				ImGui::Spacing();



				ImGui::Text("Head Aimbot Hotkey");

				ImGui::SameLine();

				if (ImGui::Button(HeadhotkeyText))
				{
					HeadisEditingHotkey = true;
				}

				if (HeadisEditingHotkey)
				{
					ImGuiIO& io = ImGui::GetIO();

				#ifdef _WIN32
					// Windows platform-specific code
					MSG message;
					while (GetMessage(&message, nullptr, 0, 0))
					{
						if (message.message == WM_KEYDOWN)
						{
							int keyCode = int(message.wParam);
							int scanCode = (message.lParam >> 16) & 0xFF;
							char keyName[256];
							if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
							{
								snprintf(HeadAimbotHotkey, sizeof(HeadAimbotHotkey), "%s", keyName);
								HeadisEditingHotkey = false;
								break;
							}
						}
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
			
				}




				ImGui::Spacing();






				ImGui::Text("Arm Aimbot Hotkey");

				ImGui::SameLine();

				if (ImGui::Button(ArmAimbotHotkey))
				{
					ArmisEditingHotkey = true;
				}

				if (ArmisEditingHotkey)
				{
					ImGuiIO& io = ImGui::GetIO();

					#ifdef _WIN32
					// Windows platform-specific code
					MSG message;
					while (GetMessage(&message, nullptr, 0, 0))
					{
						if (message.message == WM_KEYDOWN)
						{
							int keyCode = int(message.wParam);
							int scanCode = (message.lParam >> 16) & 0xFF;
							char keyName[256];
							if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
							{
								snprintf(ArmAimbotHotkey, sizeof(ArmAimbotHotkey), "%s", keyName);
								ArmisEditingHotkey = false;
								break;
							}
						}
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
					#elif defined(__APPLE__)
										// macOS platform-specific code
										// Use the appropriate method for capturing non-printable keys on macOS
					#else
										// Linux/Unix platform-specific code
										// Use the appropriate method for capturing non-printable keys on Linux/Unix
					#endif
				}








				ImGui::Spacing();


				ImGui::Text("Chest Aimbot Hotkey");

				ImGui::SameLine();

				if (ImGui::Button(ChestAimbotHotkey))
				{
					ChestisEditingHotkey = true;
				}

				if (ChestisEditingHotkey)
				{
					ImGuiIO& io = ImGui::GetIO();

#ifdef _WIN32
					// Windows platform-specific code
					MSG message;
					while (GetMessage(&message, nullptr, 0, 0))
					{
						if (message.message == WM_KEYDOWN)
						{
							int keyCode = int(message.wParam);
							int scanCode = (message.lParam >> 16) & 0xFF;
							char keyName[256];
							if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
							{
								snprintf(ChestAimbotHotkey, sizeof(ChestAimbotHotkey), "%s", keyName);
								ChestisEditingHotkey = false;
								break;
							}
						}
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
					#elif defined(__APPLE__)
										// macOS platform-specific code
										// Use the appropriate method for capturing non-printable keys on macOS
					#else
										// Linux/Unix platform-specific code
										// Use the appropriate method for capturing non-printable keys on Linux/Unix
					#endif
				}


				ImGui::Spacing();

				ImGui::Text("Foot Aimbot Hotkey");

				ImGui::SameLine();

				if (ImGui::Button(FootAimbotHotkey))
				{
					FootisEditingHotkey = true;
				}

				if (FootisEditingHotkey)
				{
					ImGuiIO& io = ImGui::GetIO();

#ifdef _WIN32
					// Windows platform-specific code
					MSG message;
					while (GetMessage(&message, nullptr, 0, 0))
					{
						if (message.message == WM_KEYDOWN)
						{
							int keyCode = int(message.wParam);
							int scanCode = (message.lParam >> 16) & 0xFF;
							char keyName[256];
							if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
							{
								snprintf(FootAimbotHotkey, sizeof(FootAimbotHotkey), "%s", keyName);
								FootisEditingHotkey = false;
								break;
							}
						}
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
#elif defined(__APPLE__)
					// macOS platform-specific code
					// Use the appropriate method for capturing non-printable keys on macOS
#else
					// Linux/Unix platform-specific code
					// Use the appropriate method for capturing non-printable keys on Linux/Unix
#endif
				}

				ImGui::Spacing();

				ImGui::Text("Leg Aimbot Hotkey");

				ImGui::SameLine();

				if (ImGui::Button(LegAimbotHotkey))
				{
					LegisEditingHotkey = true;
				}

				if (LegisEditingHotkey)
				{
					ImGuiIO& io = ImGui::GetIO();

#ifdef _WIN32
					// Windows platform-specific code
					MSG message;
					while (GetMessage(&message, nullptr, 0, 0))
					{
						if (message.message == WM_KEYDOWN)
						{
							int keyCode = int(message.wParam);
							int scanCode = (message.lParam >> 16) & 0xFF;
							char keyName[256];
							if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
							{
								snprintf(LegAimbotHotkey, sizeof(LegAimbotHotkey), "%s", keyName);
								LegisEditingHotkey = false;
								break;
							}
						}
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
#elif defined(__APPLE__)
					// macOS platform-specific code
					// Use the appropriate method for capturing non-printable keys on macOS
#else
					// Linux/Unix platform-specific code
					// Use the appropriate method for capturing non-printable keys on Linux/Unix
#endif
				}






				ImGui::EndChild();

				ImGui::Columns(1);



			}



			///////////////////////////
			///
			/// ESP
			/// 
			//////////////////////////




			if (tabb == 2) {
			

				ImGui::Columns(2, " ", false);



				ImGui::BeginChild("##TC", ImVec2(260, 100), true, ImGuiWindowFlags_None);
					ImGui::Text("TC Settings");
					ImGui::Checkbox("TC Auth", &TCAuthToggle);
					ImGui::Checkbox("TC Upkeep", &UpKeepToggle);
					ImGui::Checkbox("TC Esp", &TcEspToggle);

				ImGui::EndChild();



				ImGui::BeginChild("##Base", ImVec2(260, 215), true, ImGuiWindowFlags_None);
					ImGui::Text("Base Settings");
					ImGui::Checkbox("Large Storage", &LargeStorage);
					ImGui::Checkbox("Small Storage", &SmallStorage);
					ImGui::Checkbox("Coffins", &Coffins);
					ImGui::Checkbox("Vending Machine", &VendingMachine);
					ImGui::Checkbox("Sleeping Bag", &SleepingBag);
					ImGui::Checkbox("Workbench 1", &Workbench1);
					ImGui::Checkbox("Workbench 2", &Workbench2);
					ImGui::Checkbox("Workbench 3", &Workbench3);
				ImGui::EndChild();
				

				ImGui::BeginChild("##Ore", ImVec2(260, 100), true, ImGuiWindowFlags_None);
					ImGui::Text("Ore Settings");
					ImGui::Checkbox("Stone Ore", &StoneOreToggle);
					ImGui::Checkbox("Metal Ore", &MetalOreToggle);
					ImGui::Checkbox("Sulfur Ore", &SulfurOreToggle);
				ImGui::EndChild();




				ImGui::NextColumn();



				ImGui::BeginChild("##Collectibles", ImVec2(260, 145), true, ImGuiWindowFlags_None);
					ImGui::Text("Collectibles Settings");
					ImGui::Checkbox("Metal", &MetalPickup);
					ImGui::Checkbox("Stone", &StonePickup);
					ImGui::Checkbox("Sulfur", &SulfurPickup);
					ImGui::Checkbox("Hemp", &HempPickup);
					ImGui::Checkbox("Wood", &WoodPickup);
				ImGui::EndChild();


				ImGui::BeginChild("##Other", ImVec2(260, 275), true, ImGuiWindowFlags_None);
					ImGui::Text("Other Settings");
					ImGui::Checkbox("Stashes", &Stashes);
					ImGui::Checkbox("Airdop", &AirDrop);
					ImGui::Checkbox("Backpacks", &Backpacks);
					ImGui::Checkbox("Heli Crates", &HeliCrates);
					ImGui::Checkbox("Brown Crates", &BrownCrates);
					ImGui::Checkbox("Mili Crates", &MiliCrate);
					ImGui::Checkbox("Eliete Crates", &EliteCrate);
				ImGui::EndChild();
				

				ImGui::Columns(1);

			}



			///////////////////////////
			///
			/// WEAPON
			/// 
			//////////////////////////


			if (tabb == 3) {
				ImGui::Columns(2, " ", false);

				

					ImGui::BeginChild("OtherGuns", ImVec2(0, 100), true, ImGuiWindowFlags_None);
					ImGui::Text("Other Guns");
					ImGui::Spacing();
						ImGui::Checkbox("Rocketlauncher", &RocketlauncherESP);
						ImGui::Checkbox("Eoka", &EokaESP);
					ImGui::EndChild();

					ImGui::BeginChild("5.56Guns", ImVec2(0, 230), true, ImGuiWindowFlags_None);
						ImGui::Text("5.56 Bullets");
						ImGui::Spacing();
						ImGui::Checkbox("AK47", &AK47ESP);
						ImGui::Checkbox("LR300", &LR300ESP);
						ImGui::Checkbox("HMLMG", &HMLMGESP);
						ImGui::Checkbox("M249", &M249ESP);
						ImGui::Checkbox("L96", &L96ESP);
						ImGui::Checkbox("Bolt", &BoltactionESP);
						ImGui::Checkbox("Semi Auto Rifle", &SARESP);
						ImGui::Checkbox("M39", &M39ESP);
					ImGui::EndChild();

					ImGui::NextColumn();

					ImGui::BeginChild("PistolBullets", ImVec2(0, 200), true, ImGuiWindowFlags_None);
						ImGui::Text("Pistol Bullets");
						ImGui::Spacing();
						ImGui::Checkbox("MP5", &MP5ESP);
						ImGui::Checkbox("Custom", &CUSTOMSMGESP);
						ImGui::Checkbox("Thompson", &THOMPSONESP);
						ImGui::Checkbox("Revolver", &REVOLVERESP);
						ImGui::Checkbox("Python", &PYTHONESP);
						ImGui::Checkbox("Semi Auto Pistol", &SAPESP);
						ImGui::Checkbox("M92", &M92ESP);
					ImGui::EndChild();

					ImGui::BeginChild("ShotgunRounds", ImVec2(0, 200), true, ImGuiWindowFlags_None);
						ImGui::Text("Shotgun Rounds");
						ImGui::Spacing();
						ImGui::Checkbox("Pump", &PUMPESP);
						ImGui::Checkbox("Water Pipe", &WATERPIPEESP);
						ImGui::Checkbox("Double Barrel", &DOUBLEBARRELESP);
						ImGui::Checkbox("Spas-12", &SPASESP);
						ImGui::Checkbox("Grenade Launcher", &MGLESP);
					ImGui::EndChild();



				ImGui::Columns(1);


			}



			///////////////////////////
			///
			/// MISC
			/// 
			//////////////////////////



			if (tabb == 4) {

				ImGui::Checkbox("FOV", &FOVToggle);

				if (FOVToggle) {


					ImGui::SliderInt(" ", &i, 0, 100);
				}


				ImGui::Text("Movement");

				ImGui::Checkbox("SpiderMan", &SpiderMan);
				ImGui::Checkbox("NoFall", &NoFall);

				ImGui::Text("Admin");

				ImGui::Checkbox("Admin Mode", &AdminMode);


			}


			///////////////////////////
			///
			/// CONFIG
			/// 
			//////////////////////////

			style.Colors[ImGuiCol_Button] = ImVec4(0.137f, 0.149f, 0.180f, 1.0f);
			if (tabb == 5) 
			{

				if (ImGui::Button("Save Settings"))
				{
					SaveSettingsToJson();
				}

				if (ImGui::Button("Load Settings"))
				{
					LoadSettingsFromJson();
				}



			}
			

		

			ImGui::End();



	
	


}

