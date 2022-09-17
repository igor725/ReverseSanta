#include "input.hh"

Input::Input(HINSTANCE hInst, HWND hWnd) {
	if(DirectInput8Create(hInst, DIRECTINPUT_VERSION,
	IID_IDirectInput8, (LPVOID *)&m_lpDI, NULL) != DI_OK)
		ExitProcess(1);

	if (m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIK, NULL) != DI_OK
	|| m_lpDIK->SetDataFormat(&c_dfDIKeyboard) != DI_OK
	|| m_lpDIK->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE) != DI_OK)
		ExitProcess(1);

	if (m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIM, NULL) != DI_OK
	|| m_lpDIM->SetDataFormat(&c_dfDIMouse) != DI_OK
	|| m_lpDIM->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND) != DI_OK)
		ExitProcess(1);
}

bool Input::Capture() {
	if (m_bInputAcquired) return true;
	if (m_lpDIK->Acquire() != DI_OK) return false;
	if (m_lpDIM->Acquire() != DI_OK) {
		m_lpDIK->Unacquire();
		return false;
	}

	m_bInputAcquired = true;
	return true;
}

bool Input::Release() {
	if (!m_bInputAcquired) return false;
	m_bInputAcquired = false;
	return (m_lpDIM->Unacquire() == DI_OK) &&
	(m_lpDIK->Unacquire() == DI_OK);
}

void Input::Update(FLOAT delta, BaseRunner *runner) {
	static InputState state;

	if (m_lpDIM->GetDeviceState(sizeof(state.f_msMouseNew), &state.f_msMouseNew) != DI_OK)
		return;

	if (m_lpDIK->GetDeviceState(sizeof(state.f_lpKeyboard), state.f_lpKeyboard) != DI_OK)
		return;

	runner->OnInput(delta, &state);
	state.f_msMousePrev = state.f_msMouseNew;
}
