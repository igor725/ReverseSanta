#pragma once

#include <Windows.h>
#include <fstream>
#include <map>

#include "mesh.hh"

class Elems {
public:
	const std::map<DWORD, std::string>  m_mElemTypes = {
		{1, "PLATTFORM"},
		{2, "RECTFORM"},
		{3, "DECO"},
		{4, "ELEVATOR"},
		{5, "MOVER"},
		{6, "JUMPER"},
		{7, "ENEMY"},
		{8, "EXIT"},
		{9, "BONUS"},
		{10, "SAVEPOINT"},
		{11, "EXTRALIFE"}
	};

	struct Element {
		DWORD f_type;
		Mesh *u_mesh;
		/*Effect *u_effect;*/
		/*Anim *f_animation;*/
		FLOAT f_scaling = 0.0f;
		FLOAT f_radius = 0.0f;
		FLOAT f_speed = 0.0f;
		FLOAT f_jumpheight = 0.0f;
		FLOAT f_verticaloffset = 0.0f;
		FLOAT f_friction = 0.0f;
		FLOAT f_rotation = 0.0f;
		BYTE f_walkanim = 0;
	};

	Elems(std::ifstream *file, DWORD end);

	void OnDeviceLost();
	void OnDeviceReset(LPDIRECT3DDEVICE9 device);

	Element *Search(std::string name);

private:
	std::map<std::string, Element> m_mElements;
};
