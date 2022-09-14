#pragma once

#include <d3dx9.h>
#include <cmath>

class Camera {
public:
	Camera(FLOAT fov, FLOAT aspect, FLOAT nearvp, FLOAT farvp);

	void Update();
	inline LPD3DXMATRIX GetProjection() { return &m_mxProj; }
	inline LPD3DXMATRIX GetView() { return &m_mxView; }
	inline LPD3DXVECTOR3 GetRot() { return &m_vRot; }
	inline LPD3DXVECTOR3 GetEye() { return &m_vEye; }
	inline D3DXVECTOR3 GetForward() {
		return {
			std::sinf(m_vRot.x) * std::cosf(m_vRot.y),
			std::sinf(-m_vRot.y),
			std::cosf(m_vRot.x) * std::cosf(m_vRot.y)
		};
	}

private:
	D3DXMATRIX m_mxView, m_mxProj, m_mxRot;
	D3DXVECTOR3 m_vEye  = {0.0f, 3.0f, 0.0f},
				m_vView = {0.0f, 1.0f, 0.0f},
				m_vUp   = {0.0f, 1.0f, 0.0f},
				m_vRot  = {0.0f, 0.0f, 0.0f};
};
