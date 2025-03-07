#include "Transform.fx"

// 이녀석은 뭐냐?
struct GameEngineVertex2D
{
    float4 POSITION : POSITION;
    // SV가 붙은 시맨틱들은 인풋레이아웃으로 잡을 필요도 없어요.
    uint iVertex : SV_VertexID;
};


struct PixelOutPut
{
    float4 POSITION : SV_POSITION;
};

PixelOutPut DebugLine_VS(GameEngineVertex2D _Input)
{
    // 쉐이더 문법 모두 0인 자료형으로 초기화 하는것
    PixelOutPut Result = (PixelOutPut) 0;
    float4x4 ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
    
    if (_Input.iVertex == 0)
    {
        Result.POSITION = mul(LocalPosition, ViewProjectionMatrix);
    }
    else if (_Input.iVertex == 1)
    {
        Result.POSITION = mul(LocalScale, ViewProjectionMatrix);
    }
    
    
    
    return Result;
}

cbuffer DebugColor : register(b0)
{
    float4 Color;
}

float4 DebugLine_PS(PixelOutPut _Input) : SV_Target0
{
    return Color;
}