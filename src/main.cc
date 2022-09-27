#define _CRT_SECURE_NO_WARNINGS
#include "engine.hh"
#include "exceptions.hh"
#include <ShlObj.h>

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, INT iCmdShow) {
	(VOID)hPrev;(VOID)szCmdLine;(VOID)iCmdShow;
	// ::AttachConsole(ATTACH_PARENT_PROCESS);
	// ::freopen("CONOUT$", "w", stderr);
	// HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
	// ::SetConsoleMode(hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	try {
		Engine engine(hInst);

		MSG msg = {};
		DWORD ct = 0, lt;

		while (true) {
			while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					return (INT)msg.wParam;
			}

			lt = ct;
			ct = ::GetTickCount();
			engine.Step((ct - lt) / 1000.0f);
		}
	} catch (MyException &mex) {
		mex.Alert();
	} catch (std::exception &ex) {
		MyException mex(ex);
		mex.Alert();
	} catch (...) {
		MyException mex(std::current_exception());
		mex.Alert();
	}

	return 0;
}
