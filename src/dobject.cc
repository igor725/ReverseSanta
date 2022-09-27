#include "dobject.hh"

VOID DObject::Update() {
	D3DXMATRIX tm, rot, scale;

	D3DXMatrixTranslation(&f_mxWorld, f_vPos.x, f_vPos.y, f_vPos.z);
	D3DXMatrixIdentity(&rot);

	D3DXMatrixRotationX(&tm, f_vRot.x);
	D3DXMatrixMultiply(&rot, &rot, &tm);

	D3DXMatrixRotationY(&tm, f_vRot.y);
	D3DXMatrixMultiply(&rot, &rot, &tm);

	D3DXMatrixRotationZ(&tm, f_vRot.z);
	D3DXMatrixMultiply(&rot, &rot, &tm);

	D3DXMatrixMultiply(&f_mxWorld, &rot, &f_mxWorld);

	D3DXMatrixScaling(&scale, GetScale(), GetScale(), GetScale());
	D3DXMatrixMultiply(&f_mxWorld, &scale, &f_mxWorld);
}

BOOL DObject::IsTouching(LPD3DXVECTOR3 p) {
	auto min = f_lpMesh->GetBoundMin(f_vPos, GetScale()),
	max = f_lpMesh->GetBoundMax(f_vPos, GetScale());
	return min.x <= p->x && p->x <= max.x &&
	min.y <= p->y && p->y <= max.y && 
	min.z <= p->z && p->z <= max.z;
}

BOOL DObject::IsTouching(DObject *other, FLOAT *floor) {
	if (f_bHidden || other->f_bHidden) return false;
	auto omesh = other->f_lpMesh;
	auto max1 = f_lpMesh->GetBoundMax(f_vPos, GetScale()),
	min1 = f_lpMesh->GetBoundMin(f_vPos, GetScale()),
	max2 = omesh->GetBoundMax(other->f_vPos, other->GetScale()),
	min2 = omesh->GetBoundMin(other->f_vPos, other->GetScale());

	if ((min1.x <= max2.x) && (max1.x >= min2.x) &&
	(min1.y <= max2.y) && (max1.y >= min2.y) &&
	(min1.z <= max2.z) && (max1.z >= min2.z)) {
		*floor = max1.y;
		return true;
	}

	return false;
}

VOID DObject::Draw(LPDIRECT3DDEVICE9 device, BOOL untextured) {
	if (f_bHidden) return;
	if (f_bAlerted) {
		Update();
		f_bAlerted = false;
	}

	device->SetTransform(D3DTS_WORLD, &f_mxWorld);
	if (f_lpMesh) f_lpMesh->Draw(device, untextured);
}
