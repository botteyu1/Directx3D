﻿#include "PreCompile.h"
#include "GameEnginePhysX.h"
#include "GameEnginePhysXLevel.h"

PhysXErrorCallback  GameEnginePhysX::ErrorCallback = {};
physx::PxDefaultAllocator  GameEnginePhysX::DefaultAllocatorCallback = {};
physx::PxFoundation* GameEnginePhysX::Foundation = nullptr;
physx::PxPvd* GameEnginePhysX::Pvd = nullptr;
physx::PxPhysics* GameEnginePhysX::Physics = nullptr;
physx::PxCooking* GameEnginePhysX::Cooking = nullptr;
physx::PxDefaultCpuDispatcher* GameEnginePhysX::CpuDispatcher = nullptr;
physx::PxMaterial* GameEnginePhysX::Material = nullptr;
physx::PxMaterial* GameEnginePhysX::ClimbMaterial = nullptr;

std::map<std::string, physx::PxScene*> GameEnginePhysX::AllLevelScene;
std::set<int> GameEnginePhysX::SkipCollisionPair;

physx::PxFilterFlags PhysXFilterShader(
	physx::PxFilterObjectAttributes attribute0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attribute1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize)
{
	if (physx::PxFilterObjectIsTrigger(attribute0) || physx::PxFilterObjectIsTrigger(attribute1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		return physx::PxFilterFlag::eDEFAULT;
	}

	for (int _Pair : GameEnginePhysX::SkipCollisionPair)
	{
		if ((_Pair & filterData0.word0) && (_Pair & filterData1.word0))
		{
			pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
				physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
				physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;
			return physx::PxFilterFlag::eDEFAULT;
		}
	}

	pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
		physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
		physx::PxPairFlag::eDETECT_DISCRETE_CONTACT |
		physx::PxPairFlag::eSOLVE_CONTACT;

	return  physx::PxFilterFlag::eDEFAULT;
}

GameEnginePhysX::GameEnginePhysX()
{

}

GameEnginePhysX::~GameEnginePhysX()
{

}

void GameEnginePhysX::PhysXInit()
{
	Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, DefaultAllocatorCallback, ErrorCallback);
	
	if (nullptr == Foundation)
	{
		MsgBoxAssert("Foundation 생성에 실패했습니다.");
	}

	bool  RecordMemoryAllocations = true; // 메모리 프로파일 On

#ifdef _DEBUG
	Pvd = PxCreatePvd(*Foundation);

	if (nullptr == Pvd)
	{
		MsgBoxAssert("Pvd 생성에 실패했습니다.");
	}

	// If Use PhysX Visual Debugger use this (NetWork)
	// Default listening port is 5425 
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	
#endif

	// physx::PxTolerancesScale Scale = physx::PxTolerancesScale(); // Default Length = 1(cm), Speed = 10(cm/s)
	physx::PxTolerancesScale Scale = physx::PxTolerancesScale(); // Default Length = 1(cm), Speed = 10(cm/s)
	Scale.speed = 10;
	Scale.length = 100.f;

	Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, Scale, RecordMemoryAllocations, Pvd);

	if (nullptr == Physics)
	{
		MsgBoxAssert("Physics 생성에 실패했습니다.");
	}

	CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(3);

	if (nullptr == CpuDispatcher)
	{
		MsgBoxAssert("CpuDispatcher 생성에 실패했습니다.");
	}

	Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, physx::PxCookingParams(Physics->getTolerancesScale()));
	if (nullptr == Cooking)
	{
		MsgBoxAssert("Cooking 생성에 실패했습니다.");
	}

	Material = Physics->createMaterial(2.f, 2.f, 0.0f);
	ClimbMaterial = Physics->createMaterial(0.f, 0.f, 0.f);

	if (nullptr == Material)
	{
		MsgBoxAssert("DefaultMaterial 생성에 실패했습니다.");
	}
}

