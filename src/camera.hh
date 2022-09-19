#pragma once

#include <d3dx9.h>
#include <cmath>

static const D3DXVECTOR3 dV = {0.0f, 0.0f, 1.0f}, dU = {0.0f, 1.0f, 0.0f};

struct Camera {
	D3DXMATRIX  f_mxView, f_mxProj, f_mxRot;
	D3DXVECTOR3 f_vEye  = {0.0f, 3.0f, 0.0f},
				f_vView = {0.0f, 1.0f, 0.0f},
				f_vUp   = {0.0f, 1.0f, 0.0f},
				f_vRot  = {0.0f, 0.0f, 0.0f};

	const FLOAT f_fFollowDistance = 6.0f;
	FLOAT f_fFov = 1.03f,
	f_fNearVP = 0.01f,
	f_fFarVP = 1000.0f,
	f_fAspect;
	LPD3DXVECTOR3 f_lpvFollowPos = nullptr,
	f_lpvFollowRot = nullptr;

	Camera(FLOAT aspect) : f_fAspect(aspect) {
		UpdateProj();
		D3DXMatrixIdentity(&f_mxView);
	}

	void UpdateProj() {
		D3DXMatrixPerspectiveFovLH(&f_mxProj, f_fFov, f_fAspect, f_fNearVP, f_fFarVP);
	}

	void Update(FLOAT delta) {
		if (f_lpvFollowPos && f_lpvFollowRot) {
			D3DXVECTOR3 flfwd = {
				std::sinf(f_lpvFollowRot->y) * std::cosf(f_lpvFollowRot->x),
				std::sinf(-f_lpvFollowRot->x),
				std::cosf(f_lpvFollowRot->y) * std::cosf(f_lpvFollowRot->x)
			};

			flfwd.y = 0.5f;
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
		}

		D3DXMatrixRotationYawPitchRoll(&f_mxRot, f_vRot.x, f_vRot.y, f_vRot.z);

		D3DXVec3TransformCoord(&f_vView, &dV, &f_mxRot);
		D3DXVec3TransformCoord(&f_vUp, &dU, &f_mxRot);

		D3DXVec3Add(&f_vView, &f_vView, &f_vEye);

		D3DXMatrixLookAtLH(&f_mxView, &f_vEye, &f_vView, &f_vUp);
	}

	D3DXVECTOR3 GetForward() {
		return {
			std::sinf(f_vRot.x) * std::cosf(f_vRot.y),
			std::sinf(-f_vRot.y),
			std::cosf(f_vRot.x) * std::cosf(f_vRot.y)
		};
	}

	void SetFollow(LPD3DXVECTOR3 pos = nullptr, LPD3DXVECTOR3 rot = nullptr) {
		f_lpvFollowPos = pos, f_lpvFollowRot = rot;
	}
};
