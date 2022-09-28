#include "exceptions.hh"
#include "xpk.hh"

Xpk::Xpk(std::string &fpath) : m_fArchive(fpath, std::ios::binary) {
	EASSERT(m_fArchive.is_open());
	m_fArchive.seekg(0, std::ios::beg);
	m_fArchive.read((char *)&m_dwFileCount, 4);
	for (DWORD i = 0; i < m_dwFileCount; i++) {
		XpkFile cfile;
		m_fArchive.read((char *)&cfile.dwNameOffset, 4);
		m_fileMap.push_back(cfile);
	}

	DWORD dwNameTableSize;
	m_fArchive.read((char *)&dwNameTableSize, 4);
	m_lpNameMap = new CHAR[dwNameTableSize + 1]();
	m_fArchive.read((char *)m_lpNameMap, dwNameTableSize);
	m_fArchive.seekg(4, std::ios::cur); /* Размер блока с архивированными файлами в байтах */

	for (DWORD i = 0, o = 0; i < m_dwFileCount; i++) {
		auto &cfile = m_fileMap[i];
		m_fArchive.read((char *)&cfile.dwSize, 4);
		cfile.dwOffset = o;
		o += cfile.dwSize;
	}

	for (DWORD i = 0; i < m_dwFileCount; i++)
		m_fArchive.read((char *)&m_fileMap[i].dwCreated, 8); /* Читается 2 DWORD значения последовательно!!! */

	m_dwContentOffset = (DWORD)m_fArchive.tellg();
}

BOOL Xpk::SearchFile(std::string &name, DWORD *end, DWORD *size) {
	DWORD fileIndex = 0;
	LPCSTR nstart = m_lpNameMap;

	do {
		BOOL valid = true;
		DWORD idx = 0;
		for (; *nstart != '\0'; idx++, nstart++)
			if (valid) valid = std::toupper(name.at(idx)) == std::toupper(*nstart);

		if (valid && name.size() == idx)
			return SearchFile(m_fileMap[fileIndex], end, size);
	} while (++fileIndex < m_dwFileCount && *++nstart != '\0');

	return false;
}

BOOL Xpk::SearchFile(XpkFile &cfile, DWORD *end, DWORD *size) {
	DWORD npos = m_dwContentOffset + cfile.dwOffset;
	m_fArchive.seekg(npos, std::ios::beg);
	if (end) *end = npos + cfile.dwSize;
	if (size) *size = cfile.dwSize;
	return true;
}
