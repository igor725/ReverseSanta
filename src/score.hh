#pragma once

#include <Windows.h>
#include <fstream>

struct Scores {
	struct ScoreField {
		DWORD score;
		DWORD difficulty;
		CHAR name[48];
	};

	static const DWORD f_dwSize = 10;
	const DWORD f_dwHeader = 0xDEADBEEF;
	ScoreField f_Fields[f_dwSize] = {};

	Scores() {
		std::ifstream file ("score.dat");
		if (file.is_open()) {
			file.exceptions(file.badbit | file.eofbit | file.failbit);
			DWORD hdr; file.read((LPSTR)&hdr, 4);
			if (hdr != f_dwHeader) {
				file.seekg(0, file.beg);
				ParseOldScores(file);
				return;
			}

			file.read((LPSTR)&f_Fields, sizeof(f_Fields));
		} else {
			Put(57654, 1, "*ST. CLAUS*");
			Put(38436, 1, "*CARL CROW*");
			Put(25624, 1, "*PETER O.*");
			Put(17083, 1, "*PETR V.*");
			Put(11389, 1, "*ADAM S.*");
			Put(7593, 1, "*ACHIM H.*");
			Put(5062, 1, "*MASTER DW*");
			Put(3375, 1, "*FABIAN S.*");
			Put(2250, 1, "*ANDREAS N.*");
			Put(1500, 1, "*SONJA B.*");
			Save();
		}
	}

	BOOL Put(DWORD score, DWORD diff, LPCSTR name) {
		for (DWORD i = 0; i < f_dwSize; i++) {
			auto &field = f_Fields[i];
			if (field.score < score) {
				::strcpy_s(field.name, 47, name);
				field.difficulty = diff;
				field.score = score;
				return true;
			}
		}

		return false;
	}

	VOID Save() {
		std::ofstream file ("score.dat");
		if (file.is_open()) {
			file.write((LPSTR)&f_dwHeader, 4);
			file.write((LPSTR)&f_Fields, sizeof(f_Fields));
		}
	}

	VOID ParseOldScores(std::ifstream &file) {
		struct OldField {
			DWORD score;
			WCHAR name[12];
		} field;

		for (DWORD i = 0; i < 10; i++) {
			auto &newfield = f_Fields[i];
			file.read((LPSTR)&field, sizeof(field));
			field.score ^= 0xFFFFFFFF;
			for (DWORD j = 0; j < 12; j++)
				field.name[j] ^= 0xFFFF;
			::WideCharToMultiByte(CP_UTF8, 0, field.name,
			(int)::wcslen(field.name), newfield.name, 47, nullptr, nullptr);
			newfield.difficulty = 1; /* Т.к. оригинальная игра не имеет уровней сложности, он всегда 1 */
			newfield.score = field.score;
		}
	}
};
