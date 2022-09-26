#define _CRT_SECURE_NO_WARNINGS
#include "engine.hh"
#include "exceptions.hh"

#include <iostream>
#include <ShlObj.h>

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, INT iCmdShow) {
	(VOID)hPrev;(VOID)szCmdLine;(VOID)iCmdShow;

	if (std::strcmp(szCmdLine, "unpack") >= 0) {
		::AttachConsole(ATTACH_PARENT_PROCESS);
		::freopen("CONOUT$", "w", stderr);
		HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
		::SetConsoleMode(hConsole, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

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
					::CreateDirectoryA((".\\" + std::string{name, 0, pos}).c_str(), nullptr);

				std::ofstream ofile(name, std::ios::binary);

				if (ofile.is_open()) {
					std::cerr << "Extracting " << name << "...";
					char buf[512];
					while (size > 0) {
						if (!handle->good() || handle->eof() || !ofile.good()) {
							std::cerr << "error!" << std::endl;
							break;
						}
						DWORD cur = min(512, size);
						handle->read(buf, cur);
						ofile.write(buf, cur);
						size -= cur;
					}
					ofile.close();
					std::cerr << "done!" << std::endl;
				}
			}
		}

		return 0;
	}

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
