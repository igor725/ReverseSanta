#include "engine.hh"
#include "elems.hh"

static std::string GetStringParam(std::string &line) {
	auto start = line.find_first_of('"'),
	stop = line.find_last_of('"');
	if (stop == line.npos || start > stop)
		ExitProcess(1);

	return line.substr(start + 1, stop - start - 1);
}

Elems::Elems(std::ifstream *file, DWORD end) {
	auto engine = Engine::GetInstance();
	auto device = engine->SysGraphics()->GetDevice();

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
			else if (!inwork)
				ExitProcess(1);

			if (inwork) {
				if (line.find("TYPE") == 0) {
					for (auto &type : m_mElemTypes) {
						if (line.find(type.second, vstart) != line.npos) {
							inwork->f_eType = type.first;
							break;
						}
					}
				} else if (line.find("FILE") == 0) {
					/* TODO: Lazy mesh loading */
					inwork->f_lpMesh = new Mesh(device, GetStringParam(line));
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
					else
						ExitProcess(1);
				}
			} else
				inwork = &m_mElements[GetStringParam(line)];
		}
		file->seekg(pos);
	} while (file->tellg() < end);

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
}

void Elems::OnDeviceLost() {
	for (auto &elem : m_mElements)
		elem.second.f_lpMesh->OnDeviceLost();
}

void Elems::OnDeviceReset(LPDIRECT3DDEVICE9 device) {
	for (auto &elem : m_mElements)
		elem.second.f_lpMesh->OnDeviceReset(device);
}

Elems::Element *Elems::Search(std::string name) {
	for (auto &elem : m_mElements) {
		if (elem.first == name)
			return &elem.second;
	}

	return nullptr;
}
