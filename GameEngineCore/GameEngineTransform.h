#pragma once
#include <GameEngineBase/GameEngineMath.h>
#include <list>
#include "EngineEnum.h"

// 기하구조를 표현하고
// 부모자식관계를 처리한다.

enum class ColType
{
	// 캡슐
	// 2D에서의 충돌은 모두가 한축이 같아야 한다.
	// 우리는 충돌이 2D가 더 느려요.
	LINE2D, // z를 0으로 만들고 충돌 구 50 60개를 돌릴수가 있다.
	SPHERE2D, // z를 0으로 만들고 충돌 구 50 60개를 돌릴수가 있다.
	AABBBOX2D, // z를 0으로 만들고 충돌 Axis-Aligned Bounding 회전하지 않은 박스
	OBBBOX2D, // z를 0으로 만들고 충돌 Oriented Bounding Box 회전한 박스 <= 을 1번할 연산량으로
	LINE3D, // z를 0으로 만들고 충돌 구 50 60개를 돌릴수가 있다.
	SPHERE3D,
	AABBBOX3D,
	OBBBOX3D,
	MAX,
};


class CollisionData
{
public:
	union
	{
		// 다이렉트 x에서 지원해주는 충돌용 도형
		DirectX::BoundingSphere SPHERE;
		DirectX::BoundingBox AABB;
		DirectX::BoundingOrientedBox OBB;
	};

	CollisionData()
		: OBB()
	{

	}
};

class GameEngineTransform;
class CollisionParameter
{
public:
	CollisionData& Left;
	CollisionData& Right;
	ColType LeftType = ColType::AABBBOX2D;
	ColType RightType = ColType::AABBBOX2D;

	inline int GetLeftTypeToInt() const
	{
		return static_cast<int>(LeftType);
	}

	inline int GetRightTypeToInt() const
	{
		return static_cast<int>(RightType);
	}

	CollisionParameter(
		CollisionData& _Left,
		CollisionData& _Right,
		ColType _LeftType = ColType::AABBBOX2D,
		ColType _RightType = ColType::AABBBOX2D
	) 
		: 
		Left(_Left),
		Right(_Right),
		LeftType(_LeftType),
		RightType(_RightType)
	{

	}
};

//
//cbuffer TransformData : register(b0)
//{
//	float4 Scale;
//	float4 Rotation;
//	float4 Quaternion;
//	float4 Position;
//
//	float4 LocalScale;
//	float4 LocalRotation;
//	float4 LocalQuaternion;
//	float4 LocalPosition;
//
//	float4 WorldScale;
//	float4 WorldRotation;
//	float4 WorldQuaternion;
//	float4 WorldPosition;
//
//	float4x4 ScaleMatrix; // 크
//	float4x4 RotationMatrix; // 자
//	float4x4 PositionMatrix; // 이
//	float4x4 RevolutionMatrix; // 공
//	float4x4 ParentMatrix; // 공
//
//	float4x4 LocalWorldMatrix;
//	// 월드 공간
//	float4x4 WorldMatrix;
//
//
//	float4x4 ViewMatrix;
//	float4x4 ProjectionMatrix;
//	float4x4 ViewPort;
//
//	// 로컬 => 월드 => 뷰 => 프로젝션 
//	float4x4 WorldViewProjectionMatrix;
//};


// 왜 굳이. 
class TransformData 
{
public:
	// w가 0일때와 1일때의 차이를 잘 기억해놓자.

	//float4 Scale = float4::ONENULL;
	//float4 Rotation = float4::ZERONULL;
	//float4 Quaternion = float4::ZERO;
	//float4 Position = float4::ZERO;
	
	// 이걸 직접 수정하는 일은 없을겁니다.
	float4 LocalScale;
	float4 LocalRotation;
	float4 LocalQuaternion;
	float4 LocalPosition;

	float4 WorldScale;
	float4 WorldRotation;
	float4 WorldQuaternion;
	float4 WorldPosition;

	float4x4 ScaleMatrix; // 크
	float4x4 RotationMatrix; // 자
	float4x4 PositionMatrix; // 이
	float4x4 RevolutionMatrix; // 공
	float4x4 ParentMatrix; // 공

	float4x4 LocalWorldMatrix;
	// 월드 공간
	float4x4 WorldMatrix;


	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 ViewPort;
	float4x4 WorldViewMatrix;
	// 로컬 => 월드 => 뷰 => 프로젝션 
	float4x4 WorldViewProjectionMatrix;

	void WorldDecompos() 
	{
		WorldMatrix.Decompose(WorldScale, WorldQuaternion, WorldPosition);
		WorldRotation = WorldQuaternion.QuaternionToEulerDeg();
	}

