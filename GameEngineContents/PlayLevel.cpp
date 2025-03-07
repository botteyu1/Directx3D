#include "PreCompile.h"

#include "PlayLevel.h"
#include "Player.h"

#include "GameEngineNetWindow.h"
#include "ContentsControlWindow.h"

#include "MonsterHpBar.h"

#include "WorldMap.h"
#include "Boss_Vordt.h"
#include "Monster_HollowSoldier.h"
#include "Monster_LothricKn.h"
#include "Monster_HollowSoldier_Spear.h"
#include "DummyActor.h"
#include "Monster_Hollow_RaggedRobes.h"

#include "AllFadeEffect.h"


#include "MainUIActor.h"
#include "UIAlertMaanger.h"
#include "UILocationAlert.h"
#include "UILoading.h"
#include "UISystemManager.h"

#include "Object_bonfire.h"

PlayLevel::PlayLevel()
{
}

PlayLevel::~PlayLevel()
{
}

void PlayLevel::Start()
{
	ContentLevel::Start();
	GameEngineInput::AddInputObject(this);

	GetMainCamera()->Transform.SetWorldRotation({ 0.0f,0.0f,0.0f });
	GetMainCamera()->Transform.SetWorldPosition({ 0.0f, 0.0f, -1000.0f });

	{
		std::shared_ptr<GameEngineLight> Object = CreateActor<GameEngineLight>(0);
		//Object->CreateShadowMap();
	}

}

void PlayLevel::Update(float _Delta)
{
	ContentLevel::Update(_Delta);

	if (GameEngineInput::IsDown('6',this))
	{
		UISystem->OnSystem(Enum_SystemType::Object_bonfire);
		//UIAlertMaanger::CallAlert(Enum_AlertType::BoneFire);
	}
	if (GameEngineInput::IsDown('7',this))
	{
		UISystem->OnSystem(Enum_SystemType::Object_FogWall);
		//UIAlertMaanger::CallAlert(Enum_AlertType::Destroy);
	}
	if (GameEngineInput::IsDown('8',this))
	{
		UISystem->OffSystem();
		//UIAlertMaanger::CallAlert(Enum_AlertType::YouDie);
	}

}

