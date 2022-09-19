#include "exceptions.hh"
#include "input.hh"

Input::Input(HINSTANCE hInst, HWND hWnd) {
	IASSERT(DirectInput8Create(hInst, DIRECTINPUT_VERSION,
	IID_IDirectInput8, (LPVOID *)&m_lpDI, NULL));

	/* Инициализируем клавиатуру */
	IASSERT(m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIK, NULL));
	IASSERT(m_lpDIK->SetDataFormat(&c_dfDIKeyboard));
	IASSERT(m_lpDIK->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE));

	/* Инициализируем мышу */
	IASSERT(m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIM, NULL));
	IASSERT(m_lpDIM->SetDataFormat(&c_dfDIMouse));
	IASSERT(m_lpDIM->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND));
}

BOOL Input::Capture() {
	if (m_bInputAcquired) return true;
	if (m_lpDIK->Acquire() != DI_OK) return false;
	if (m_lpDIM->Acquire() != DI_OK) {
		m_lpDIK->Unacquire();
		return false;
	}

	m_bInputAcquired = true;
	return true;
}

BOOL Input::Release() {
	if (!m_bInputAcquired) return false;
	m_bInputAcquired = false;
	return (m_lpDIM->Unacquire() == DI_OK) &&
	(m_lpDIK->Unacquire() == DI_OK);
}

void Input::Update(FLOAT delta, BaseRunner *runner) {
	static InputState state;

	if (auto hRes = m_lpDIM->GetDeviceState(sizeof(state.f_msMouseNew), &state.f_msMouseNew))
		if (hRes != DIERR_NOTACQUIRED) throw DIException(hRes, EAFINFO);

	if (auto hRes = m_lpDIK->GetDeviceState(sizeof(state.f_lpKeyboard), &state.f_lpKeyboard))
		if (hRes != DIERR_NOTACQUIRED) throw DIException(hRes, EAFINFO);

	runner->OnInput(delta, &state);
	state.f_msMousePrev = state.f_msMouseNew;
}
