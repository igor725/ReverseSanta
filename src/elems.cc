#include "exceptions.hh"
#include "engine.hh"
#include "elems.hh"
#include "todo.hh"

static std::string GetStringParam(std::string &line) {
	auto start = line.find_first_of('"'),
	stop = line.find_last_of('"');
	EASSERT(stop != line.npos && start < stop);

	return line.substr(start + 1, stop - start - 1);
}

static DWORD joaat(LPCSTR key) {
	DWORD hash = 0x00000000;

	for (DWORD i = 0; key[i] != '\0'; i++) {
		hash += (uint8_t)key[i];
		hash += hash << 0x0A;
		hash ^= hash >> 0x06;
	}

	hash += hash << 0x03;
	hash ^= hash >> 0x0B;
	hash += hash << 0x0F;

	return hash;
}

Elems::Elems(std::string fpath) : m_mElements({}) {
	auto engine = Engine::GetInstance();
	auto virtfs = engine->SysVirtFs();
	auto cache = engine->SysCache();
	DWORD end;

	if (auto file = virtfs->Open(fpath, &end)) {
		Element *inwork = nullptr;
		do {
			char ch;
			std::string line;
			while (!file->eof() && file->read(&ch, 1) && ch != '\n')
				if(ch != '\r') line.push_back(ch);

			auto pos = file->tellg();
			if (line.length() > 0) {
				auto vstart = line.find_last_of('\t');

				if (line.find("ELEMENT") == 0)
					inwork = nullptr;
				else EASSERT(inwork != nullptr);

				if (inwork) {
					if (line.find("TYPE") == 0) {
						for (DWORD i = PLATFORM; i < END_TYPE; i++) {
							if (line.find(m_aElemTypes[i], vstart) != line.npos) {
								inwork->f_eType = Type(i);
								break;
							}
						}
					} else if (line.find("FILE") == 0) {
						TODO("Lazy mesh loading");
						inwork->f_lpMesh = cache->GetMesh(GetStringParam(line));
					} else if (line.find("EFFECT") == 0) {
					} else if (line.find("WALKANIM") == 0) {
							inwork->f_walkanim = (BYTE)std::strtol(line.substr(vstart).c_str(), nullptr, 10);
					} else {
						auto value = std::strtof(line.substr(vstart).c_str(), nullptr);

						if (line.find("SCALING") == 0)
							inwork->f_fScaling = value;
						else if (line.find("RADIUS") == 0)
							inwork->f_fRadius = value;
						else if (line.find("SPEED") == 0)
							inwork->f_fSpeed = value;
						else if (line.find("JUMPHEIGHT") == 0)
							inwork->f_fJumpHeight = value;
						else if (line.find("VERTICALOFFSET") == 0)
							inwork->f_fVerticalOffset = value;
						else if (line.find("FRICTION") == 0)
							inwork->f_fFriction = value;
						else if (line.find("ROTATION") == 0)
							inwork->f_fRotation = value;
						else EASSERT(0 && "Unknown elements.txt field");
					}
				} else {
					auto name = GetStringParam(line);
					inwork = &m_mElements[name];
					inwork->f_dwHash = joaat(name.c_str());
				}
			}
			file->seekg(pos);
		} while (file->tellg() < end);
		virtfs->Close(file);

		/* What the actual fuck!? */
		for (auto &elem : m_mElements) {
			switch (elem.second.f_eType) {
				case Elems::SAVEPOINT:
					break;
				case Elems::ENEMY:
				case Elems::ELEVATORENEMY:
					if (elem.second.f_fSpeed != 0.0f)
						break;
				default:
					elem.second.f_fScaling /= 100.0f;
					break;
			}
		}

		return;
	}
}

Elems::Element *Elems::Search(std::string name) {
	for (auto &elem : m_mElements) {
		if (elem.first == name)
			return &elem.second;
	}

	return nullptr;
}
