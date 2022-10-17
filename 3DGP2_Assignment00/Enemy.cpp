#include "Enemy.h"

XMFLOAT3 RandomDirection()
{
	XMFLOAT3 xmf3Direction;
	xmf3Direction.x = Random() * 2.0f - 1.0f;
	xmf3Direction.y = Random() * 2.0f - 1.0f;
	xmf3Direction.z = Random() * 2.0f - 1.0f;

	
	return(Vector3::Normalize(xmf3Direction));

}
CEnemy::CEnemy()
{
	direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	fWanderingTime = 0.0f;
	fVelocity = 0.0f;
	m_pPlayer = NULL;
	m_pObject = NULL;
}

CEnemy::~CEnemy()
{
}

void CEnemy::Update(float fTimeElapsed)
{
	XMFLOAT3 cur_Position = m_pObject->GetPosition();
	XMFLOAT3 reult_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	if (m_pObject)
	{
		UpdateDirection(fTimeElapsed);
	}
}

void CEnemy::UpdateDirection(float fTimeElapsed)
{
	XMFLOAT3 cur_Position = m_pObject->GetPosition();
	if (m_pPlayer)
	{
		direction = Vector3::Subtract(m_pPlayer->GetPosition(), cur_Position);
	}
	else
	{
		fWanderingTime += fTimeElapsed;
		if (fWanderingTime > 1.0f)
		{
			direction = RandomDirection();
		}
	}
}
