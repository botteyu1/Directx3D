#include "Transform.fx"
#include "RenderBase.fx"
#include "Light.fx"

struct GameEngineVertex2D
{
    float4 POSITION : POSITION;
    float4 NORMAL : NORMAL;
    float4 TEXCOORD : TEXCOORD;
    float4 TANGENT : TANGENT;
    float4 BINORMAL : BINORMAL;
    float4 BLENDWEIGHT : BLENDWEIGHT;
    int4 BLENDINDICES : BLENDINDICES;
};


struct PixelOutPut
{
    // 픽셀쉐이더에 보내느 ㄴ역
    float4 POSITION : SV_Position;
    float4 TEXCOORD : TEXCOORD;
    float4 VIEWPOSITION : POSITION;
    float4 VIEWNORMAL : NORMAL;
    float4 VIEWTANGENT : TANGENT;
    float4 VIEWBINORMAL : BINORMAL;
};

PixelOutPut FBXStaticColorShader_VS(GameEngineVertex2D _Input)
{
    // _Input 0.5 0.5
    
    // 쉐이더 문법 모두 0인 자료형으로 초기화 하는것
    PixelOutPut Result = (PixelOutPut)0;
    
    float4 InputPos = _Input.POSITION;
    InputPos.w = 1.0f;
    float4 InputNormal = _Input.NORMAL;
    InputNormal.w = 0.0f;
    
    Result.VIEWPOSITION = mul(InputPos, WorldViewMatrix);
    // Result.VIEWPOSITION = mul(_Input.NORMAL, WorldMatrix);
    Result.VIEWPOSITION.w = 1.0f;
    
    _Input.NORMAL.w = 0.0f;
    Result.VIEWNORMAL = mul(_Input.NORMAL, WorldViewMatrix);
    // Result.VIEWNORMAL = mul(_Input.NORMAL, WorldMatrix);
    Result.VIEWNORMAL.w = 0.0f;
    
    _Input.TANGENT.w = 0.0f;
    Result.VIEWTANGENT = mul(_Input.TANGENT, WorldViewMatrix);
    // Result.VIEWNORMAL = mul(_Input.NORMAL, WorldMatrix);
    Result.VIEWTANGENT.w = 0.0f;
    
    _Input.BINORMAL.w = 0.0f;
    Result.VIEWBINORMAL = mul(_Input.BINORMAL, WorldViewMatrix);
    // Result.VIEWNORMAL = mul(_Input.NORMAL, WorldMatrix);
    Result.VIEWBINORMAL.w = 0.0f;
    
    Result.POSITION = mul(InputPos, WorldViewProjectionMatrix);
    
    Result.TEXCOORD = _Input.TEXCOORD;
    return Result;
}

// b버퍼 14
// t텍스처 
// s샘플러 
// 둘의 슬롯은 공유되지 않는다.

// 보통 샘플러와 텍스처는 1쌍으로 사용되기 때문에
// 샘플러는 텍스처의 색상을 가져올때 보간을 해주거나 특수한 공식등을 내부에서
// 사용해주는 용도가 있다.

// 우리 규칙

struct PixelOut
{
    float4 Color0 : SV_Target0;
    float4 Color1 : SV_Target1;
    float4 Color2 : SV_Target1;
    float4 Color3 : SV_Target1;
    float4 Color4 : SV_Target1;
    float4 Color5 : SV_Target1;
    float4 Color6 : SV_Target1;
    float4 Color7 : SV_Target1;
};

// SV_Target0
// SV_Target1
// SV_Target2
// SV_Target3
// SV_Target4

Texture2D NormalTexture : register(t2);
SamplerState NormalTextureSAMPLER : register(s2);

PixelOut FBXStaticColorShader_PS(PixelOutPut _Input) : SV_Target0
{
    PixelOut Result = (PixelOut)0.0f;
    float4 Color = BaseColor;
    Color.a = 1;
    
    if (0 != IsNormal)
    {
        _Input.VIEWNORMAL = NormalTexCalculate(NormalTexture, NormalTextureSAMPLER, _Input.TEXCOORD, _Input.VIEWTANGENT, _Input.VIEWBINORMAL, _Input.VIEWNORMAL);
    }
    
        float4 DiffuseRatio = (float4) 0.0f;
    float4 SpacularRatio = (float4) 0.0f;
    float4 AmbientRatio = (float4) 0.0f;

    if (1 == IsLight)
    {
        for (int i = 0; i < LightCount; ++i)
        {
            DiffuseRatio += CalDiffuseLight(_Input.VIEWNORMAL, AllLight[i]);
            SpacularRatio += CalSpacularLight(_Input.VIEWPOSITION, _Input.VIEWNORMAL, AllLight[i]);
            AmbientRatio += CalAmbientLight(AllLight[i]);
        }
        
        float A = Color.w;
        Color.xyz = Color.xyz * (DiffuseRatio.xyz + SpacularRatio.xyz + AmbientRatio.xyz);
        Color.a = A;
    }
    
    // 버텍스 쉐이더에서 계산하면
    // 퐁쉐이딩.
    
    if (0 < Target0)
    {
        Result.Color0 = Color;
    }
    if (0 < Target1)
    {
        Result.Color1 = Color;
    }
    if (0 < Target2)
    {
        Result.Color2 = Color;
    }
    if (0 < Target3)
    {
        Result.Color3 = Color;
    }
    
    return Result;
}