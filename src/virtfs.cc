#include "virtfs.hh"

std::ifstream *VirtFs::Open(std::string path, DWORD *end, DWORD *size) {
	auto direct = new std::ifstream(/*"unpacked\\" + */path, std::ios::binary | std::ios::ate);
	if(direct->is_open()) {
		DWORD fsz = (DWORD)direct->tellg();
		if (size) *size = fsz;
		if (end) *end = fsz;
		direct->seekg(0, direct->beg);
		return direct;
	}

	if (m_lpXpk.SearchFile(path, end, size)) {
		delete direct;
		return m_lpXpk.GetHandle();
	}

	return direct;
}

void VirtFs::Close(std::ifstream *file) {
	if (file != m_lpXpk.GetHandle()) {
		file->close();
		delete file;
	}
}
