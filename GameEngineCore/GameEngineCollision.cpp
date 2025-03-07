#include "PreCompile.h"
#include "GameEngineCollision.h"
#include "GameEngineActor.h"
#include "GameEngineLevel.h"
#include "GameEngineCollisionGroup.h"

GameEngineCollision::GameEngineCollision() 
{
}

GameEngineCollision::~GameEngineCollision() 
{
}

void GameEngineCollision::Start()
{
	GetActor()->GetLevel()->PushCollision(GetDynamic_Cast_This<GameEngineCollision>());
}

bool GameEngineCollision::Collision(int _Order)
{
	if (false == GetLevel()->Collisions.contains(_Order))
	{
		return false;
	}
	std::shared_ptr<GameEngineCollisionGroup> OtherGroup = GetLevel()->Collisions[_Order];
	return OtherGroup->Collision(GetDynamic_Cast_This<GameEngineCollision>());
}

bool GameEngineCollision::Collision(int _Order, const float4& _NextPos)
{
	if (false == GetLevel()->Collisions.contains(_Order))
	{
		return false;
	}
	std::shared_ptr<GameEngineCollisionGroup> OtherGroup = GetLevel()->Collisions[_Order];
	return OtherGroup->Collision(GetDynamic_Cast_This<GameEngineCollision>(), _NextPos);
}

bool GameEngineCollision::Collision(int _Order, std::function<void(std::vector<GameEngineCollision*>& _Collisions)> _Collision)
{
	if (false == GetLevel()->Collisions.contains(_Order))
	{
		return false;
	}
	std::shared_ptr<GameEngineCollisionGroup> OtherGroup = GetLevel()->Collisions[_Order];
	return OtherGroup->Collision(GetDynamic_Cast_This<GameEngineCollision>(), _Collision);
}

bool GameEngineCollision::Collision(int _Order, const float4& _Next, std::function<void(std::vector<GameEngineCollision*>& _Collisions)> _Collision)
{
	if (false == GetLevel()->Collisions.contains(_Order))
	{
		return false;
	}
	std::shared_ptr<GameEngineCollisionGroup> OtherGroup = GetLevel()->Collisions[_Order];
	return OtherGroup->Collision(GetDynamic_Cast_This<GameEngineCollision>(), _Next, _Collision);
}

bool GameEngineCollision::CollisionLineEvent(int _Order, float4 _EndLine, const EventParameter& _Event)
{
	if (false == GetLevel()->Collisions.contains(_Order))
	{
		return false;
	}
	std::shared_ptr<GameEngineCollisionGroup> OtherGroup = GetLevel()->Collisions[_Order];

	std::set<GameEngineCollision*>::iterator Start = Others.begin();
	std::set<GameEngineCollision*>::iterator End = Others.end();

	for (; Start != End; )
	{
		GameEngineCollision* OtherCol = *Start;

		// 여기서 터질것이다.
		if (false == OtherCol->IsDeath())
		{
			++Start;
			continue;
		}

		Start = Others.erase(Start);
	}

	// 라인은 나의 extents를 end로 돌리고 들어간다.
	Transform.ColData.OBB.Extents = _EndLine.Float3;

	return OtherGroup->CollisionEvent(GetDynamic_Cast_This<GameEngineCollision>(), _Event);
}

bool GameEngineCollision::CollisionEvent(int _Order, const EventParameter& _Event)
{
	if (false == GetLevel()->Collisions.contains(_Order))
	{
		return false;
	}
	std::shared_ptr<GameEngineCollisionGroup> OtherGroup = GetLevel()->Collisions[_Order];

	std::set<GameEngineCollision*>::iterator Start = Others.begin();
	std::set<GameEngineCollision*>::iterator End = Others.end();

	for (; Start != End; )
	{
		GameEngineCollision* OtherCol = *Start;

		// 여기서 터질것이다.
		if (false == OtherCol->IsDeath())
		{
			++Start;
			continue;
		}

		Start = Others.erase(Start);
	}


	return OtherGroup->CollisionEvent(GetDynamic_Cast_This<GameEngineCollision>(), _Event);
}

void GameEngineCollision::Release()
{
	// 내가 지금 사라질것인데. 예전에 나랑 충돌했던 충돌체들에게
	// 내가 죽으니 날 굳이 들고 있을 필요가 없다.
	//for (std::shared_ptr<GameEngineCollision> Collision : Others)
	//{
	//	Collision->Others.erase(GetDynamic_Cast_This<GameEngineCollision>());
	//}
}

void GameEngineCollision::Update(float _Delta)
{
	// 함수는 호출되는것 만으로 이미 처리를 한겁니다.
	if (true == GameEngineLevel::IsDebug)
	{
		switch (CollisionType)
		{
		case ColType::SPHERE2D:
			GameEngineDebug::DrawSphere2D(Transform, ColColor);
			break;
		case ColType::AABBBOX2D:
			GameEngineDebug::DrawBox2D(Transform, ColColor);
			break;
		case ColType::OBBBOX2D:
			GameEngineDebug::DrawBox2D(Transform, ColColor);
			break;
		case ColType::SPHERE3D:
			GameEngineDebug::DrawSphere2D(Transform, ColColor);
			break;
		case ColType::AABBBOX3D:
			GameEngineDebug::DrawBox3D(Transform, ColColor);
			break;
		case ColType::OBBBOX3D:
			GameEngineDebug::DrawBox3D(Transform, ColColor);
			break;
		case ColType::MAX:
			break;
		default:
			break;
		}
	}
}