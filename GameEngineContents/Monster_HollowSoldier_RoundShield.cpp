#include "PreCompile.h"
#include "Monster_HollowSoldier_RoundShield.h"
#include "BoneSocketCollision.h"
#include "Monster_HitInteraction.h"

Monster_HollowSoldier_RoundShield::Monster_HollowSoldier_RoundShield()
{
}

Monster_HollowSoldier_RoundShield::~Monster_HollowSoldier_RoundShield()
{
}

void Monster_HollowSoldier_RoundShield::Start()
{
	Monster_Hollow::Start();

	MeshOnOffSwitch(Enum_Hollow_MeshIndex::Sword);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::RoundShield);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::ChainMail);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::ShoulderRobe);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::Helmat);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::RightKneePads);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::LeftKneePads);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::Shoes);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::SmallLeatherVest);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::Pants);
	MeshOnOffSwitch(Enum_Hollow_MeshIndex::TopRobe);

	// Status
	Stat.SetHp(167);
	Stat.SetAtt(15);
	Stat.SetPoise(100);
	Stat.SetSouls(ContentsRandom::RandomInt(30, 50));

	ShieldCollision = CreateSocketCollision(Enum_CollisionOrder::Monster_Shield, Enum_Hollow_BoneType::RoundShield, { float4(35.0f, 80.0f, 80.0f), float4::ZERONULL, float4(0.0f, 0.0f, -0.12f)}, "RoundShield");
	ShieldCollision->SetCollisionType(ColType::OBBBOX3D);
	ShieldCollision->On();

	// 105 4 7 / 0.33 0.0 0.02
	SwordCollision = CreateSocketCollision(Enum_CollisionOrder::MonsterAttack, Enum_Hollow_BoneType::Sword, { float4(106.0f, 4.0f, 7.0f), float4::ZERONULL, float4(0.33f, 0.0f, 0.015f) });
	SwordCollision->SetCollisionType(ColType::OBBBOX3D);

	Sword.Init(this, SwordCollision.get());
	Sword.On();

	ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1);

	CreateMonsterUI(73);
}
void Monster_HollowSoldier_RoundShield::Update(float _Delta)
{
	Monster_Hollow::Update(_Delta);
	StateUpdate(_Delta);
}
void Monster_HollowSoldier_RoundShield::Release()
{
	Monster_Hollow::Release();
}
void Monster_HollowSoldier_RoundShield::LevelStart(class GameEngineLevel* _NextLevel)
{
	Monster_Hollow::LevelStart(_NextLevel);
}

void Monster_HollowSoldier_RoundShield::WakeUp()
{
	ChangeState(Enum_HollowSoldier_RoundShield_State::Scout);
}

void Monster_HollowSoldier_RoundShield::Reset()
{
	DeathValue = false;

	Sword.ResetRecord();

	Stat.SetHp(167);
	Stat.SetPoise(100);
	Stat.SetAtt(15);

	Hit.SetHit(false);

	MeshOn(Enum_Hollow_MeshIndex::Sword);
	MeshOn(Enum_Hollow_MeshIndex::RoundShield);
	SetFlagNull();
	SetTargeting(nullptr);

	RecognizeCollision->On();
	BodyCollision->On();
	MonsterCollision->On();
	ShieldCollision->On();

	ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1);
}

