#include "PreCompile.h"
#include "GameEngineRenderUnit.h"
#include "GameEngineMesh.h"
#include "GameEngineInputLayOut.h"
#include "GameEngineMaterial.h"
#include "GameEngineRenderTarget.h"
#include "GameEngineShader.h"
#include "GameEngineVertexShader.h"
#include "GameEnginePixelShader.h"
#include "GameEngineGeometryShader.h"
#include "GameEngineRenderer.H"
#include "GameEngineLevel.h"
#include "GameEngineCamera.h"


GameEngineRenderUnit::GameEngineRenderUnit() 
{
}

GameEngineRenderUnit::~GameEngineRenderUnit() 
{
}

void GameEngineRenderUnit::SetText(const std::string& _Font, const std::string& _Text, float _Scale, float4 _Color, FW1_TEXT_FLAG Flag)
{
	Font = GameEngineFont::Find(_Font);

	if (nullptr == Font)
	{
		MsgBoxAssert("로드하지 않는 폰트를 사용하려고 했습니다.");
	}
	

	FontText = _Text;
	FontScale = _Scale;
	FontColor = _Color;
	FontFlag = Flag;
}

void GameEngineRenderUnit::ChangeText(std::string_view _Text)
{
	if (nullptr == Font)
	{
		MsgBoxAssert("존재하지 않는 폰트를 참조하려 했습니다.");
		return;
	}

	FontText = _Text;
}

void GameEngineRenderUnit::ChangeFontScale(float _Size)
{
	if (nullptr == Font)
	{
		MsgBoxAssert("존재하지 않는 폰트를 참조하려 했습니다.");
		return;
	}

	FontScale = _Size;
}

void GameEngineRenderUnit::SetTextColor(const float4& _Color /*= float4::RED*/)
{
	if (nullptr == Font)
	{
		MsgBoxAssert("존재하지 않는 폰트를 참조하려 했습니다.");
		return;
	}

	FontColor = _Color;
}

void GameEngineRenderUnit::SetTextAlpha(float _AlphaValue /*= 0.0f*/)
{
	if (nullptr == Font)
	{
		MsgBoxAssert("존재하지 않는 폰트를 참조하려 했습니다.");
		return;
	}

	FontColor.A = _AlphaValue;
}


void GameEngineRenderUnit::ResSetting()
{
	if (nullptr != Font)
	{
		return;
	}

	Mesh->InputAssembler1();
	Material->VertexShader();
	Material->GeometryShader();
	LayOut->Setting();
	Mesh->InputAssembler2();
	Material->Rasterizer();
	Material->PixelShader();
	Material->Blend();
	Material->DepthStencil();

	ShaderResHelper.AllShaderResourcesSetting();
}

void GameEngineRenderUnit::ResReset()
{
	Material->GeometryShaderReset();
	ShaderResHelper.AllShaderResourcesReset();
}



void GameEngineRenderUnit::Draw()
{
	if (nullptr != Font)
	{
		float4x4 ViewPort;
		float4 Scale = GameEngineCore::MainWindow.GetScale();
		ViewPort.ViewPort(Scale.X, Scale.Y, 0, 0);

		GameEngineActor* Parent = ParentRenderer->GetParent<GameEngineActor>();

		if (nullptr != Parent && ParentRenderer->GetCamera()->GetProjectionType() == EPROJECTIONTYPE::Orthographic)
		{
			float4 ScreenPos = ParentRenderer->Transform.GetWorldPosition();

			ScreenPos *= ParentRenderer->Transform.GetConstTransformDataRef().ViewMatrix;
			ScreenPos *= ParentRenderer->Transform.GetConstTransformDataRef().ProjectionMatrix;
			ScreenPos *= ViewPort;
			// WindowPos
			Font->FontDraw(FontText, FontScale, ScreenPos, FontColor, FontFlag);

			GameEngineCore::GetContext()->GSSetShader(nullptr, nullptr, 0);
		}
		else if (nullptr != Parent && ParentRenderer->GetCamera()->GetProjectionType() == EPROJECTIONTYPE::Perspective)
		{
			float4 ScreenPos = ParentRenderer->Transform.GetWorldPosition();

			ScreenPos *= ParentRenderer->Transform.GetConstTransformDataRef().ViewMatrix;
			ScreenPos *= ParentRenderer->Transform.GetConstTransformDataRef().ProjectionMatrix;


			const float RHW = 1.0f / ScreenPos.W;

			float4 PosInScreenSpace = float4(ScreenPos.X * RHW, ScreenPos.Y * RHW, ScreenPos.Z * RHW, ScreenPos.W);
			const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
			const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

			float4 RayStartViewRectSpace;

			RayStartViewRectSpace.X = NormalizedX * Scale.X;
			RayStartViewRectSpace.Y = NormalizedY * Scale.Y;

			float4 Result = RayStartViewRectSpace + float4(0, 0, 0, 0);


			Font->FontDraw(FontText, FontScale, Result, FontColor, FontFlag);
			GameEngineCore::GetContext()->GSSetShader(nullptr, nullptr, 0);
		}


		return;
	}

	switch (Mode)
	{
	case RenderMode::Indexed:
		Mesh->IndexedDraw();
		break;
	case RenderMode::Instancing:
		Mesh->InstancingDraw(InstancingCount);
		break;
	default:
		break;
	}
}

