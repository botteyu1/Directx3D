#include "MeshBase_Header.fx"


struct DeferrdOut
{
    float4 DifColor : SV_Target1;
    float4 PosColor : SV_Target2;
    float4 NorColor : SV_Target3; 
    float4 SpcColor : SV_Target4;
    float4 MatColor : SV_Target5;
    //float4 WorldPosColor : SV_Target6;
};


Texture2D DiffuseTexture : register(t0);
SamplerState DiffuseTextureSampler : register(s0);
Texture2D NormalTexture : register(t2);
SamplerState NormalTextureSAMPLER : register(s2);
Texture2D SpecularTexture : register(t3);
SamplerState SpecularTextureSAMPLER : register(s3);

void Mesh_PS_Update(inout PixelOutPut _Input, inout DeferrdOut _Result)
{
    float4 Color = DiffuseTexture.Sample(DiffuseTextureSampler, _Input.TEXCOORD.xy);
    
    
    if (AlphaValue > 0.0f || AlphaValue > 1.0f)
    {
        if (AlphaValue >= Color.a)
        {
            clip(-1);
        }
    }
     
    Color.w = 1.0f;
    
    _Result.DifColor = Color;
    _Result.DifColor.xyz *= DiffuseTexMult;
    _Result.PosColor = _Input.VIEWPOSITION;
    //_Result.WorldPosColor = _Input.WorldPOSITION;
    
    
    _Result.SpcColor = float4(SpecularTexture.Sample(SpecularTextureSAMPLER, _Input.TEXCOORD.xy).rgb, 1.0f);
  
    _Result.NorColor = -NormalTexCalculate(NormalTexture, NormalTextureSAMPLER, _Input.TEXCOORD, _Input.VIEWTANGENT, _Input.VIEWBINORMAL, _Input.VIEWNORMAL);
    _Result.NorColor.w = 1.f;
    
    float Rough = 1.0f - ((_Result.SpcColor.x + _Result.SpcColor.y + _Result.SpcColor.z) / 3.0f);
    
    if (Roughness < 0.0f)
    {
        _Result.MatColor.x = Rough; //Roughness
    }
    else
    {
        _Result.MatColor.x = Roughness;
    }
    
    if (Metalic < 0.0f)
    {
        _Result.MatColor.y = 1.0f - Rough; //Metalic
    }
    else
    {
        _Result.MatColor.y = Metalic;
    }
    _Result.MatColor.w = 1.0f;
    
    
    
     
    //_Result.NorColor.w = 1.0f;
    
}