void Monster_HollowSoldier_RoundShield::ChangeState(Enum_HollowSoldier_RoundShield_State _State)
{
	if (_State != ShieldState)
	{
		switch (_State)
		{
		case Enum_HollowSoldier_RoundShield_State::Idle1:
			State_Idle1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Idle2:
			State_Idle2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Idle3:
			State_Idle3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Idle2ToIdle1:
			State_Idle2ToIdle1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Idle1ToIdle2:
			State_Idle1ToIdle2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Idle2ToIdle3:
			State_Idle2ToIdle3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Idle3ToIdle2:
			State_Idle3ToIdle2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Scout:
			State_Scout_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Front:
			State_Walk_Front_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Back:
			State_Walk_Back_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Left:
			State_Walk_Left_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Right:
			State_Walk_Right_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Front3:
			State_Walk_Front3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Back3:
			State_Walk_Back3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Left3:
			State_Walk_Left3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Walk_Right3:
			State_Walk_Right3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Run:
			State_Run_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Run3:
			State_Run3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::RH_VerticalSlash:
			State_RH_VerticalSlash_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::RH_HorizontalSlash:
			State_RH_HorizontalSlash_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::RH_ComboAttack:
			State_RH_ComboAttack_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::RH_TwinSlash:
			State_RH_TwinSlash_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Attack1:
			State_Attack1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Attack2:
			State_Attack2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Attack3:
			State_Attack3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Attack4:
			State_Attack4_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Attack5:
			State_Attack5_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Attack6:
			State_Attack6_Start();
			break;
		/*case Enum_HollowSoldier_RoundShield_State::Attack7:
			State_Attack7_Start();
			break;*/
		case Enum_HollowSoldier_RoundShield_State::Turn_Left2:
			State_Turn_Left2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Right2:
			State_Turn_Right2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice2:
			State_Turn_Left_Twice2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice2:
			State_Turn_Right_Twice2_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Left1:
			State_Turn_Left1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Right1:
			State_Turn_Right1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice1:
			State_Turn_Left_Twice1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice1:
			State_Turn_Right_Twice1_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Left3:
			State_Turn_Left3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Right3:
			State_Turn_Right3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice3:
			State_Turn_Left_Twice3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice3:
			State_Turn_Right_Twice3_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Guard:
			State_Guard_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::GuardBreak:
			State_GuardBreak_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::AttackFail:
			State_AttackFail_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Parrying:
			State_Parrying_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Hit_Front:
			State_Hit_Front_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Hit_Back:
			State_Hit_Back_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Hit_Left:
			State_Hit_Left_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Hit_Right:
			State_Hit_Right_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::HitToDeath:
			State_HitToDeath_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::BackAttackHit:
			State_BackAttackHit_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::BackAttackDeath:
			State_BackAttackDeath_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::AfterGuardBreakHit:
			State_AfterGuardBreakHit_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::AfterGuardBreakDeath:
			State_AfterGuardBreakDeath_Start();
			break;
		case Enum_HollowSoldier_RoundShield_State::Death:
			State_Death_Start();
			break;
		default:
			break;
		}
	}
	ShieldState = _State;
}
void Monster_HollowSoldier_RoundShield::StateUpdate(float _Delta)
{
	switch (ShieldState)
	{
	case Enum_HollowSoldier_RoundShield_State::Idle1:
		return State_Idle1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Idle2:
		return State_Idle2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Idle3:
		return State_Idle3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Idle2ToIdle1:
		return State_Idle2ToIdle1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Idle1ToIdle2:
		return State_Idle1ToIdle2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Idle2ToIdle3:
		return State_Idle2ToIdle3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Idle3ToIdle2:
		return State_Idle3ToIdle2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Scout:
		return State_Scout_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Front:
		return State_Walk_Front_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Back:
		return State_Walk_Back_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Left:
		return State_Walk_Left_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Right:
		return State_Walk_Right_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Front3:
		return State_Walk_Front3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Back3:
		return State_Walk_Back3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Left3:
		return State_Walk_Left3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Walk_Right3:
		return State_Walk_Right3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Run:
		return State_Run_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Run3:
		return State_Run3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::RH_VerticalSlash:
		return State_RH_VerticalSlash_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::RH_HorizontalSlash:
		return State_RH_HorizontalSlash_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::RH_ComboAttack:
		return State_RH_ComboAttack_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::RH_TwinSlash:
		return State_RH_TwinSlash_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Attack1:
		return State_Attack1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Attack2:
		return State_Attack2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Attack3:
		return State_Attack3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Attack4:
		return State_Attack4_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Attack5:
		return State_Attack5_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Attack6:
		return State_Attack6_Update(_Delta);
	/*case Enum_HollowSoldier_RoundShield_State::Attack7:
		return State_Attack7_Update(_Delta);*/
	case Enum_HollowSoldier_RoundShield_State::Turn_Left2:
		return State_Turn_Left2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Right2:
		return State_Turn_Right2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice2:
		return State_Turn_Left_Twice2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice2:
		return State_Turn_Right_Twice2_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Left1:
		return State_Turn_Left1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Right1:
		return State_Turn_Right1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice1:
		return State_Turn_Left_Twice1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice1:
		return State_Turn_Right_Twice1_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Left3:
		return State_Turn_Left3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Right3:
		return State_Turn_Right3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice3:
		return State_Turn_Left_Twice3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice3:
		return State_Turn_Right_Twice3_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Guard:
		return State_Guard_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::GuardBreak:
		return State_GuardBreak_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::AttackFail:
		return State_AttackFail_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Parrying:
		return State_Parrying_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Hit_Front:
		return State_Hit_Front_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Hit_Back:
		return State_Hit_Back_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Hit_Left:
		return State_Hit_Left_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Hit_Right:
		return State_Hit_Right_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::HitToDeath:
		return State_HitToDeath_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::BackAttackHit:
		return State_BackAttackHit_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::BackAttackDeath:
		return State_BackAttackDeath_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::AfterGuardBreakHit:
		return State_AfterGuardBreakHit_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::AfterGuardBreakDeath:
		return State_AfterGuardBreakDeath_Update(_Delta);
	case Enum_HollowSoldier_RoundShield_State::Death:
		return State_Death_Update(_Delta);
	default:
		break;
	}
}

