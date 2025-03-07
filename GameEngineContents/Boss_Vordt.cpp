﻿#include "PreCompile.h"
#include "Boss_Vordt.h"
#include "BoneSocketCollision.h"
#include "DS3DummyData.h"
#include "ContentsLight.h"
#include "LoadingLevel.h"

#define BOSS_HP 1328
#define PHYSX_RADIUS 280.f
#define PHYSX_HALFHEIGHT 5.f

void Boss_State_GUI::Start()
{

}

void Boss_State_GUI::OnGUI(GameEngineLevel* _Level, float _DeltaTime)
{
	if (nullptr == Linked_Boss)
	{
		return;
	}

	ImGui::NewLine();

	if (true == ImGui::Checkbox("Awake", &Linked_Boss->IsUpdateValue))
	{
		Linked_Boss->OnOffSwitch();
		Linked_Boss->OnOffSwitch();
	}

	ImGui::NewLine();

	if (ImGui::Checkbox("AI_OFF", &Linked_Boss->AI_Off))
	{
		switch (Linked_Boss->AI_Off)
		{
		case 0:
			Linked_Boss->AI_Start();
			break;
		case 1:
			Linked_Boss->AI_Stop();
			break;
		}
	}

	ImGui::NewLine();

	int Index = 0;

	if (0 == StateNames.size())
	{
		const std::map<int, State>* AllStates = Linked_Boss->MainState.GetAllStates();

		if (0 == StateNames.capacity())
		{
			StateNames.reserve(AllStates->size());
			StateIndex.reserve(AllStates->size());
		}

		std::map<int, State>::const_iterator StartIter = AllStates->begin();
		std::map<int, State>::const_iterator EndIter = AllStates->end();
		for (; StartIter != EndIter; ++StartIter)
		{
			StateNames.push_back(StartIter->second.Name.c_str());
			StateIndex.push_back(StartIter->first);
		}
	}

	{
		if (ImGui::ListBox("State", &Index, &StateNames[0], static_cast<int>(StateNames.size())))
		{
			Linked_Boss->MainState.ChangeState(StateIndex[Index]);
		}
	}

	ImGui::NewLine();

	{
		int Frame = Linked_Boss->MainRenderer->GetCurAnimationFrame();
		std::string String = "AniFrame\n";
		String += std::to_string(Frame);
		ImGui::Text(String.c_str());
	}

	ImGui::NewLine();

	bool IsRootMotion = Linked_Boss->MainRenderer->GetCurAnimation()->IsRootMotion();
	if (true == ImGui::Checkbox("RootMotion", &IsRootMotion))
	{
		Linked_Boss->MainRenderer->GetCurAnimation()->SwitchRootMotion();
	}

	ImGui::NewLine();

	bool IsRotation = Linked_Boss->MainRenderer->GetCurAnimation()->IsRootMotionRot();
	if (true == ImGui::Checkbox("RootMotionRot", &IsRotation))
	{
		Linked_Boss->MainRenderer->GetCurAnimation()->SwitchRootMotionRot();
	}

	ImGui::NewLine();

	{
		physx::PxVec3 Vec = Linked_Boss->Capsule->GetLinearVelocity();

		Vec.x = floor(Vec.x * 100) / 100;
		Vec.y = floor(Vec.y * 100) / 100;
		if (Vec.y < 0.001f)
		{
			Vec.y = 0.00f;
		}
		Vec.z = floor(Vec.z * 100) / 100;

		std::string Speed = "Speed\n";
		Speed += "X : " + std::to_string(Vec.x) + "\n";
		Speed += "Y : " + std::to_string(Vec.y) + "\n";
		Speed += "Z : " + std::to_string(Vec.z) + "\n";
		ImGui::Text(Speed.c_str());
	}

	ImGui::NewLine();

	{
		physx::PxVec3 Vec = Linked_Boss->Capsule->GetWorldPosition();

		Vec.x = floor(Vec.x * 100) / 100;
		Vec.y = floor(Vec.y * 100) / 100;
		Vec.z = floor(Vec.z * 100) / 100;

		std::string Pos = "Pos\n";
		Pos += "X : " + std::to_string(Vec.x) + "\n";
		Pos += "Y : " + std::to_string(Vec.y) + "\n";
		Pos += "Z : " + std::to_string(Vec.z) + "\n";
		ImGui::Text(Pos.c_str());
	}

	ImGui::NewLine();

	{
		std::string String = "Z Dir\n";
		ImGui::Text(String.c_str());
		ImGui::DragFloat("Z Dir", Linked_Boss->Capsule->GetDirPtr(), 0.1f, -180.f, 180.f);
	}

	ImGui::NewLine();

	{
		std::string String = "Z Rotation\n";
		ImGui::Text(String.c_str());
		float4 Rot = Linked_Boss->Transform.GetWorldRotationEuler();
		if (true == ImGui::DragFloat("Z Rot", &Rot.Y, 0.1f, -180.f, 180.f))
		{
			Linked_Boss->Capsule->SetWorldRotation(Rot);
		}
	}

	ImGui::NewLine();

	{
		bool Result = Linked_Boss->Capsule->IsGravity();
		std::string Gravity = "IsGravity\n";
		switch (Result)
		{
		case 0:
			Gravity += " False";
			break;
		default:
			Gravity += " True";
			break;
		}
		ImGui::Text(Gravity.c_str());
	}

	ImGui::NewLine();

	{
		float Angle = Linked_Boss->Capsule->GetDir();
		std::string cAngle = "My Angle : ";
		cAngle += std::to_string(Angle);
		ImGui::Text(cAngle.c_str());
	}

	ImGui::NewLine();

	if (true == Linked_Boss->IsTargeting())
	{
		float Angle = Linked_Boss->GetTargetAngle();
		std::string cAngle = "Target Angle : ";
		cAngle += std::to_string(Angle);
		cAngle += "(";

		std::string eAngle = "";
		switch (Linked_Boss->mTargetState.mTargetDeg)
		{
		case Enum_TargetDeg::Deg_Null:
			eAngle += "Null";
			break;
		case Enum_TargetDeg::Deg_Front:
			eAngle += "Front";
			break;
		case Enum_TargetDeg::Deg_Side:
			eAngle += "Side";
			break;
		case Enum_TargetDeg::Deg_Back:
			eAngle += "Back";
			break;
		default:
			break;
		}

		cAngle += eAngle + ")";

		ImGui::Text(cAngle.c_str());

		Enum_RotDir Dir = Linked_Boss->GetRotDir_e();
		std::string cDir = "Rotation Dir : ";

		switch (Dir)
		{
		case Enum_RotDir::Not_Rot:
			cDir += "Not_Rot";
			break;
		case Enum_RotDir::Left:
			cDir += "Left";
			break;
		case Enum_RotDir::Right:
			cDir += "Right";
			break;
		default:
			MsgBoxAssert("존재하지 않는 회전방향입니다.");
			break;
		}

		ImGui::Text(cDir.c_str());
	}

	ImGui::NewLine();

	if (true == Linked_Boss->IsTargeting())
	{
		float Dis = Linked_Boss->GetTargetDistance();
		std::string cDistance = "Target Distance : ";
		cDistance += std::to_string(Dis);
		cDistance += "(";

		std::string eDistance = "";
		switch (Linked_Boss->mTargetState.mTargetDis)
		{
		case Enum_TargetDis::Dis_Null:
			eDistance += "Null";
			break;
		case Enum_TargetDis::Dis_Attach:
			eDistance += "Attach";
			break;
		case Enum_TargetDis::Dis_Close:
			eDistance += "Close";
			break;
		case Enum_TargetDis::Dis_Middle:
			eDistance += "Middle";
			break;
		case Enum_TargetDis::Dis_Long:
			eDistance += "Long";
			break;
		case Enum_TargetDis::Dis_Far:
			eDistance += "Far";
			break;
		case Enum_TargetDis::Dis_OutOfRange:
			eDistance += "OutOfRange";
			break;
		default:
			break;
		}

		cDistance += eDistance +")";

		ImGui::Text(cDistance.c_str());
	}

	ImGui::NewLine();

	{
		if (true == ImGui::Button("SetWorldOrigin", ImVec2(250, 30)))
		{
			Linked_Boss->Capsule->SetWorldPosition({0.f, 0.f, 0.f});
			Linked_Boss->Capsule->SetWorldRotation({0.f, 180.f, 0.f});
			Linked_Boss->MainState.ChangeState(Enum_BossState::Idle);
		}
	}
}

