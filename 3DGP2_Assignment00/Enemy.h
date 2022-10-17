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
	void SetPlayer(CGameObject* pPlayer) { m_pObject = pPlayer; }
	void SetObject(CGameObject* pObject) { m_pObject = pObject; }
};