void Monster_HollowSoldier_RoundShield::ChangeAttackState()
{
	if (GetTargetDistance_e() == Enum_TargetDist::Melee)
	{
		AttackPattern = ContentsRandom::RandomInt(8, 13);
	}
	else
	{
		AttackPattern = ContentsRandom::RandomInt(1, 10);
	}
	
	switch (AttackPattern)
	{
	case 1:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack1);
		break;
	case 2:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack2);
		break;
	case 3:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack3);
		break;
	case 4:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack4);
		break;
	case 5:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack5);
		break;
	case 6:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack6);
		break;
	/*case 7:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Attack7);
		break;*/
	case 8:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Walk_Back3);
		break;
	case 9:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Walk_Left3);
		break;
	case 10:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Walk_Right3);
		break;
	case 11:
	case 12:
	case 13:
		ChangeState(Enum_HollowSoldier_RoundShield_State::Walk_Front3);
		break;
	default:
		break;
	}
}

void Monster_HollowSoldier_RoundShield::ChangeHitState()
{
	if (true == Hit.IsHit())
	{
		if (true == IsFlag(Enum_ActorFlag::FrontStab))
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::AfterGuardBreakHit);
			return;
		}

		if (true == IsFlag(Enum_ActorFlag::BackStab))
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::BackAttackHit);
			return;
		}

		if (Stat.GetHp() <= 0)
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::HitToDeath);
			return;
		}

		if (true == Hit.IsGuardSuccesss())
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Guard);
			return;
		}

		if (true == IsFlag(Enum_ActorFlag::Guard_Break))
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::GuardBreak);
			return;
		}

		Enum_DirectionXZ_Quat HitDir = Hit.GetHitDir();
		//BodyCollision->Off();

		switch (HitDir)
		{
		case Enum_DirectionXZ_Quat::F:
			ShieldState = Enum_HollowSoldier_RoundShield_State::Max;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Hit_Front);
			break;
		case Enum_DirectionXZ_Quat::R:
			ShieldState = Enum_HollowSoldier_RoundShield_State::Max;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Hit_Right);
			break;
		case Enum_DirectionXZ_Quat::B:
			ShieldState = Enum_HollowSoldier_RoundShield_State::Max;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Hit_Back);
			break;
		case Enum_DirectionXZ_Quat::L:
			ShieldState = Enum_HollowSoldier_RoundShield_State::Max;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Hit_Left);
			break;
		default:
			ShieldState = Enum_HollowSoldier_RoundShield_State::Max;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Hit_Front);
			break;
		}
	}
	else
	{
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Idle1_Start()
{
	MainRenderer->ChangeAnimation("c1100_Idle1");
}
void Monster_HollowSoldier_RoundShield::State_Idle1_Update(float _Delta)
{
	ChangeHitState();

	//StateTime += _Delta;

	// 트리거 발동시
	/*if (StateTime >= 5.0f)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Scout);
	}*/
}

void Monster_HollowSoldier_RoundShield::State_Idle2_Start()
{
	MainRenderer->ChangeAnimation("c1100_Idle2");
}
void Monster_HollowSoldier_RoundShield::State_Idle2_Update(float _Delat)
{
	ChangeHitState();
	// 

	// 거의 상시로? 아니면 일정 거리 안에 있으면?
	if (true)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2ToIdle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Idle3_Start()
{
	ShieldCollision->On();
	// GuardOn
	MainRenderer->ChangeAnimation("c1100_Idle3");
}
void Monster_HollowSoldier_RoundShield::State_Idle3_Update(float _Delta)
{
	StateTime += _Delta;
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (BodyCollision->IsUpdate() == false)
	{
		BodyCollision->On();
	}

	const float AbsTargetAngle = std::fabs(BaseActor::GetTargetAngle());
	if (AbsTargetAngle >= 90.0f && AbsTargetAngle <= 150.0f)
	{
		//Turn
		if (Enum_RotDir::Left == GetRotDir_e())
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Turn_Left3);
			return;
		}
		else
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Turn_Right3);
			return;
		}
	}
	else if (AbsTargetAngle > 150.0f)
	{
		// TurnTwice
		if (Enum_RotDir::Left == GetRotDir_e())
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice3);
			return;
		}
		else
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Turn_Right_Twice3);
			return;
		}
	}
	else if (AbsTargetAngle < 80.0f)
	{
		if (false == IsTargetInAngle(3.0f))
		{
			RotToTarget(_Delta);
		}
	}

	/*EventParameter AttackParameter;
	AttackParameter.Stay = [&](class GameEngineCollision* _This, class GameEngineCollision* _Other)
		{
			IsAttack = true;
		};
	AttackParameter.Exit = [&](class GameEngineCollision* _This, class GameEngineCollision* _Other)
		{
			IsAttack = false;
		};
	AttackRangeCollision->CollisionEvent(Enum_CollisionOrder::Dummy, AttackParameter);*/

	if (StateTime >= 0.1f)
	{
		// 거리 구하기
		if (GetTargetDistance_e() == Enum_TargetDist::Long)
		{
			StateTime = 0.0;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Run3);
			return;
		}
		else if (GetTargetDistance_e() == Enum_TargetDist::Medium)
		{
			StateTime = 0.0f;
			ChangeState(Enum_HollowSoldier_RoundShield_State::Walk_Front3);
			return;
		}
		else
		{
			StateTime = 0.0f;
			ChangeAttackState();
			/*if (IsAttack == false)
			{
				StateTime = 0.0f;
				ChangeState(Enum_HollowSoldier_RoundShield_State::Walk_Front3);
			}
			else
			{
				StateTime = 0.0f;
				ChangeAttackState();
			}*/
		}

	}
}

