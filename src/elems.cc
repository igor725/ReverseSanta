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
							inwork->f_type = type.first;
							break;
						}
					}
				} else if (line.find("FILE") == 0) {
					auto path = GetStringParam(line);
					auto ext = path.find_last_of('.');
					if (path.find("ani", ext))
						path.replace(ext, 4, ".x");

					inwork->f_file.u_mesh = new Mesh(device, path);
				} else if (line.find("EFFECT") == 0) {
				} else if (line.find("WALKANIM") == 0) {
						inwork->f_walkanim = (BYTE)std::strtol(line.substr(vstart).c_str(), nullptr, 10);
				} else {
					auto value = std::strtof(line.substr(vstart).c_str(), nullptr);

					if (line.find("SCALING") == 0)
						inwork->f_scaling = value;
					else if (line.find("RADIUS") == 0)
						inwork->f_radius = value;
					else if (line.find("SPEED") == 0)
						inwork->f_speed = value;
					else if (line.find("JUMPHEIGHT") == 0)
						inwork->f_jumpheight = value;
					else if (line.find("VERTICALOFFSET") == 0)
						inwork->f_verticaloffset = value;
					else if (line.find("FRICTION") == 0)
						inwork->f_friction = value;
					else if (line.find("ROTATION") == 0)
						inwork->f_rotation = value;
					else
						ExitProcess(1);
				}
			} else
				inwork = &m_mElements[GetStringParam(line)];
		}
		file->seekg(pos);
	} while (file->tellg() < end);
}

Elems::Element *Elems::Search(std::string name) {
	for (auto &elem : m_mElements) {
		if (elem.first == name)
			return &elem.second;
	}

	return nullptr;
}