void Boss_State_GUI::Reset()
{
	StateNames.clear();
	Linked_Boss = nullptr;
}

void AI_State::Update(float _Delta)
{
	CurCoolDown -= _Delta;
}

void AI_State::Reset()
{
	CurCoolDown = 0.f;
}

void Vordt_HitCollision::Off()
{
	BodyCollision->Off();
	L_Claf_Collision->Off();
	L_ClafWist1_Collision->Off();
	L_Foot_Collision->Off();
	L_Toe_Collision->Off();
	L_UpArmTwist_Collision->Off();
	L_ForeArm_Collision->Off();
	L_Hand_Collision->Off();
	R_Claf_Collision->Off();
	R_ClafWist1_Collision->Off();
	R_Foot_Collision->Off();
	R_Toe_Collision->Off();
	R_UpArmTwist_Collision->Off();
	R_ForeArm_Collision->Off();
	R_Hand_Collision->Off();
	Neck_Collision->Off();
	Throw_Collision->Off();
}

void Vordt_HitCollision::On()
{
	BodyCollision->On();
	L_Claf_Collision->On();
	L_ClafWist1_Collision->On();
	L_Foot_Collision->On();
	L_Toe_Collision->On();
	L_UpArmTwist_Collision->On();
	L_ForeArm_Collision->On();
	L_Hand_Collision->On();
	R_Claf_Collision->On();
	R_ClafWist1_Collision->On();
	R_Foot_Collision->On();
	R_Toe_Collision->On();
	R_UpArmTwist_Collision->On();
	R_ForeArm_Collision->On();
	R_Hand_Collision->On();
	Neck_Collision->On();
	Throw_Collision->On();
}

void Vordt_HitCollision::Release()
{
	if (nullptr != BodyCollision)
	{
		BodyCollision->Death();
		BodyCollision = nullptr;
	}

	if (nullptr != L_Claf_Collision)
	{
		L_Claf_Collision->Death();
		L_Claf_Collision = nullptr;
	}

	if (nullptr != L_Foot_Collision)
	{
		L_Foot_Collision->Death();
		L_Foot_Collision = nullptr;
	}

	if (nullptr != L_Toe_Collision)
	{
		L_Toe_Collision->Death();
		L_Toe_Collision = nullptr;
	}

	if (nullptr != L_UpArmTwist_Collision)
	{
		L_UpArmTwist_Collision->Death();
		L_UpArmTwist_Collision = nullptr;
	}

	if (nullptr != L_ForeArm_Collision)
	{
		L_ForeArm_Collision->Death();
		L_ForeArm_Collision = nullptr;
	}

	if (nullptr != L_Hand_Collision)
	{
		L_Hand_Collision->Death();
		L_Hand_Collision = nullptr;
	}

	if (nullptr != R_Claf_Collision)
	{
		R_Claf_Collision->Death();
		R_Claf_Collision = nullptr;
	}

	if (nullptr != R_Foot_Collision)
	{
		R_Foot_Collision->Death();
		R_Foot_Collision = nullptr;
	}

	if (nullptr != R_Toe_Collision)
	{
		R_Toe_Collision->Death();
		R_Toe_Collision = nullptr;
	}

	if (nullptr != R_UpArmTwist_Collision)
	{
		R_UpArmTwist_Collision->Death();
		R_UpArmTwist_Collision = nullptr;
	}

	if (nullptr != R_ForeArm_Collision)
	{
		R_ForeArm_Collision->Death();
		R_ForeArm_Collision = nullptr;
	}

	if (nullptr != R_Hand_Collision)
	{
		R_Hand_Collision->Death();
		R_Hand_Collision = nullptr;
	}

	if (nullptr != Neck_Collision)
	{
		Neck_Collision->Death();
		Neck_Collision = nullptr;
	}

	if (nullptr != Throw_Collision)
	{
		Throw_Collision->Death();
		Throw_Collision = nullptr;
	}
}

