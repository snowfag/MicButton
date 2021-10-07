// MicButton.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MicButton.h"
#include "Winuser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

using namespace std;

HRESULT hr;
IAudioEndpointVolume *endpointVolume = NULL;
IMMDevice *defaultDevice = NULL;
IMMDeviceCollection *pEndpoints;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fEatKeystroke = FALSE;

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
			if (fEatKeystroke = (p->vkCode == 0x7C)) {
				if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) // Keydown
				{
					hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
					hr = endpointVolume->SetMute(FALSE, NULL);
					endpointVolume->Release();
				}
				else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) // Keyup
				{
					hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
					hr = endpointVolume->SetMute(TRUE, NULL);
					endpointVolume->Release();
				}
				break;
			}
			break;
		}
	}
	return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}
int main()
{
	FreeConsole();
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(nullptr);
	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	if (hModule != nullptr)
	{
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			CoInitialize(NULL);
			IMMDeviceEnumerator *deviceEnumerator = NULL;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);

			bool pressed = false;
			hr = deviceEnumerator->EnumAudioEndpoints(
				eRender, DEVICE_STATE_ACTIVE,
				&pEndpoints);
			hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &defaultDevice);
			deviceEnumerator->Release();
			deviceEnumerator = NULL;


			hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);


			hr = endpointVolume->SetMute(TRUE, NULL);
			endpointVolume->Release();


			MSG msg;
			while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	else
	{
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}


	UnhookWindowsHookEx(hhkLowLevelKybd);

	return nRetCode;
}
