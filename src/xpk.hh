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

	bool SearchFile(XpkFile &cfile, DWORD *end, DWORD *size);

	std::string &ReadFileName(XpkFile &cfile, std::string &fname);

	bool SearchFile(std::string name, DWORD *end, DWORD *size);

	inline std::vector<XpkFile> &GetFiles() { return m_fileMap; }
	inline std::ifstream *GetHandle() { return &m_fArchive; }

private:
	std::ifstream m_fArchive;
	std::vector<XpkFile> m_fileMap;
	DWORD m_dwFileCount, m_dwNameTableOffset,
	m_dwContentOffset;
};
