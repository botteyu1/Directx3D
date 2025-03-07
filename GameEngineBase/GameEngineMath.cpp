#include "PreCompile.h"
#include "GameEngineMath.h"


const float4 float4::WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
const float4 float4::RED = {1.0f, 0.0f, 0.0f, 1.0f};
const float4 float4::GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
const float4 float4::BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };
const float4 float4::BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };

const float4 float4::ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
const float4 float4::ONENULL = { 1.0f, 1.0f, 1.0f, 0.0f };
const float4 float4::ZERO = { 0.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::ZERONULL = { 0.0f, 0.0f, 0.0f, 0.0f };

const float4 float4::LEFT = { -1.0f, 0.0f, 0.0f, 0.0f };
const float4 float4::RIGHT = { 1.0f, 0.0f, 0.0f, 0.0f };
const float4 float4::UP = { 0.0f, 1.0f, 0.0f, 0.0f };
const float4 float4::DOWN = { 0.0f, -1.0f, 0.0f, 0.0f };
const float4 float4::FORWARD = { 0.0f, 0.0f, 1.0f, 0.0f };
const float4 float4::BACKWARD = { 0.0f, 0.0f, -1.0f, 0.0f };

const float4x4 float4x4::Iden;

const float GameEngineMath::PI = 3.14159265358979323846264338327950288419716939937510f;
const float GameEngineMath::PI2 = PI * 2.0f;

// Radian To Degree, Degree To Radian
const float GameEngineMath::R2D = 180.0f / GameEngineMath::PI;
const float GameEngineMath::D2R = GameEngineMath::PI / 180.0f;


float4 float4::operator*(const float4x4& _Other) const
{
	float4 Result;
	const float4& A = *this;
	const float4x4& B = _Other;

	//Result.Arr2D[0][0] = (A.Arr2D[0][0] * B.Arr2D[0][0]) + (A.Arr2D[0][1] * B.Arr2D[1][0]) + (A.Arr2D[0][2] * B.Arr2D[2][0]) + (A.Arr2D[0][3] * B.Arr2D[3][0]);
	//Result.Arr2D[0][1] = (A.Arr2D[0][0] * B.Arr2D[0][1]) + (A.Arr2D[0][1] * B.Arr2D[1][1]) + (A.Arr2D[0][2] * B.Arr2D[2][1]) + (A.Arr2D[0][3] * B.Arr2D[3][1]);
	//Result.Arr2D[0][2] = (A.Arr2D[0][0] * B.Arr2D[0][2]) + (A.Arr2D[0][1] * B.Arr2D[1][2]) + (A.Arr2D[0][2] * B.Arr2D[2][2]) + (A.Arr2D[0][3] * B.Arr2D[3][2]);
	//Result.Arr2D[0][3] = (A.Arr2D[0][0] * B.Arr2D[0][3]) + (A.Arr2D[0][1] * B.Arr2D[1][3]) + (A.Arr2D[0][2] * B.Arr2D[2][3]) + (A.Arr2D[0][3] * B.Arr2D[3][3]);

	Result = DirectX::XMVector4Transform(A.DirectXVector, B.DirectXMatrix);

	return Result;
}

float4& float4::operator*=(const class float4x4& _Other)
{
	float4 Result = operator*(_Other);
	*this = Result;
	return *this;
}

float4 float4::VectorRotationToRadX(const float4& _Value, const float _Rad)
{
	float4x4 Rot;
	Rot.RotationXRad(_Rad);
	return _Value * Rot;
}

float4 float4::VectorRotationToRadY(const float4& _Value, const float _Rad)
{
	float4x4 Rot;
	Rot.RotationYRad(_Rad);
	return _Value * Rot;
}

float4 float4::VectorRotationToRadZ(const float4& _Value, const float _Rad)
{
	float4x4 Rot;
	Rot.RotationZRad(_Rad);
	return _Value * Rot;
}


float4 float4::MatrixToQuaternion(const class float4x4& M)
{
	float4 Return;

	if (M.ArrVector[0].IsNearlyZero() || M.ArrVector[1].IsNearlyZero() || M.ArrVector[2].IsNearlyZero())
	{
		Return.X = 0.0f;
		Return.Y = 0.0f;
		Return.Z = 0.0f;
		Return.W = 1.0f;
		return Return;
	}

	float	s;

	// Check diagonal (trace)
	const float tr = M.Arr2D[0][0] + M.Arr2D[1][1] + M.Arr2D[2][2];

	if (tr > 0.0f)
	{
		float InvS = InvSqrt(tr + 1.f);
		Return.W = 0.5f * (1.f / InvS);
		s = 0.5f * InvS;

		Return.X = (M.Arr2D[1][2] - M.Arr2D[2][1]) * s;
		Return.Y = (M.Arr2D[2][0] - M.Arr2D[0][2]) * s;
		Return.Z = (M.Arr2D[0][1] - M.Arr2D[1][0]) * s;
	}
	else
	{
		// diagonal is negative
		int i = 0;

		if (M.Arr2D[1][1] > M.Arr2D[0][0])
			i = 1;

		if (M.Arr2D[2][2] > M.Arr2D[i][i])
			i = 2;

		static const int nxt[3] = { 1, 2, 0 };
		const int j = nxt[i];
		const int k = nxt[j];

		s = M.Arr2D[i][i] - M.Arr2D[j][j] - M.Arr2D[k][k] + 1.0f;

		float InvS = InvSqrt(s);

		float qt[4];
		qt[i] = 0.5f * (1.f / InvS);

		s = 0.5f * InvS;

		qt[3] = (M.Arr2D[j][k] - M.Arr2D[k][j]) * s;
		qt[j] = (M.Arr2D[i][j] + M.Arr2D[j][i]) * s;
		qt[k] = (M.Arr2D[i][k] + M.Arr2D[k][i]) * s;

		Return.X = qt[0];
		Return.Y = qt[1];
		Return.Z = qt[2];
		Return.W = qt[3];
	}

	return Return;
}

float4x4 float4::QuaternionToMatrix()
{
	return DirectX::XMMatrixRotationQuaternion(DirectXVector);
}

float4 float4::QuaternionMulQuaternion(const float4& _Quaternion)
{
	return DirectX::XMQuaternionMultiply(DirectXVector, _Quaternion.DirectXVector);
}