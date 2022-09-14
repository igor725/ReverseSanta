#pragma once

#include <d3dx9.h>

class Camera {
public:
	Camera(FLOAT fov, FLOAT aspect, FLOAT nearvp, FLOAT farvp);

	void Update();
	inline void Move(FLOAT yaw, FLOAT pitch) { m_fYaw += yaw, m_fPitch += pitch; }
	inline LPD3DXMATRIX GetProjection() { return &m_mxProj; }
	inline LPD3DXMATRIX GetView() { return &m_mxView; }
	inline FLOAT GetYaw() { return m_fYaw; }
	inline FLOAT GetPitch() { return m_fPitch; }
	inline LPD3DXVECTOR3 GetEye() { return &m_vEye; }

private:
	FLOAT m_fYaw, m_fPitch;
	D3DXMATRIX m_mxView, m_mxProj, m_mxRot;
	D3DXVECTOR3 m_vEye, m_vView, m_vUp;
};
