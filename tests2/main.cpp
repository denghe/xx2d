#include "main.h"

#define DIRECTINPUT_VERSION 0x0800

#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#include <Windows.h>

#include <cassert>
#include <cstdio>
#include <memory>
#include <new>

#include <wrl/client.h>

#include <commctrl.h>
#include <basetsd.h>

#pragma warning(push)
#pragma warning(disable:6000 28251)
#include <dinput.h>
#pragma warning(pop)

#include <dinputd.h>
#include <oleauto.h>
#include <shellapi.h>
using namespace Microsoft::WRL;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "comctl32.lib")

LPDIRECTINPUT8          g_pDI = nullptr;
LPDIRECTINPUTDEVICE8    g_pJoystick = nullptr;
struct DI_ENUM_CONTEXT {
    DIJOYCONFIG* pPreferredJoyCfg;
    bool bPreferredJoyCfgValid;
};


//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
    VOID* pContext) noexcept {
    auto pEnumContext = static_cast<DI_ENUM_CONTEXT*>(pContext);

    // Skip anything other than the perferred joystick device as defined by the control panel.  
    // Instead you could store all the enumerated joysticks and let the user pick.
    if (pEnumContext->bPreferredJoyCfgValid &&
        !IsEqualGUID(pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance))
        return DIENUM_CONTINUE;

    // Obtain an interface to the enumerated joystick.
    HRESULT hr = g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, nullptr);

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if (FAILED(hr))
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}


//-----------------------------------------------------------------------------
// Name: InitDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
HRESULT InitDirectInput(/*HWND hDlg*/) noexcept {
    HRESULT hr;

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    // Create a DInput object
    if (FAILED(hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
        IID_IDirectInput8, (VOID**)&g_pDI, nullptr)))
        return hr;

    DIJOYCONFIG PreferredJoyCfg = {};
    DI_ENUM_CONTEXT enumContext;
    enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
    enumContext.bPreferredJoyCfgValid = false;

    ComPtr<IDirectInputJoyConfig8> pJoyConfig;
    if (FAILED(hr = g_pDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
        return hr;

    PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);
    if (SUCCEEDED(pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE))) {
        // This function is expected to fail if no joystick is attached
        enumContext.bPreferredJoyCfgValid = true;
    }

    // Look for a simple joystick we can use for this sample program.
    if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
        EnumJoysticksCallback,
        &enumContext, DIEDFL_ATTACHEDONLY)))
        return hr;

    // Make sure we got a joystick
    if (!g_pJoystick) {
        //MessageBoxW(nullptr, L"Joystick not found. The sample will now exit.",
        //    L"DirectInput Sample",
        //    MB_ICONERROR | MB_OK);
        //EndDialog(hDlg, 0);
        std::cout << "Joystick not found" << std::endl;
        return S_OK;
    }

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
    if (FAILED(hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2)))
        return hr;


    return S_OK;
}

double minInterval{ 99999 }, lastSeconds{};
std::array<bool, 128> btnPressedStates;

//-----------------------------------------------------------------------------
// Name: UpdateInputState()
// Desc: Get the input device's state and display it.
//-----------------------------------------------------------------------------
HRESULT UpdateInputState(/*HWND hDlg*/) noexcept {
    if (!g_pJoystick)
        return S_OK;


    // Poll the device to read the current state
    HRESULT hr = g_pJoystick->Poll();
    if (FAILED(hr)) {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = g_pJoystick->Acquire();
        while (hr == DIERR_INPUTLOST)
            hr = g_pJoystick->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK;
    }

    // Get the input's device state
    DIJOYSTATE2 js;
    if (FAILED(hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
        return hr; // The device should have been acquired during the Poll()



    //bool anyButtonPressed = false;
    //for (int i = 0; i < 128; i++) {
    //    if (js.rgbButtons[i] & 0x80) {
    //        if (anyButtonPressed) {
    //            std::cout << " ";
    //        } else {
    //            anyButtonPressed = true;
    //        }
    //        std::cout << i;
    //    }
    //}
    //if (anyButtonPressed) {
    //    std::cout << std::endl;
    //}

    std::array<bool, 128> bs;
    for (int i = 0; i < 128; i++) {
        bs[i] = js.rgbButtons[i] & 0x80;
    }

    auto secs = xx::NowSteadyEpochSeconds();
    if (btnPressedStates != bs) {
        btnPressedStates = bs;
        auto d = secs - lastSeconds;
        lastSeconds = secs;
        if (d > 0 && d < minInterval) {
            minInterval = d;
            std::cout << minInterval << std::endl;
        }
    }

    return S_OK;
}


int main() {

	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		return -1;
	InitCommonControls();

	if (FAILED(InitDirectInput())) return -2;
    g_pJoystick->Acquire();

    while (true) {
        if (FAILED(UpdateInputState())) return -3;
    }

	CoUninitialize();
	return 0;
}




void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);

	monsters.EmplaceShared(*this);
}

int GameLooper::Update() {
	timePool += xx::engine.delta;
	while (timePool >= fds) {
		timePool -= fds;
		
		for (auto& m : monsters) {
			m->coro();
		}
	}

	fpsViewer.Draw(fontBase);
	return 0;
}


//
//struct A {
//	int64_t typeId = 0;
//};
//
//struct B {
//	int64_t typeId = 1;
//	bool b1{true};
//};
//
//struct C1 : B {
//	bool b2{};
//};
//
//struct C2 {
//	int64_t typeId = 2;
//	bool b1{ true };
//	bool b2{};
//};
//
//struct Foo {
//	int typeId = 1;
//	~Foo() {
//		std::cout << "~Foo " << typeId << std::endl;
//	}
//};
//struct Bar {
//	int typeId = 2;
//	~Bar() {
//		std::cout << "~Bar " << typeId << std::endl;
//	}
//};
//
//int main() {
//	xx::Shared<A> c1, c2;
//	c1.Emplace<C1>();
//	c2.Emplace<C2>();
//
//	std::cout << c1->typeId << std::endl;
//	std::cout << c2->typeId << std::endl;
//	return 0;
//
//
//	auto g = std::make_unique<GameLooper>();
//	return g->Run("tests2");
//}
