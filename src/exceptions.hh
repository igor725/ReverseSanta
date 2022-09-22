#pragma once

#include <string>
#include <exception>
#include <Windows.h>
#include <DbgHelp.h>
#include <d3d9.h>
#include <dinput.h>

struct MyException : public std::exception {
	LPCWSTR TrimPath(LPCWSTR path) {
		LPCWSTR prev[2] = {path, nullptr};

		while (*path != L'\0') {
			if (*path++ == L'\\') {
				prev[0] = prev[1];
				prev[1] = path;
			}
		}

		return prev[0];
	}

public:
	MyException(LPCWSTR path) : m_lpCallStack(TrimPath(path)) {
		m_lpCallStack += L"\r\n\r\n";

		if (!SymInitializeW(GetCurrentProcess(), nullptr, true)) {
			m_lpCallStack += L"SymInitialize() failed!";
			return;
		}

		LPVOID stack[16];
		CHAR buffer[sizeof(SYMBOL_INFOW) + (MAX_SYM_NAME * 2)];

		PSYMBOL_INFOW sym = (PSYMBOL_INFOW)buffer;
		sym->MaxNameLen = MAX_SYM_NAME;
		sym->SizeOfStruct = sizeof(SYMBOL_INFOW);

		WORD frames = CaptureStackBackTrace(2, 16, stack, nullptr);
		IMAGEHLP_LINEW64 ln = {};
		ln.SizeOfStruct = sizeof(IMAGEHLP_LINEW64);

		WCHAR framestr[256];
		for (WORD i = 0; i < frames; i++) {
			if (SymFromAddrW(GetCurrentProcess(), (DWORD64)stack[i], nullptr, sym)) {
				if (std::swprintf(framestr, 256, L"Frame #%d: %ls = %p\r\n", i, sym->Name, (LPVOID)sym->Address) > 0)
					m_lpCallStack += framestr;
				if (SymGetLineFromAddrW64(GetCurrentProcess(), (DWORD64)sym->Address, (PDWORD)&stack[i], &ln)) {
					if (std::swprintf(framestr, 256, L"\tin %ls:%lu\r\n\r\n", TrimPath(ln.FileName), ln.LineNumber) > 0)
						m_lpCallStack += framestr;
				} else
					m_lpCallStack += L"\tin ...\r\n\r\n";

				/* Дальше по стеку идут неинтересные CRT функции винды */
				if (sym->Address == (DWORD64)&WinMain) break;
			} else {
				m_lpCallStack += L"(SysFromAddr failed here)";
				break;
			}
		}

		SymCleanup(GetCurrentProcess());
	}

	MyException(std::exception_ptr ptr) {
		ApplyFormattedMessage(L"Uncaught exception: %p\r\n%ls", &ptr, GetCallStack());
	}

	~MyException() {
		::LocalFree(m_lpMessage);
	}

