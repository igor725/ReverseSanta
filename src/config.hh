#pragma once

#include <string>
#include <fstream>

struct Config {
	enum ModFlags {
		None = 0,
		General = 1 << 0,
		Camera = 1 << 1,
		Graphics = 1 << 2,
		Audio = 1 << 3,
		Binds = 1 << 4,

		NeedRestart = 1 << 31,
		Anything = 0xFFFFFFFF
	};

	std::string f_sPath;
	unsigned int f_flModified = None;
	void (*f_lpUpdateHook)(Config *);
	struct Data {
		char f_lpPath[32] = ".\\xmas.xpk";
		int f_iDifficulty = 1;
		float f_fVolume[3] = {1.0f, 1.0f, 1.0f};
		float f_fFOV = 1.03f;
		float f_fRenderDistance = 100.0f;
		int f_iWidth = 1024, f_iHeight = 768;
		bool f_bVSync = true, f_bBorderless = false;
	} f_Data, f_Prev;

	Config(std::string path, void (*hook)(Config *)) : f_sPath(path), f_lpUpdateHook(hook) { if (!Load()) Save(); }
	~Config() { if (f_flModified) Save(); }

	inline bool IsChanged(int field) { return (f_flModified & field) != 0; }

	inline int GetWidth() { return f_Data.f_iWidth; }
	inline int GetHeight() { return f_Data.f_iHeight; }
	inline float GetVolume(unsigned int i) { return i < 3 ? f_Data.f_fVolume[i] : 0.0f; }
	inline int GetDifficulty() { return f_Data.f_iDifficulty; }
	inline char *GetArchivePath() { return f_Data.f_lpPath; }
	inline float GetFOV() { return f_Data.f_fFOV; }
	inline float GetRenderDistance() { return f_Data.f_fRenderDistance; }
	inline bool GetVSync() { return f_Data.f_bVSync; }
	inline bool GetBorderless() { return f_Data.f_bBorderless; }

	inline void SetVolume(unsigned int i, float vol) { f_Data.f_fVolume[i] = vol; f_flModified |= Audio; }
	inline void SetDifficulty(int d) { f_Data.f_iDifficulty = d; f_flModified |= General; }
	inline void SetFOV(float fov) { f_Data.f_fFOV = fov; f_flModified |= Camera; }
	inline void SetRenderDistance(float rd) { f_Data.f_fRenderDistance = rd; f_flModified |= Camera; }
	inline void SetVSync(bool vsync) { f_Data.f_bVSync = vsync; f_flModified |= Graphics; }
	inline void SetBorderless(bool fs) { f_Data.f_bBorderless = fs; f_flModified |= Graphics; }

	inline bool Update() {
		if (f_flModified == 0) return false;
		bool nr = IsChanged(NeedRestart);
		f_lpUpdateHook(this);
		f_flModified &= NeedRestart;
		f_Prev = f_Data;
		return nr;
	}

	inline bool Reset() {
		f_Data = {}, f_Prev = {};
		f_flModified |= Anything;
		return Save();
	}

	inline bool Revert() {
		if (f_flModified == 0) return false;
		f_flModified &= NeedRestart;
		f_Data = f_Prev;
		return Save();
	}

	bool Save() {
		std::ofstream file (f_sPath, std::ios::binary);
		file.exceptions(file.badbit | file.eofbit | file.failbit);
		file.write((const char *)&f_Data, sizeof(f_Data));
		return Update();
	}

	bool Load() {
		std::ifstream file (f_sPath, std::ios::binary);
		if (!file.is_open()) return false;
		file.exceptions(file.badbit | file.eofbit | file.failbit);
		file.read((char *)&f_Data, sizeof(f_Data));
		f_flModified |= ~NeedRestart;
		return Update();
	}
};
