#pragma once

#include <Windows.h>
#include <exception>
#include <iostream>

struct MyException : public std::exception {
public:
	MyException(LPCWSTR path) : m_lpFile(path) {}

	MyException(std::exception_ptr ptr) {
		ApplyFormattedMessage(L"Uncaught exception: %p", &ptr);
	}

	~MyException() {
		::LocalFree(m_lpMessage);
	}

	LPCWSTR GetFileInfo() {
		return m_lpFile;
	}

	LPWSTR GetPointer() {
		return (LPWSTR)&m_lpMessage;
	}

	LPWSTR GetString() {
		return m_lpMessage;
	}

	template<typename... Ts>
	void ApplyFormattedMessage(LPCWSTR format, Ts... args) {
		auto sz = std::swprintf(nullptr, 0, format, args...) + 2;
		m_lpMessage = (LPWSTR)LocalAlloc(LMEM_ZEROINIT, (sz * 2) + 2);
		if (m_lpMessage) std::swprintf(m_lpMessage, sz, format, args...);
	}

private:
	LPWSTR m_lpMessage = nullptr;
	LPCWSTR m_lpFile = nullptr;
};

struct AssertException : public MyException {
public:
	AssertException(LPCWSTR cond, LPCWSTR path) : MyException(path) {
		ApplyFormattedMessage(m_lpFormat, cond, GetFileInfo());
	}

private:
	LPCWSTR m_lpFormat = L"Assertion failed: %ls";
};

#define D3DECASE(N) case N: \
	codestr = L ## #N;\
	break;

struct D3DException : public MyException {
public:
	D3DException(HRESULT code, LPCWSTR path) : MyException(path) {
		WCHAR hexbuf[24] = {0};
		LPCWSTR codestr = L"D3DERR_UNKNOWN";
		switch (code) {
			D3DECASE(D3DERR_WRONGTEXTUREFORMAT);
			D3DECASE(D3DERR_UNSUPPORTEDCOLOROPERATION);
			D3DECASE(D3DERR_UNSUPPORTEDCOLORARG);
			D3DECASE(D3DERR_UNSUPPORTEDALPHAOPERATION);
			D3DECASE(D3DERR_UNSUPPORTEDALPHAARG);
			D3DECASE(D3DERR_TOOMANYOPERATIONS);
			D3DECASE(D3DERR_CONFLICTINGTEXTUREFILTER);
			D3DECASE(D3DERR_UNSUPPORTEDFACTORVALUE);
			D3DECASE(D3DERR_CONFLICTINGRENDERSTATE);
			D3DECASE(D3DERR_UNSUPPORTEDTEXTUREFILTER);
			D3DECASE(D3DERR_CONFLICTINGTEXTUREPALETTE);
			D3DECASE(D3DERR_DRIVERINTERNALERROR);
			D3DECASE(D3DERR_NOTFOUND);
			D3DECASE(D3DERR_MOREDATA);
			D3DECASE(D3DERR_DEVICELOST);
			D3DECASE(D3DERR_DEVICENOTRESET);
			D3DECASE(D3DERR_NOTAVAILABLE);
			D3DECASE(D3DERR_OUTOFVIDEOMEMORY);
			D3DECASE(D3DERR_INVALIDDEVICE);
			D3DECASE(D3DERR_INVALIDCALL);
			D3DECASE(D3DERR_DRIVERINVALIDCALL);
			D3DECASE(D3DERR_WASSTILLDRAWING);
			D3DECASE(D3DOK_NOAUTOGEN);

#if !defined(D3D_DISABLE_9EX)
			D3DECASE(D3DERR_DEVICEREMOVED);
			D3DECASE(S_NOT_RESIDENT);
			D3DECASE(S_RESIDENT_IN_SHARED_MEMORY);
			D3DECASE(S_PRESENT_MODE_CHANGED);
			D3DECASE(S_PRESENT_OCCLUDED);
			D3DECASE(D3DERR_DEVICEHUNG);
			D3DECASE(D3DERR_UNSUPPORTEDOVERLAY);
			D3DECASE(D3DERR_UNSUPPORTEDOVERLAYFORMAT);
			D3DECASE(D3DERR_CANNOTPROTECTCONTENT);
			D3DECASE(D3DERR_UNSUPPORTEDCRYPTO);
			D3DECASE(D3DERR_PRESENT_STATISTICS_DISJOINT);
#endif

			D3DECASE(E_FAIL);
			D3DECASE(E_INVALIDARG);
			D3DECASE(E_OUTOFMEMORY);

			default:
				if (std::swprintf(hexbuf, 12, L"0x%X", code) > 0)
					codestr = hexbuf;
				break;
		}

		ApplyFormattedMessage(m_lpFormat, codestr, GetFileInfo());
	}
private:
	LPCWSTR m_lpFormat = L"D3D9 error occured: %ls\r\n%ls";
};

#define WIDE2(x) L##x
#define WIDE(x) WIDE2(x)
#define TOSTR2(x) #x
#define TOSTR(x) TOSTR2(x)

#define EAFINFO L"File: " WIDE(__FILE__) L":" WIDE(TOSTR(__LINE__))
#define EASSERT(C) if ((C) == 0) throw AssertException(WIDE(#C), EAFINFO);
#define DASSERT(E) if (auto _ECODE = (E)) \
	if (_ECODE != S_OK && _ECODE != D3DERR_DEVICELOST && _ECODE != D3DERR_DEVICENOTRESET) \
		throw D3DException(_ECODE, EAFINFO)
