#include "lang.hh"
#include "engine.hh"

Language::Language(std::string &path) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();

	if (auto file = virtfs->Open(path, nullptr, nullptr)) {
		DWORD header; file->read((CHAR *)&header, 4);
		EASSERT(header == 0x7379746D && "Invalid header");
		file->read((CHAR *)&m_dwOffsetsSize, 4); /* Кол-во элементов массива строк */
		m_lpdwOffsets = new DWORD[m_dwOffsetsSize];
		file->read((CHAR *)&m_dwStringsSize, 4); /* Размер массива строк */
		file->read((CHAR *)m_lpdwOffsets, m_dwOffsetsSize * 4); /* Заполняем массив смещений данными */
		m_lpStrings = new CHAR[m_dwStringsSize];
		file->read(m_lpStrings, m_dwStringsSize); /* Читаем массив со строчками */
		virtfs->Close(file);
		return;
	}

	EASSERT(0 && "Failed to read language file");
}

Language::~Language() {
	delete m_lpdwOffsets;
	delete m_lpStrings;
}

LPCWSTR Language::Get(DWORD id) {
	EASSERT(id < m_dwOffsetsSize);
	return (LPCWSTR)m_lpStrings[m_lpdwOffsets[id]];
}
