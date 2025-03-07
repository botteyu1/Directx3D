#include "PreCompile.h"
#include "Monster_LothricKn.h"

#include "DummyActor.h"
#include "PatrolPath.h"

static constexpr float MIN_TIME_STEPSTATE = 0.5f;
static constexpr float MAX_TIME_STEPSTATE = 2.0f;
static constexpr float MAX_AGGRO_TIME = 8.0f;

static constexpr int SWING_ATTACK_STIFFNESS = 12;
static constexpr int SHOT_ATTACK_STIFFNESS = 20;
static constexpr int SHIELD_ATTACK_STIFFNESS = 18;
static constexpr int DH_ATTACK_STIFFNESS_RATIO = 2;

#define SELECT_ENEMY_TO_PLAYER
#ifdef SELECT_ENEMY_TO_PLAYER
#define TARGET_ORDER Enum_CollisionOrder::Player
#else
#define TARGET_ORDER Enum_CollisionOrder::Dummy

#endif 


bool Monster_LothricKn::IsFrame(int _StartFrame, int _EndFrame /*= -1*/) const
{
	int CurFrame = MainRenderer->GetCurAnimationFrame();

	if (_StartFrame <= CurFrame)
	{
		if (-1 == _EndFrame)
		{
			return true;
		}


		if (_EndFrame >= CurFrame)
		{
			return true;
		}
	}

	return false;
}

bool Monster_LothricKn::IsFrameOnce(int _StartFrame)
{
	if (true == MainRenderer->IsFrameChange())
	{
		int CurFrame = MainRenderer->GetCurAnimationFrame();
		return (_StartFrame == CurFrame);
	}

	return false;
}

bool Monster_LothricKn::IsFrameEnd()
{
	return MainRenderer->IsCurAnimationEnd();
}

void Monster_LothricKn::StateTimeSet(float _fMin, float _fMax)
{
	fMaxStateTime = ContentsRandom::Randomfloat(_fMin, _fMax);
}

void Monster_LothricKn::ResetStateTime()
{
	fMaxStateTime = 0.0f;
}

void Monster_LothricKn::SetCombatMode(eCombatState _Combat)
{
	if (CombatState == _Combat)
	{
		return;
	}

	CombatState = _Combat;

	switch (_Combat)
	{
	case Monster_LothricKn::eCombatState::Normal:
		SetFlag(Enum_ActorFlag::TwoHand, false);
		SetFlag(Enum_ActorFlag::Guarding, false);
		Shield.Off();
		break;
	case Monster_LothricKn::eCombatState::Two_Handed:
		SetFlag(Enum_ActorFlag::TwoHand, true);
		SetFlag(Enum_ActorFlag::Guarding, false);
		Shield.Off();
		break;
	case Monster_LothricKn::eCombatState::Guarding:
		SetFlag(Enum_ActorFlag::TwoHand, false);
		SetFlag(Enum_ActorFlag::Guarding, true);
		Shield.On();
		break;
	case Monster_LothricKn::eCombatState::None:
		break;
	default:
		break;
	}
}



////////////////////////////////////////////////////////////////////////////////////////
////////////////////// State
////////////////////////////////////////////////////////////////////////////////////////