	void LocalDecompos()
	{
		LocalWorldMatrix.Decompose(LocalScale, LocalQuaternion, LocalPosition);
		LocalRotation = LocalQuaternion.QuaternionToEulerDeg();
	}

	void LocalCalculation(const float4x4& _Parent)
	{
		//ScaleMatrix.Scale(LocalScale);
		//RotationMatrix.RotationDeg(LocalRotation);
		//PositionMatrix.Position(LocalPosition);

		// 짐벌락 해결
		LocalQuaternion = LocalRotation.EulerDegToQuaternion();

		LocalWorldMatrix.Compose(LocalScale, LocalQuaternion, LocalPosition);
		WorldMatrix = LocalWorldMatrix * _Parent;
	}

	void WorldViewProjectionCalculation()
	{
		WorldViewMatrix = WorldMatrix * ViewMatrix;
		WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
	}

	void operator=(const TransformData& _Other)
	{
		memcpy_s(this, sizeof(TransformData), &_Other, sizeof(TransformData));
	}

	TransformData()
	{

	}

	TransformData(const TransformData& _Other)
	{
		memcpy_s(this, sizeof(TransformData), &_Other, sizeof(TransformData));
	}
};

// 설명 :
class GameEngineTransform : public DebugObject
{
public:
	// constrcuter destructer
	GameEngineTransform();
	~GameEngineTransform();

	// delete Function
	//GameEngineTransform(const GameEngineTransform& _Other) = delete;
	//GameEngineTransform(GameEngineTransform&& _Other) noexcept = delete;
	//GameEngineTransform& operator=(const GameEngineTransform& _Other) = delete;
	//GameEngineTransform& operator=(GameEngineTransform&& _Other) noexcept = delete;

	static bool TriToRay(const float4& _Origin, const float4& _Dir, const float4& _TriV0, const float4& _TriV1, const float4& _TriV2, float& _Dis);

	void OrthographicLH(float _Width, float _Height, float _Near, float _Far)
	{
		TransData.ProjectionMatrix.OrthographicLH(_Width, _Height, _Near, _Far);
	}

	void PerspectiveFovLHDeg(float _FovAngle, float _Width, float _Height, float _Near, float _Far)
	{
		TransData.ProjectionMatrix.PerspectiveFovLHDeg(_FovAngle, _Width, _Height, _Near, _Far);
	}

	void LookToLH(const float4& _EyePos, const float4& _EyeDir, const float4& _EyeUp)
	{
		TransData.ViewMatrix.LookToLH(_EyePos, _EyeDir, _EyeUp);
	}

	const TransformData& GetConstTransformDataRef()
	{
		return TransData;
	}

	void SetLocalScale(const float4& _Value)
	{
		TransData.LocalScale = _Value;
		TransformUpdate();
	}

	void AddLocalScale(const float4& _Value)
	{
		TransData.LocalScale += _Value;
		TransformUpdate();
	}

	void SetLocalRotation(const float4& _Value)
	{
		TransData.LocalRotation = _Value;
		TransformUpdate();
	}

	void AddLocalRotation(const float4& _Value)
	{
		TransData.LocalRotation += _Value;
		TransformUpdate();
	}

	void SetLocalPosition(const float4& _Value)
	{
		TransData.LocalPosition = _Value;
		TransformUpdate();
	}

	void SetLocalMatrix(const float4x4& _Value)
	{
		_Value.Decompose(TransData.LocalScale, TransData.LocalRotation, TransData.LocalPosition);
		TransformUpdate();
	}

	void AddLocalPosition(const float4& _Value)
	{
		TransData.LocalPosition += _Value;
		TransformUpdate();
	}

	bool AbsoluteScale = false;
	bool AbsolutePosition = false;
	bool AbsoluteRotation = false;

	void SetWorldScale(const float4& _Value)
	{
		AbsoluteScale = true;
		TransData.WorldScale = _Value;
		TransformUpdate();
	}

	void SetWorldRotation(const float4& _Value)
	{
		AbsoluteRotation = true;
		TransData.WorldRotation = _Value;
		TransformUpdate();
	}

	void SetWorldPosition(const float4& _Value)
	{
		AbsolutePosition = true;
		TransData.WorldPosition = _Value;
		TransformUpdate();
	}

	void SetWorld(const float4& _S, const float4& _R, const float4& _T)
	{
		AbsoluteScale = true;
		AbsoluteRotation = true;
		AbsolutePosition = true;
		TransData.WorldScale = _S;
		TransData.WorldRotation = _R;
		TransData.WorldPosition = _T;
		TransformUpdate();
	}

