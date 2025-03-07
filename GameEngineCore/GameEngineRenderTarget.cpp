#include "PreCompile.h"
#include "GameEngineRenderTarget.h"
#include "GameEngineCore.h"

bool GameEngineRenderTarget::IsDepth = true;

GameEngineRenderUnit GameEngineRenderTarget::MergeUnit;

void GameEngineRenderTarget::RenderTargetReset()
{
	ID3D11RenderTargetView* ArrRenderTarget[MAX_RENDER_TARGET_SETTING_COUNT] = {nullptr,};

	GameEngineCore::GetContext()->OMSetRenderTargets(MAX_RENDER_TARGET_SETTING_COUNT, ArrRenderTarget, nullptr);
	// GameEngineCore::GetContext()->RSSetViewports(static_cast<UINT>(ViewPorts.size()), &ViewPorts[0]);
}

void GameEngineRenderTarget::MergeRenderUnitInit()
{
	GameEngineRenderTarget::MergeUnit.SetMesh("FullRect");
	GameEngineRenderTarget::MergeUnit.SetMaterial("TargetMerge");
	
};

GameEngineRenderTarget::GameEngineRenderTarget() 
{
}

GameEngineRenderTarget::~GameEngineRenderTarget() 
{
}


// 다이렉트에서 무슨일을 하건 우리는 결국 device와 컨텍스트에
void GameEngineRenderTarget::Clear()
{
	for (size_t i = 0; i < RTV.size(); i++)
	{
		GameEngineCore::GetContext()->ClearRenderTargetView(RTV[i], ClearColor[i].Arr1D);
	}

	ID3D11DepthStencilView* DSV = DepthTexture != nullptr ? DepthTexture->GetDSV() : nullptr;

	if (nullptr != DSV)
	{
		GameEngineCore::GetContext()->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH , 1.0f, 0);
	}
}

void GameEngineRenderTarget::Clear(int _num)
{
	GameEngineCore::GetContext()->ClearRenderTargetView(RTV[_num], ClearColor[_num].Arr1D);
}

void GameEngineRenderTarget::Setting()
{
	RenderTargetReset();

	ID3D11DepthStencilView* DSV = DepthTexture != nullptr ? DepthTexture->GetDSV() : nullptr;

	/*if (0 >=  RTV.size())
	{
		MsgBoxAssert("만들어지지 않은 랜더타겟을 세팅하려고 했습니다.");
		return;
	}*/

	/*if (false == IsDepth)
	{
		DSV = nullptr;
	}*/

	// 8개 이상 넣으면 터진다.

	if (RTV.size() == 0)
	{
		GameEngineCore::GetContext()->OMSetRenderTargets(0, nullptr, DSV);
	}
	else
	{
		GameEngineCore::GetContext()->OMSetRenderTargets(static_cast<UINT>(RTV.size()), &RTV[0], DSV);
	}

	
	GameEngineCore::GetContext()->RSSetViewports(static_cast<UINT>(ViewPorts.size()), &ViewPorts[0]);
}

void GameEngineRenderTarget::AddNewTexture(DXGI_FORMAT _Format, float4 _Scale, float4 _ClearColor)
{
	D3D11_TEXTURE2D_DESC Desc = { 0 };
	Desc.ArraySize = 1;
	Desc.Width = _Scale.uiX();
	Desc.Height = _Scale.uiY();
	Desc.Format = _Format;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.MipLevels = 1;
	// 디폴트
	// 이 텍스처의 메모리를 그래픽카드가 가지게 해라.
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	// 용도 랜더타겟으로 사용하기 위해서 만든다.
	// D3D11_BIND_SHADER_RESOURCE 텍스처로 쉐이더에 세팅할수 있게 하기 위해서 만든다.

	std::shared_ptr<GameEngineTexture> Tex = GameEngineTexture::Create(Desc);

	AddNewTexture(Tex, _ClearColor);
}

void GameEngineRenderTarget::AddNewTexture(std::shared_ptr<GameEngineTexture> _Texture, float4 _ClearColor)
{
	std::shared_ptr<GameEngineTexture> Tex = _Texture;

	Textures.push_back(Tex);

	// 뷰포트도 생성
	D3D11_VIEWPORT ViewPortData;
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = static_cast<float>(_Texture->GetScale().uiX());
	ViewPortData.Height = static_cast<float>(_Texture->GetScale().uiY());
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;

	RTV.push_back(Tex->GetRTV());
	SRV.push_back(Tex->GetSRV());
	ClearColor.push_back(_ClearColor);
	ViewPorts.push_back(ViewPortData);

}

void GameEngineRenderTarget::CreateViewports(float4 _Size)