void Monster_HollowSoldier_RoundShield::State_Idle2ToIdle1_Start()
{
	MainRenderer->ChangeAnimation("c1100_Idle2ToIdle1");
}
void Monster_HollowSoldier_RoundShield::State_Idle2ToIdle1_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 39)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Idle1ToIdle2_Start()
{
	MainRenderer->ChangeAnimation("c1100_Idle1ToIdle2");
}
void Monster_HollowSoldier_RoundShield::State_Idle1ToIdle2_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 29)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Idle2ToIdle3_Start()
{
	ShieldCollision->On();
	MainRenderer->ChangeAnimation("c1100_Idle2ToIdle3");
}
void Monster_HollowSoldier_RoundShield::State_Idle2ToIdle3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 19)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Idle3ToIdle2_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_Idle3ToIdle2");
}
void Monster_HollowSoldier_RoundShield::State_Idle3ToIdle2_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 31)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Scout_Start()
{
	ScoutTime = 0.0f;
	MainRenderer->ChangeAnimation("c1100_Scout");
}
void Monster_HollowSoldier_RoundShield::State_Scout_Update(float _Delta)
{
	ChangeHitState();

	ScoutTime += _Delta;

	if (ScoutTime >= 8.0f)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Turn_Left_Twice1);
		return;
	}

	EventParameter RecognizeParameter;
	RecognizeParameter.Enter = [&](class GameEngineCollision* _This, class GameEngineCollision* _Other)
		{

		};
	if (true == RecognizeCollision->CollisionEvent(Enum_CollisionOrder::Player, RecognizeParameter))
	{
		FindTarget();
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1ToIdle2);
		return;

	}
}

