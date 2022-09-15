#include "camera.hh"

static const D3DXVECTOR3 dV = {0.0f, 0.0f, 1.0f}, dU = {0.0f, 1.0f, 0.0f};

Camera::Camera(FLOAT fov, FLOAT aspect, FLOAT nearvp, FLOAT farvp) {
	D3DXMatrixPerspectiveFovLH(&m_mxProj, fov, aspect, nearvp, farvp);
	D3DXMatrixIdentity(&m_mxView);
}

void Camera::Update(FLOAT delta) {
	if (m_lpvFollowPos && m_lpvFollowRot) {
		D3DXVECTOR3 plfwd = {
			std::sinf(m_lpvFollowRot->y) * std::cosf(m_lpvFollowRot->x),
			std::sinf(-m_lpvFollowRot->x),
			std::cosf(m_lpvFollowRot->y) * std::cosf(m_lpvFollowRot->x)
		};

		plfwd.y = 0.4f;
		auto back = plfwd * -7.0f;
		auto dizcampos = *m_lpvFollowPos - back;
		auto dizcampdiff = m_vEye - dizcampos;
		auto length = D3DXVec3Length(&dizcampdiff);
		if (length > 1.0f)
			D3DXVec3Lerp(&m_vEye, &m_vEye, &dizcampos, delta + (1.0f / length) * 0.7f);
		else m_vEye = dizcampos;

		auto dcamplpdiff = m_vEye - *m_lpvFollowPos;
		if (dcamplpdiff.x != 0.0f)
			m_vRot.x = std::atan(dcamplpdiff.x / dcamplpdiff.z);
		if (dcamplpdiff.z > 0.0f)
			m_vRot.x += D3DX_PI;
		if (dcamplpdiff.y != 0.0f)
			m_vRot.y = std::atan(dcamplpdiff.y / std::sqrt(dcamplpdiff.x * dcamplpdiff.x + dcamplpdiff.z * dcamplpdiff.z));
	}

	D3DXMatrixRotationYawPitchRoll(&m_mxRot, m_vRot.x, m_vRot.y, m_vRot.z);

	D3DXVec3TransformCoord(&m_vView, &dV, &m_mxRot);
	D3DXVec3TransformCoord(&m_vUp, &dU, &m_mxRot);

	D3DXVec3Add(&m_vView, &m_vView, &m_vEye);

	D3DXMatrixLookAtLH(&m_mxView, &m_vEye, &m_vView, &m_vUp);
}