void Vordt_AttackCollision::Off()
{
	BodyCollision->Off();
	HeadCollision->Off();
	WeaponCollision->Off();
	R_HandCollision->Off();
}

void Vordt_AttackCollision::ResetRecord()
{
	mBodyHitInteraction.ResetRecord();
	mHeadHitInteraction.ResetRecord();
	mWeaponHitInteraction.ResetRecord();
	mHandHitInteraction.ResetRecord();
}

void Vordt_AttackCollision::Release()
{
	if (nullptr != WeaponCollision)
	{
		WeaponCollision->Death();
		WeaponCollision = nullptr;
	}

	if (nullptr != BodyCollision)
	{
		BodyCollision->Death();
		BodyCollision = nullptr;
	}

	if (nullptr != HeadCollision)
	{
		HeadCollision->Death();
		HeadCollision = nullptr;
	}

	if (nullptr != R_HandCollision)
	{
		R_HandCollision->Death();
		R_HandCollision = nullptr;
	}
}


Boss_Vordt::Boss_Vordt()
{

}

Boss_Vordt::~Boss_Vordt()
{

}

void Boss_Vordt::LevelStart(GameEngineLevel* _PrevLevel)
{
	AI_Stop();
	Off();
}

void Boss_Vordt::LevelEnd(GameEngineLevel* _NextLevel)
{
	GUI->Linked_Boss = nullptr;
	GUI->Off();
	// Death();
}

