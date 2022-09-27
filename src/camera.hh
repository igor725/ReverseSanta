#pragma once

#include <d3dx9.h>
#include <cmath>

struct Camera {
	D3DXMATRIX  f_mxView, f_mxProj, f_mxRot;
	D3DXVECTOR3 f_vEye  = {0.0f, 3.0f, 0.0f},
				f_vView = {0.0f, 1.0f, 0.0f},
				f_vUp   = {0.0f, 1.0f, 0.0f},
				f_vRot  = {0.0f, 0.0f, 0.0f};

	FLOAT f_fRenderDistance = 100.0f,
	f_fFollowDistance = 6.0f,
	f_fFollowHeightMult = 0.5f;
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

	inline VOID UpdateProj() {
		D3DXMatrixPerspectiveFovLH(&f_mxProj, f_fFov, f_fAspect, f_fNearVP, f_fFarVP);
	}

	VOID Update(FLOAT delta);

	inline D3DXVECTOR3 GetForward() {
		return {
			std::sinf(f_vRot.x) * std::cosf(f_vRot.y),
			std::sinf(-f_vRot.y),
			std::cosf(f_vRot.x) * std::cosf(f_vRot.y)
		};
	}

	inline BOOL IsVisible(D3DXVECTOR3 dvec) {
		return D3DXVec3Length(&(dvec -= f_vEye)) < f_fRenderDistance;
	}

	inline VOID SetFollow(LPD3DXVECTOR3 pos = nullptr, LPD3DXVECTOR3 rot = nullptr) {
		f_lpvFollowPos = pos, f_lpvFollowRot = rot;
	}
};
