#include "PreCompile.h"
#include "TestLevel_Monster.h"

#include <GameEngineCore/GameEngineGUI.h>

#include "ContentsLight.h"

#include "Monster_LothricKn.h"
#include "Monster_HollowSoldier_RoundShield.h"
#include "Monster_HollowSoldier_Sword.h"
#include "Monster_HollowSoldier_Spear.h"
#include "Monster_HollowSoldier_Lantern.h"
#include "Monster_Hollow_Unarmed.h"
#include "Monster_Hollow_RaggedRobes.h"

TestLevel_Monster::TestLevel_Monster() 
{
}

TestLevel_Monster::~TestLevel_Monster() 
{
}

void TestLevel_Monster::Start()
{
	ContentLevel::Start();

	std::shared_ptr<GameEngineLight> Object = CreateActor<GameEngineLight>(0);

	GetLevelRenderTarget()->SetClearColor(float4::BLUE);
}

void TestLevel_Monster::Update(float _Delta)
{
	ContentLevel::Update(_Delta);

	if (true)
	{
		std::vector<float4> Path
		{
			float4(0.0f),
			float4(0.0f,0.0f,500.0f),
			float4(500.0f,0.0f,500.0f),
			float4(500.0f)
		};

		for (const float4& Pos : Path)
		{
			GameEngineDebug::DrawSphere2D(float4(5.0f, 5.0f, 5.0f), float4::ZERONULL, Pos, float4(1.0f,1.0f));
		}
	}

	RayCast({ 100.0f, }, { 0.0f,0.0f, 5.0f }, 1000.0f);
}

void TestLevel_Monster::LevelStart(GameEngineLevel* _PrevLevel)
{
	// Test Ground
	physx::PxPhysics* Physics = GameEnginePhysX::GetPhysics();
	physx::PxMaterial* mMaterial = GameEnginePhysX::GetDefaultMaterial();
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*Physics, physx::PxPlane(0, 1, 0, 50), *mMaterial);
	Scene->addActor(*groundPlane);

	std::vector<float4> Path
	{
		float4(0.0f),
		float4(0.0f,0.0f,500.0f),
		float4(500.0f,0.0f,500.0f),
		float4(500.0f)
	};

	std::shared_ptr<Monster_LothricKn> LothricKn = CreateActor<Monster_LothricKn>(static_cast<int>(Enum_UpdateOrder::Monster), "LothricKn");
	LothricKn->SetPatrolPath(Path, 0, true);
	LothricKn->SetIdleType(Enum_Lothric_IdleType::Patrol);
	LothricKn->SetWPosition(float4(100.0f, 0.0f, 0.0f));
	LothricKn->WakeUp();
	LothricKn->DebugOn();

	//std::shared_ptr<Monster_LothricKn> LothricKn1 = CreateActor<Monster_LothricKn>(static_cast<int>(Enum_UpdateOrder::Monster), "LothricKn1");
	//LothricKn1->SetIdleType(Enum_Lothric_IdleType::Standing);
	//LothricKn1->SetWPosition(float4(300.0f, 0.0f, 0.0f));


	std::shared_ptr<Monster_HollowSoldier_Sword> Hollow = CreateActor<Monster_HollowSoldier_Sword>(static_cast<int>(Enum_UpdateOrder::Monster), "Hollow");
	Hollow->SetWPosition(float4(-400.0f, 0.0f, 0.0f));
	//Hollow->Transform.SetWorldRotation(float4(0.0f, 180.0f, 0.0f));
	//Hollow->Transform.SetWorldRotation(float4(0.0f, 90.0f, 0.0f));

	/*std::shared_ptr<Monster_Hollow_Unarmed> Unarmed = CreateActor<Monster_Hollow_Unarmed>(static_cast<int>(Enum_UpdateOrder::Monster), "Unarmed");
	Unarmed->SetWPosition(float4(-200.0f, 0.0f, 0.0f));*/
	//std::shared_ptr<Monster_Hollow_Unarmed> UnarmedHollow = CreateActor<Monster_Hollow_Unarmed>(static_cast<int>(Enum_UpdateOrder::Monster), "UnarmedHollow");
	//UnarmedHollow->SetWPosition(float4(-700.0f, 0.0f, 0.0f));

	GetMainCamera()->Transform.SetWorldPosition(float4(0.0f, 0.0f, -1000.0f));
}

void TestLevel_Monster::LevelEnd(GameEngineLevel* _NextLevel)
{
	AllDeathObjectGroupConvert<Monster_LothricKn>(Enum_UpdateOrder::Monster);
	AllDeathObjectGroupConvert<Monster_HollowSoldier_Sword>(Enum_UpdateOrder::Monster);
}