void Monster_HollowSoldier_RoundShield::State_Walk_Front_Start()
{
	MainRenderer->ChangeAnimation("c1100_Walk_Front");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Front_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_Walk_Back_Start()
{
	MainRenderer->ChangeAnimation("c1100_Walk_Back");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Back_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_Walk_Left_Start()
{
	MainRenderer->ChangeAnimation("c1100_Walk_Left");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Left_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_Walk_Right_Start()
{
	MainRenderer->ChangeAnimation("c1100_Walk_Right");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Right_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_Walk_Front3_Start()
{
	ShieldCollision->On();
	MainRenderer->ChangeAnimation("c1100_Walk_Front3");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Front3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (false == IsTargetInAngle(3.0f))
	{
		RotToTarget(_Delta);
	}

	if (MainRenderer->GetCurAnimationFrame() >= 41)
	{
		if (GetTargetDistance_e() == Enum_TargetDist::Long)
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Run3);
			return;
		}
		else
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
			return;
		}
	}
	

	/*EventParameter AttackParameter;
	AttackParameter.Enter = [&](class GameEngineCollision* _This, class GameEngineCollision* _Other)
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		};
	AttackRangeCollision->CollisionEvent(Enum_CollisionOrder::Dummy, AttackParameter);*/
}

void Monster_HollowSoldier_RoundShield::State_Walk_Back3_Start()
{
	ShieldCollision->On();
	WalkToChangeTime = ContentsRandom::Randomfloat(0.7f, 2.8f);
	MainRenderer->ChangeAnimation("c1100_Walk_Back3");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Back3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	WalkTime += _Delta;

	ChangeHitState();

	if (false == IsTargetInAngle(3.0f))
	{
		RotToTarget(_Delta);
	}

	if (WalkTime >= WalkToChangeTime)
	{
		WalkTime = 0.0f;
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Walk_Left3_Start()
{
	ShieldCollision->On();
	WalkToChangeTime = ContentsRandom::Randomfloat(0.65f, 2.6f);
	MainRenderer->ChangeAnimation("c1100_Walk_Left3");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Left3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	WalkTime += _Delta;

	ChangeHitState();

	if (false == IsTargetInAngle(3.0f))
	{
		RotToTarget(_Delta);
	}

	if (WalkTime >= WalkToChangeTime)
	{
		WalkTime = 0.0f;
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Walk_Right3_Start()
{
	ShieldCollision->On();
	WalkToChangeTime = ContentsRandom::Randomfloat(0.65f, 2.6f);
	MainRenderer->ChangeAnimation("c1100_Walk_Right3");
}
void Monster_HollowSoldier_RoundShield::State_Walk_Right3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	WalkTime += _Delta;

	ChangeHitState();

	if (false == IsTargetInAngle(3.0f))
	{
		RotToTarget(_Delta);
	}

	if (WalkTime >= WalkToChangeTime)
	{
		WalkTime = 0.0f;
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Run_Start()
{
	MainRenderer->ChangeAnimation("c1100_Run");
}
void Monster_HollowSoldier_RoundShield::State_Run_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_Run3_Start()
{
	ShieldCollision->On();
	//WalkToChangeTime = ContentsRandom::Randomfloat(0.8f, 1.6f);
	MainRenderer->ChangeAnimation("c1100_Run3");
}
void Monster_HollowSoldier_RoundShield::State_Run3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	//WalkTime += _Delta;

	ChangeHitState();

	if (false == IsTargetInAngle(3.0f))
	{
		RotToTarget(_Delta);
	}

	if (GetTargetDistance_e() == Enum_TargetDist::Close)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}

	//if(WalkTime >= WalkToChangeTime)
	//{
	//	WalkTime = 0.0f;
	//	if (GetTargetDistance_e() == Enum_TargetDist::Close)
	//	{
	//		// RunToSting 없나? 조사해야함.
	//		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
	//		return;
	//	}
	//}
}

void Monster_HollowSoldier_RoundShield::State_RH_VerticalSlash_Start()
{
	MainRenderer->ChangeAnimation("c1100_RH_VerticalSlash");
}
void Monster_HollowSoldier_RoundShield::State_RH_VerticalSlash_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_RH_HorizontalSlash_Start()
{
	MainRenderer->ChangeAnimation("c1100_RH_HorizontalSlash");
}
void Monster_HollowSoldier_RoundShield::State_RH_HorizontalSlash_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_RH_ComboAttack_Start()
{
	MainRenderer->ChangeAnimation("c1100_RH_ComboAttack");
}
void Monster_HollowSoldier_RoundShield::State_RH_ComboAttack_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_RH_TwinSlash_Start()
{
	MainRenderer->ChangeAnimation("c1100_RH_TwinSlash");
}
void Monster_HollowSoldier_RoundShield::State_RH_TwinSlash_Update(float _Delta)
{

}

void Monster_HollowSoldier_RoundShield::State_Attack1_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_RH_VerticalSlash");
}
void Monster_HollowSoldier_RoundShield::State_Attack1_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (CheckAnimationName("c1100_RH_VerticalSlash"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 23 && MainRenderer->GetCurAnimationFrame() <= 27)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 28)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 57)
		{
			Sword.ResetRecord();
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
			return;
		}
	}
}

void Monster_HollowSoldier_RoundShield::State_Attack2_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_RH_VerticalSlash");
}
void Monster_HollowSoldier_RoundShield::State_Attack2_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (CheckAnimationName("c1100_RH_VerticalSlash"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 23 && MainRenderer->GetCurAnimationFrame() <= 27)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 28)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 35)
		{
			Sword.ResetRecord();
			MainRenderer->ChangeAnimation("c1100_RH_HorizontalSlash");
		}
	}

	if (CheckAnimationName("c1100_RH_HorizontalSlash"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 0 && MainRenderer->GetCurAnimationFrame() <= 4)
		{
			if (false == IsTargetInAngle(3.0f))
			{
				RotToTarget(_Delta);
			}
			MainRenderer->GetCurAnimation()->SetBlendTime(0.4f);
		}

		if (MainRenderer->GetCurAnimationFrame() >= 11 && MainRenderer->GetCurAnimationFrame() <= 15)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 16)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 45)
		{
			Sword.ResetRecord();
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
			return;
		}
	}
}

