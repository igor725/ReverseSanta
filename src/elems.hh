#pragma once

#include <Windows.h>
#include <fstream>
#include <map>

#include "mesh.hh"

class Elems {
public:
	enum Type {
		NONE,
		PLATFORM,
		RECTFORM,
		DECO,
		ELEVATORENEMY,
		ELEVATOR,
		MOVER,
		JUMPER,
		ENEMY,
		EXIT,
		BONUS,
		SAVEPOINT,
		EXTRALIFE,
		END_TYPE,

		DWORD_MAX = 0xFFFFFFFF
	};

	const std::string m_aElemTypes[END_TYPE] = {
		"NONE", "PLATTFORM", "RECTFORM",
		"DECO", "ELEVATORENEMY", "ELEVATOR",
		"MOVER", "JUMPER", "ENEMY", "EXIT",
		"BONUS", "SAVEPOINT", "EXTRALIFE"   
	};

	struct Element {
		Type f_eType;
		Mesh *f_lpMesh;
		/*Effect *f_lpEffect;*/
		/*Anim *f_lpAnimation;*/

		FLOAT f_fScaling = 1.0f;
		FLOAT f_fRadius = 0.0f;
		FLOAT f_fSpeed = 0.0f;
		FLOAT f_fJumpHeight = 0.0f;
		FLOAT f_fVerticalOffset = 0.0f;
		FLOAT f_fFriction = 0.0f;
		FLOAT f_fRotation = 0.0f;
		BYTE f_walkanim = 0;
	};

	Elems(std::ifstream *file, DWORD end);

	Element *Search(std::string name);

private:
	std::map<std::string, Element> m_mElements;
};
