﻿#include "PreCompile.h"
#include "GameEnginePhysXComponent.h"
#include "GameEnginePhysXLevel.h"

GameEnginePhysXComponent::GameEnginePhysXComponent()
{

}

GameEnginePhysXComponent::~GameEnginePhysXComponent()
{

}

void GameEnginePhysXComponent::LevelStart(GameEngineLevel* _PrevLevel)
{

}

void GameEnginePhysXComponent::LevelEnd(GameEngineLevel* _NextLevel)
{

}

void GameEnginePhysXComponent::Start()
{
	CurPhysXLevel = dynamic_cast<GameEnginePhysXLevel*>(GetLevel());
	if (nullptr == CurPhysXLevel)
	{
		MsgBoxAssert("PhysX 객체는 PhysXLevel에서만 생성할 수 있습니다.");
		return;
	}

	Scene = CurPhysXLevel->GetScene();
	ParentActor = GetActor();
}

void GameEnginePhysXComponent::Update(float _Delta)
{
	Positioning(_Delta);
}

void GameEnginePhysXComponent::Release()
{
	if (nullptr != Shape)
	{
		Shape->release();
		Shape = nullptr;
	}

	if (nullptr != ComponentActor)
	{
		Scene->lockWrite();
		ComponentActor->release();
		ComponentActor = nullptr;
		Scene->unlockWrite();
	}
}

/*
physx::PxForceMode
eFORCE == unit of mass * distance/ time^2, i.e. a force
eIMPULSE  == unit of mass * distance /time
eVELOCITY_CHANGE  == unit of distance / time, i.e. the effect is mass independent: a velocity change. // ignore mass
eACCELERATION  == unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
*/