void GameEngineRenderUnit::SetMesh(std::string_view _Name)
{
	Mesh = GameEngineMesh::Find(_Name);

	SetMesh(Mesh);
}

void GameEngineRenderUnit::SetMaterial(std::string_view _Name)
{
	Material = GameEngineMaterial::Find(_Name);

	SetMaterial(Material);
}


void GameEngineRenderUnit::Camerapushback(RenderPath _RenderPath)
{
	
	//분리
	if (nullptr != ParentRenderer)
	{
		GameEngineCamera* Camera = ParentRenderer->GetCamera();
		int CameraOrder = ParentRenderer->GetCamera()->GetOrder();

		std::shared_ptr<GameEngineRenderUnit> Unit = shared_from_this();

		if (_RenderPath == RenderPath::None)
		{
			//기본 세팅
			if (false == Material->GetPixelShader()->IsDeferred())
			{
				Path = RenderPath::Forward;
				Camera->Units[RenderPath::Forward][Unit->GetOrder()].push_back(Unit);
			}
			else
			{
				Path = RenderPath::Deferred;
				Camera->Units[RenderPath::Deferred][Unit->GetOrder()].push_back(Unit);
			}
		}
		else
		{
			// 강제
			Path = _RenderPath;
			Camera->Units[_RenderPath][Unit->GetOrder()].push_back(Unit);
		}
	}
}

void GameEngineRenderUnit::SetMesh(std::shared_ptr<GameEngineMesh> _Mesh)
{
	Mesh = _Mesh;

	if (nullptr == Mesh)
	{
		MsgBoxAssert("존재하지 않는 매쉬를 세팅하려고 했습니다.");
	}

	if (nullptr == LayOut && nullptr != Material)
	{
		LayOut = std::make_shared<GameEngineInputLayOut>();
		LayOut->ResCreate(Mesh->GetVertexBuffer(), Material->GetVertexShader());
	}
}


void GameEngineRenderUnit::SetMaterial(std::shared_ptr<GameEngineMaterial> _Material)
{
	Material = _Material;

	ShaderResHelper.ResClear();

	if (nullptr == Material)
	{
		MsgBoxAssert("존재하지 않는 머티리얼을 세팅하려고 했습니다.");
	}

	if (nullptr == LayOut && nullptr != Mesh)
	{
		LayOut = std::make_shared<GameEngineInputLayOut>();
		LayOut->ResCreate(Mesh->GetVertexBuffer(), Material->GetVertexShader());
	}

	// 버텍스 쉐이더와 픽셀쉐이더가 다 들어있는 상태다.

	// 랜더러의 쉐이더 리소스 헬퍼에
	// 버텍스와 픽셀쉐이더의 리소스정보들을 복사 받습니다.
	ShaderResHelper.ShaderResCopy(Material->GetVertexShader().get());
	ShaderResHelper.ShaderResCopy(Material->GetGeometryShader().get());
	ShaderResHelper.ShaderResCopy(Material->GetPixelShader().get());

	// 이걸 회사의 약속.

	if (nullptr != ParentRenderer
		&& ShaderResHelper.IsConstantBuffer("TransformData"))
	{
		const TransformData& Data = ParentRenderer->Transform.GetConstTransformDataRef();
		ShaderResHelper.SetConstantBufferLink("TransformData", Data);
	}

	if (nullptr != ParentRenderer
		&& ShaderResHelper.IsConstantBuffer("RenderBaseInfo"))
	{
		ShaderResHelper.SetConstantBufferLink("RenderBaseInfo", ParentRenderer->RenderBaseInfoValue);
	}

	if (nullptr != ParentRenderer
		&& ShaderResHelper.IsConstantBuffer("LightDatas"))
	{
		ShaderResHelper.SetConstantBufferLink("LightDatas", ParentRenderer->GetLevel()->LightDataObject);
	}


}

void GameEngineRenderUnit::ShadowOn()
{
	if (1 == ParentRenderer->RenderBaseInfoValue.IsAnimation)
	{
		std::shared_ptr<GameEngineMaterial> Mat = GameEngineMaterial::Find("ShadowAni");

		if (nullptr == ShadowLayOut && nullptr != Material)
		{
			ShadowLayOut = std::make_shared<GameEngineInputLayOut>();
			ShadowLayOut->ResCreate(Mesh->GetVertexBuffer(), Mat->GetVertexShader());
		}
	}
	else
	{
		std::shared_ptr<GameEngineMaterial> Mat = GameEngineMaterial::Find("ShadowStatic");

		if (nullptr == ShadowLayOut && nullptr != Material)
		{
			ShadowLayOut = std::make_shared<GameEngineInputLayOut>();
			ShadowLayOut->ResCreate(Mesh->GetVertexBuffer(), Mat->GetVertexShader());
		}
	}


}


void GameEngineRenderUnit::Render()
{
	if (nullptr != ParentRenderer)
	{
		if (false == ParentRenderer->IsUpdate() || false == IsUpdate())
		{
			return;
		}
	}

	if (nullptr == Mesh)
	{
		MsgBoxAssert("매쉬가 세팅되지 않았습니다.");
	}

	if (nullptr == Material)
	{
		MsgBoxAssert("머티리얼이 세팅되지 않았습니다.");
	}

	ResSetting();
	Draw();
	ResReset();
}