void Boss_Vordt::Start()
{
	// BaseActor::Start();
	SetID(Enum_ActorType::Boss_Vordt);
	GameEngineInput::AddInputObject(this);

	if (nullptr == MainRenderer)
	{
		MainRenderer = CreateComponent<GameContentsFBXRenderer>(Enum_RenderOrder::Monster);
	}

	MainRenderer->Transform.SetLocalScale({ W_SCALE, W_SCALE, W_SCALE });
	MainRenderer->Transform.SetLocalPosition({ 0.f, 0.f, -130.0f });

	if (nullptr == Capsule)
	{
		Capsule = CreateComponent<GameEnginePhysXCapsule>();
		Capsule->PhysXComponentInit(PHYSX_RADIUS, PHYSX_HALFHEIGHT);
		Capsule->SetPositioningComponent();
		Capsule->SetFiltering(Enum_CollisionOrder::Monster);
	}

	StatInit();

	mFaceLight.FaceLightForward = FaceLightInit();
	mFaceLight.FaceLightBack = FaceLightInit();
	mFaceLight.FaceLightLeft = FaceLightInit();
	mFaceLight.FaceLightRight = FaceLightInit();

	// Boss Mesh
	{
		if (nullptr == GameEngineFBXMesh::Find("c2240.fbx"))
		{
			GameEngineFile File;
			File.MoveParentToExistsChild("ContentsResources");
			File.MoveChild("ContentsResources\\Mesh\\Boss\\c2240.fbx");
			GameEngineFBXMesh::Load(File.GetStringPath());
		}

		MainRenderer->SetFBXMesh("c2240.FBX", "FBX_Animation"); // Bone 136
	}

	// Boss Animation
	{
		GameEngineDirectory Dir;
		Dir.MoveParentToExistsChild("ContentsResources");
		Dir.MoveChild("ContentsResources\\Mesh\\c2240\\Animation");
		std::vector<GameEngineFile> Files = Dir.GetAllFile({ ".fbx" }, true);

		for (size_t i = 0; i < Files.size(); i++)
		{
			GameEngineFBXAnimation::Load(Files[i].GetStringPath());
		}

		// Animation
		MainRenderer->CreateFBXAnimation("Howling", "Howling.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Idle", "Idle.FBX", { ONE_FRAME_DTIME, true });
		MainRenderer->CreateFBXAnimation("Walk_Front", "Walk_Front.FBX", { ONE_FRAME_DTIME, true });
		MainRenderer->CreateFBXAnimation("Walk_Left", "Walk_Left.FBX", { ONE_FRAME_DTIME, true });
		MainRenderer->CreateFBXAnimation("Walk_Right", "Walk_Right.FBX", { ONE_FRAME_DTIME, true });
		MainRenderer->CreateFBXAnimation("Jump_Back", "Jump_Back.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Jump_Left", "Jump_Left.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Jump_Right", "Jump_Right.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Breath", "Breath.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Combo1_Step1", "Combo1_Step1.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Combo1_Step2", "Combo1_Step2.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Combo1_Step3", "Combo1_Step3.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Combo2_Step1", "Combo2_Step1.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Combo2_Step2", "Combo2_Step2.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Death", "Death.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Death_Groggy", "Death_Groggy.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Sweep&Sweep_Left", "Sweep&Sweep_Left.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Sweep&Sweep_Right", "Sweep&Sweep_Right.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_001", "Hit_001.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_002", "Hit_002.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_003_Left", "Hit_003_Left.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_003_Right", "Hit_003_Right.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_004_Groggy", "Hit_004_Groggy.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Groggy", "Hit_Groggy.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Down_001_Front", "Hit_Down_001_Front.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Down_001_Right", "Hit_Down_001_Right.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Down_001_Left", "Hit_Down_001_Left.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Down_004", "Hit_Down_004.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Down_005", "Hit_Down_005.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Hit_Down_006", "Hit_Down_006.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Rush&Hit&Turn&Rush", "Rush&Hit&Turn&Rush.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Rush&Hit&Turn", "Rush&Hit&Turn.FBX", { ONE_FRAME_DTIME, true });
		MainRenderer->CreateFBXAnimation("Rush&Turn", "Rush&Turn.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Rush_Attack", "Rush_Attack.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Rush_Attack_002", "Rush_Attack_002.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Rush_Front", "Rush_Front.FBX", { ONE_FRAME_DTIME * 1.5f, true });
		MainRenderer->CreateFBXAnimation("Sweep_001", "Sweep_001.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Sweep_002", "Sweep_002.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Thrust", "Thrust.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Turn_Left", "Turn_Left.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Turn_Left_Twice", "Turn_Left_Twice.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Turn_Right", "Turn_Right.FBX", { ONE_FRAME_DTIME, false });
		MainRenderer->CreateFBXAnimation("Turn_Right_Twice", "Turn_Right_Twice.FBX", { ONE_FRAME_DTIME, false });

		FrameEventInit();

		//////// Root Motion
		// Rotate to StartDir
		MainRenderer->SetRootMotionComponent(Capsule.get());
		MainRenderer->SetRootMotion("Breath");
		MainRenderer->SetRootMotion("Death");
		MainRenderer->SetRootMotion("Death_Groggy");
		MainRenderer->SetRootMotion("Hit_001");
		MainRenderer->SetRootMotion("Hit_002");
		MainRenderer->SetRootMotion("Hit_003_Left");
		MainRenderer->SetRootMotion("Hit_003_Right");
		MainRenderer->SetRootMotion("Hit_004_Groggy");
		MainRenderer->SetRootMotion("Hit_Groggy");
		MainRenderer->SetRootMotion("Howling");
		MainRenderer->SetRootMotion("Rush_Front");
		MainRenderer->SetRootMotion("Rush&Turn"); // 
		MainRenderer->SetRootMotion("Turn_Left");
		MainRenderer->SetRootMotion("Turn_Left_Twice");
		MainRenderer->SetRootMotion("Turn_Right");
		MainRenderer->SetRootMotion("Turn_Right_Twice");

		// Rotate to RealTimeDir
		MainRenderer->SetRootMotion("Idle", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Jump_Back", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Jump_Left", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Jump_Right", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Walk_Front", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Walk_Left", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Walk_Right", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Combo1_Step1", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Combo1_Step2", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Combo1_Step3", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Combo2_Step1", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Combo2_Step2", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Hit_Down_001_Front", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Hit_Down_001_Right", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Hit_Down_001_Left", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Hit_Down_004", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Hit_Down_005", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Hit_Down_006", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Thrust", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Sweep&Sweep_Left", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Sweep&Sweep_Right", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Sweep_001", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Sweep_002", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Rush_Attack", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Rush_Attack_002", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Rush&Hit&Turn", "", Enum_RootMotionMode::RealTimeDir);
		MainRenderer->SetRootMotion("Rush&Hit&Turn&Rush", "", Enum_RootMotionMode::RealTimeDir); // 

		MainRenderer->SetAllRootMotionMoveRatio(0.8f, 0.8f, 0.8f);
		MainRenderer->SetRootMotionMoveRatio("Walk_Front", 0.6f, 0.6f, 1.5f);
		MainRenderer->SetRootMotionMoveRatio("Walk_Left", 1.5f, 0.6f, 0.6f);
		MainRenderer->SetRootMotionMoveRatio("Walk_Right", 1.5f, 0.6f, 0.6f);

		//////// Blend
		MainRenderer->SetBlendTime("Breath", 10);
		MainRenderer->SetBlendTime("Death", 2);
		MainRenderer->SetBlendTime("Death_Groggy", 2);
		MainRenderer->SetBlendTime("Hit_001", 1);
		MainRenderer->SetBlendTime("Hit_002", 1);
		MainRenderer->SetBlendTime("Hit_003_Left", 1);
		MainRenderer->SetBlendTime("Hit_003_Right", 1);
		MainRenderer->SetBlendTime("Hit_004_Groggy", 1);
		MainRenderer->SetBlendTime("Hit_Groggy", 2);
		MainRenderer->SetBlendTime("Howling", 7);
		MainRenderer->SetBlendTime("Rush_Front", 10);
		MainRenderer->SetBlendTime("Rush&Turn", 10);
		MainRenderer->SetBlendTime("Turn_Left", 12);
		MainRenderer->SetBlendTime("Turn_Left_Twice", 12);
		MainRenderer->SetBlendTime("Turn_Right", 12);
		MainRenderer->SetBlendTime("Turn_Right_Twice", 12);
		MainRenderer->SetBlendTime("Idle", 7);
		MainRenderer->SetBlendTime("Jump_Back", 7);
		MainRenderer->SetBlendTime("Jump_Left", 7);
		MainRenderer->SetBlendTime("Jump_Right", 7);
		MainRenderer->SetBlendTime("Walk_Front", 15);
		MainRenderer->SetBlendTime("Walk_Left", 15);
		MainRenderer->SetBlendTime("Walk_Right", 15);
		MainRenderer->SetBlendTime("Combo1_Step1", 10);
		MainRenderer->SetBlendTime("Combo1_Step2", 10);
		MainRenderer->SetBlendTime("Combo1_Step3", 10);
		MainRenderer->SetBlendTime("Combo2_Step1", 10);
		MainRenderer->SetBlendTime("Combo2_Step2", 10);
		MainRenderer->SetBlendTime("Hit_Down_001_Front", 10);
		MainRenderer->SetBlendTime("Hit_Down_001_Right", 10);
		MainRenderer->SetBlendTime("Hit_Down_001_Left", 10);
		MainRenderer->SetBlendTime("Hit_Down_004", 10);
		MainRenderer->SetBlendTime("Hit_Down_005", 10);
		MainRenderer->SetBlendTime("Hit_Down_006", 10);
		MainRenderer->SetBlendTime("Thrust", 12);
		MainRenderer->SetBlendTime("Sweep&Sweep_Left", 10);
		MainRenderer->SetBlendTime("Sweep&Sweep_Right", 10);
		MainRenderer->SetBlendTime("Sweep_001", 10);
		MainRenderer->SetBlendTime("Sweep_002", 10);
		MainRenderer->SetBlendTime("Rush_Attack", 10);
		MainRenderer->SetBlendTime("Rush_Attack_002", 10);
		MainRenderer->SetBlendTime("Rush&Hit&Turn", 10);
		MainRenderer->SetBlendTime("Rush&Hit&Turn&Rush", 10);

	}

	if (nullptr == GameEngineGUI::FindGUIWindow<Boss_State_GUI>("Boss_State"))
	{
		GameEngineGUI::CreateGUIWindow<Boss_State_GUI>("Boss_State");
	}

	StateInit();

	GUI = GameEngineGUI::FindGUIWindow<Boss_State_GUI>("Boss_State");
	GUI->Off();

	//// Sound
	{
		GameEngineDirectory Dir;
		Dir.SetCurrentPath();
		Dir.MoveParentToExistsChild("ContentsResources");
		Dir.MoveChild("ContentsResources\\Sound\\c2240");
		std::vector<GameEngineFile> AllFile = Dir.GetAllFile();

		for (int i = 0; i < AllFile.size(); i++)
		{
			GameEngineSound::Sound3DLoad(AllFile[i].GetStringPath());
		}
	}

	// RockOnCollision
	if (nullptr == RockOnCollision)
	{
		RockOnCollision = CreateComponent<GameEngineCollision>(Enum_CollisionOrder::Monster);
		RockOnCollision->Transform.SetLocalPosition({ 0.f, PHYSX_RADIUS / 2.f + PHYSX_HALFHEIGHT, 0.f });
		RockOnCollision->Transform.SetWorldScale({ 1.f, 1.f, 1.f });
	}

	//////// Socket Collision
	// HitCollision
	BSCol_TransitionParameter ColParameter;
	if (nullptr == mHitCollision.BodyCollision)
	{
		ColParameter.S = float4(350.f, 350.f, 350.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, 0.f);

		mHitCollision.BodyCollision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 21, ColParameter, "Hit_Collision");
		mHitCollision.BodyCollision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.BodyCollision->On();
	}

	if (nullptr == mHitCollision.L_Claf_Collision)
	{
		ColParameter.S = float4(140.f, 140.f, 140.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, 0.f);

		mHitCollision.L_Claf_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 7, ColParameter, "Hit_Collision");
		mHitCollision.L_Claf_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_Claf_Collision->On();
	}

	if (nullptr == mHitCollision.L_ClafWist1_Collision)
	{
		ColParameter.S = float4(120.f, 120.f, 120.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(1.f, 0.f, 0.f);

		mHitCollision.L_ClafWist1_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 9, ColParameter, "Hit_Collision");
		mHitCollision.L_ClafWist1_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_ClafWist1_Collision->On();
	}

	if (nullptr == mHitCollision.L_Foot_Collision)
	{
		ColParameter.S = float4(75.f, 75.f, 75.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.24f, 0.f, 0.f);

		mHitCollision.L_Foot_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 10, ColParameter, "Hit_Collision");
		mHitCollision.L_Foot_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_Foot_Collision->On();
	}

	if (nullptr == mHitCollision.L_Toe_Collision)
	{
		ColParameter.S = float4(65.f, 65.f, 65.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, -0.16f);

		mHitCollision.L_Toe_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 11, ColParameter, "Hit_Collision");
		mHitCollision.L_Toe_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_Toe_Collision->On();
	}

	if (nullptr == mHitCollision.L_UpArmTwist_Collision)
	{
		ColParameter.S = float4(125.f, 125.f, 125.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, 0.f);

		mHitCollision.L_UpArmTwist_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 27, ColParameter, "Hit_Collision");
		mHitCollision.L_UpArmTwist_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_UpArmTwist_Collision->On();
	}

	if (nullptr == mHitCollision.L_ForeArm_Collision)
	{
		ColParameter.S = float4(110.f, 110.f, 110.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.16f, 0.f, 0.f);

		mHitCollision.L_ForeArm_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 30, ColParameter, "Hit_Collision");
		mHitCollision.L_ForeArm_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_ForeArm_Collision->On();
	}

	if (nullptr == mHitCollision.L_Hand_Collision)
	{
		ColParameter.S = float4(110.f, 110.f, 110.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(-0.16f, 0.f, 0.f);

		mHitCollision.L_Hand_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 32, ColParameter, "Hit_Collision");
		mHitCollision.L_Hand_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.L_Hand_Collision->On();
	}

	if (nullptr == mHitCollision.R_Claf_Collision)
	{
		ColParameter.S = float4(140.f, 140.f, 140.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, 0.f);

		mHitCollision.R_Claf_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 16, ColParameter, "Hit_Collision");
		mHitCollision.R_Claf_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_Claf_Collision->On();
	}

	if (nullptr == mHitCollision.R_ClafWist1_Collision)
	{
		ColParameter.S = float4(120.f, 120.f, 120.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(1.f, 0.f, 0.f);

		mHitCollision.R_ClafWist1_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 18, ColParameter, "Hit_Collision");
		mHitCollision.R_ClafWist1_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_ClafWist1_Collision->On();
	}

	if (nullptr == mHitCollision.R_Foot_Collision)
	{
		ColParameter.S = float4(75.f, 75.f, 75.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.24f, 0.f, 0.f);

		mHitCollision.R_Foot_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 19, ColParameter, "Hit_Collision");
		mHitCollision.R_Foot_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_Foot_Collision->On();
	}

	if (nullptr == mHitCollision.R_Toe_Collision)
	{
		ColParameter.S = float4(65.f, 65.f, 65.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, -0.16f);

		mHitCollision.R_Toe_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 20, ColParameter, "Hit_Collision");
		mHitCollision.R_Toe_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_Toe_Collision->On();
	}

	if (nullptr == mHitCollision.R_UpArmTwist_Collision)
	{
		ColParameter.S = float4(125.f, 125.f, 125.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, 0.f);

		mHitCollision.R_UpArmTwist_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 53, ColParameter, "Hit_Collision");
		mHitCollision.R_UpArmTwist_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_UpArmTwist_Collision->On();
	}

	if (nullptr == mHitCollision.R_ForeArm_Collision)
	{
		ColParameter.S = float4(110.f, 110.f, 110.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.16f, 0.f, 0.f);

		mHitCollision.R_ForeArm_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 56, ColParameter, "Hit_Collision");
		mHitCollision.R_ForeArm_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_ForeArm_Collision->On();
	}

	if (nullptr == mHitCollision.R_Hand_Collision)
	{
		ColParameter.S = float4(110.f, 110.f, 110.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(-0.16f, 0.f, 0.f);

		mHitCollision.R_Hand_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 58, ColParameter, "Hit_Collision");
		mHitCollision.R_Hand_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.R_Hand_Collision->On();
	}

	if (nullptr == mHitCollision.Neck_Collision)
	{
		ColParameter.S = float4(150.f, 150.f, 150.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.68f, 0.f, 0.28f);

		mHitCollision.Neck_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 75, ColParameter, "Hit_Collision");
		mHitCollision.Neck_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.Neck_Collision->On();
	}

	if (nullptr == mHitCollision.Throw_Collision)
	{
		ColParameter.S = float4(150.f, 150.f, 150.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, -0.68f, 0.6f);

		mHitCollision.Throw_Collision = CreateSocketCollision(Enum_CollisionOrder::Monster_Body, 99, ColParameter, "Hit_Collision");
		mHitCollision.Throw_Collision->SetCollisionType(ColType::SPHERE3D);
		mHitCollision.Throw_Collision->On();
	}

	// AttackCollision
	if (nullptr == mAttackCollision.WeaponCollision)
	{
		ColParameter.S = float4(150.f, 150.f, 500.f);
		ColParameter.R = float4(170.f);
		ColParameter.T = float4(0.f, 0.f, 1.55f);

		mAttackCollision.WeaponCollision = CreateSocketCollision(Enum_CollisionOrder::MonsterAttack, 47, ColParameter, "Weapon");
		mAttackCollision.WeaponCollision->SetCollisionType(ColType::OBBBOX3D);
		mAttackCollision.mWeaponHitInteraction.Init(this, mAttackCollision.WeaponCollision.get());
	}

	if (nullptr == mAttackCollision.BodyCollision)
	{
		ColParameter.S = float4(300.f, 300.f, 300.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, -0.5f);

		mAttackCollision.BodyCollision = CreateSocketCollision(Enum_CollisionOrder::MonsterAttack, 22, ColParameter, "Body");
		mAttackCollision.BodyCollision->SetCollisionType(ColType::SPHERE3D);
		mAttackCollision.mBodyHitInteraction.Init(this, mAttackCollision.BodyCollision.get());
	}

	if (nullptr == mAttackCollision.HeadCollision)
	{
		ColParameter.S = float4(200.f, 200.f, 200.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(0.f, 0.f, -0.5f);

		mAttackCollision.HeadCollision = CreateSocketCollision(Enum_CollisionOrder::MonsterAttack, 76, ColParameter, "Head");
		mAttackCollision.HeadCollision->SetCollisionType(ColType::SPHERE3D);
		mAttackCollision.mHeadHitInteraction.Init(this, mAttackCollision.HeadCollision.get());
	}

	if (nullptr == mAttackCollision.R_HandCollision)
	{
		ColParameter.S = float4(180.f, 70.f, 40.f);
		ColParameter.R = float4(0.f);
		ColParameter.T = float4(-0.5f, 0.f, -0.15f);

		mAttackCollision.R_HandCollision = CreateSocketCollision(Enum_CollisionOrder::MonsterAttack, 57, ColParameter, "R_Hand");
		mAttackCollision.R_HandCollision->SetCollisionType(ColType::OBBBOX3D);
		mAttackCollision.mHandHitInteraction.Init(this, mAttackCollision.R_HandCollision.get());
	}

	DS3DummyData::LoadDummyData(static_cast<int>(Enum_ActorType::Boss_Vordt));
	DummyPolyMinVolume = 200.f;
	DummyPolyMaxVolume = 8000.f;
}

