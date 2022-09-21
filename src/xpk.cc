#include "exceptions.hh"
#include "xpk.hh"

Xpk::Xpk(std::string fpath) : m_fArchive(fpath, std::ios::binary) {
	EASSERT(m_fArchive.is_open());
	m_fArchive.seekg(0, std::ios::beg);
	m_fArchive.read((char *)&m_dwFileCount, 4);
	for (DWORD i = 0; i < m_dwFileCount; i++) {
		XpkFile cfile = {0};
		m_fArchive.read((char *)&cfile.dwNameOffset, 4);
		m_fileMap.push_back(cfile);
	}

	DWORD dwNameTableSize;
	m_fArchive.read((char *)&dwNameTableSize, 4);
	m_dwNameTableOffset = (DWORD)m_fArchive.tellg();
	m_fArchive.seekg(dwNameTableSize + 4, std::ios::cur); /*Пропускаем таблицу имён и какие-то странные байты*/

	for (DWORD i = 0; i < m_dwFileCount; i++) {
		XpkFile &cfile = m_fileMap[i];
		m_fArchive.read((char *)&cfile.dwSize, 4);
		if (i > 0) {
			XpkFile &pfile = m_fileMap[i - 1];
			cfile.dwOffset = pfile.dwOffset + pfile.dwSize;
		}
	}

	for (DWORD i = 0; i < m_dwFileCount; i++) {
		XpkFile &cfile = m_fileMap[i];
		m_fArchive.read((char *)&cfile.dwCreated, 8); /*Читается 2 DWORD значения последовательно!!!*/
	}

	m_dwContentOffset = (DWORD)m_fArchive.tellg();
}

std::string &Xpk::ReadFileName(XpkFile &cfile, std::string &fname) {
	char ch;
	m_fArchive.seekg(m_dwNameTableOffset + cfile.dwNameOffset, std::ios::beg);
	while (m_fArchive.good() && m_fArchive.read(&ch, 1) && ch != '\0')
		fname.push_back(ch);
	return fname;
}

static bool cmpnames(std::string &s1, std::string &s2) {
	return ((s1.size() == s2.size()) && std::equal(s1.begin(), s1.end(), s2.begin(), [](char &c1, char &c2) {
		return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
	}));
}

BOOL Xpk::SearchFile(std::string name, DWORD *end, DWORD *size) {
	DWORD fileIndex = 0;
	m_fArchive.seekg(m_dwNameTableOffset, std::ios::beg);
	do {
		std::string fileName;
		char ch;
		while (m_fArchive.good() && m_fArchive.read(&ch, 1) && ch != '\0')
			fileName.push_back(ch);

		if (cmpnames(fileName, name))
			return SearchFile(m_fileMap[fileIndex], end, size);
	} while (++fileIndex < m_dwFileCount);

	return false;
}

BOOL Xpk::SearchFile(XpkFile &cfile, DWORD *end, DWORD *size) {
	DWORD npos = m_dwContentOffset + cfile.dwOffset;
	m_fArchive.seekg(npos, std::ios::beg);
	if (end) *end = npos + cfile.dwSize;
	if (size) *size = cfile.dwSize;
	return true;
}
