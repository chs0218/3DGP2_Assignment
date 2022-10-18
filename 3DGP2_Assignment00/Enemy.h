#pragma once
#include "Object.h"

class CGameObject;
class CEnemy
{
private:
	XMFLOAT3 direction;
	float fWanderingTime;
	float fVelocity;
	CGameObject* m_pPlayer;
	CGameObject* m_pObject;
public:
	CEnemy();
	~CEnemy();
	void Update(float fTimeElapsed);
	void UpdateDirection(float fTimeElapsed);
	void SetPlayer(CGameObject* pPlayer) { m_pPlayer = pPlayer; }
	void SetObject(CGameObject* pObject, float f_Width, float f_Length, int nColumnSize, int nColumnSpace, int h = -1);
	void Animate(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