void Monster_HollowSoldier_RoundShield::State_Attack3_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_RH_VerticalSlash");
}
void Monster_HollowSoldier_RoundShield::State_Attack3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (CheckAnimationName("c1100_RH_VerticalSlash"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 23 && MainRenderer->GetCurAnimationFrame() <= 27)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 28)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 35)
		{
			Sword.ResetRecord();
			MainRenderer->ChangeAnimation("c1100_RH_HorizontalSlash");
		}
	}

	if (CheckAnimationName("c1100_RH_HorizontalSlash"))
	{

		if (MainRenderer->GetCurAnimationFrame() >= 0 && MainRenderer->GetCurAnimationFrame() <= 2)
		{
			if (false == IsTargetInAngle(3.0f))
			{
				RotToTarget(_Delta);
			}
			MainRenderer->GetCurAnimation()->SetBlendTime(0.4f);
		}

		if (MainRenderer->GetCurAnimationFrame() >= 11 && MainRenderer->GetCurAnimationFrame() <= 15)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 16)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 22)
		{
			Sword.ResetRecord();
			MainRenderer->ChangeAnimation("c1100_TH_VerticalSlash");
		}
	}

	if (CheckAnimationName("c1100_TH_VerticalSlash"))
	{

		if (MainRenderer->GetCurAnimationFrame() >= 0 && MainRenderer->GetCurAnimationFrame() <= 4)
		{
			if (false == IsTargetInAngle(3.0f))
			{
				RotToTarget(_Delta);
			}
			MainRenderer->GetCurAnimation()->SetBlendTime(0.6f);
			MainRenderer->ChangeCurFrame(5);
		}

		if (MainRenderer->GetCurAnimationFrame() >= 25 && MainRenderer->GetCurAnimationFrame() <= 28)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 29)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 85)
		{
			Sword.ResetRecord();
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
			return;
		}
	}
}

void Monster_HollowSoldier_RoundShield::State_Attack4_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_TH_VerticalSlash");
}
void Monster_HollowSoldier_RoundShield::State_Attack4_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (CheckAnimationName("c1100_TH_VerticalSlash"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 25 && MainRenderer->GetCurAnimationFrame() <= 28)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 29)
		{
			Sword.Off();
		}
		
		if (MainRenderer->GetCurAnimationFrame() >= 85)
		{
			Sword.ResetRecord();
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
			return;
		}
	}
}

void Monster_HollowSoldier_RoundShield::State_Attack5_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_RH_TwinSlash");
}
void Monster_HollowSoldier_RoundShield::State_Attack5_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (CheckAnimationName("c1100_RH_TwinSlash"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 13 && MainRenderer->GetCurAnimationFrame() <= 16)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 17)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 18 && MainRenderer->GetCurAnimationFrame() <= 19)
		{
			Sword.ResetRecord();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 31 && MainRenderer->GetCurAnimationFrame() <= 34)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}
		}

		if (MainRenderer->GetCurAnimationFrame() >= 35)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 85)
		{
			Sword.ResetRecord();
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
			return;
		}
	}
}

void Monster_HollowSoldier_RoundShield::State_Attack6_Start()
{
	ShieldCollision->Off();
	MainRenderer->ChangeAnimation("c1100_RH_Sting");
}
void Monster_HollowSoldier_RoundShield::State_Attack6_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);

	ChangeHitState();

	if (CheckAnimationName("c1100_RH_Sting"))
	{
		if (MainRenderer->GetCurAnimationFrame() >= 21 && MainRenderer->GetCurAnimationFrame() <= 26)
		{
			Sword.On();

			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
			if (true == Sword.GetBlock())
			{
				if (true == IsFlag(Enum_ActorFlag::Break_Posture))
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::Parrying);
					return;
				}
				else
				{
					ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
					return;
				}
			}
			else
			{
				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
			}

		}

		if (MainRenderer->GetCurAnimationFrame() >= 27)
		{
			Sword.Off();
		}

		if (MainRenderer->GetCurAnimationFrame() >= 66)
		{
			Sword.ResetRecord();
			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
			return;
		}
	}
}

