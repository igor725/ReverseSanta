#pragma once

#include <Windows.h>
#include <string>

class Language {
public:
	Language(std::string &path);
	~Language();

	LPCWSTR Get(DWORD id);

private:
	DWORD m_dwOffsetsSize, m_dwStringsSize;
	LPSTR m_lpStrings;
	LPDWORD m_lpdwOffsets;
};
