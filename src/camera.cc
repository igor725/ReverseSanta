#include "camera.hh"

VOID Camera::Update(FLOAT delta) {
	if (f_lpvFollowPos && f_lpvFollowRot) {
		D3DXVECTOR3 flfwd = {
			std::sinf(f_lpvFollowRot->y) * std::cosf(f_lpvFollowRot->x),
			f_fFollowHeightMult,
			std::cosf(f_lpvFollowRot->y) * std::cosf(f_lpvFollowRot->x)
		};

		auto back = flfwd * -f_fFollowDistance;
		auto dizcampos = *f_lpvFollowPos - back;
		dizcampos.y = max(dizcampos.y, -5.0f);
		auto dizcampdiff = f_vEye - dizcampos;
		auto length = D3DXVec3Length(&dizcampdiff);
		if (length > 1.0f)
			D3DXVec3Lerp(&f_vEye, &f_vEye, &dizcampos, delta + (1.0f / length) * 0.7f);
		else f_vEye = dizcampos;

		auto dcamplpdiff = f_vEye - *f_lpvFollowPos;
		if (dcamplpdiff.x != 0.0f) {
			f_vRot.x = std::atan(dcamplpdiff.x / dcamplpdiff.z);
			if (dcamplpdiff.z >= 0.0f)
				f_vRot.x += D3DX_PI;
		}
		if (dcamplpdiff.y != 0.0f)
			f_vRot.y = std::atan(dcamplpdiff.y / std::sqrt(dcamplpdiff.x * dcamplpdiff.x + dcamplpdiff.z * dcamplpdiff.z));
	
		f_vRot.z = f_lpvFollowRot->z;
	}

	D3DXMatrixRotationYawPitchRoll(&f_mxRot, f_vRot.x, f_vRot.y, f_vRot.z);

	const D3DXVECTOR3 v = {0.0f, 0.0f, 1.0f}, u = {0.0f, 1.0f, 0.0f};
	D3DXVec3TransformCoord(&f_vView, &v, &f_mxRot);
	D3DXVec3TransformCoord(&f_vUp, &u, &f_mxRot);

	D3DXVec3Add(&f_vView, &f_vView, &f_vEye);

	D3DXMatrixLookAtLH(&f_mxView, &f_vEye, &f_vView, &f_vUp);
}
