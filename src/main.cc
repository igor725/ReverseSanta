#include "engine.hh"

#include <ShlObj.h>

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, INT iCmdShow) {
	(VOID)hPrev;(VOID)szCmdLine;(VOID)iCmdShow;
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen("CONOUT$", "w", stderr);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	if (std::strcmp(szCmdLine, "unpack") >= 0) {
		Xpk xpk("xmas.xpk");
		std::ifstream *handle = xpk.GetHandle();
		for (auto &file : xpk.GetFiles()) {
			DWORD size;
			std::string name;
			if (xpk.SearchFile(file, nullptr, &size)) {
				auto sv = handle->tellg();
				xpk.ReadFileName(file, name);
				handle->seekg(sv, std::ios::beg);
				auto pos = name.find_last_of('\\');
				if (pos != name.npos)
					CreateDirectoryA((".\\" + std::string{name, 0, pos}).c_str(), nullptr);

				std::ofstream ofile(name, std::ios::binary);

				if (ofile.is_open()) {
					char buf[512];
					while (size > 0) {
						DWORD cur = min(512, size);
						handle->read(buf, cur);
						ofile.write(buf, cur);
						size -= cur;
					}
					ofile.close();
				}
			}
		}
		return 0;
	}

	Engine engine(hInst);

	MSG msg;
	DWORD ct = 0, lt;

	while (auto run = true) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT) run = false;
		}

		lt = ct;
		ct = GetTickCount();
		engine.Step((ct - lt) / 1000.0f);
	}

	return (INT)msg.wParam;
}