	void AddWorldScale(const float4& _Value)
	{
		SetWorldScale(GetWorldScale() + _Value);
	}

	void AddWorldRotation(const float4& _Value)
	{
		SetWorldRotation(GetWorldRotationEuler() + _Value);
	}

	void AddWorldPosition(const float4& _Value)
	{
		SetWorldPosition(GetWorldPosition() + _Value);
	}

	float4 GetWorldScale() const
	{
		return TransData.WorldScale;
	}

	float4 GetWorldRotationEuler() const
	{
		return TransData.WorldRotation;
	}

	float4 GetWorldPosition() const
	{
		return TransData.WorldPosition;
	}

	float4 GetWorldQuaternion() const
	{
		return TransData.WorldQuaternion;
	}

	float4 GetLocalScale() const
	{
		return TransData.LocalScale;
	}

	float4 GetLocalRotationEuler() const
	{
		return TransData.LocalRotation;
	}

	float4 GetLocalPosition() const
	{
		return TransData.LocalPosition;
	}

	// 회전 그 자체로 한 오브젝트의 앞 위 오른쪽
	// [1][0][0][0] 오른쪽
	// [0][1][0][0] 위
	// [0][0][1][0] 앞
	// [0][0][0][1]

	float4 GetWorldForwardVector() const
	{
		return TransData.WorldMatrix.ArrVector[2].NormalizeReturn();
	}

	float4 GetWorldBackVector() const
	{
		return -(TransData.WorldMatrix.ArrVector[2].NormalizeReturn());
	}

	float4 GetWorldRightVector() const
	{
		return TransData.WorldMatrix.ArrVector[0].NormalizeReturn();
	}

	float4 GetWorldLeftVector() const
	{
		return -(TransData.WorldMatrix.ArrVector[0].NormalizeReturn());
	}

	float4 GetWorldUpVector() const
	{
		return TransData.WorldMatrix.ArrVector[1].NormalizeReturn();
	}

	float4 GetWorldDownVector() const
	{
		return TransData.WorldMatrix.ArrVector[1].NormalizeReturn();
	}

	const float4x4& GetWorldMatrix() const
	{
		return TransData.WorldMatrix;
	}

	float4 GetLocalForwardVector() const
	{
		return TransData.LocalWorldMatrix.ArrVector[2].NormalizeReturn();
	}

	float4 GetLocalBackVector() const
	{
		return -(TransData.LocalWorldMatrix.ArrVector[2].NormalizeReturn());
	}

	float4 GetLocalRightVector() const
	{
		return TransData.LocalWorldMatrix.ArrVector[0].NormalizeReturn();
	}

	float4 GetLocalLeftVector() const
	{
		return -(TransData.LocalWorldMatrix.ArrVector[0].NormalizeReturn());
	}

	float4 GetLocalUpVector() const
	{
		return TransData.LocalWorldMatrix.ArrVector[1].NormalizeReturn();
	}

	float4 GetLocalDownVector() const
	{
		return TransData.LocalWorldMatrix.ArrVector[1].NormalizeReturn();
	}

	void CalculationViewAndProjection(const TransformData& _Transform);

	void CalculationViewAndProjection(const float4x4& _View, const float4x4& _Projection);

	void TransformUpdate();

	void SetParent(GameEngineTransform& _Parent)
	{
		if (nullptr != Parent)
		{
			Parent->Childs.remove(this);
		}

		Parent = &_Parent;

		if (nullptr == Parent)
		{
			MsgBoxAssert("부모가 존재하지 않는 트랜스폼이 존재합니다.");
		}

		TransData.LocalWorldMatrix = TransData.WorldMatrix * Parent->TransData.WorldMatrix.InverseReturn();

		TransData.LocalDecompos();

		Parent->Childs.push_back(this);

		AbsoluteReset();

		TransformUpdate();
	}

	void AbsoluteReset() 
	{
		AbsoluteScale = AbsoluteRotation = AbsolutePosition = false;
	}

	void SetTransformData(const TransformData& Data)
	{
		TransData = Data;
	}

	void CalChilds();

	float4x4 GetWorldViewProjectionMatrix()
	{
		return TransData.WorldViewProjectionMatrix;
	}

	// 트랜스폼은 충돌 타입이 정해져 있지 않는다.
	//                    내가 사각형이고            날                           상대는 구               상대
	static bool Collision(const CollisionParameter& _Data);

	CollisionData ColData;
protected:

private:

	GameEngineTransform* Parent = nullptr;
	std::list<GameEngineTransform*> Childs;
	TransformData TransData;

};


