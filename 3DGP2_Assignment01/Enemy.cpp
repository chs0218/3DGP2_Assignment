#include "Enemy.h"
#include "Shader.h"

XMFLOAT3 RandomPositionInSphere(XMFLOAT3 xmf3Center, float fRadius, int nColumn, int nColumnSpace)
{
	float fAngle = Random() * 360.0f * (2.0f * 3.14159f / 360.0f);

	XMFLOAT3 xmf3Position;
	xmf3Position.x = xmf3Center.x + fRadius * sin(fAngle);
	xmf3Position.y = xmf3Center.y - (nColumn * float(nColumnSpace) / 2.0f) + (nColumn * nColumnSpace) + Random();
	xmf3Position.z = xmf3Center.z + fRadius * cos(fAngle);

	return(xmf3Position);
}

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
	m_bEnable = true;
	direction = RandomDirection();
	fWanderingTime = 0.0f;
	fVelocity = 50.0f;
	m_pPlayer = NULL;
	m_pObject = NULL;
	m_xmOOBB = BoundingOrientedBox{ XMFLOAT3{0.0f, 0.0f, 0.0f}, XMFLOAT3{5.0f, 5.0f, 5.0f}, XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
}

CEnemy::~CEnemy()
{
}

void CEnemy::Update(CGameObject* pPlayer, float fTimeElapsed)
{
	XMFLOAT3 cur_Position = m_pObject->GetPosition();
	XMFLOAT3 reult_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	if (m_pObject)
	{
		UpdateDirection(pPlayer, fTimeElapsed);
		if (Vector3::Length(direction) > 0.0f)
			m_pObject->SetLookAt(XMFLOAT3(cur_Position.x + direction.x, cur_Position.y, cur_Position.z + direction.z), XMFLOAT3(0.0f, 1.0f, 0.0f));
		else if (m_pPlayer)
		{
			
			m_pObject->SetLookAt(m_pPlayer->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
		}

		XMFLOAT3 deltaPos = Vector3::ScalarProduct(direction, fVelocity * fTimeElapsed);
		reult_Position = Vector3::Add(cur_Position, deltaPos);
		m_pObject->SetPosition(reult_Position);
		if (m_pUpdatedContext)
			OnUpdateCallback(fTimeElapsed);
		UpdateBoundingBox();
	}
}

void CEnemy::UpdateDirection(CGameObject* pPlayer, float fTimeElapsed)
{
	XMFLOAT3 cur_Position = m_pObject->GetPosition();
	if (m_pPlayer)
	{
		direction = Vector3::Subtract(m_pPlayer->GetPosition(), cur_Position);
		if (Vector3::Length(direction) < 100.0f)
			direction = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	}
	else
	{
		XMFLOAT3 deltaPos = Vector3::Subtract(pPlayer->GetPosition(), cur_Position);
		if (Vector3::Length(deltaPos) < 300.0f)
			SetPlayer(pPlayer);
		else
		{
			fWanderingTime += fTimeElapsed;
			if (fWanderingTime > 5.0f)
			{
				fWanderingTime = 0.0f;
				direction = RandomDirection();
			}
		}
	}
}

void CEnemy::OnUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3Position = m_pObject->GetPosition();
	int z = (int)(xmf3Position.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z, bReverseQuad) + 6.0f;
	float fLength = pTerrain->GetLength();
	float fWidth = pTerrain->GetWidth();
	if (xmf3Position.x > fWidth - CORRECTION)
	{
		direction.x = 0.0f;
		xmf3Position.x = fWidth - CORRECTION;
	}
	if (xmf3Position.x < CORRECTION)
	{
		direction.x = 0.0f;
		xmf3Position.x = CORRECTION;
	}
	if (xmf3Position.y > 500.0f)
	{
		direction.y = 0.0f;
		xmf3Position.y = 500.0f;
	}
	if (xmf3Position.y < fHeight)
	{
		direction.y = 0.0f;
		xmf3Position.y = fHeight;
	}
	if (xmf3Position.z > fLength - CORRECTION)
	{
		direction.z = 0.0f;
		xmf3Position.z = fLength - CORRECTION;
	}
	if (xmf3Position.z < CORRECTION)
	{
		direction.z = 0.0f;
		xmf3Position.z = CORRECTION;
	}
	m_pObject->SetPosition(xmf3Position);
}

void CEnemy::SetObject(std::shared_ptr<CGameObject> pObject, float f_Width, float f_Length, int nColumnSize, int nColumnSpace, int h)
{
	m_pObject = pObject;
	if (h != -1)
		m_pObject->SetPosition(RandomPositionInSphere(XMFLOAT3(f_Width / 2.0f, 300.0f, f_Length / 2.0f), Random(800.0f, 1000.0f), h - int(floor(nColumnSize / 2.0f)), nColumnSpace));
	else
		m_pObject->SetPosition(RandomPositionInSphere(XMFLOAT3(f_Width / 2.0f, 300.0f, f_Length / 2.0f), Random(0.0f, 1000.0f), nColumnSize - int(floor(nColumnSize / 2.0f)), nColumnSpace));
	m_pObject->Rotate(0.0f, 45.0f, 0.0f);
	m_pObject->PrepareAnimate();
}

void CEnemy::SetContext(void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	m_pUpdatedContext = pTerrain;
}

void CEnemy::UpdateBoundingBox()
{
	XMFLOAT4X4 xmf4x5Transform = m_pObject->GetTransform();
	m_xmOOBB.Transform(m_xmOOBB_Result, XMLoadFloat4x4(&xmf4x5Transform));
	XMStoreFloat4(&m_xmOOBB_Result.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
}

void CEnemy::Animate(float fTimeElapsed)
{
	if (m_pObject)
		m_pObject->Animate(fTimeElapsed, NULL);
}

void CEnemy::CheckCollision(std::vector<CBullet*> m_pBullets)
{
	if (m_pBullets.data())
	{
		for (CBullet* pBullet : m_pBullets)
		{
			if (pBullet->CheckCollision(m_xmOOBB_Result))
			{
				m_bEnable = false;
				if (CMultiSpriteObjectsShader::Instance())
				{
					CMultiSpriteObjectsShader::Instance()->AddObject(m_pObject->GetPosition());
				}
				break;
			}
		}
	}
}

void CEnemy::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pObject)
	{
		if (m_bEnable)
		{
			m_pObject->UpdateTransform(NULL);
			m_pObject->Render(pd3dCommandList, pCamera);
		}
	}
}