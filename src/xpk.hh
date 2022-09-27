#pragma once

#include <Windows.h>
#include <fstream>
#include <vector>

class Xpk {
public:
	struct XpkFile {
		DWORD dwNameOffset;
		DWORD dwSize, dwOffset;
		DWORD dwCreated, dwModified;
	};

	Xpk(std::string fname);

	BOOL SearchFile(XpkFile &cfile, DWORD *end, DWORD *size);
	BOOL SearchFile(std::string name, DWORD *end, DWORD *size);

	inline std::vector<XpkFile> &GetFiles() { return m_fileMap; }
	inline std::ifstream *GetHandle() { return &m_fArchive; }

private:
	std::ifstream m_fArchive;
	std::vector<XpkFile> m_fileMap;
	LPCSTR m_lpNameMap;
	DWORD m_dwFileCount,
	m_dwContentOffset;
};