//void Monster_HollowSoldier_RoundShield::State_Attack7_Start()
//{
//	MainRenderer->ChangeAnimation("c1100_LH_Fist");
//}
//void Monster_HollowSoldier_RoundShield::State_Attack7_Update(float _Delta)
//{
//	SetFlag(Enum_ActorFlag::Guarding, false);
//
//	ChangeHitState();
//
//	if (CheckAnimationName("c1100_LH_Fist"))
//	{
//		// 방패로 때리네...?
//		// 24 ~ 28 Frame
//		// 빼버릴까...
//
//		if (MainRenderer->GetCurAnimationFrame() >= 43)
//		{
//			MainRenderer->ChangeAnimation("c1100_RH_ChargingSting");
//		}
//	}
//
//	if (CheckAnimationName("c1100_RH_ChargingSting"))
//	{
//		if (MainRenderer->GetCurAnimationFrame() >= 0 && MainRenderer->GetCurAnimationFrame() <= 4)
//		{
//			if (false == IsTargetInAngle(3.0f))
//			{
//				RotToTarget(_Delta);
//			}
//			MainRenderer->GetCurAnimation()->SetBlendTime(0.4f);
//			//MainRenderer->ChangeCurFrame(5);
//		}
//
//		if (MainRenderer->GetCurAnimationFrame() >= 44 && MainRenderer->GetCurAnimationFrame() <= 48)
//		{
//			Sword.On();
//
//			Sword.CollisionToShield(Enum_CollisionOrder::Player_Shield);
//			if (true == Sword.GetBlock())
//			{
//				ChangeState(Enum_HollowSoldier_RoundShield_State::AttackFail);
//			}
//			else
//			{
//				Sword.CollisionToBody(Enum_CollisionOrder::Player_Body);
//			}
//
//		}
//
//		if (MainRenderer->GetCurAnimationFrame() >= 49)
//		{
//			Sword.Off();
//		}
//
//		if (MainRenderer->GetCurAnimationFrame() >= 98)
//		{
//			ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
//		}
//	}
//}