physx::PxScene* GameEnginePhysX::CreateLevelScene()
{
	std::string UpperName = GameEngineString::ToUpperReturn(GameEngineCore::GetCurLevel()->GetName());

	if (true == AllLevelScene.contains(UpperName))
	{
		return AllLevelScene[UpperName];
	}

	physx::PxSceneDesc SceneDesc = physx::PxSceneDesc(Physics->getTolerancesScale());

	// Scene Gravity
	SceneDesc.gravity = physx::PxVec3(0.0f, -GRAVITY_FORCE, 0.0f);
	physx::PxVec3 vec = SceneDesc.gravity;

	// FilterShader
	SceneDesc.filterShader = PhysXFilterShader; // 충돌 필터링을 위한 FilterShader
	SceneDesc.flags = physx::PxSceneFlag::eREQUIRE_RW_LOCK;
	// Set Scenes Limit Datas
	physx::PxSceneLimits SceneLimitsData = physx::PxSceneLimits();
	SceneLimitsData.maxNbActors = SCENE_MAX_ACTOR/*UINT32_MAX - 1*/;
	// SceneLimitsData.maxNbBodies = 0/*UINT32_MAX - 1*/;
	// SceneLimitsData.maxNbStaticShapes = 0;
	// SceneLimitsData.maxNbDynamicShapes = 0;
	// SceneLimitsData.maxNbAggregates = 0;
	// SceneLimitsData.maxNbConstraints = 0;
	SceneLimitsData.maxNbRegions = 255; // Maximum 256
	// SceneLimitsData.maxNbBroadPhaseOverlaps = 0;
	
	SceneDesc.limits = SceneLimitsData;

	SceneDesc.cpuDispatcher = CpuDispatcher;

	physx::PxScene* Scene = Physics->createScene(SceneDesc);
	Scene->lockWrite();

#ifdef _DEBUG
	physx::PxPvdSceneClient* PvdClient = Scene->getScenePvdClient();
	if (nullptr != PvdClient)
	{
		PvdClient->setScenePvdFlags(
			physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS |
			physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES |
			physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS);
	}

	Scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);

#endif
	
	AllLevelScene[UpperName] = Scene;
	Scene->unlockWrite();
	return Scene;
}


void GameEnginePhysX::PhysXRelease()
{
	if (nullptr != Cooking)
	{
		Cooking->release();
		Cooking = nullptr;
	}

	if (nullptr != CpuDispatcher)
	{
		CpuDispatcher->release();
		CpuDispatcher = nullptr;
	}

	for (std::pair<const std::string, physx::PxScene*>& _Pair : AllLevelScene)
	{
		if (nullptr != _Pair.second)
		{
			_Pair.second->release();
			_Pair.second = nullptr;
		}
	}

	if (nullptr != Material)
	{
		Material->release();
		Material = nullptr;
	}

	if (nullptr != Physics)
	{
		Physics->release();
		Physics = nullptr;
	}

	if (nullptr != Pvd)
	{
		physx::PxPvdTransport* transport = Pvd->getTransport();
		Pvd->release();
		Pvd = nullptr;

		if (nullptr != transport)
		{
			transport->release();
			transport = nullptr;
		}
	}

	if (nullptr != Foundation)
	{
		Foundation->release();
		Foundation = nullptr;
	}

	for (void* _Data : GameEnginePhysXLevel::AllData)
	{
		free(_Data);
	}
}

physx::PxScene* GameEnginePhysX::FindScene(std::string_view _SceneName)
{
	std::string UpperName = GameEngineString::ToUpperReturn(_SceneName.data());

	if (false == AllLevelScene.contains(UpperName))
	{
		MsgBoxAssert("존재하지 않는 Scene입니다.\nScene Name : " + UpperName);
		return nullptr;
	}

	return AllLevelScene[UpperName];
}

void GameEnginePhysX::PushSkipCollisionPair(int _ArgCount, ...)
{
	va_list Args;
	va_start(Args, _ArgCount);

	int PushValue = -1;
	for (int i = 0; i < _ArgCount; i++)
	{
		if (0 == i)
		{
			PushValue = va_arg(Args, int);
		}
		else
		{
			PushValue |= va_arg(Args, int);
		}
	}
	SkipCollisionPair.insert(PushValue);
	va_end(Args);
	return;
}

void GameEnginePhysX::PopSkipCollisionPair(int _ArgCount, ...)
{
	va_list Args;
	va_start(Args, _ArgCount);

	int PopValue = -1;
	for (int i = 0; i < _ArgCount; i++)
	{
		if (0 == i)
		{
			PopValue = va_arg(Args, int);
		}
		else
		{
			PopValue |= va_arg(Args, int);
		}
	}

	if (true == SkipCollisionPair.contains(PopValue))
	{
		SkipCollisionPair.erase(PopValue);
	}
	va_end(Args);
	return;
}