void Boss_Vordt::Update(float _Delta)
{
	BaseActor::Update(_Delta);

	MostPrioritzedUpdate();
	AIUpdate(_Delta);
	TargetStateUpdate();
	CollisionUpdate();
	PhaseChangeCheck();
	HitUpdate(_Delta);
	FaceLightUpdate();
}

void Boss_Vordt::Release()
{
	if (nullptr != GUI)
	{
		GUI->Reset();
		GUI->Off();
		GUI = nullptr;
	}

	if (nullptr != MainRenderer)
	{
		MainRenderer->Death();
		MainRenderer = nullptr;
	}

	mAttackCollision.Release();
	mHitCollision.Release();

	if (nullptr != Capsule)
	{
		Capsule->Death();
		Capsule = nullptr;
	}

	AI_States.clear();

	BaseActor::Release();
}

void Boss_Vordt::StatInit()
{
	Stat.SetHp(BOSS_HP);
	Stat.SetAtt(0);
	Stat.SetSouls(3000);
	Stat.SetPoise(100);
}

void Boss_Vordt::AI_Start()
{
	MainState.ChangeState(Enum_BossState::Howling);
	DummyPolySoundOn();
	AI_Off = false;
	mHitCollision.On();
	GUI->On();
	GUI->Linked_Boss = this;
}