{	// 뷰포트도 생성
	D3D11_VIEWPORT ViewPortData;
	ViewPortData.TopLeftX = 0;
	ViewPortData.TopLeftY = 0;
	ViewPortData.Width = static_cast<float>(_Size.uiX());
	ViewPortData.Height = static_cast<float>(_Size.uiY());
	ViewPortData.MinDepth = 0.0f;
	ViewPortData.MaxDepth = 1.0f;

	ViewPorts.push_back(ViewPortData);

}

void GameEngineRenderTarget::CreateDepthTexture(int _Index/* = 0*/)
{
	// 텍스처를 직접 만드는 첫번째 작업
	D3D11_TEXTURE2D_DESC Desc = {0,};

	// 텍스처를 3차원으로 만들것이냐인데.
	Desc.ArraySize = 1;

	Desc.Width = Textures[_Index]->GetScale().uiX();
	Desc.Height = Textures[_Index]->GetScale().uiY();

	// 0~부터 1사이의 float이기 때문에. 
	// 1바이트를 남기고 24비트 float을 만듭니다.
	// 남은 8비트를 스텐실이라는 것으로 사용하는데 내일 설명드리겠습니다.
	Desc.Format = DXGI_FORMAT_R32_TYPELESS;

	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	Desc.MipLevels = 1;
	// 그래픽카드에 넣어놔라.
	Desc.Usage = D3D11_USAGE_DEFAULT;

	Desc.CPUAccessFlags = 0;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	DepthTexture = GameEngineTexture::Create(Desc);
}

void GameEngineRenderTarget::CreateDepthTexture(float4 _Size)
{
	// 텍스처를 직접 만드는 첫번째 작업
	D3D11_TEXTURE2D_DESC Desc = { 0, };

	// 텍스처를 3차원으로 만들것이냐인데.
	Desc.ArraySize = 1;

	Desc.Width = _Size.uiX();
	Desc.Height = _Size.uiY();

	// 0~부터 1사이의 float이기 때문에. 
	// 1바이트를 남기고 24비트 float을 만듭니다.
	// 남은 8비트를 스텐실이라는 것으로 사용하는데 내일 설명드리겠습니다.
	Desc.Format = DXGI_FORMAT_R32_TYPELESS;

	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	Desc.MipLevels = 1;
	// 그래픽카드에 넣어놔라.
	Desc.Usage = D3D11_USAGE_DEFAULT;

	Desc.CPUAccessFlags = 0;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	DepthTexture = GameEngineTexture::Create(Desc);
}

void GameEngineRenderTarget::Copy(unsigned int ThisTarget, std::shared_ptr<GameEngineRenderTarget> _Target, unsigned int _CopyTarget)
{
	Clear();
	Merge(ThisTarget, _Target, _CopyTarget);
}

void GameEngineRenderTarget::Merge(unsigned int ThisTarget, std::shared_ptr<GameEngineRenderTarget> _Target, unsigned int _CopyTarget)
{
	Setting();

	

	MergeUnit.ShaderResHelper.SetTexture("DiffuseTex", _Target->Textures[_CopyTarget]);
	MergeUnit.ShaderResHelper.SetSampler("DiffuseTexSampler", "POINT");
	MergeUnit.Render();
	MergeUnit.ShaderResHelper.AllShaderResourcesReset();
}

void GameEngineRenderTarget::PostEffect(float _DeltaTime)
{
	RenderTargetReset();

	for (std::shared_ptr<Effect>& Effect : Effects)
	{
		if (false == Effect->IsUpdate())
		{
			continue;
		}

		Effect->RenderBaseInfoValue.DeltaTime = _DeltaTime;
		Effect->RenderBaseInfoValue.AccDeltaTime += _DeltaTime;
		Effect->EffectProcess(_DeltaTime);
	}
}

void GameEngineRenderTarget::EffectInit(Effect* _Effect)
{
	_Effect->EffectTarget = this;
	_Effect->Start();
}

std::shared_ptr<GameEngineRenderTarget> GameEngineRenderTarget::CreateChildRenderTarget(std::vector<int> _Index)
{
	std::shared_ptr<GameEngineRenderTarget> NewRenderTarget = std::make_shared<GameEngineRenderTarget>();

	for (size_t i = 0; i < _Index.size(); i++)
	{
		NewRenderTarget->RTV.push_back(RTV[_Index[i]]);
		NewRenderTarget->SRV.push_back(SRV[_Index[i]]);
		NewRenderTarget->ClearColor.push_back(ClearColor[_Index[i]]);
		NewRenderTarget->ViewPorts.push_back(ViewPorts[_Index[i]]);
	}

	return NewRenderTarget;
}

void GameEngineRenderTarget::EffectUpdate(float _DeltaTime, std::shared_ptr<GameEngineCamera> _Camera)
{
	for (std::shared_ptr<Effect> Effect : Effects)
	{
		Effect->Update(_DeltaTime, _Camera);
	}
}