void PlayLevel::LevelStart(GameEngineLevel* _PrevLevel)
{
	/*{
		std::shared_ptr<WorldMap> GameMap = CreateActor<WorldMap>(0, "WorldMap");
	}*/

	


	{

	}

	GameEngineGUI::CreateGUIWindow<ContentsControlWindow>("Test");


	//GameEngineCore::GetBackBufferRenderTarget()->SetClearColor({ 1.0f,1.0f,1.0f,1.0f });

	CoreWindow = GameEngineGUI::FindGUIWindow<GameEngineCoreWindow>("GameEngineCoreWindow");

	

	
	{
		/*std::shared_ptr<Monster_LothricKn> Monster = CreateActor<Monster_LothricKn>(Enum_UpdateOrder::Monster, "Lothric");
		Monster->SetIdleType(Enum_Lothric_IdleType::Sit);
		Monster->WakeUp(); */

		//std::shared_ptr<Monster_Hollow_RaggedRobes> Monster2 = CreateActor<Monster_Hollow_RaggedRobes>(0);
		// GameMap->Transform.SetWorldPosition({ 0.0f,000.0f,-1000.3f });

		//std::shared_ptr<Monster_HollowSoldier_Spear> GameMap2 = CreateActor<Monster_HollowSoldier_Spear>(0, "WorldMap");

		//GameMap2->Transform.SetWorldPosition({ 0.0f,000.0f,-2000.3f });
		
		PlayerObject = CreateActor<Player>(0, "Player");
		// Monster->SetTargeting(PlayerObject.get());
		
		
	
		
		// 시작위치
		//PlayerObject->Transform.SetLocalPosition({ -1400.0f, 5101.0f, -5331.0f });
		

		/*std::shared_ptr<Monster_LothricKn> LothricKn = CreateActor<Monster_LothricKn>(static_cast<int>(Enum_UpdateOrder::Monster), "LothricKn");
		LothricKn->SetIdleType(Enum_Lothric_IdleType::Standing);
		LothricKn->SetWPosition(float4(100.0f, 0.0f, 0.0f));
		LothricKn->WakeUp();*/
	}
	
	// Boss_Object = CreateActor<Boss_Vordt>(Enum_UpdateOrder::Monster, "Boss_Vordt");
	// Boss_Object->Transform.SetWorldPosition({ 0.0f,000.0f,-1000.3f });

	{
		if (nullptr == GameEngineSprite::Find("Dark.png"))
		{
			GameEngineDirectory Dir;
			Dir.MoveParentToExistsChild("ContentsResources");
			Dir.MoveChild("ContentsResources");
			Dir.MoveChild("UITexture");
			std::vector<GameEngineFile> Files = Dir.GetAllFile();
			for (GameEngineFile& pFiles : Files)
			{
				GameEngineTexture::Load(pFiles.GetStringPath());
				GameEngineSprite::CreateSingle(pFiles.GetFileName());
			}
		}

		if (nullptr == GameEngineSprite::Find("DarkSoulsIII_Main_Menu_Theme.wav"))
		{
			GameEngineDirectory Dir;
			Dir.MoveParentToExistsChild("ContentsResources");
			Dir.MoveChild("ContentsResources\\Sound\\UI");

			std::vector<GameEngineFile> Files = Dir.GetAllFile();
			for (GameEngineFile& pFiles : Files)
			{
				GameEngineSound::SoundLoad(pFiles.GetStringPath());
			}
		}


		MainUI = CreateActor<MainUIActor>(Enum_UpdateOrder::UI);
		// MainUI->CreateBossUI(Boss_Object.get());
		MainUI->CreateAlertManger();
		MainUI->CreateAndCheckEsteUI(PlayerObject.get());
		MainUI->CreateAndCheckPlayerGaugeBar(PlayerObject.get());

		// NewLocation
		std::shared_ptr<UILocationAlert> UILot = CreateActor<UILocationAlert>();
		UILot->SetCollision(float4(200.0f, 200.0f, 200.0f), float4(300.0f, 0.0f, 0.0f));

		// LitBoneFire
		// std::shared_ptr<UIAlert_BoneLit> LitBone = CreateActor<UIAlert_BoneLit>();

		// 로딩테스트
		std::shared_ptr<UILoading> Loading = CreateActor<UILoading>(Enum_UpdateOrder::UI);

		std::shared_ptr<UISystemManager> Sys = CreateActor<UISystemManager>();


		UISystem = CreateActor<UISystemManager>(1);
		
	}

	GameEngineCore::GetBackBufferRenderTarget()->SetClearColor({ 1, 1, 1, 1 });

	// Effect
	FadeObject->FadeIn();
	FadeObject->On();

	// Test Ground
	physx::PxPhysics* Physics = GameEnginePhysX::GetPhysics();
	physx::PxMaterial* mMaterial = GameEnginePhysX::GetDefaultMaterial();
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*Physics, physx::PxPlane(0, 1, 0, 50), *mMaterial);
	Scene->addActor(*groundPlane);
}

void PlayLevel::LevelEnd(GameEngineLevel* _NextLevel)
{
	if (nullptr != GameEngineSprite::Find("Dark.Png"))
	{
		GameEngineDirectory Dir;
		Dir.MoveParentToExistsChild("ContentsResources");
		Dir.MoveChild("ContentsResources");
		Dir.MoveChild("UITexture");
		std::vector<GameEngineFile> Files = Dir.GetAllFile();
		for (GameEngineFile& pFiles : Files)
		{
			GameEngineSprite::Release(pFiles.GetFileName());
			GameEngineTexture::Release(pFiles.GetFileName());
		}
	}
}