void Boss_Vordt::AI_Stop()
{
	MainState.ChangeState(Enum_BossState::Idle);
	DummyPolySoundOff();
	AI_Off = true;
	mHitCollision.Off();
}

bool Boss_Vordt::GetHit(const HitParameter& _Para /*= HitParameter()*/)
{
	if (nullptr == _Para.pAttacker)
	{
		MsgBoxAssert("공격자를 모르고 사용할 수 없는 기능입니다.");
		return false;
	}

	if (true == Hit.IsHit())
	{
		return false;
	}

	BaseActor* CurAttacker = _Para.pAttacker;

	const int AttackerAtt = CurAttacker->GetAtt();
	const int Stiffness = _Para.iStiffness;

	std::string SoundName = "damage_SE";
	if (1 != HitSoune_Count)
	{
		SoundName += std::to_string(HitSoune_Count);
	}

	SoundName += ".wav";
	GameEngineSound::Sound3DPlay(SoundName, Transform.GetWorldPosition());

	++HitSoune_Count;

	if (3 < HitSoune_Count)
	{
		HitSoune_Count = 1;
	}

	Stat.AddPoise(-20);
	Stat.AddHp(-AttackerAtt);
	Hit.SetHit(true);
	Hit.SetHitDir(_Para.eDir);
	return true;
}

