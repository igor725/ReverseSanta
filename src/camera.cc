#include "camera.hh"

#include <iostream>

static const D3DXVECTOR3 dV = {0.0f, 0.0f, 1.0f}, dU = {0.0f, 1.0f, 0.0f};

Camera::Camera(FLOAT fov, FLOAT aspect, FLOAT nearvp, FLOAT farvp) : m_vView(0.0f, 1.0f, 0.0f), m_vUp(0.0f, 1.0f, 0.0f), m_vEye(0.0f, 3.0f, 0.0f) {
	D3DXMatrixPerspectiveFovLH(&m_mxProj, fov, aspect, nearvp, farvp);
	D3DXMatrixIdentity(&m_mxView);

	m_fYaw = 0.0f, m_fPitch = 0.0f;
}

void Camera::Update() {
	D3DXMatrixRotationYawPitchRoll(&m_mxRot, m_fYaw, m_fPitch, 0);

	D3DXVec3TransformCoord(&m_vView, &dV, &m_mxRot);
	D3DXVec3TransformCoord(&m_vUp, &dU, &m_mxRot);

	D3DXVec3Add(&m_vView, &m_vView, &m_vEye);

	D3DXMatrixLookAtLH(&m_mxView, &m_vEye, &m_vView, &m_vUp);
}
