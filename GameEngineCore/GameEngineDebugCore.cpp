#include "PreCompile.h"
#include "GameEngineDebugCore.h"
#include "GameEngineRenderUnit.h"
#include "GameEngineLevel.h"

class GameEngineDebugInfo 
{
public:
	float4 Color = float4::RED;
	TransformData Data;
	class GameEngineCamera* Camera;
	GameEngineRenderUnit Unit;

};

std::vector<GameEngineDebugInfo> DebugUnit;
GameEngineLevel* GameEngineDebug::GameEngineDebugCore::CurLevel = nullptr;


void GameEngineDebug::GameEngineDebugCore::DebugInit()
{
	// 이녀석 개수 1개가 뭔가를 1개 그릴수 있다.
	DebugUnit.resize(100);
}

void GameEngineDebug::GameEngineDebugCore::DebugRender()
{
	for (size_t i = 0; i < DebugUnit.size(); i++)
	{
		DebugUnit[i].Unit.ResSetting();
		DebugUnit[i].Unit.Draw();
	}

	DebugUnit.clear();
}

void GameEngineDebug::DrawBox2D(const GameEngineTransform& _Trans, float4 _Color , GameEngineCamera* _Camera)
{
	GameEngineDebug::DrawBox2D(_Trans.GetWorldScale(), _Trans.GetWorldRotationEuler(), _Trans.GetWorldPosition(), _Color, _Camera);
}

void GameEngineDebug::DrawBox2D(float4 _Scale, float4 _Rot, float4 _Pos, float4 _Color , GameEngineCamera* _Camera)
{
	GameEngineDebug::DrawMesh("Rect", _Scale, _Rot, _Pos, _Color, _Camera);
}

void GameEngineDebug::DrawBox3D(const GameEngineTransform& _Trans, float4 _Color , GameEngineCamera* _Camera)
{
	GameEngineDebug::DrawBox3D(_Trans.GetWorldScale(), _Trans.GetWorldRotationEuler(), _Trans.GetWorldPosition(), _Color, _Camera);
}

void GameEngineDebug::DrawBox3D(float4 _Scale, float4 _Rot, float4 _Pos, float4 _Color , GameEngineCamera* _Camera)
{
	GameEngineDebug::DrawMesh("Box", _Scale, _Rot, _Pos, _Color, _Camera);
}

void GameEngineDebug::DrawSphere2D(const GameEngineTransform& _Trans, float4 _Color /*= float4::RED*/, class GameEngineCamera* _Camera /*= nullptr*/)
{
	GameEngineDebug::DrawSphere2D(_Trans.GetWorldScale(), _Trans.GetWorldRotationEuler(), _Trans.GetWorldPosition(), _Color, _Camera);
}
void GameEngineDebug::DrawSphere2D(float4 _Scale, float4 _Rot, float4 _Pos, float4 _Color /*= float4::RED*/, class GameEngineCamera* _Camera /*= nullptr*/)
{
	if (abs(_Scale.Y) >= abs(_Scale.X))
	{
		_Scale.X = _Scale.Y;
	}
	_Scale.Y = _Scale.Z = _Scale.X;
	GameEngineDebug::DrawMesh("Sphere", _Scale, _Rot, _Pos, _Color, _Camera);
}

void GameEngineDebug::DrawMesh(std::string_view _Mesh, float4 _Scale, float4 _Rot, float4 _Pos, float4 _Color /*= float4::RED*/, class GameEngineCamera* _Camera /*= nullptr*/)
{
	if (nullptr == _Camera)
	{
		_Camera = GameEngineDebug::GameEngineDebugCore::CurLevel->GetMainCamera().get();
	}

	if (false == GameEngineLevel::IsDebug)
	{
		return;
	}



	GameEngineDebugInfo& Value = DebugUnit.emplace_back();
	Value.Camera = _Camera;
	Value.Unit.SetMesh(_Mesh);
	Value.Unit.SetMaterial("2DTextureWire");

	Value.Color = _Color;
	Value.Data.LocalScale = _Scale;
	Value.Data.LocalRotation = _Rot;
	Value.Data.LocalPosition = _Pos;
	Value.Data.LocalCalculation(float4x4::Iden);
	Value.Data.ViewMatrix = _Camera->Transform.GetConstTransformDataRef().ViewMatrix;
	Value.Data.ProjectionMatrix = _Camera->Transform.GetConstTransformDataRef().ProjectionMatrix;
	Value.Data.WorldViewProjectionCalculation();

	Value.Unit.ShaderResHelper.SetConstantBufferLink("TransformData", Value.Data);
	Value.Unit.ShaderResHelper.SetConstantBufferLink("DebugColor", Value.Color);
}

void GameEngineDebug::DrawLine(float4 _Start, float4 _End, float4 _Color/* = float4::RED*/, class GameEngineCamera* _Camera/* = nullptr*/)
{
	if (nullptr == _Camera)
	{
		_Camera = GameEngineDebug::GameEngineDebugCore::CurLevel->GetMainCamera().get();
	}

	if (false == GameEngineLevel::IsDebug)
	{
		return;
	}

	GameEngineDebugInfo& Value = DebugUnit.emplace_back();
	Value.Camera = _Camera;
	Value.Unit.SetMesh("Line");
	Value.Unit.SetMaterial("2DDebugLine");

	Value.Data.LocalPosition = _End;
	Value.Data.LocalScale = _Start;

	Value.Color = _Color;
	Value.Data.ViewMatrix = _Camera->Transform.GetConstTransformDataRef().ViewMatrix;
	Value.Data.ProjectionMatrix = _Camera->Transform.GetConstTransformDataRef().ProjectionMatrix;
	Value.Data.WorldViewProjectionCalculation();

	Value.Unit.ShaderResHelper.SetConstantBufferLink("TransformData", Value.Data);
	Value.Unit.ShaderResHelper.SetConstantBufferLink("DebugColor", Value.Color);
}