bool Boss_Vordt::GetHitToShield(const HitParameter& _Para /*= HitParameter()*/)
{
	return false;
}

void Boss_Vordt::Reset()
{
	if (nullptr == this)
	{
		return;
	}

	for (std::pair<Enum_BossState, AI_State> _Pair : AI_States)
	{
		_Pair.second.Reset();
	}

	Hit_CoolDown = 0.f;
	HitSoune_Count = 1;
	Rush_Combo_Count = 0;
	Groggy_Count = 0;
	mBoss_Phase = Enum_Boss_Phase::Phase_1;
	SetFlagNull();
	StatInit();
	AI_Stop();
	BaseActor::SetWorldPosition(ResponPos);
	BaseActor::SetWorldRotation(ResponRot);
}

void Boss_Vordt::TargetStateUpdate()
{
	if (false == IsTargeting())
	{
		mTargetState.mTargetDeg = Enum_TargetDeg::Deg_Null;
		mTargetState.mTargetDis = Enum_TargetDis::Dis_Null;
		return;
	}

	float CurTargetAngle = abs(GetTargetAngle());
	float CurTargetDis = GetTargetDistance();

	CurTargetAngle /= Degree_Standard;
	CurTargetDis /= Distance_Standard;

	int ICurTargetAngle = static_cast<int>(CurTargetAngle);
	int ICurTargetDis = static_cast<int>(CurTargetDis);

	if (Enum_TargetDis::Dis_Attach > ICurTargetDis)
	{
		ICurTargetDis = Enum_TargetDis::Dis_Attach;
	}
	else if (Enum_TargetDis::Dis_OutOfRange < ICurTargetDis)
	{
		ICurTargetDis = Enum_TargetDis::Dis_OutOfRange;
	}

	if (Enum_TargetDeg:: Deg_Front > ICurTargetAngle)
	{
		ICurTargetAngle = Enum_TargetDeg::Deg_Front;
	}
	else if (Enum_TargetDeg::Deg_Back < ICurTargetAngle)
	{
		ICurTargetAngle = Enum_TargetDeg::Deg_Back;
	}

	mTargetState.mTargetDeg = ICurTargetAngle;
	mTargetState.mTargetDis = ICurTargetDis;
}

bool Boss_Vordt::ChangeAI_State(Enum_BossState _State)
{
	if (false == AI_States.contains(_State))
	{
		MsgBoxAssert("존재하지 않는 Boss_State 입니다.");
		return false;
	}

	AI_State& CurAIState = AI_States[_State];

	if (0 < CurAIState.CurCoolDown)
	{
		return false;
	}

	CurAIState.CurCoolDown = CurAIState.StateCoolDown;
	MainState.ChangeState(_State);
	return true;
}

void Boss_Vordt::PhaseChangeCheck()
{
	if (Enum_Boss_Phase::Phase_2 == mBoss_Phase)
	{
		return;
	}

	int CurHp = GetHp();
	if (BOSS_HP * 0.6f > CurHp)
	{
		mBoss_Phase = Enum_Boss_Phase::Phase_2;
		MainState.ChangeState(Enum_BossState::Howling);
		mJumpTableManager.ClearJumpTable();
	}
}

