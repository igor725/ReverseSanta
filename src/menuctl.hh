#pragma once

#include <string>
#include <vector>
#include "exceptions.hh"
#undef CALLBACK // Виндовый дефайн, он нам не понадобится

class MenuController;

class MenuBase {
public:
	typedef void(*Callback)(MenuBase *self);

	MenuBase(std::string title) : m_sTitle(title) {}

	MenuController *GetController() { return m_lpController; }
	void SetController(MenuController *ctl) { m_lpController = ctl; }

	virtual void Draw() {}

protected:
	std::string m_sTitle = "NO TITLE";
	MenuController *m_lpController = nullptr;
};

class MenuButtons : public MenuBase {
public:
	MenuButtons(std::string title, bool hide = false) : MenuBase(title), m_bHideTitle(hide) {}

	void AddButton(std::string title, Callback cb) {
		m_vItems.push_back({CALLBACK, title, cb});
	}

	void AddButton(std::string title, MenuBase *menu) {
		m_vItems.push_back({SUBMENU, title, menu});
	}

	void AddCloseButton() {
		m_vItems.push_back({CLOSE, "Go back"});
	}

	void Draw();

private:
	enum ItemType {
		INVALID,
		SUBMENU,
		CALLBACK,
		CLOSE
	};
	struct MenuItem {
		ItemType f_eType;
		std::string f_sTitle;
		union {
			MenuBase *f_lpSubMenu;
			Callback f_lpCallback;
		} f_uData;

		MenuItem(ItemType type, std::string title) : f_eType(type), f_sTitle(title) {}

		MenuItem(ItemType type, std::string title, Callback cb) : f_eType(type), f_sTitle(title) {
			f_uData.f_lpCallback = cb;
		}

		MenuItem(ItemType type, std::string title, MenuBase *menu) : f_eType(type), f_sTitle(title) {
			f_uData.f_lpSubMenu = menu;
		}
	};

	std::vector<MenuItem> m_vItems = {};
	bool m_bHideTitle = false;
};

class MenuController {
public:
	~MenuController() {
		for (auto it : m_vRegistred)
			delete it;
	}

	void Draw() {
		if (m_vStack.size() > 0)
			m_vStack.back()->Draw();
	}

	void AddMenu(MenuBase *menu) {
		if (IsRegistred(menu)) return;
		menu->SetController(this);
		m_vRegistred.push_back(menu);
	}

	bool IsRegistred(MenuBase *menu) {
		for (auto it : m_vRegistred)
			if (it == menu) return true;
		
		return false;
	}

	void ShowMenu(MenuBase *menu) {
		EASSERT(IsRegistred(menu) == true);
		m_vStack.push_back(menu);
	}

	void CloseMenu() {
		if (m_vStack.size() > 0)
			m_vStack.pop_back();
	}

private:
	std::vector<MenuBase *> m_vRegistred = {};
	std::vector<MenuBase *> m_vStack = {};
};
