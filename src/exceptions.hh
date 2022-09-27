#pragma once

#include <string>
#include <exception>
#include <Windows.h>

struct MyException : public std::exception {
public:
	MyException(LPCWSTR path);
	MyException(std::exception &ex);
	MyException(std::exception_ptr ptr);
	~MyException();

	VOID Alert();

	LPCWSTR GetCallStack();

	template<typename... Ts>
	VOID ApplyFormattedMessage(LPCWSTR format, Ts... args) {
		auto sz = std::swprintf(nullptr, 0, format, args...) + 2;
		m_lpMessage = (LPWSTR)LocalAlloc(LMEM_ZEROINIT, (sz * 2) + 2);
		if (m_lpMessage) std::swprintf(m_lpMessage, sz, format, args...);
	}

private:
	LPWSTR m_lpMessage = nullptr;
	std::wstring m_lpCallStack;
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
	D3DException(HRESULT code, LPCWSTR path);

private:
	LPCWSTR m_lpFormat = L"D3D9 error occured: %ls\r\n%ls";
};

struct DIException : public MyException {
public:
	DIException(HRESULT code, LPCWSTR path);

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