void GameEnginePhysXComponent::MoveForce(const physx::PxVec3 _Force, bool _IgnoreGravity/* = false*/)
{
	if (false == JudgeDynamic())
	{
		MsgBoxAssert("Dynamic객체만 움직일 수 있습니다.");
	}

	physx::PxRigidDynamic* DynamicActor = ComponentActor->is<physx::PxRigidDynamic>();

	physx::PxVec3 CurLV = physx::PxVec3({ 0.0f });
	if (false == _IgnoreGravity)
	{
		Scene->lockRead();
		CurLV = DynamicActor->getLinearVelocity();
		Scene->unlockRead();
		if (0.f < CurLV.y)
		{
			CurLV.y = 0.f;
		}
	}

	Scene->lockWrite();
	DynamicActor->setLinearVelocity({ _Force.x, _Force.y + CurLV.y, _Force.z }); // 현재 중력을 받아오기 위해
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::AddForce(const physx::PxVec3 _Force)
{
	if (false == JudgeDynamic())
	{
		MsgBoxAssert("Dynamic객체만 움직일 수 있습니다.");
	}
	physx::PxRigidDynamic* DynamicActor = ComponentActor->is<physx::PxRigidDynamic>();

	Scene->lockWrite();
	DynamicActor->addForce(_Force, physx::PxForceMode::eVELOCITY_CHANGE);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::SetWorldPosition(const float4& _Pos)
{
	physx::PxVec3 Pos = { _Pos.X, _Pos.Y , _Pos.Z };

	float4 WorldDeg = Transform.GetWorldRotationEuler();
	float4 WorldQuat = WorldDeg.EulerDegToQuaternion();
	physx::PxQuat Quat = physx::PxQuat(WorldQuat.X, WorldQuat.Y, WorldQuat.Z, WorldQuat.W);

	physx::PxTransform Transform(Pos, Quat);
	Scene->lockWrite();
	ComponentActor->setGlobalPose(Transform);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::SetWorldRotation(const float4& _Degree)
{
	float4 WorldDeg = { _Degree.X, _Degree.Y , _Degree.Z };

	SetDir(WorldDeg.Y);

	Scene->lockRead();
	physx::PxTransform Transform = ComponentActor->getGlobalPose();
	Scene->unlockRead();
	float4 WorldQuat = WorldDeg.EulerDegToQuaternion();
	physx::PxQuat Quat = physx::PxQuat(WorldQuat.X, WorldQuat.Y, WorldQuat.Z, WorldQuat.W);

	Transform.q = Quat;
	Scene->lockWrite();
	ComponentActor->setGlobalPose(Transform);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::AddWorldRotation(const float4& _Degree)
{
	Scene->lockRead();
	physx::PxTransform Transform = ComponentActor->getGlobalPose();
	Scene->unlockRead();
	physx::PxQuat Quat = Transform.q;
	float4 WorldDeg = { Quat.x, Quat.y, Quat.z, Quat.w };
	WorldDeg = WorldDeg.QuaternionToEulerDeg();
	WorldDeg += _Degree;
	if (0 > WorldDeg.X)
	{
		WorldDeg.X += 360.0f;
	}
	if (0 > WorldDeg.Y)
	{
		WorldDeg.Y += 360.0f;
	}
	if (0 > WorldDeg.Z)
	{
		WorldDeg.Z += 360.0f;
	}

	SetDir(WorldDeg.Y);

	WorldDeg = WorldDeg.EulerDegToQuaternion();

	Quat = physx::PxQuat(WorldDeg.X, WorldDeg.Y, WorldDeg.Z, WorldDeg.W);
	Transform.q = Quat;
	Scene->lockWrite();
	ComponentActor->setGlobalPose(Transform);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::ResetMove(int _Axies)
{
	if (false == JudgeDynamic())
	{
		MsgBoxAssert("Dynamic객체만 움직일 수 있습니다.");
	}
	physx::PxRigidDynamic* DynamicActor = ComponentActor->is<physx::PxRigidDynamic>();

	Scene->lockRead();
	physx::PxVec3 CurLV = DynamicActor->getLinearVelocity();
	Scene->unlockRead();

	if (Enum_Axies::X & _Axies)
	{
		CurLV.x = 0.0f;
	}

	if (Enum_Axies::Y & _Axies)
	{
		CurLV.y = 0.0f;
	}

	if (Enum_Axies::Z & _Axies)
	{
		CurLV.z = 0.0f;
	}

	Scene->lockWrite();
	DynamicActor->setLinearVelocity({ CurLV.x, CurLV.y, CurLV.z });
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::SetFiltering(int _CollisionOrder, int _TargetCollisionOrder)
{
	SetFiltering(_CollisionOrder, _TargetCollisionOrder, 0, 0);
}

void GameEnginePhysXComponent::SetFiltering(FilterData& _FilterData)
{
	SetFiltering(_FilterData.Word0, _FilterData.Word1, _FilterData.Word2, _FilterData.Word3);
}

bool GameEnginePhysXComponent::JudgeDynamic()
{
	Scene->lockRead();
	physx::PxType Type = ComponentActor->getConcreteType();
	Scene->unlockRead();

	if (physx::PxConcreteType::eRIGID_DYNAMIC != Type)
	{
		return false;
	}

	return true;
}


void GameEnginePhysXComponent::Positioning(float _Delta)
{
	switch (IsPositioningComponent)
	{
	case 0:
	{
		// Set MyPos to Reflect ParentPos
		const TransformData& LocalTransform = GameEngineObject::Transform.GetConstTransformDataRef();
		SetWorldPosition(LocalTransform.WorldPosition);
		SetWorldRotation(LocalTransform.WorldRotation);
		break;
	}
	default:
	{
		// Set ParentPos to Reflect MyPos
		// Component's Local Transform
		Scene->lockRead();
		physx::PxTransform Transform = ComponentActor->getGlobalPose();
		Scene->unlockRead();
		const TransformData& LocalTransform = GameEngineObject::Transform.GetConstTransformDataRef();

		physx::PxVec3 ComponentPos = Transform.p;
		physx::PxQuat ComponentQuat = Transform.q;

		float4 ParentPos = { ComponentPos.x, ComponentPos.y, ComponentPos.z , 1.0f };
		float4 Degree = float4(ComponentQuat.x, ComponentQuat.y, ComponentQuat.z, ComponentQuat.w).QuaternionToEulerDeg();

		ParentPos -= LocalTransform.LocalPosition; // Component Local Pos

		ParentActor->Transform.SetWorldRotation(Degree);
		ParentActor->Transform.SetWorldPosition(ParentPos);
		break;
	}
	}
}

void GameEnginePhysXComponent::SetFiltering(int _MyCollisionOrder, int _Target1CollisionOrder, int _Target2CollisionOrder, int _Target3CollisionOrder)
{
	if (nullptr == ComponentActor)
	{
		MsgBoxAssert("Component를 Init후 사용해 주세요.");
	}

	physx::PxFilterData FilterData;
	FilterData.word0 = _MyCollisionOrder;
	FilterData.word1 = _Target1CollisionOrder;
	FilterData.word2 = _Target2CollisionOrder;
	FilterData.word3 = _Target3CollisionOrder;

	Scene->lockRead();
	physx::PxU32 ShapeCount = ComponentActor->getNbShapes();

	if (0 >= ShapeCount)
	{
		return;
	}

	physx::PxShape** Shapes = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * ShapeCount);

	ComponentActor->getShapes(Shapes, ShapeCount);
	Scene->unlockRead();

	Scene->lockWrite();
	for (physx::PxU32 i = 0; i < ShapeCount; i++)
	{
		physx::PxShape* CurShape = Shapes[i];
		ComponentActor->detachShape(*CurShape);
		CurShape->setSimulationFilterData(FilterData);
		ComponentActor->attachShape(*CurShape);
	}
	Scene->unlockWrite();

	free(Shapes);
}



void GameEnginePhysXComponent::GravityOn()
{
	Scene->lockWrite();
	ComponentActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::GravityOff()
{
	Scene->lockWrite();
	ComponentActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::RayCastTargetOn()
{ 
	Scene->lockWrite();
	ComponentActor->detachShape(*Shape);
	Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	ComponentActor->attachShape(*Shape);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::RayCastTargetOff()
{
	Scene->lockWrite();
	ComponentActor->detachShape(*Shape);
	Shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
	ComponentActor->attachShape(*Shape);
	Scene->unlockWrite();
}

void GameEnginePhysXComponent::CollisionOn(bool _GravityOn /*= true*/)
{
	Scene->lockWrite();
	ComponentActor->detachShape(*Shape);
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	ComponentActor->attachShape(*Shape);
	Scene->unlockWrite();

	if (true == _GravityOn)
	{
		GravityOn();
	}
}

void GameEnginePhysXComponent::CollisionOff(bool _GravityOff /*= true*/)
{
	Scene->lockWrite();
	ComponentActor->detachShape(*Shape);
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	ComponentActor->attachShape(*Shape);
	Scene->unlockWrite();

	if (true == _GravityOff)
	{
		GravityOff();
	}
}

void GameEnginePhysXComponent::ChangeMaterial(physx::PxMaterial* const* _Material)
{
	if (_Material == Material)
	{
		return;
	}

	Scene->lockWrite();
	ComponentActor->detachShape(*Shape);
	Material = _Material;
	Shape->setMaterials(_Material, 1);
	ComponentActor->attachShape(*Shape);
	Scene->unlockWrite();
}