	VOID Alert() {
		::MessageBox(nullptr, m_lpMessage, L"Exception thrown", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	LPCWSTR GetCallStack() {
		return m_lpCallStack.c_str();
	}

	LPWSTR GetPointer() {
		return (LPWSTR)&m_lpMessage;
	}

	template<typename... Ts>
	VOID ApplyFormattedMessage(LPCWSTR format, Ts... args) {
		auto sz = std::swprintf(nullptr, 0, format, args...) + 2;
		m_lpMessage = (LPWSTR)LocalAlloc(LMEM_ZEROINIT, (sz * 2) + 2);
		if (m_lpMessage) std::swprintf(m_lpMessage, sz, format, args...);
	}

private:
	LPWSTR m_lpMessage = nullptr;
	std::wstring m_lpCallStack = nullptr;
};

struct AssertException : public MyException {
public:
	AssertException(LPCWSTR cond, LPCWSTR path) : MyException(path) {
		ApplyFormattedMessage(m_lpFormat, cond, GetCallStack());
	}

private:
	LPCWSTR m_lpFormat = L"Assertion failed: %ls\r\n%ls";
};

#define CODECASE(N) case N: \
	codestr = L ## #N;\
	break;

struct D3DException : public MyException {
public:
	D3DException(HRESULT code, LPCWSTR path) : MyException(path) {
		WCHAR hexbuf[24] = {0};
		LPCWSTR codestr = L"D3DERR_UNKNOWN";
		switch (code) {
			CODECASE(D3DERR_WRONGTEXTUREFORMAT);
			CODECASE(D3DERR_UNSUPPORTEDCOLOROPERATION);
			CODECASE(D3DERR_UNSUPPORTEDCOLORARG);
			CODECASE(D3DERR_UNSUPPORTEDALPHAOPERATION);
			CODECASE(D3DERR_UNSUPPORTEDALPHAARG);
			CODECASE(D3DERR_TOOMANYOPERATIONS);
			CODECASE(D3DERR_CONFLICTINGTEXTUREFILTER);
			CODECASE(D3DERR_UNSUPPORTEDFACTORVALUE);
			CODECASE(D3DERR_CONFLICTINGRENDERSTATE);
			CODECASE(D3DERR_UNSUPPORTEDTEXTUREFILTER);
			CODECASE(D3DERR_CONFLICTINGTEXTUREPALETTE);
			CODECASE(D3DERR_DRIVERINTERNALERROR);
			CODECASE(D3DERR_NOTFOUND);
			CODECASE(D3DERR_MOREDATA);
			CODECASE(D3DERR_DEVICELOST);
			CODECASE(D3DERR_DEVICENOTRESET);
			CODECASE(D3DERR_NOTAVAILABLE);
			CODECASE(D3DERR_OUTOFVIDEOMEMORY);
			CODECASE(D3DERR_INVALIDDEVICE);
			CODECASE(D3DERR_INVALIDCALL);
			CODECASE(D3DERR_DRIVERINVALIDCALL);
			CODECASE(D3DERR_WASSTILLDRAWING);
			CODECASE(D3DOK_NOAUTOGEN);

#if !defined(D3D_DISABLE_9EX)
			CODECASE(D3DERR_DEVICEREMOVED);
			CODECASE(S_NOT_RESIDENT);
			CODECASE(S_RESIDENT_IN_SHARED_MEMORY);
			CODECASE(S_PRESENT_MODE_CHANGED);
			CODECASE(S_PRESENT_OCCLUDED);
			CODECASE(D3DERR_DEVICEHUNG);
			CODECASE(D3DERR_UNSUPPORTEDOVERLAY);
			CODECASE(D3DERR_UNSUPPORTEDOVERLAYFORMAT);
			CODECASE(D3DERR_CANNOTPROTECTCONTENT);
			CODECASE(D3DERR_UNSUPPORTEDCRYPTO);
			CODECASE(D3DERR_PRESENT_STATISTICS_DISJOINT);
#endif

			CODECASE(E_FAIL);
			CODECASE(E_INVALIDARG);
			CODECASE(E_OUTOFMEMORY);
			CODECASE(E_NOINTERFACE);
			CODECASE(E_ACCESSDENIED);

			default:
				if (std::swprintf(hexbuf, 12, L"0x%X", code) > 0)
					codestr = hexbuf;
				break;
		}

		ApplyFormattedMessage(m_lpFormat, codestr, GetCallStack());
	}
private:
	LPCWSTR m_lpFormat = L"D3D9 error occured: %ls\r\n%ls";
};

struct DIException : public MyException {
public:
	DIException(HRESULT code, LPCWSTR path) : MyException(path) {
		WCHAR hexbuf[24] = {0};
		LPCWSTR codestr = L"DIERR_UNKNOWN";
		switch (code) {
			CODECASE(DIERR_BETADIRECTINPUTVERSION);
			CODECASE(DIERR_OLDDIRECTINPUTVERSION);
			CODECASE(DIERR_DEVICENOTREG);
			CODECASE(DIERR_NOTINITIALIZED);
			CODECASE(DIERR_ACQUIRED);


			CODECASE(E_FAIL);
			CODECASE(E_HANDLE);
			CODECASE(E_INVALIDARG);
			CODECASE(E_OUTOFMEMORY);
			CODECASE(E_NOINTERFACE);
			CODECASE(E_ACCESSDENIED);

			default:
				if (std::swprintf(hexbuf, 12, L"0x%X", code) > 0)
					codestr = hexbuf;
				break;
		}

		ApplyFormattedMessage(m_lpFormat, codestr, GetCallStack());
	}
private:
	LPCWSTR m_lpFormat = L"DInput8 error occured: %ls\r\n%ls";
};

#define WIDE2(x) L##x
#define WIDE(x) WIDE2(x)
#define TOSTR2(x) #x
#define TOSTR(x) TOSTR2(x)

#define EAFINFO L"File: " WIDE(__FILE__) L":" WIDE(TOSTR(__LINE__))
#define EASSERT(C) if ((C) == 0) throw AssertException(WIDE(#C), EAFINFO);
#define DASSERT(E) if (auto _ECODE_ = (E)) \
	if (_ECODE_ != D3DERR_DEVICELOST && _ECODE_ != D3DERR_DEVICENOTRESET) \
		throw D3DException(_ECODE_, EAFINFO)

#define IASSERT(E) if (auto _ECODE_ = (E)) \
	throw DIException(_ECODE_, EAFINFO)
