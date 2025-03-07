#include "PreCompile.h"
#include "TestObject_Shader.h"

TestObject_Shader::TestObject_Shader() 
{
}

TestObject_Shader::~TestObject_Shader()
{
}

void TestObject_Shader::Start()
{
	BaseMonster::Start();
	{
		//MainRenderer = CreateComponent<GameEngineFBXRenderer>();
		MainRenderer->SetFBXMesh("c1100.FBX", "FBX_Animation"); // Bone 136
		//MainRenderer->Transform.SetLocalScale({ 50.0f, 50.0f, 50.0f });
		//MainRenderer->Transform.SetLocalRotation({ 0.0f, 0.0f, -90.0f });

		MainRenderer->CreateFBXAnimation("c1100_Idle1", "c1100_Idle1.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Idle2", "c1100_Idle2.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Idle3", "c1100_Idle3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_LH_Fist", "c1100_LH_Fist.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Pray1", "c1100_Pray1.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Pray2", "c1100_Pray2.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Pray3", "c1100_Pray3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_RH_ChargingSting", "c1100_RH_ChargingSting.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_RH_HorizontalSlash", "c1100_RH_HorizontalSlash.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_RH_RunToSting", "c1100_RH_RunToSting.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_RH_Sting", "c1100_RH_Sting.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_RH_TwinSlash", "c1100_RH_TwinSlash.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_RH_VerticalSlash", "c1100_RH_VerticalSlash.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Run", "c1100_Run.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Run3", "c1100_Run3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Scout", "c1100_Scout.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_TH_VerticalSlash", "c1100_TH_VerticalSlash.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Left", "c1100_Turn_Left.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Left_Twice", "c1100_Turn_Left_Twice.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Left_Twice2", "c1100_Turn_Left_Twice2.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Left_Twice3", "c1100_Turn_Left_Twice3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Left2", "c1100_Turn_Left2.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Left3", "c1100_Turn_Left3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Right", "c1100_Turn_Right.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Right_Twice", "c1100_Turn_Right_Twice.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Right_Twice2", "c1100_Turn_Right_Twice2.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Right_Twice3", "c1100_Turn_Right_Twice3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Right2", "c1100_Turn_Right2.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Turn_Right3", "c1100_Turn_Right3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Back", "c1100_Walk_Back.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Back3", "c1100_Walk_Back3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Front", "c1100_Walk_Front.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Front3", "c1100_Walk_Front3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Left", "c1100_Walk_Left.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Left3", "c1100_Walk_Left3.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Right", "c1100_Walk_Right.FBX", { 0.1f, true });
		MainRenderer->CreateFBXAnimation("c1100_Walk_Right3", "c1100_Walk_Right3.FBX", { 0.1f, true });
		MainRenderer->ChangeAnimation("c1100_Idle1");
		MainRenderer->RenderBaseInfoValue.IsShadow = 1;



		

		// MainUI���� ��
		//HpBar = CreateComponent<GameEngineUIRenderer>(30);
		//HpBar->Transform.SetLocalPosition({400, 300});
		//HpBar->SetImageScale({ 3000.0f, 3000.0f });
	}

}

void TestObject_Shader::Update(float _Delta)
{
}

void TestObject_Shader::Release()
{
	BaseMonster::Release();
}

void TestObject_Shader::LevelEnd(GameEngineLevel* _NextLevel)
{
}
