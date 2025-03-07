
struct LightData
{
    float4x4 LightViewMatrix;
    float4x4 LightViewInverseMatrix;
    float4x4 LightProjectionMatrix;
    float4x4 LightProjectionInverseMatrix;
    float4x4 LightViewProjectionMatrix;
    float4x4 CameraViewInverseMatrix;
    float4 LightColor;
    float4 AmbientLight;
    float4 LightPos;
    float4 LightDir;
    float4 LightRevDir;
    float4 ViewLightPos;
    float4 ViewLightDir;
    float4 ViewLightRevDir;
    float4 CameraPosition;
    float ShadowTargetSizeX;
    float ShadowTargetSizeY;
    float LightNear;
    float LightFar;
    float DifLightPower;
    float SpcLightPower;
    float AmbLightPower;
    float SpcPow;
    int LightType;
    float PointLightRange;
    float LightPower;
    float ForceLightPower;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float Def1;
};

cbuffer LightDatas : register(b12)
{
    int LightCount;
    LightData AllLight[64];
};

cbuffer OneLightData : register(b13)
{
    LightData LightDataValue;
};

float4 CalDiffuseLight(float4 _Normal, LightData _Data)
{
    // 0~1
    float4 ResultRatio = 0.0f;
    
    float4 N = float4(normalize(_Normal.xyz), 0.0f);
    float4 L = float4(normalize(_Data.ViewLightRevDir.xyz), 0.0f);
    
    ResultRatio.xyzw = max(0.0f, dot(N.xyz, L.xyz));
    return ResultRatio * _Data.DifLightPower;
}

float4 CalSpacularLight(float4 _Pos, float4 _Normal, LightData _Data)
{
    // 0~1
    float4 ResultRatio = 0.0f;
    
    float3 N = normalize(_Normal.xyz);
    float3 L = normalize(_Data.ViewLightRevDir.xyz);
    
    float3 ReflectionN = normalize(2.0f * _Normal.xyz * dot(L.xyz, N.xyz) - L.xyz);
    
    float3 EyeL = normalize(_Data.CameraPosition.xyz - _Pos.xyz);
    
    float Result = max(0.0f, dot(ReflectionN.xyz, EyeL.xyz));
    ResultRatio.xyzw = pow(Result, _Data.SpcPow);
    
    return ResultRatio * _Data.SpcLightPower;
}

float4 CalAmbientLight(LightData _Data)
{
    return _Data.AmbientLight /** _Data.LightPower*/ * _Data.AmbLightPower;
}


float4 NormalTexCalculate(Texture2D NormalTex, SamplerState Smp, float4 UV, float4 _Tangent, float4 _BiNormal, float4 _Normal)
{
    float4 TexNormal = NormalTex.Sample(Smp, UV.xy);
    
    // 색상에는 음수색상이라는게 없어서
    // 노말을 계산한걸 텍스처로 만들때
    // +1 * 0.5f를 한다면 을 해버린다.
    // 그걸 원래대로 돌려주는 공식
    TexNormal = TexNormal * 2.0f - 1.0f;
    TexNormal.w = 0.0f;
    TexNormal = normalize(TexNormal);
    // 그냥 텍스처에서 가져온 normal 아직 회전안되었다.
    
    
    float4 Result = (float4) 0.0f;
    
    // 새로운 회전행렬을 만들어 낸다.
    // 탄젠트 스페이스라고 부른다.
    float3x3 TangentSpace = float3x3(_Tangent.xyz, _BiNormal.xyz, _Normal.xyz);
    Result.xyz = mul(TexNormal.xyz, TangentSpace);
    
    return Result;

}