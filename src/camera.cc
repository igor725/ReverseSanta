#include "camera.hh"

static const D3DXVECTOR3 dV = {0.0f, 0.0f, 1.0f}, dU = {0.0f, 1.0f, 0.0f};

Camera::Camera(FLOAT fov, FLOAT aspect, FLOAT nearvp, FLOAT farvp) {
	D3DXMatrixPerspectiveFovLH(&m_mxProj, fov, aspect, nearvp, farvp);
	D3DXMatrixIdentity(&m_mxView);
}

void Camera::Update() {
	D3DXMatrixRotationYawPitchRoll(&m_mxRot, m_vRot.x, m_vRot.y, m_vRot.z);

	D3DXVec3TransformCoord(&m_vView, &dV, &m_mxRot);
	D3DXVec3TransformCoord(&m_vUp, &dU, &m_mxRot);

	D3DXVec3Add(&m_vView, &m_vView, &m_vEye);

	D3DXMatrixLookAtLH(&m_mxView, &m_vEye, &m_vView, &m_vUp);
}