void Monster_LothricKn::CreateFSM()
{
	MainState.CreateState(Enum_LothricKn_State::None ,{               .Start = std::bind(&Monster_LothricKn::Start_None,this, std::placeholders::_1),  });
	MainState.CreateState(Enum_LothricKn_State::Debug ,{              .Start = std::bind(&Monster_LothricKn::Start_Debug,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_Debug,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Idle_Standing ,{      .Start = std::bind(&Monster_LothricKn::Start_Idle_Standing,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_Idle_Standing,this, std::placeholders::_1,std::placeholders::_2),        .End = std::bind(&Monster_LothricKn::End_Idle_Standing, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Idle_Standing1 ,{     .Start = std::bind(&Monster_LothricKn::Start_Idle_Standing1,this, std::placeholders::_1),     .Stay = std::bind(&Monster_LothricKn::Update_Idle_Standing1,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Idle_Sit ,{	          .Start = std::bind(&Monster_LothricKn::Start_Idle_Sit,this, std::placeholders::_1),           .Stay = std::bind(&Monster_LothricKn::Update_Idle_Sit,this, std::placeholders::_1,std::placeholders::_2),             .End = std::bind(&Monster_LothricKn::End_Idle_Sit, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Sleep, {              .Start = std::bind(&Monster_LothricKn::StartSleep,this, std::placeholders::_1),                                                                                                                                     .End = std::bind(&Monster_LothricKn::EndSleep,this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Patrol, {             .Start = std::bind(&Monster_LothricKn::Start_Patrol,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_Patrol,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Patrol_L_Turn, {      .Start = std::bind(&Monster_LothricKn::Start_Patrol_L_Turn,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_Patrol_Turn,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Patrol_R_Turn, {      .Start = std::bind(&Monster_LothricKn::Start_Patrol_R_Turn,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_Patrol_Turn,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Patrol_L_TurnTwice, { .Start = std::bind(&Monster_LothricKn::Start_Patrol_L_TurnTwice,this, std::placeholders::_1), .Stay = std::bind(&Monster_LothricKn::Update_Patrol_Turn,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Patrol_R_TurnTwice, { .Start = std::bind(&Monster_LothricKn::Start_Patrol_R_TurnTwice,this, std::placeholders::_1), .Stay = std::bind(&Monster_LothricKn::Update_Patrol_Turn,this, std::placeholders::_1,std::placeholders::_2) });
	MainState.CreateState(Enum_LothricKn_State::Combo_Att_11, {       .Start = std::bind(&Monster_LothricKn::Start_Combo_Att_11,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Combo_Att_11,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Combo_Att_11, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Combo_Att_12, {       .Start = std::bind(&Monster_LothricKn::Start_Combo_Att_12,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Combo_Att_12,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Combo_Att_12, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Combo_Att_13, {       .Start = std::bind(&Monster_LothricKn::Start_Combo_Att_13,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Combo_Att_13,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Combo_Att_13, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Combo_Att_21, {       .Start = std::bind(&Monster_LothricKn::Start_Combo_Att_21,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Combo_Att_21,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Combo_Att_21, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Combo_Att_22, {       .Start = std::bind(&Monster_LothricKn::Start_Combo_Att_22,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Combo_Att_22,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Combo_Att_22, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Combo_Att_23, {       .Start = std::bind(&Monster_LothricKn::Start_Combo_Att_23,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Combo_Att_23,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Combo_Att_23, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::RH_Att_HitDown, {     .Start = std::bind(&Monster_LothricKn::Start_RH_Att_HitDown,this, std::placeholders::_1),     .Stay = std::bind(&Monster_LothricKn::Update_RH_Att_HitDown,this, std::placeholders::_1,std::placeholders::_2),       .End = std::bind(&Monster_LothricKn::End_RH_Att_HitDown, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::LH_ShieldAttack, {    .Start = std::bind(&Monster_LothricKn::Start_LH_ShieldAttack,this, std::placeholders::_1),    .Stay = std::bind(&Monster_LothricKn::Update_LH_ShieldAttack,this, std::placeholders::_1,std::placeholders::_2),      .End = std::bind(&Monster_LothricKn::End_LH_ShieldAttack, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::RH_Rear_Att, {        .Start = std::bind(&Monster_LothricKn::Start_RH_Rear_Att,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_RH_Rear_Att,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_RH_Rear_Att, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::L_Turn, {             .Start = std::bind(&Monster_LothricKn::Start_L_Turn,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_L_Turn,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_L_Turn, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::R_Turn, {             .Start = std::bind(&Monster_LothricKn::Start_R_Turn,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_R_Turn,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_R_Turn, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::L_TurnTwice, {        .Start = std::bind(&Monster_LothricKn::Start_L_TurnTwice,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_L_TurnTwice,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_L_TurnTwice, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::R_TurnTwice, {        .Start = std::bind(&Monster_LothricKn::Start_R_TurnTwice,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_R_TurnTwice,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_R_TurnTwice, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::L_Side_Step, {        .Start = std::bind(&Monster_LothricKn::Start_L_Side_Step,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_L_Side_Step,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_L_Side_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::R_Side_Step, {        .Start = std::bind(&Monster_LothricKn::Start_R_Side_Step,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_R_Side_Step,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_R_Side_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Step, {             .Start = std::bind(&Monster_LothricKn::Start_F_Step,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_F_Step,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_F_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::B_Step, {             .Start = std::bind(&Monster_LothricKn::Start_B_Step,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_B_Step,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_B_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Run, {                .Start = std::bind(&Monster_LothricKn::Start_Run,this, std::placeholders::_1),                .Stay = std::bind(&Monster_LothricKn::Update_Run,this, std::placeholders::_1,std::placeholders::_2),                  .End = std::bind(&Monster_LothricKn::End_Run, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::SitUp, {              .Start = std::bind(&Monster_LothricKn::Start_SitUp,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_SitUp,this, std::placeholders::_1,std::placeholders::_2),                .End = std::bind(&Monster_LothricKn::End_SitUp, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_Hold, {            .Start = std::bind(&Monster_LothricKn::Start_DH_Hold,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_DH_Hold,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_DH_Hold, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_UnHold, {          .Start = std::bind(&Monster_LothricKn::Start_DH_UnHold,this, std::placeholders::_1),          .Stay = std::bind(&Monster_LothricKn::Update_DH_UnHold,this, std::placeholders::_1,std::placeholders::_2),            .End = std::bind(&Monster_LothricKn::End_DH_UnHold, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_Stab_Att, {        .Start = std::bind(&Monster_LothricKn::Start_DH_Stab_Att,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_DH_Stab_Att,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_DH_Stab_Att, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_Swing_Att, {       .Start = std::bind(&Monster_LothricKn::Start_DH_Swing_Att,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_DH_Swing_Att,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_DH_Swing_Att, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_L_Side_Step, {     .Start = std::bind(&Monster_LothricKn::Start_DH_L_Side_Step,this, std::placeholders::_1),     .Stay = std::bind(&Monster_LothricKn::Update_DH_Walk,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_DH_L_Side_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_R_Side_Step, {     .Start = std::bind(&Monster_LothricKn::Start_DH_R_Side_Step,this, std::placeholders::_1),     .Stay = std::bind(&Monster_LothricKn::Update_DH_Walk,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_DH_R_Side_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_F_Step, {          .Start = std::bind(&Monster_LothricKn::Start_DH_F_Step,this, std::placeholders::_1),          .Stay = std::bind(&Monster_LothricKn::Update_DH_Walk,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_DH_F_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::DH_B_Step, {          .Start = std::bind(&Monster_LothricKn::Start_DH_B_Step,this, std::placeholders::_1),          .Stay = std::bind(&Monster_LothricKn::Update_DH_Walk,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_DH_B_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_Up, {               .Start = std::bind(&Monster_LothricKn::Start_G_Up,this, std::placeholders::_1),               .Stay = std::bind(&Monster_LothricKn::Update_G_Up,this, std::placeholders::_1,std::placeholders::_2),                 .End = std::bind(&Monster_LothricKn::End_G_Up, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_Down, {             .Start = std::bind(&Monster_LothricKn::Start_G_Down,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_G_Down,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_G_Down, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_L_Side_Step, {      .Start = std::bind(&Monster_LothricKn::Start_G_L_Side_Step,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_G_Walk,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_G_L_Side_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_R_Side_Step, {      .Start = std::bind(&Monster_LothricKn::Start_G_R_Side_Step,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_G_Walk,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_G_R_Side_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_F_Step, {           .Start = std::bind(&Monster_LothricKn::Start_G_F_Step,this, std::placeholders::_1),           .Stay = std::bind(&Monster_LothricKn::Update_G_Walk,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_G_F_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_B_Step, {           .Start = std::bind(&Monster_LothricKn::Start_G_B_Step,this, std::placeholders::_1),           .Stay = std::bind(&Monster_LothricKn::Update_G_Walk,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_G_B_Step, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_L_Turn, {           .Start = std::bind(&Monster_LothricKn::Start_G_L_Turn,this, std::placeholders::_1),           .Stay = std::bind(&Monster_LothricKn::Update_G_L_Turn,this, std::placeholders::_1,std::placeholders::_2),             .End = std::bind(&Monster_LothricKn::End_G_L_Turn, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_R_Turn, {           .Start = std::bind(&Monster_LothricKn::Start_G_R_Turn,this, std::placeholders::_1),           .Stay = std::bind(&Monster_LothricKn::Update_G_R_Turn,this, std::placeholders::_1,std::placeholders::_2),             .End = std::bind(&Monster_LothricKn::End_G_R_Turn, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_L_TurnTwice, {      .Start = std::bind(&Monster_LothricKn::Start_G_L_TurnTwice,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_G_L_TurnTwice,this, std::placeholders::_1,std::placeholders::_2),        .End = std::bind(&Monster_LothricKn::End_G_L_TurnTwice, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_R_TurnTwice, {      .Start = std::bind(&Monster_LothricKn::Start_G_R_TurnTwice,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_G_R_TurnTwice,this, std::placeholders::_1,std::placeholders::_2),        .End = std::bind(&Monster_LothricKn::End_G_R_TurnTwice, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_Run, {              .Start = std::bind(&Monster_LothricKn::Start_G_Run,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_G_Run,this, std::placeholders::_1,std::placeholders::_2),                .End = std::bind(&Monster_LothricKn::End_G_Run, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_Att_Bash, {         .Start = std::bind(&Monster_LothricKn::Start_G_Att_Bash,this, std::placeholders::_1),         .Stay = std::bind(&Monster_LothricKn::Update_G_Att_Bash,this, std::placeholders::_1,std::placeholders::_2),           .End = std::bind(&Monster_LothricKn::End_G_Att_Bash, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Hit_W, {            .Start = std::bind(&Monster_LothricKn::Start_F_Hit_W,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_Hit_W,this, std::placeholders::_1,std::placeholders::_2),                .End = std::bind(&Monster_LothricKn::End_F_Hit_W, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::B_Hit_W, {            .Start = std::bind(&Monster_LothricKn::Start_B_Hit_W,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_Hit_W,this, std::placeholders::_1,std::placeholders::_2),                .End = std::bind(&Monster_LothricKn::End_B_Hit_W, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::R_Hit_W, {            .Start = std::bind(&Monster_LothricKn::Start_R_Hit_W,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_Hit_W,this, std::placeholders::_1,std::placeholders::_2),                .End = std::bind(&Monster_LothricKn::End_R_Hit_W, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::L_Hit_W, {            .Start = std::bind(&Monster_LothricKn::Start_L_Hit_W,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_Hit_W,this, std::placeholders::_1,std::placeholders::_2),                .End = std::bind(&Monster_LothricKn::End_L_Hit_W, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Hit, {              .Start = std::bind(&Monster_LothricKn::Start_F_Hit,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_Hit,this, std::placeholders::_1,std::placeholders::_2),                  .End = std::bind(&Monster_LothricKn::End_F_Hit, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::B_Hit, {              .Start = std::bind(&Monster_LothricKn::Start_B_Hit,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_Hit,this, std::placeholders::_1,std::placeholders::_2),                  .End = std::bind(&Monster_LothricKn::End_B_Hit, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::R_Hit, {              .Start = std::bind(&Monster_LothricKn::Start_R_Hit,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_Hit,this, std::placeholders::_1,std::placeholders::_2),                  .End = std::bind(&Monster_LothricKn::End_R_Hit, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::L_Hit, {              .Start = std::bind(&Monster_LothricKn::Start_L_Hit,this, std::placeholders::_1),              .Stay = std::bind(&Monster_LothricKn::Update_Hit,this, std::placeholders::_1,std::placeholders::_2),                  .End = std::bind(&Monster_LothricKn::End_L_Hit, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_F_Hit_W, {          .Start = std::bind(&Monster_LothricKn::Start_G_F_Hit_W,this, std::placeholders::_1),          .Stay = std::bind(&Monster_LothricKn::Update_G_F_Hit_W,this, std::placeholders::_1,std::placeholders::_2),            .End = std::bind(&Monster_LothricKn::End_G_F_Hit_W, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_F_Hit_W_PushBack, { .Start = std::bind(&Monster_LothricKn::Start_G_F_Hit_W_PushBack,this, std::placeholders::_1), .Stay = std::bind(&Monster_LothricKn::Update_G_F_Hit_W_PushBack,this, std::placeholders::_1,std::placeholders::_2),   .End = std::bind(&Monster_LothricKn::End_G_F_Hit_W_PushBack, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_F_Hit, {            .Start = std::bind(&Monster_LothricKn::Start_G_F_Hit,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_G_F_Hit,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_G_F_Hit, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_F_Hit_PushBack, {   .Start = std::bind(&Monster_LothricKn::Start_G_F_Hit_PushBack,this, std::placeholders::_1),   .Stay = std::bind(&Monster_LothricKn::Update_G_F_Hit_PushBack,this, std::placeholders::_1,std::placeholders::_2),     .End = std::bind(&Monster_LothricKn::End_G_F_Hit_PushBack, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_F_Hit_S_PushBack, { .Start = std::bind(&Monster_LothricKn::Start_G_F_Hit_S_PushBack,this, std::placeholders::_1), .Stay = std::bind(&Monster_LothricKn::Update_G_F_Hit_S_PushBack,this, std::placeholders::_1,std::placeholders::_2),   .End = std::bind(&Monster_LothricKn::End_G_F_Hit_S_PushBack, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Block_Shield, {       .Start = std::bind(&Monster_LothricKn::Start_Block_Shield,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_Block_Shield,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_Block_Shield, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::G_Break, {            .Start = std::bind(&Monster_LothricKn::Start_G_Break,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_G_Break,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_G_Break, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::Break_Down, {         .Start = std::bind(&Monster_LothricKn::Start_Break_Down,this, std::placeholders::_1),         .Stay = std::bind(&Monster_LothricKn::Update_Break_Down,this, std::placeholders::_1,std::placeholders::_2),           .End = std::bind(&Monster_LothricKn::End_Break_Down, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Death, {            .Start = std::bind(&Monster_LothricKn::Start_F_Death,this, std::placeholders::_1),            .Stay = std::bind(&Monster_LothricKn::Update_F_Death,this, std::placeholders::_1,std::placeholders::_2),              .End = std::bind(&Monster_LothricKn::End_F_Death, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Death_End, {        .Start = std::bind(&Monster_LothricKn::Start_F_Death_End,this, std::placeholders::_1),        .Stay = std::bind(&Monster_LothricKn::Update_F_Death_End,this, std::placeholders::_1,std::placeholders::_2),          .End = std::bind(&Monster_LothricKn::End_F_Death_End, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Death_B, {          .Start = std::bind(&Monster_LothricKn::Start_F_Death_B,this, std::placeholders::_1),          .Stay = std::bind(&Monster_LothricKn::Update_F_Death_B,this, std::placeholders::_1,std::placeholders::_2),            .End = std::bind(&Monster_LothricKn::End_F_Death_B, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Death_B_End, {      .Start = std::bind(&Monster_LothricKn::Start_F_Death_B_End,this, std::placeholders::_1),      .Stay = std::bind(&Monster_LothricKn::Update_F_Death_B_End,this, std::placeholders::_1,std::placeholders::_2),        .End = std::bind(&Monster_LothricKn::End_F_Death_B_End, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::B_Stab, {             .Start = std::bind(&Monster_LothricKn::Start_B_Stab,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_B_Stab,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_B_Stab,this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::B_Stab_Death, {       .Start = std::bind(&Monster_LothricKn::Start_B_Stab_Death,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_B_Stab_Death,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_B_Stab_Death, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::B_Stab_Death_End, {   .Start = std::bind(&Monster_LothricKn::Start_B_Stab_Death_End,this, std::placeholders::_1),   .Stay = std::bind(&Monster_LothricKn::Update_B_Stab_Death_End,this, std::placeholders::_1,std::placeholders::_2),     .End = std::bind(&Monster_LothricKn::End_B_Stab_Death_End, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Stab, {             .Start = std::bind(&Monster_LothricKn::Start_F_Stab,this, std::placeholders::_1),             .Stay = std::bind(&Monster_LothricKn::Update_F_Stab,this, std::placeholders::_1,std::placeholders::_2),               .End = std::bind(&Monster_LothricKn::End_F_Stab,this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Stab_Death, {       .Start = std::bind(&Monster_LothricKn::Start_F_Stab_Death,this, std::placeholders::_1),       .Stay = std::bind(&Monster_LothricKn::Update_F_Stab_Death,this, std::placeholders::_1,std::placeholders::_2),         .End = std::bind(&Monster_LothricKn::End_F_Stab_Death, this, std::placeholders::_1) });
	MainState.CreateState(Enum_LothricKn_State::F_Stab_Death_End, {   .Start = std::bind(&Monster_LothricKn::Start_F_Stab_Death_End,this, std::placeholders::_1),   .Stay = std::bind(&Monster_LothricKn::Update_F_Stab_Death_End,this, std::placeholders::_1,std::placeholders::_2),     .End = std::bind(&Monster_LothricKn::End_F_Stab_Death_End, this, std::placeholders::_1) });

	MainState.ChangeState(Enum_LothricKn_State::Debug);
}

void Monster_LothricKn::StartSleep(GameEngineState* _State)
{
	Off();
}

void Monster_LothricKn::Start_None(GameEngineState* _State)
{
	Enum_LothricKn_State FineState = GetStateToAggroTable();
	_State->ChangeState(FineState);
}

void Monster_LothricKn::Start_Debug(GameEngineState* _State)
{
	// MainRenderer->ChangeAnimation("Idle_Standing1");
}

void Monster_LothricKn::Start_Idle_Standing(GameEngineState* _State)
{
	ContentsDebug::NUllCheck(PatrolCollision.get());
	PatrolCollision->On();
	Sword.Off();
	Shield.Off();
	OnWeaponMask();
	MainRenderer->ChangeAnimation("Idle_Standing");
}

void Monster_LothricKn::Start_Idle_Standing1(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("Idle_Standing1");
}

void Monster_LothricKn::Start_Idle_Sit(GameEngineState* _State)
{
	ContentsDebug::NUllCheck(PatrolCollision.get());
	PatrolCollision->On();
	Sword.Off();
	Shield.Off();
	MaskReset();
	MainRenderer->ChangeAnimation("Idle_Sit");
}

void Monster_LothricKn::Start_Idle_Gaurding(GameEngineState* _State)
{
	Debug.DebugOn();
	SetCombatMode(eCombatState::Guarding);
}

void Monster_LothricKn::Start_Patrol(GameEngineState* _State)
{
	ContentsDebug::NUllCheck(PatrolCollision.get());
	PatrolCollision->On();
	Sword.Off();
	Shield.Off();
	OnWeaponMask();
	SetPatrolCollision(true);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("Patrol");
}

void Monster_LothricKn::Start_Patrol_L_Turn(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("L_Turn");
}

void Monster_LothricKn::Start_Patrol_R_Turn(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("R_Turn");
}

void Monster_LothricKn::Start_Patrol_L_TurnTwice(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("L_TurnTwice");
}

void Monster_LothricKn::Start_Patrol_R_TurnTwice(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("R_TurnTwice");
}

void Monster_LothricKn::Start_Combo_Att_11(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("Combo_Att_11");
}

void Monster_LothricKn::Start_Combo_Att_12(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("Combo_Att_12");
}

void Monster_LothricKn::Start_Combo_Att_13(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("Combo_Att_13");
}

void Monster_LothricKn::Start_Combo_Att_21(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("Combo_Att_21");
}

void Monster_LothricKn::Start_Combo_Att_22(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("Combo_Att_22");
}

void Monster_LothricKn::Start_Combo_Att_23(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("Combo_Att_23");
}

void Monster_LothricKn::Start_RH_Att_HitDown(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("RH_Att_HitDown");
}

void Monster_LothricKn::Start_LH_ShieldAttack(GameEngineState* _State)
{
	Shield.ResetRecord();
	MainRenderer->ChangeAnimation("LH_ShieldAttack");
}

void Monster_LothricKn::Start_RH_Rear_Att(GameEngineState* _State)
{
	Sword.ResetRecord();
	MainRenderer->ChangeAnimation("RH_Rear_Att");
}

void Monster_LothricKn::Start_L_Turn(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("L_Turn");
}

void Monster_LothricKn::Start_R_Turn(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("R_Turn");
}

void Monster_LothricKn::Start_L_TurnTwice(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("L_TurnTwice");
}

void Monster_LothricKn::Start_R_TurnTwice(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("R_TurnTwice");
}

void Monster_LothricKn::Start_L_Side_Step(GameEngineState* _State)
{
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("L_Side_Step");
}

void Monster_LothricKn::Start_R_Side_Step(GameEngineState* _State)
{
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("R_Side_Step");
}

void Monster_LothricKn::Start_F_Step(GameEngineState* _State)
{
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("F_Step");
}

void Monster_LothricKn::Start_B_Step(GameEngineState* _State)
{
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("B_Step");
}

void Monster_LothricKn::Start_Run(GameEngineState* _State)
{
	ResetStateTime();
	MainRenderer->ChangeAnimation("Run");
}

void Monster_LothricKn::Start_SitUp(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("SitUp");
}

void Monster_LothricKn::Start_DH_Hold(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Two_Handed);
	MainRenderer->ChangeAnimation("DH_Hold");
}

void Monster_LothricKn::Start_DH_UnHold(GameEngineState* _State)
{
	MainRenderer->ChangeAnimation("DH_UnHold");
}

void Monster_LothricKn::Start_DH_Stab_Att(GameEngineState* _State)
{
	Sword.ResetRecord();
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("DH_Stab_Att");
}

void Monster_LothricKn::Start_DH_Swing_Att(GameEngineState* _State)
{
	Sword.ResetRecord();
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("DH_Swing_Att");
}

void Monster_LothricKn::Start_DH_L_Side_Step(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Two_Handed);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("DH_L_Side_Step");
}

void Monster_LothricKn::Start_DH_R_Side_Step(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Two_Handed);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("DH_R_Side_Step");
}

void Monster_LothricKn::Start_DH_F_Step(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Two_Handed);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("DH_F_Step");
}

void Monster_LothricKn::Start_DH_B_Step(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Two_Handed);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("DH_B_Step");
}

void Monster_LothricKn::Start_G_Up(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_Up");
}

void Monster_LothricKn::Start_G_Down(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("G_Down");
}

void Monster_LothricKn::Start_G_L_Side_Step(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("G_L_Side_Step");
}

void Monster_LothricKn::Start_G_R_Side_Step(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("G_R_Side_Step");
}

void Monster_LothricKn::Start_G_F_Step(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("G_F_Step");
}

void Monster_LothricKn::Start_G_B_Step(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	StateTimeSet(MIN_TIME_STEPSTATE, MAX_TIME_STEPSTATE);
	MainRenderer->ChangeAnimation("G_B_Step");
}

void Monster_LothricKn::Start_G_L_Turn(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_L_Turn");
}

void Monster_LothricKn::Start_G_R_Turn(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_R_Turn");
}

void Monster_LothricKn::Start_G_L_TurnTwice(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_L_TurnTwice");
}

void Monster_LothricKn::Start_G_R_TurnTwice(GameEngineState* _State) 
{
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_R_TurnTwice");
}

void Monster_LothricKn::Start_G_Run(GameEngineState* _State)
{
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_Run");
}

void Monster_LothricKn::Start_G_Att_Bash(GameEngineState* _State) 
{
	Shield.ResetRecord();
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("G_Att_Bash");
}

void Monster_LothricKn::Start_F_Hit_W(GameEngineState* _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("F_Hit_W", true);
}

void Monster_LothricKn::Start_B_Hit_W(GameEngineState * _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("B_Hit_W", true);
}

void Monster_LothricKn::Start_R_Hit_W(GameEngineState * _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("R_Hit_W", true);
}

void Monster_LothricKn::Start_L_Hit_W(GameEngineState * _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("L_Hit_W", true);
}

void Monster_LothricKn::Start_F_Hit(GameEngineState* _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("F_Hit", true);
}

void Monster_LothricKn::Start_B_Hit(GameEngineState* _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("B_Hit", true);
}

void Monster_LothricKn::Start_R_Hit(GameEngineState* _State)
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("R_Hit", true);
}

void Monster_LothricKn::Start_L_Hit(GameEngineState* _State) 
{
	Hit.SetHit(false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("L_Hit", true);
}


void Monster_LothricKn::Start_G_F_Hit_W(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_F_Hit_W", true);
}

void Monster_LothricKn::Start_G_F_Hit_W_PushBack(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_F_Hit_W_PushBack", true);
}

void Monster_LothricKn::Start_G_F_Hit(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_F_Hit", true);
}

void Monster_LothricKn::Start_G_F_Hit_PushBack(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_F_Hit_PushBack", true);
}

void Monster_LothricKn::Start_G_F_Hit_S_PushBack(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	SetCombatMode(eCombatState::Guarding);
	MainRenderer->ChangeAnimation("G_F_Hit_S_PushBack", true);
}

void Monster_LothricKn::Start_Block_Shield(GameEngineState * _State)
{
	Hit.SetHit(false);
	SetFlag(Enum_ActorFlag::Block_Shield, false);
	MainRenderer->ChangeAnimation("Block_Shield");
}

void Monster_LothricKn::Start_G_Break(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	Stat.SetPoise(100);
	SetFlag(Enum_ActorFlag::Guard_Break, false);
	SetFlag(Enum_ActorFlag::Groggy, true);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("G_Break");
}

void Monster_LothricKn::Start_Break_Down(GameEngineState * _State)
{
	Hit.SetHit(false);
	Hit.SetGuardSuccesss(false);
	Stat.SetPoise(100);
	SetFlag(Enum_ActorFlag::Break_Posture, false);
	SetFlag(Enum_ActorFlag::Groggy, true);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("Break_Posture");
}

void Monster_LothricKn::Start_F_Death(GameEngineState * _State)
{
	OffAllCollision();
	DeathProcess();
	MainRenderer->ChangeAnimation("F_Death");
}

void Monster_LothricKn::Start_F_Death_End(GameEngineState * _State)
{
	MainRenderer->ChangeAnimation("F_Death_End");
}

void Monster_LothricKn::Start_F_Death_B(GameEngineState * _State)
{
	OffAllCollision();
	DeathProcess();
	MainRenderer->ChangeAnimation("F_Death_B");
}

void Monster_LothricKn::Start_F_Death_B_End(GameEngineState * _State)
{
	MainRenderer->ChangeAnimation("F_Death_B_End");
}

void Monster_LothricKn::Start_B_Stab(GameEngineState * _State)
{
	Hit.SetInvincible(true);
	Hit.SetHit(false);
	SetFlag(Enum_ActorFlag::BackStab, false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("B_Stab");
}

void Monster_LothricKn::Start_B_Stab_Death(GameEngineState * _State)
{
	if (Capsule)
	{
		Capsule->Off();
	}
	OffAllCollision();
	MainRenderer->ChangeAnimation("B_Stab_Death");
}

void Monster_LothricKn::Start_B_Stab_Death_End(GameEngineState * _State)
{
	MainRenderer->ChangeAnimation("B_Stab_Death_End");
}

void Monster_LothricKn::Start_F_Stab(GameEngineState * _State)
{
	Hit.SetInvincible(true);
	Hit.SetHit(false);
	SetFlag(Enum_ActorFlag::FrontStab, false);
	SetCombatMode(eCombatState::Normal);
	MainRenderer->ChangeAnimation("F_Stab");
}

void Monster_LothricKn::Start_F_Stab_Death(GameEngineState * _State)
{
	OffAllCollision();
	MainRenderer->ChangeAnimation("F_Stab_Death");
}

void Monster_LothricKn::Start_F_Stab_Death_End(GameEngineState * _State)
{
	MainRenderer->ChangeAnimation("F_Stab_Death_End");
}




////////////////////////////////////////////////////////////
////////////// Update-State

void Monster_LothricKn::Update_Debug(float _DeltaTime, GameEngineState* _State)
{
	if (GameEngineInput::IsDown('U', this))
	{
		_State->ChangeState(Enum_LothricKn_State::Combo_Att_11);
		return;
	}
}

void Monster_LothricKn::Update_Idle_Standing(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	bool IsFindTarget = FindAndSetTarget(TARGET_ORDER);
	if (IsFindTarget)
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_Idle_Standing1(float _DeltaTime, GameEngineState* _State)
{
	Enum_LothricKn_State FindState = GetStateToAggroTable();
	if (Enum_LothricKn_State::None == FindState)
	{
		MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
		return;
	}

	_State->ChangeState(FindState);
	return;
}

void Monster_LothricKn::Update_Idle_Sit(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	bool IsFindTarget = FindAndSetTarget(TARGET_ORDER);
	if (IsFindTarget)
	{
		_State->ChangeState(Enum_LothricKn_State::SitUp);
		return;
	}
}

void Monster_LothricKn::Update_Idle_Gaurding(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}
}

void Monster_LothricKn::Update_Patrol(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	bool IsFindTarget = FindAndSetTarget(TARGET_ORDER);
	if (IsFindTarget)
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			MainState.ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		MainState.ChangeState(FindMovementState);
		return;
	}

	if (nullptr == PathObject)
	{
		MsgBoxAssert("정찰할 길을 지정하지 않았는데 정찰하려 했습니다.");
		return;
	}

	const float4 WPos = Transform.GetWorldPosition();
	if (PathObject->IsArrive(WPos))
	{
		PathObject->ChangePath();
	}

	Enum_LothricKn_State FindState = GetStateToPatrolTable();
	if (Enum_LothricKn_State::None != FindState)
	{
		MainState.ChangeState(FindState);
		return;
	}
	else
	{
		RotToPatrolPath(_DeltaTime, 10.0f, 500.0f, 30.0f);
	}
}

void Monster_LothricKn::Update_Patrol_Turn(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameEnd())
	{
		bool IsFindTarget = FindAndSetTarget(TARGET_ORDER);
		if (IsFindTarget)
		{
			Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
			if (Enum_LothricKn_State::None != FindDodgeState)
			{
				MainState.ChangeState(FindDodgeState);
				return;
			}

			Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
			MainState.ChangeState(FindMovementState);
			return;
		}

		_State->ChangeState(Enum_LothricKn_State::Patrol);
		return;
	}
}

void Monster_LothricKn::Update_Combo_Att_11(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(19))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, FRONT_ANGLE))
		{
			_State->ChangeState(Enum_LothricKn_State::Combo_Att_12);
			return;
		}
	}

	if (IsFrameOnce(55))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (Enum_LothricKn_State::None != FindState)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(60))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(17, 19))
	{
		AttackToPlayer(eAttackType::Sword, SWING_ATTACK_STIFFNESS);
	}

	if (IsFrameOnce(20))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_Combo_Att_12(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(21))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, FRONT_ANGLE))
		{
			_State->ChangeState(Enum_LothricKn_State::Combo_Att_13);
			return;
		}
	}

	if (IsFrameOnce(44))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (Enum_LothricKn_State::None != FindState)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(51))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(19, 21))
	{
		AttackToPlayer(eAttackType::Sword, SWING_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(22))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_Combo_Att_13(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(55))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (Enum_LothricKn_State::None != FindState)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(61))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(20, 22))
	{
		AttackToPlayer(eAttackType::Sword, SHOT_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(23))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_Combo_Att_21(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(23))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, FRONT_ANGLE))
		{
			_State->ChangeState(Enum_LothricKn_State::Combo_Att_22);
			return;
		}
	}

	if (IsFrameOnce(42))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (FindState != Enum_LothricKn_State::None)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(50))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(21, 23))
	{
		AttackToPlayer(eAttackType::Sword, SWING_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(24))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_Combo_Att_22(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(32))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, FRONT_ANGLE))
		{
			_State->ChangeState(Enum_LothricKn_State::Combo_Att_23);
			return;
		}
	}

	if (IsFrameOnce(57))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (FindState != Enum_LothricKn_State::None)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(66))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(22, 24))
	{
		AttackToPlayer(eAttackType::Sword, SWING_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(25))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_Combo_Att_23(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(58))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, FRONT_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (FindState != Enum_LothricKn_State::None)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(60))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(17, 19))
	{
		AttackToPlayer(eAttackType::Sword, SHOT_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(20))
	{
		AttackDone(eAttackType::Sword);
	}
}


void Monster_LothricKn::Update_RH_Att_HitDown(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(45))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (FindState != Enum_LothricKn_State::None)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(51))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(24, 26))
	{
		AttackToPlayer(eAttackType::Sword, 20);
	}
	if (IsFrameOnce(27))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_LH_ShieldAttack(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(41))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (FindState != Enum_LothricKn_State::None)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(45))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(17, 19))
	{
		AttackToPlayer(eAttackType::Shield, SHIELD_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(20))
	{
		AttackDone(eAttackType::Shield);
	}
}

void Monster_LothricKn::Update_RH_Rear_Att(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(48))
	{
		if (true == CanAttack(W_SCALE * MELEE_RANGE, SIDE_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (FindState != Enum_LothricKn_State::None)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(53))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}

	if (IsFrame(17, 21))
	{
		AttackToPlayer(eAttackType::Sword, SHOT_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(22))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_L_Turn(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_R_Turn(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_L_TurnTwice(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_R_TurnTwice(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_L_Side_Step(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	const float fStateTime = _State->GetStateTime();

	bool AngleCheck = false == IsTargetInAngle(MIN_ROT_ANGLE);
	bool TimeCheck = fMaxStateTime < fStateTime;
	bool DistCheck = Enum_TargetDist::Medium != Monster_LothricKn::GetTargetDistance_e();

	if (AngleCheck)
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (TimeCheck || DistCheck)
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_R_Side_Step(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	const float fStateTime = _State->GetStateTime();

	bool AngleCheck = false == IsTargetInAngle(MIN_ROT_ANGLE);
	bool TimeCheck = fMaxStateTime < fStateTime;
	bool DistCheck = Enum_TargetDist::Medium != Monster_LothricKn::GetTargetDistance_e();

	if (AngleCheck)
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (TimeCheck || DistCheck)
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_F_Step(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	const float fStateTime = _State->GetStateTime();

	bool AngleCheck = false == IsTargetInAngle(MIN_ROT_ANGLE);
	bool TimeCheck = fMaxStateTime < fStateTime;
	bool DistCheck = Enum_TargetDist::Medium != Monster_LothricKn::GetTargetDistance_e();

	if (AngleCheck)
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (TimeCheck || DistCheck)
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_B_Step(float _DeltaTime, GameEngineState* _State)
{
	if (true == CheckAndSetHitState())
	{
		return;
	}

	const float fStateTime = _State->GetStateTime();

	bool AngleCheck = false == IsTargetInAngle(MIN_ROT_ANGLE);
	bool TimeCheck = fMaxStateTime < fStateTime;
	bool DistCheck = Enum_TargetDist::Medium != Monster_LothricKn::GetTargetDistance_e();

	if (AngleCheck)
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (TimeCheck || DistCheck)
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_Run(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (fMaxStateTime > _State->GetStateTime())
	{
		// ReturnPatrolRoute

		// _State->ChangeState(Enum_LothricKn_State::);
		return;
	}

	// Check
	if (false == IsTargetInAngle(MIN_ROT_ANGLE))
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (true == IsTargetInRange(Enum_TargetDist::Long))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_SitUp(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(15))
	{
		OnWeaponMask();
	}

	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(29))
	{
		if (CanAttack(W_SCALE * MELEE_RANGE, FRONT_ANGLE))
		{
			Enum_LothricKn_State FindState = GetStateToAttackTable();
			if (Enum_LothricKn_State::None != FindState)
			{
				_State->ChangeState(FindState);
				return;
			}
		}
	}

	if (IsFrame(31))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_DH_Hold(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(18))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	} 

	if (IsFrame(19))
	{
		Enum_LothricKn_State FindState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_DH_UnHold(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(17))
	{
		SetCombatMode(eCombatState::Normal);
	}

	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(17))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	}

	if (IsFrame(19))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_DH_Stab_Att(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(44))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	}

	if (IsFrame(48))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}

	if (IsFrame(16, 20))
	{
		AttackToPlayer(eAttackType::Sword, SHOT_ATTACK_STIFFNESS * DH_ATTACK_STIFFNESS_RATIO);
	}
	if (IsFrameOnce(21))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_DH_Swing_Att(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(45))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	}

	if (IsFrame(49))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}

	if (IsFrame(15, 18))
	{
		AttackToPlayer(eAttackType::Sword, SWING_ATTACK_STIFFNESS * DH_ATTACK_STIFFNESS_RATIO);
	}
	if (IsFrameOnce(19))
	{
		AttackDone(eAttackType::Sword);
	}
}

void Monster_LothricKn::Update_DH_Walk(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	const float fStateTime = _State->GetStateTime();

	bool AngleCheck = (false == IsTargetInAngle(MIN_ROT_ANGLE));
	bool TimeCheck = fMaxStateTime < fStateTime;
	bool DistCheck = Enum_TargetDist::Medium != Monster_LothricKn::GetTargetDistance_e();

	if (AngleCheck)
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (DistCheck)
	{
		Enum_LothricKn_State FindAttackState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindAttackState)
		{
			_State->ChangeState(FindAttackState);
			return;
		}
	}

	if (TimeCheck)
	{
		Enum_LothricKn_State FindMoveMentState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMoveMentState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMoveMentState);
		return;
	}
}

void Monster_LothricKn::Update_G_Up(float _DeltaTime, GameEngineState* _State)\
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(10))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	}

	if (IsFrame(13))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_Down(float _DeltaTime, GameEngineState* _State)\
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(10))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	}

	if (IsFrame(13))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_Walk(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	const float fStateTime = _State->GetStateTime();

	bool AngleCheck = (false == IsTargetInAngle(MIN_ROT_ANGLE));
	bool TimeCheck = fMaxStateTime < fStateTime;

	if (AngleCheck)
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (TimeCheck)
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_L_Turn(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_R_Turn(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_L_TurnTwice(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_R_TurnTwice(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_Run(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (fMaxStateTime > _State->GetStateTime())
	{
		// ReturnPatrolRoute

		// _State->ChangeState(Enum_LothricKn_State::);
		return;
	}

	// Check
	if (false == IsTargetInAngle(MIN_ROT_ANGLE))
	{
		RotToTarget(_DeltaTime, MIN_ROTSPEED_TO_TARGET, MAX_ROTSPEED_TO_TARGET);
	}

	if (true == IsTargetInRange(Enum_TargetDist::Medium))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}
}

void Monster_LothricKn::Update_G_Att_Bash(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(46))
	{
		Enum_LothricKn_State FindState = GetStateToAttackTable();
		if (Enum_LothricKn_State::None != FindState)
		{
			_State->ChangeState(FindState);
			return;
		}
	}

	if (IsFrame(59))
	{
		Enum_LothricKn_State FindState = GetStateToAggroTable();
		if (Enum_LothricKn_State::None == FindState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindState);
		return;
	}

	if (IsFrame(26, 31))
	{
		AttackToPlayer(eAttackType::Shield, SHIELD_ATTACK_STIFFNESS);
	}
	if (IsFrameOnce(32))
	{
		AttackDone(eAttackType::Shield);
	}
}

void Monster_LothricKn::Update_Hit_W(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameEnd())
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}

	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(10))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}
	
	if (IsFrame(11))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_Hit(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(23))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(24))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_G_F_Hit_W(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(10))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(17))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_G_F_Hit_W_PushBack(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(10))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(17))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_G_F_Hit(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(13))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrameOnce(19))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}
	}

	if (IsFrame(20))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_G_F_Hit_PushBack(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(13))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(20))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_G_F_Hit_S_PushBack(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(13))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(20))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_Block_Shield(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(34))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(35))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_G_Break(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(50))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(51))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}

		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_Break_Down(float _DeltaTime, GameEngineState* _State)
{
	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(56))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrameOnce(58))
	{
		Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable();
		if (Enum_LothricKn_State::None != FindDodgeState)
		{
			_State->ChangeState(FindDodgeState);
			return;
		}
	}

	if (IsFrame(59))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_F_Death(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(55))
	{
		SetFlag(Enum_ActorFlag::Death, true);
	}

	if (IsFrameOnce(105))
	{
		HideWeaponMask();
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		_State->ChangeState(Enum_LothricKn_State::F_Death_End);
		return;
	}
}

void Monster_LothricKn::Update_F_Death_End(float _DeltaTime, GameEngineState* _State)
{
}

void Monster_LothricKn::Update_F_Death_B(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(45))
	{
		DeathProcess();
	}

	if (IsFrameOnce(50))
	{
		HideWeaponMask();
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		_State->ChangeState(Enum_LothricKn_State::F_Death_End);
		return;
	}
}

void Monster_LothricKn::Update_F_Death_B_End(float _DeltaTime, GameEngineState* _State)
{

}

void Monster_LothricKn::Update_B_Stab(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(125))
	{
		Hit.SetInvincible(false);
	}

	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(80))
	{
		if (true == Stat.IsDeath())
		{
			_State->ChangeState(Enum_LothricKn_State::B_Stab_Death);
			return;
		}
	}

	if (IsFrameOnce(142))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(144))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		if (Enum_LothricKn_State::None == FindMovementState)
		{
			MsgBoxAssert("해당 상태는 등록되지 않았습니다.");
			return;
		}

		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_B_Stab_Death(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(50))
	{
		DeathProcess();
	}

	if (IsFrameOnce(52))
	{
		HideWeaponMask();
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		_State->ChangeState(Enum_LothricKn_State::B_Stab_Death_End);
		return;
	}
}

void Monster_LothricKn::Update_B_Stab_Death_End(float _DeltaTime, GameEngineState* _State)
{

}

void Monster_LothricKn::Update_F_Stab(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(169))
	{
		Hit.SetInvincible(false);
	}

	// Hit Logic
	if (true == CheckAndSetHitState())
	{
		return;
	}

	if (IsFrameOnce(140))
	{
		if (true == Stat.IsDeath())
		{
			_State->ChangeState(Enum_LothricKn_State::F_Stab_Death);
			return;
		}
	}

	if (IsFrameOnce(174))
	{
		// Attack Logic
		if (true == CheckAndSetAttackState())
		{
			return;
		}
	}

	if (IsFrame(176))
	{
		Enum_LothricKn_State FindMovementState = GetStateToMovementTable();
		_State->ChangeState(FindMovementState);
		return;
	}
}

void Monster_LothricKn::Update_F_Stab_Death(float _DeltaTime, GameEngineState* _State)
{
	if (IsFrameOnce(55))
	{
		DeathProcess();
	}

	if (IsFrameOnce(64))
	{
		HideWeaponMask();
	}

	if (true == MainRenderer->IsCurAnimationEnd())
	{
		_State->ChangeState(Enum_LothricKn_State::F_Stab_Death_End);
		return;
	}
}

void Monster_LothricKn::Update_F_Stab_Death_End(float _DeltaTime, GameEngineState* _State)
{

}




////////////////////////////////////////////////////////////
////////////// End

void Monster_LothricKn::EndSleep(GameEngineState* _State)
{
	On();
}

void Monster_LothricKn::End_Idle_Standing(GameEngineState* _State)
{
	OnWeaponMask();
	SetCombatMode(eCombatState::Normal);
}

void Monster_LothricKn::End_Idle_Sit(GameEngineState* _State)
{
	OnWeaponMask();
	SetCombatMode(eCombatState::Normal);
}

void Monster_LothricKn::End_Idle_Gaurding(GameEngineState* _State)
{
	Debug.DebugOff();
	AttackDone(eAttackType::Shield);
}

void Monster_LothricKn::End_Combo_Att_11(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_Combo_Att_12(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_Combo_Att_13(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_Combo_Att_21(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_Combo_Att_22(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_Combo_Att_23(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_RH_Att_HitDown(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_LH_ShieldAttack(GameEngineState* _State)
{
	AttackDone(eAttackType::Shield);
}

void Monster_LothricKn::End_RH_Rear_Att(GameEngineState* _State)
{
	AttackDone(eAttackType::Sword);
}

void Monster_LothricKn::End_L_Turn(GameEngineState* _State)
{

}

void Monster_LothricKn::End_R_Turn(GameEngineState* _State)
{

}

void Monster_LothricKn::End_L_TurnTwice(GameEngineState* _State)
{

}

void Monster_LothricKn::End_R_TurnTwice(GameEngineState* _State)
{

}

void Monster_LothricKn::End_L_Side_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_R_Side_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_B_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_Run(GameEngineState* _State)
{

}

void Monster_LothricKn::End_SitUp(GameEngineState* _State)
{
	OnWeaponMask();
}

void Monster_LothricKn::End_DH_Hold(GameEngineState* _State)
{

}

void Monster_LothricKn::End_DH_UnHold(GameEngineState* _State)
{

}

void Monster_LothricKn::End_DH_Stab_Att(GameEngineState* _State)
{
	Sword.Off();
}

void Monster_LothricKn::End_DH_Swing_Att(GameEngineState* _State)
{
	Sword.Off();
}

void Monster_LothricKn::End_DH_L_Side_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_DH_R_Side_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_DH_F_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_DH_B_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_Up(GameEngineState* _State)
{
	
}

void Monster_LothricKn::End_G_Down(GameEngineState* _State)
{
	
}

void Monster_LothricKn::End_G_L_Side_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_R_Side_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_F_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_B_Step(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_L_Turn(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_R_Turn(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_L_TurnTwice(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_R_TurnTwice(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_Run(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_Att_Bash(GameEngineState* _State)
{
	AttackDone(eAttackType::Shield);
}

void Monster_LothricKn::End_F_Hit_W(GameEngineState* _State)
{

}

void Monster_LothricKn::End_B_Hit_W(GameEngineState* _State)
{

}

void Monster_LothricKn::End_R_Hit_W(GameEngineState* _State)
{

}

void Monster_LothricKn::End_L_Hit_W(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Hit(GameEngineState* _State)
{

}

void Monster_LothricKn::End_B_Hit(GameEngineState* _State)
{

}

void Monster_LothricKn::End_R_Hit(GameEngineState* _State)
{

}

void Monster_LothricKn::End_L_Hit(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_F_Hit_W(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_F_Hit_W_PushBack(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_F_Hit(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_F_Hit_PushBack(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_F_Hit_S_PushBack(GameEngineState* _State)
{

}

void Monster_LothricKn::End_Block_Shield(GameEngineState* _State)
{

}

void Monster_LothricKn::End_G_Break(GameEngineState* _State)
{
	SetFlag(Enum_ActorFlag::Groggy, false);
}

void Monster_LothricKn::End_Break_Down(GameEngineState* _State)
{
	SetFlag(Enum_ActorFlag::Groggy, false);
}

void Monster_LothricKn::End_F_Death(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Death_End(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Death_B(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Death_B_End(GameEngineState* _State)
{

}

void Monster_LothricKn::End_B_Stab(GameEngineState* _State)
{
	SetFlag(Enum_ActorFlag::BackStab, false);
}

void Monster_LothricKn::End_B_Stab_Death(GameEngineState* _State)
{

}

void Monster_LothricKn::End_B_Stab_Death_End(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Stab(GameEngineState* _State)
{
	SetFlag(Enum_ActorFlag::FrontStab, false);
}

void Monster_LothricKn::End_F_Stab_Death(GameEngineState* _State)
{

}

void Monster_LothricKn::End_F_Stab_Death_End(GameEngineState* _State)
{

}


////////////////////////////////////////////////////////////
////////////// State Func

bool Monster_LothricKn::CanAttack(float _fDist, float _fDir) const
{
	const float Dist = BaseActor::GetTargetDistance();

	const float AbsTargetAngle = std::fabs(BaseActor::GetTargetAngle());

	bool DistCheck = (Dist < _fDist);
	bool AngleCheck = (AbsTargetAngle < _fDir);

	if (AngleCheck && DistCheck)
	{
		return true;
	}

	return false;
}

bool Monster_LothricKn::IsTargetInAngle(float _fAngle) const
{
	const float AbsTargetAngle = std::fabs(BaseActor::GetTargetAngle());
	return (AbsTargetAngle < _fAngle);
}

bool Monster_LothricKn::CheckAndSetHitState()
{
	if (true == Hit.IsInvincible())
	{
		return false;
	}

	Enum_LothricKn_State FindState = GetStateToHitTable();
	if (Enum_LothricKn_State::None != FindState)
	{
		MainState.ChangeState(FindState);
		return true;
	}

	return false;
}

bool Monster_LothricKn::CheckAndSetAttackState()
{
	Enum_LothricKn_State FindState = GetStateToAttackTable();
	if (Enum_LothricKn_State::None != FindState)
	{
		MainState.ChangeState(FindState);
		return true;
	}

	return false;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToAggroTable()
{
	GameEngineActor* pTarget = GetTargetPointer();
	if (nullptr == pTarget)
	{
		MsgBoxAssert("타겟이 존재하지 않는데 상태를 반환하려 했습니다.");
		return Enum_LothricKn_State::None;
	}

	Enum_TargetAngle eTargetRot = Monster_LothricKn::GetTargetAngle_e();
	Enum_TargetDist eTargetDist = Monster_LothricKn::GetTargetDistance_e();

	Enum_LothricKn_State FindAttackState = GetStateToAttackTable(eTargetDist, eTargetRot);
	if (Enum_LothricKn_State::None != FindAttackState)
	{
		return FindAttackState;
	}

	Enum_LothricKn_State FindDodgeState = GetStateToDodgeTable(eTargetDist, eTargetRot);
	if (Enum_LothricKn_State::None != FindDodgeState)
	{
		return FindDodgeState;
	}

	Enum_LothricKn_State FindMovementState = GetStateToMovementTable(eTargetDist, eTargetRot);
	if (Enum_LothricKn_State::None != FindMovementState)
	{
		return FindMovementState;
	}

	MsgBoxAssert("기본 상태가 등록되지 않았습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToMovementTable() const
{
	Enum_TargetAngle eTargetRot = Monster_LothricKn::GetTargetAngle_e();
	Enum_TargetDist eTargetDist = Monster_LothricKn::GetTargetDistance_e();

	Enum_LothricKn_State FindState = GetStateToMovementTable(eTargetDist, eTargetRot);
	return FindState;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToMovementTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	switch (CombatState)
	{
	case Monster_LothricKn::eCombatState::Normal:
		return GetStateToNormalMovementTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::Two_Handed:
		return GetStateToDHMovementTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::Guarding:
		return GetStateToGMovementTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::None:
	default:
		break;
	}

	MsgBoxAssert("해당 상태를 초기화해주지 않았습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToNormalMovementTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Normal != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetAngle::Back == _eTAngle)
	{
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetDist::Long == _eTDist)
	{
		if (Enum_TargetAngle::Front == _eTAngle)
		{
			return Enum_LothricKn_State::Run;
		}
	}

	if (Enum_TargetAngle::Side == _eTAngle)
	{
		if (Enum_RotDir::Left == BaseActor::GetRotDir_e())
		{
			return Enum_LothricKn_State::L_Turn;
		}

		if (Enum_RotDir::Right == BaseActor::GetRotDir_e())
		{
			return Enum_LothricKn_State::R_Turn;
		}
	}

	const int ModeChance = ContentsRandom::RandomInt(0, 7);
	enum eModeChance
	{
		Normal = 5,
		TwoHand = 6,
		Gaurding = 7,
	};

	if (eModeChance::TwoHand == ModeChance)
	{
		return Enum_LothricKn_State::DH_Hold;
	}

	if (eModeChance::Gaurding == ModeChance)
	{
		return Enum_LothricKn_State::G_Up;
	}

	const int DirChance = ContentsRandom::RandomInt(0, 2);
	enum eDirChance
	{
		Left = 0,
		Right = 1,
		Front = 2,
	};

	switch (DirChance)
	{
	case eDirChance::Left:
		return Enum_LothricKn_State::L_Side_Step;
	case eDirChance::Right:
		return Enum_LothricKn_State::R_Side_Step;
	case eDirChance::Front:
		return Enum_LothricKn_State::F_Step;
	default:
		break;
	}

	MsgBoxAssert("행동을 찾지 못했습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToDHMovementTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Two_Handed != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetAngle::Back == _eTAngle)
	{
		return Enum_LothricKn_State::DH_UnHold;
	}

	if (Enum_TargetDist::Long == _eTDist)
	{
		return Enum_LothricKn_State::DH_UnHold;
	}

	const int iChance = ContentsRandom::RandomInt(0, 2);
	enum eMoveType
	{
		Left,
		Right,
		Forward,
	};

	switch (iChance)
	{
	case eMoveType::Left:
		return Enum_LothricKn_State::DH_L_Side_Step;
	case eMoveType::Right:
		return Enum_LothricKn_State::DH_R_Side_Step;
	case eMoveType::Forward:
		return Enum_LothricKn_State::DH_F_Step;
	default:
		break;
	}

	MsgBoxAssert("행동을 찾지 못했습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToGMovementTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Guarding != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetAngle::Back == _eTAngle)
	{
		return Enum_LothricKn_State::G_Down;
	}

	if (Enum_TargetDist::Long == _eTDist)
	{
		if (Enum_TargetAngle::Front == _eTAngle)
		{
			return Enum_LothricKn_State::G_Run;
		}
	}

	const int iModeChance = ContentsRandom::RandomInt(0, 15);
	bool bChangeMode = (0 == iModeChance);
	if (bChangeMode)
	{
		return Enum_LothricKn_State::G_Down;
	}

	const int iChance = ContentsRandom::RandomInt(0, 2);
	enum MoveType
	{
		Left,
		Right,
		Forward,
	};

	switch (iChance)
	{
	case Left:
		return Enum_LothricKn_State::G_L_Side_Step;
	case Right:
		return Enum_LothricKn_State::G_R_Side_Step;
	case Forward:
		return Enum_LothricKn_State::G_F_Step;
	default:
		break;
	}

	MsgBoxAssert("행동을 찾지 못했습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToAttackTable()
{
	Enum_TargetAngle eTargetRot = Monster_LothricKn::GetTargetAngle_e();
	Enum_TargetDist eTargetDist = Monster_LothricKn::GetTargetDistance_e();

	Enum_LothricKn_State FindState = GetStateToAttackTable(eTargetDist, eTargetRot);
	return FindState;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToAttackTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle)
{
	switch (CombatState)
	{
	case Monster_LothricKn::eCombatState::Normal:
		return GetStateToNormalAttackTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::Two_Handed:
		return GetStateToDHAttackTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::Guarding:
		return GetStateToGAttackTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::None:
		break;
	default:
		break;
	}

	MsgBoxAssert("해당 상태를 초기화해주지 않았습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToNormalAttackTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle)
{
	if (eCombatState::Normal != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetDist::Close == _eTDist)
	{
		if (Enum_TargetAngle::Front == _eTAngle)
		{
			const int iChance = ContentsRandom::RandomInt(0, 8);
			enum eAttackType
			{
				Combo = 0,
				Hit_Down = 3,
			};

			if (eAttackType::Hit_Down == iChance)
			{
				return Enum_LothricKn_State::RH_Att_HitDown;
			}
		}
	}

	if (true == TargetRangeCmp(_eTDist, Enum_TargetDist::Melee))
	{
		if (Enum_TargetAngle::Side == _eTAngle)
		{
			if (Enum_RotDir::Left == BaseActor::GetRotDir_e())
			{
				return Enum_LothricKn_State::LH_ShieldAttack;
			}

			if (Enum_RotDir::Right == BaseActor::GetRotDir_e())
			{
				return Enum_LothricKn_State::RH_Rear_Att;
			}
		}

		const int ModeChance = ContentsRandom::RandomInt(0, 8);
		enum class eModeChance
		{
			TwoHand = 0,
			Shield = 1,
		};

		if (static_cast<int>(eModeChance::TwoHand) == ModeChance)
		{
			return Enum_LothricKn_State::DH_Hold;
		}
		if (static_cast<int>(eModeChance::Shield) == ModeChance)
		{
			return Enum_LothricKn_State::G_Up;
		}

		enum eAttackType
		{
			Combo1 = 0,
			Combo2,
			None,
		};

		Enum_LothricKn_State AttackState = Enum_LothricKn_State::None;

		switch (AttackRecord)
		{
		case eAttackType::Combo1:
			AttackState = Enum_LothricKn_State::Combo_Att_11;
			++AttackRecord;
			break;
		case eAttackType::Combo2:
			AttackState = Enum_LothricKn_State::Combo_Att_21;
			AttackRecord = eAttackType::Combo1;
			break;
		default:
			break;
		}

		return AttackState;
	}

	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToDHAttackTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Two_Handed != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetDist::Close == _eTDist)
	{
		if (Enum_TargetAngle::Side == _eTAngle)
		{
			return Enum_LothricKn_State::DH_Swing_Att;
		}
	}

	if (true == TargetRangeCmp(_eTDist, Enum_TargetDist::Melee))
	{
		if (Enum_TargetAngle::Front == _eTAngle)
		{
			const int iChance = ContentsRandom::RandomInt(1, 3);
			if (1 == iChance)
			{
				return Enum_LothricKn_State::DH_Stab_Att;
			}
		}
	}

	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToGAttackTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Guarding != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetDist::Melee == _eTDist)
	{
		if (Enum_TargetAngle::Front == _eTAngle)
		{
			const int iChance = ContentsRandom::RandomInt(0, 5);
			if (0 == iChance)
			{
				return Enum_LothricKn_State::G_Att_Bash;
			}
		}
	}

	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToDodgeTable() const
{
	Enum_TargetAngle eTargetRot = Monster_LothricKn::GetTargetAngle_e();
	Enum_TargetDist eTargetDist = Monster_LothricKn::GetTargetDistance_e();

	Enum_LothricKn_State FindState = GetStateToDodgeTable(eTargetDist, eTargetRot);
	return FindState;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToDodgeTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	switch (CombatState)
	{
	case Monster_LothricKn::eCombatState::Normal:
		return GetStateToNormalDodgeTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::Two_Handed:
		return GetStateToDHDodgeTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::Guarding:
		return GetStateToGDodgeTable(_eTDist, _eTAngle);
	case Monster_LothricKn::eCombatState::None:
	default:
		break;
	}

	MsgBoxAssert("해당 상태를 초기화해주지 않았습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToNormalDodgeTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Normal != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetAngle::Back == _eTAngle)
	{
		if (Enum_RotDir::Left == BaseActor::GetRotDir_e())
		{
			return Enum_LothricKn_State::L_TurnTwice;
		}

		if (Enum_RotDir::Right == BaseActor::GetRotDir_e())
		{
			return Enum_LothricKn_State::R_TurnTwice;
		}
	}

	if (Enum_TargetDist::Long == _eTDist)
	{
		if (Enum_TargetAngle::Side == _eTAngle)
		{
			if (Enum_RotDir::Left == BaseActor::GetRotDir_e())
			{
				return Enum_LothricKn_State::L_Turn;
			}

			if (Enum_RotDir::Right == BaseActor::GetRotDir_e())
			{
				return Enum_LothricKn_State::R_Turn;
			}
		}
	}

	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToDHDodgeTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Two_Handed != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	MsgBoxAssert("상태 테이블이 비어있습니다.");
	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToGDodgeTable(Enum_TargetDist _eTDist, Enum_TargetAngle _eTAngle) const
{
	if (eCombatState::Guarding != CombatState)
	{
		MsgBoxAssert("현재 상태로 해당 테이블을 반환할 수 없습니다.");
		return Enum_LothricKn_State::None;
	}

	if (Enum_TargetAngle::Back == _eTAngle)
	{
		if (Enum_RotDir::Left == BaseActor::GetRotDir_e())
		{
			return Enum_LothricKn_State::G_L_TurnTwice;
		}

		if (Enum_RotDir::Right == BaseActor::GetRotDir_e())
		{
			return Enum_LothricKn_State::G_R_TurnTwice;
		}
	}

	if (Enum_TargetDist::Long == _eTDist)
	{
		if (Enum_TargetAngle::Side == _eTAngle)
		{
			if (Enum_RotDir::Left == BaseActor::GetRotDir_e())
			{
				return Enum_LothricKn_State::G_L_Turn;
			}

			if (Enum_RotDir::Right == BaseActor::GetRotDir_e())
			{
				return Enum_LothricKn_State::G_R_Turn;
			}
		}
	}

	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToHitTable()
{
	bool bHit = (true == Hit.IsHit());
	if (bHit)
	{
		Hit.SetHit(false);

		if (true == IsFlag(Enum_ActorFlag::FrontStab))
		{
			return Enum_LothricKn_State::F_Stab;
		}

		if (true == IsFlag(Enum_ActorFlag::BackStab))
		{
			return Enum_LothricKn_State::B_Stab;
		}

		bool DeathCheck = (0 >= Stat.GetHp());
		if (DeathCheck)
		{
			return Enum_LothricKn_State::F_Death;
		}

		if (true == Hit.IsGuardSuccesss())
		{
			Hit.SetGuardSuccesss(false);
			return Enum_LothricKn_State::G_F_Hit_W;
		}

		if (true == IsFlag(Enum_ActorFlag::Guard_Break))
		{
			return Enum_LothricKn_State::G_Break;
		}

		if (true == IsFlag(Enum_ActorFlag::Break_Posture))
		{
			return Enum_LothricKn_State::Break_Down;
		}

		if (true == IsFlag(Enum_ActorFlag::Block_Shield))
		{
			return Enum_LothricKn_State::Block_Shield;
		}

		Enum_DirectionXZ_Quat eDir = Hit.GetHitDir();
		switch (eDir)
		{
		case Enum_DirectionXZ_Quat::F:
			return Enum_LothricKn_State::F_Hit;
		case Enum_DirectionXZ_Quat::R:
			return Enum_LothricKn_State::R_Hit;
		case Enum_DirectionXZ_Quat::B:
			return Enum_LothricKn_State::B_Hit;
		case Enum_DirectionXZ_Quat::L:
			return Enum_LothricKn_State::L_Hit;
		default:
			return Enum_LothricKn_State::F_Hit;
		}
	}

	return Enum_LothricKn_State::None;
}

Enum_LothricKn_State Monster_LothricKn::GetStateToPatrolTable()
{
	const float4 MyPos = Transform.GetWorldPosition();
	const float MyRot = Transform.GetWorldRotationEuler().Y;
	const float4 PathPos = PathObject->GetCurPath();

	float4 FrontVector = float4::VectorRotationToDegY(float4::FORWARD, MyRot);
	float4 TargetVector = PathPos - MyPos;
	TargetVector.Y = 0.0f;
	TargetVector.Normalize();

	const float4 Angle = DirectX::XMVector3AngleBetweenNormalsEst(FrontVector.DirectXVector, TargetVector.DirectXVector);
	const float Degree = Angle.X * GameEngineMath::R2D;
	const float4 CrossResult = float4::Cross3D(FrontVector, TargetVector);

	enum class eDir
	{
		Left,
		Right,
	};

	eDir TurnDir;
	if (CrossResult.Y < 0.0f)
	{
		TurnDir = eDir::Left;
	}
	else
	{
		TurnDir = eDir::Right;
	}

	if (Degree < 45.0f)
	{
		// 직진
		return Enum_LothricKn_State::None;
	}
	else if (Degree >= 45.0f && Degree < 135.f)
	{
		if (eDir::Left == TurnDir)
		{
			return Enum_LothricKn_State::Patrol_L_Turn;
		}
		else
		{
			return Enum_LothricKn_State::Patrol_R_Turn;
		}
	}
	else
	{
		if (eDir::Left == TurnDir)
		{
			return Enum_LothricKn_State::Patrol_L_TurnTwice;
		}
		else
		{
			return Enum_LothricKn_State::Patrol_R_TurnTwice;
		}
	}

	return Enum_LothricKn_State::None;
}

void Monster_LothricKn::RotToPatrolPath(float _DeltaTime, float _fMinSpeed, float _fMaxSpeed, float _DeclinePoint /*= 45.0f*/)
{
	const float4 MyPos = Transform.GetWorldPosition();
	const float MyRot = Transform.GetWorldRotationEuler().Y;
	const float4 PathPos = PathObject->GetCurPath();

	float4 FrontVector = float4::VectorRotationToDegY(float4::FORWARD, MyRot);
	float4 TargetVector = PathPos - MyPos;
	TargetVector.Y = 0.0f;
	TargetVector.Normalize();

	const float4 Angle = DirectX::XMVector3AngleBetweenNormalsEst(FrontVector.DirectXVector, TargetVector.DirectXVector);
	const float Degree = Angle.X * GameEngineMath::R2D;
	const float4 CrossResult = float4::Cross3D(FrontVector, TargetVector);

	float Speed = _fMaxSpeed;

	float fRotDir = 0.0f;
	if (CrossResult.Y < 0.0f)
	{
		fRotDir = -1.0f;
	}
	else
	{
		fRotDir = 1.0f;
	}

	const float fAbsTargetAngle = Degree;
	if (fAbsTargetAngle < _DeclinePoint)
	{
		const float Ratio = fAbsTargetAngle / _DeclinePoint;
		Speed = std::lerp(_fMinSpeed, _fMaxSpeed, Ratio);
	}

	const float RotAngle = fRotDir * Speed * _DeltaTime;

	if (nullptr != Capsule)
	{
		Capsule->AddWorldRotation(float4(0.0f, RotAngle, 0.0f));
	}
}