void Monster_HollowSoldier_RoundShield::State_Turn_Left2_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Left2");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Left2_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 35)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Right2_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Right2");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Right2_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 35)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Left_Twice2_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Left_Twice2");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Left_Twice2_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 38)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Right_Twice2_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Right_Twice2");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Right_Twice2_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 38)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Left1_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Left");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Left1_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 35)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Scout);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Right1_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Right");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Right1_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 35)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Left_Twice1_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Left_Twice");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Left_Twice1_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 38)
	{
		//ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1);
		ChangeState(Enum_HollowSoldier_RoundShield_State::Scout);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Right_Twice1_Start()
{
	MainRenderer->ChangeAnimation("c1100_Turn_Right_Twice");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Right_Twice1_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 38)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle1);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Left3_Start()
{
	ShieldCollision->On();
	MainRenderer->ChangeAnimation("c1100_Turn_Left3");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Left3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 35)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Right3_Start()
{
	ShieldCollision->On();
	MainRenderer->ChangeAnimation("c1100_Turn_Right3");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Right3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 35)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Left_Twice3_Start()
{
	ShieldCollision->On();
	MainRenderer->ChangeAnimation("c1100_Turn_Left_Twice3");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Left_Twice3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 38)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Turn_Right_Twice3_Start()
{
	ShieldCollision->On();
	MainRenderer->ChangeAnimation("c1100_Turn_Right_Twice3");
}
void Monster_HollowSoldier_RoundShield::State_Turn_Right_Twice3_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 38)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Guard_Start()
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	MainRenderer->ChangeAnimation("c1100_Guard");
}
void Monster_HollowSoldier_RoundShield::State_Guard_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, true);

	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 25)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle3);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_GuardBreak_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	SetFlag(Enum_ActorFlag::Guarding, false);
	SetFlag(Enum_ActorFlag::Break_Posture, false);
	SetFlag(Enum_ActorFlag::Groggy, true);
	MainRenderer->ChangeAnimation("c1100_GuardBreak");
}
void Monster_HollowSoldier_RoundShield::State_GuardBreak_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 64)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_AttackFail_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Sword.Off();
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("c1100_AttackFail");
}
void Monster_HollowSoldier_RoundShield::State_AttackFail_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 40)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Parrying_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Sword.Off();
	Sword.ResetRecord();
	SetFlag(Enum_ActorFlag::Break_Posture, false);
	SetFlag(Enum_ActorFlag::Groggy, true);
	MainRenderer->ChangeAnimation("c1100_Parrying");
}
void Monster_HollowSoldier_RoundShield::State_Parrying_Update(float _Delta)
{
	SetFlag(Enum_ActorFlag::Guarding, false);
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 64)
	{
		SetFlag(Enum_ActorFlag::Groggy, false);
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Hit_Front_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Sword.Off();
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("c1100_Hit_Front");
	MainRenderer->ChangeCurFrame(0);
}
void Monster_HollowSoldier_RoundShield::State_Hit_Front_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 46)
	{
		//BodyCollision->On();
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Hit_Back_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Sword.Off();
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("c1100_Hit_Back");
	MainRenderer->ChangeCurFrame(0);
}
void Monster_HollowSoldier_RoundShield::State_Hit_Back_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 46)
	{
		//BodyCollision->On();
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Hit_Left_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Sword.Off();
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("c1100_Hit_Left");
	MainRenderer->ChangeCurFrame(0);
}
void Monster_HollowSoldier_RoundShield::State_Hit_Left_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 46)
	{
		//BodyCollision->On();
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_Hit_Right_Start()
{
	ShieldCollision->Off();
	Hit.SetHit(false);
	Sword.Off();
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("c1100_Hit_Right");
	MainRenderer->ChangeCurFrame(0);
}
void Monster_HollowSoldier_RoundShield::State_Hit_Right_Update(float _Delta)
{
	ChangeHitState();

	if (MainRenderer->GetCurAnimationFrame() >= 46)
	{
		//BodyCollision->On();
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_HitToDeath_Start()
{
	if (DeathValue == false)
	{
		DeathFunc();
	}

	Sword.Off();
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("c1100_HitToDeath");
}
void Monster_HollowSoldier_RoundShield::State_HitToDeath_Update(float _Delta)
{
	// 죽는 애니메이션 재생중 무기 Off
	if (MainRenderer->GetCurAnimationFrame() >= 58)
	{
		MeshOff(Enum_Hollow_MeshIndex::Sword);
		MeshOff(Enum_Hollow_MeshIndex::RoundShield);
	}

	if (MainRenderer->GetCurAnimationFrame() >= static_cast<int>(MainRenderer->GetCurAnimation()->End))
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Death);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_BackAttackHit_Start()
{
	Hit.SetHit(false);
	MainRenderer->ChangeAnimation("c1100_BackAttackHit");
}
void Monster_HollowSoldier_RoundShield::State_BackAttackHit_Update(float _Delta)
{
	if (Stat.GetHp() <= 0)
	{
		if (MainRenderer->GetCurAnimationFrame() >= 80)
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::BackAttackDeath);
			return;
		}
	}

	if (MainRenderer->GetCurAnimationFrame() >= 167)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_BackAttackDeath_Start()
{
	if (DeathValue == false)
	{
		DeathFunc();
	}

	MainRenderer->ChangeAnimation("c1100_BackAttackDeath");
}
void Monster_HollowSoldier_RoundShield::State_BackAttackDeath_Update(float _Delta)
{
	if (MainRenderer->GetCurAnimationFrame() >= 56)
	{
		MeshOff(Enum_Hollow_MeshIndex::Sword);
		MeshOff(Enum_Hollow_MeshIndex::RoundShield);
	}
}

void Monster_HollowSoldier_RoundShield::State_AfterGuardBreakHit_Start()
{
	Hit.SetHit(false);
	SetFlag(Enum_ActorFlag::Groggy, false);
	MainRenderer->ChangeAnimation("c1100_AfterGuardBreakHit");
}
void Monster_HollowSoldier_RoundShield::State_AfterGuardBreakHit_Update(float _Delta)
{
	if (Stat.GetHp() <= 0)
	{
		if (MainRenderer->GetCurAnimationFrame() >= 140)
		{
			ChangeState(Enum_HollowSoldier_RoundShield_State::AfterGuardBreakDeath);
			return;
		}
	}

	if (MainRenderer->GetCurAnimationFrame() >= 194)
	{
		ChangeState(Enum_HollowSoldier_RoundShield_State::Idle2);
		return;
	}
}

void Monster_HollowSoldier_RoundShield::State_AfterGuardBreakDeath_Start()
{
	if (DeathValue == false)
	{
		DeathFunc();
	}

	MainRenderer->ChangeAnimation("c1100_AfterGuardBreakDeath");
}
void Monster_HollowSoldier_RoundShield::State_AfterGuardBreakDeath_Update(float _Delta)
{
	if (MainRenderer->GetCurAnimationFrame() >= 63)
	{
		MeshOff(Enum_Hollow_MeshIndex::Sword);
		MeshOff(Enum_Hollow_MeshIndex::RoundShield);
	}
}

void Monster_HollowSoldier_RoundShield::State_Death_Start()
{
	MainRenderer->ChangeAnimation("c1100_Death");
}
void Monster_HollowSoldier_RoundShield::State_Death_Update(float _Delta)
{

}