void Boss_Vordt::HitUpdate(float _Delta)
{
	if (false == Hit.IsHit())
	{
		return;
	}

	Hit_CoolDown -= _Delta;

	if (0.f >= Hit_CoolDown)
	{
		SetHit(false);
		Hit_CoolDown = HIT_COOLDOWN;
	}
}

void Boss_Vordt::MostPrioritzedUpdate()
{
	GroggyCheck();
	DeathCheck();
}

void Boss_Vordt::DeathCheck()
{
	if (Enum_BossState::Death == MainState.GetCurState() || Enum_BossState::Death_Groggy == MainState.GetCurState())
	{
		return;
	}

	if (0 < GetHp())
	{
		return;
	}

	mJumpTableManager.ClearJumpTable();
	MainState.ChangeState(Enum_BossState::Death);
	Capsule->Off();
	mHitCollision.Off();
}

void Boss_Vordt::GroggyCheck()
{
	if (0 < GetPoise())
	{
		return;
	}

	Enum_DirectionXZ_Quat Dir = Hit.GetHitDir();

	if (Enum_DirectionXZ_Quat::Center == Dir)
	{
		Stat.SetPoise(100);
		return;
	}

	if (2 <= Groggy_Count)
	{
		SetFlag(Enum_ActorFlag::Groggy, true);
		MainState.ChangeState(Enum_BossState::Hitten_Groggy);
		Groggy_Count = 0;
	}
	else
	{
		switch (Dir)
		{
		case Enum_DirectionXZ_Quat::F:
			MainState.ChangeState(Enum_BossState::Hitten_Front);
			break;
		case Enum_DirectionXZ_Quat::R:
			MainState.ChangeState(Enum_BossState::Hitten_Right);
			break;
		case Enum_DirectionXZ_Quat::B:
			MainState.ChangeState(Enum_BossState::Hitten_Back);
			break;
		case Enum_DirectionXZ_Quat::L:
			MainState.ChangeState(Enum_BossState::Hitten_Left);
			break;
		case Enum_DirectionXZ_Quat::Center:
		default:
			break;
		}
		++Groggy_Count;
	}

	mJumpTableManager.ClearJumpTable();
	Stat.SetPoise(100);
}

void Boss_Vordt::AIUpdate(float _Delta)
{
	for (std::pair<const Enum_BossState, AI_State>& _Pair : AI_States)
	{
		_Pair.second.Update(_Delta);
	}

	if (100 <= Stat.GetPoise())
	{
		return;
	}

	if (0.f < Stat_CoolDown)
	{
		Stat_CoolDown -= _Delta;
		return;
	}

	Stat_CoolDown = STAT_COOLDOWN;

	Stat.AddPoise(1);
}

void Boss_Vordt::CollisionUpdate()
{
	if (true == mAttackCollision.WeaponCollision->IsUpdate())
	{
		mAttackCollision.mWeaponHitInteraction.CollisionToShield(Enum_CollisionOrder::Player_Shield);
		mAttackCollision.mWeaponHitInteraction.CollisionToBody(Enum_CollisionOrder::Player_Body);
	}

	if (true == mAttackCollision.BodyCollision->IsUpdate())
	{
		mAttackCollision.mBodyHitInteraction.CollisionToShield(Enum_CollisionOrder::Player_Shield);
		mAttackCollision.mBodyHitInteraction.CollisionToBody(Enum_CollisionOrder::Player_Body);
	}

	if (true == mAttackCollision.HeadCollision->IsUpdate())
	{
		mAttackCollision.mHeadHitInteraction.CollisionToShield(Enum_CollisionOrder::Player_Shield);
		mAttackCollision.mHeadHitInteraction.CollisionToBody(Enum_CollisionOrder::Player_Body);
	}

	if (true == mAttackCollision.R_HandCollision->IsUpdate())
	{
		mAttackCollision.mHandHitInteraction.CollisionToShield(Enum_CollisionOrder::Player_Shield);
		mAttackCollision.mHandHitInteraction.CollisionToBody(Enum_CollisionOrder::Player_Body);
	}
}


std::shared_ptr< ContentsLight> Boss_Vordt::FaceLightInit()
{
	++mFaceLight.FaceLightNumber;

	std::shared_ptr< ContentsLight> Light = GetLevel()->CreateActor< ContentsLight>(Enum_UpdateOrder::Light, "BossFaceLight" + std::to_string(mFaceLight.FaceLightNumber));
	Light->SetLightType(Enum_LightType::Point);

	LightData Data = Light->GetLightData();

	Data.quadraticAttenuation = 0.00006f;
	Data.LightPower = 10.f;
	Data.LightColor = { 0.6f,0.8f,1.0f };

	Light->SetLightData(Data);

	return Light;

}

void Boss_Vordt::FaceLightUpdate()
{
	float RotationY = Transform.GetWorldRotationEuler().Y;
	float4 WorldPostion = Transform.GetWorldPosition();

	float4 revolution = float4::VectorRotationToDegY(float4{ 0.0f, 250.0f, 350.0f }, RotationY);
	mFaceLight.FaceLightForward->Transform.SetLocalPosition(WorldPostion + revolution);

	revolution = float4::VectorRotationToDegY(float4{ 0.0f, 250.0f, -350.0f }, RotationY);
	mFaceLight.FaceLightBack->Transform.SetLocalPosition(WorldPostion + revolution);

	revolution = float4::VectorRotationToDegY(float4{ 350.0f, 250.0f, 0.0f }, RotationY);
	mFaceLight.FaceLightRight->Transform.SetLocalPosition(WorldPostion + revolution);

	revolution = float4::VectorRotationToDegY(float4{ -350.0f, 250.0f, 0.0f }, RotationY);
	mFaceLight.FaceLightLeft->Transform.SetLocalPosition(WorldPostion + revolution);
}

