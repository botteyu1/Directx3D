#include "PreCompile.h"
#include "GameEngineFBXAnimation.h"
#include <GameEngineBase/GameEngineFile.h>

GameEngineFBXAnimation::GameEngineFBXAnimation() 
{
}

GameEngineFBXAnimation::~GameEngineFBXAnimation() 
{
}

std::shared_ptr<GameEngineFBXAnimation> GameEngineFBXAnimation::Load(std::string_view& _Path, std::string_view _Name)
{
	std::shared_ptr<GameEngineFBXAnimation> Res = CreateRes(_Name);
	Res->SetPath(_Path);
	return Res;
}


void GameEngineFBXAnimation::AnimationMatrixLoad(std::shared_ptr <GameEngineFBXMesh> _Mesh, const std::string_view& _Name, int _AnimationIndex)
{
	if (0 != AnimationDatas.size())
	{
		return;
	}

	GameEngineFile File = GameEngineFile(GetPath());
	File.ChangeExtension("AnimationFBX");

	if (true == File.IsExits())
	{
		File.Open(FileOpenType::Read, FileDataType::Binary);
		GameEngineSerializer Ser;
		File.DataAllRead(Ser);
		Ser >> AnimationDatas;
		return;
	}

	Initialize();

	if (0 == AnimationDatas.size())
	{
		MsgBoxAssert("애니메이션 데이터가 존재하지 않는 매쉬로 애니메이션을 만들려고 했습니다");
		return;
	}

	if (0 != AnimationDatas[_AnimationIndex].AniFrameData.size())
	{
		// 행렬 계산까지 다 끝났다.
		// 이미 로드가 끝난 녀석이야.
		return;
	}

	// 본 개수만큼 애니메이션 행렬을 확장한다.
	AnimationDatas[_AnimationIndex].AniFrameData.resize(_Mesh->GetBoneCount());

	if (nullptr == _Mesh->RootNode)
	{
		// ???이게 
		// 나중에 해야할거 같다. 
		// MsgBoxAssert("FBX 본매쉬가 로드가 안된 상태입니다.");
		GameEngineFile File = _Mesh->GetPath();
		std::string FolderPath = File.GetFolderPath();
		_Mesh->FBXInit(FolderPath + "\\" + _Mesh->FBXMeshName);
	}

	ProcessAnimationLoad(_Mesh, _Mesh->RootNode, _AnimationIndex);

	ProcessAnimationCheckState(_Mesh, _AnimationIndex);

	// 이게 졸라 느리다.
	GameEngineSerializer Ser;
	Ser << AnimationDatas;

	File.Open(FileOpenType::Write, FileDataType::Binary);
	File.Write(Ser);

}


void GameEngineFBXAnimation::ProcessAnimationCheckState(std::shared_ptr <GameEngineFBXMesh> _Fbx, int userAniDataIndex)
{
	// 뛴다
	FbxExAniData& userAniData = AnimationDatas.at(userAniDataIndex);
	fbxsdk::FbxLongLong fbxTime = userAniData.EndTime.Get() - userAniData.StartTime.Get() + 1;

	// 애니메이션 정보가 비어있는 녀석등은 보통 offset이라고 하는 T
	// 
	// 몸통
	size_t aniFrameDataSize = userAniData.AniFrameData.size();
	for (size_t aniFrameDataIndex = 0; aniFrameDataIndex < aniFrameDataSize; ++aniFrameDataIndex)
	{
		FbxExBoneFrame& aniFrameData = userAniData.AniFrameData.at(aniFrameDataIndex);
		// 비어있을때.
		if (0 == aniFrameData.BoneMatData.size())
		{
			aniFrameData.BoneMatData.resize(fbxTime);
			Bone& curBone = _Fbx->AllBones[aniFrameDataIndex];
			aniFrameData.BoneIndex = curBone.Index;
			aniFrameData.BoneParentIndex = curBone.ParentIndex;
			if (-1 != curBone.ParentIndex)
			{
				FbxExBoneFrame& parentAniFrameData = userAniData.AniFrameData.at(curBone.ParentIndex);
				for (fbxsdk::FbxLongLong start = 0; start < fbxTime; ++start)
				{
					aniFrameData.BoneMatData[start].Time = parentAniFrameData.BoneMatData[start].Time;
					aniFrameData.BoneMatData[start].LocalAnimation = float4x4ToFbxAMatrix(curBone.BonePos.Local);
					aniFrameData.BoneMatData[start].GlobalAnimation = parentAniFrameData.BoneMatData[start].GlobalAnimation * aniFrameData.BoneMatData[start].LocalAnimation;
					aniFrameData.BoneMatData[start].FrameMat = FbxMatTofloat4x4(aniFrameData.BoneMatData[start].GlobalAnimation);
					aniFrameData.BoneMatData[start].S = FbxVecTofloat4(aniFrameData.BoneMatData[start].GlobalAnimation.GetS());
					aniFrameData.BoneMatData[start].Q = FbxQuaternionTofloat4(aniFrameData.BoneMatData[start].GlobalAnimation.GetQ());
					aniFrameData.BoneMatData[start].T = FbxVecToTransform(aniFrameData.BoneMatData[start].GlobalAnimation.GetT());
				}
			}
			else
			{
				for (fbxsdk::FbxLongLong start = 0; start < fbxTime; ++start)
				{
					aniFrameData.BoneMatData[start].Time = 0;
					aniFrameData.BoneMatData[start].LocalAnimation = float4x4ToFbxAMatrix(curBone.BonePos.Local);
					aniFrameData.BoneMatData[start].GlobalAnimation = aniFrameData.BoneMatData[start].LocalAnimation;
					aniFrameData.BoneMatData[start].FrameMat = FbxMatTofloat4x4(aniFrameData.BoneMatData[start].GlobalAnimation);
					aniFrameData.BoneMatData[start].S = FbxVecTofloat4(aniFrameData.BoneMatData[start].GlobalAnimation.GetS());
					aniFrameData.BoneMatData[start].Q = FbxQuaternionTofloat4(aniFrameData.BoneMatData[start].GlobalAnimation.GetQ());
					aniFrameData.BoneMatData[start].T = FbxVecToTransform(aniFrameData.BoneMatData[start].GlobalAnimation.GetT());
				}
			}
		}
	}
}

void GameEngineFBXAnimation::ProcessAnimationLoad(std::shared_ptr <GameEngineFBXMesh> _Mesh, fbxsdk::FbxNode* pNode, int _index)
{
	{
		// 모든 노드를 다 훑기 위해서

		fbxsdk::FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
		if (nullptr != pNodeAttribute)
		{
			fbxsdk::FbxNodeAttribute::EType typetype = pNodeAttribute->GetAttributeType();
			switch (pNodeAttribute->GetAttributeType())
			{
			case fbxsdk::FbxNodeAttribute::eLODGroup:
			{
				if (0 != pNode->GetChildCount())
				{
					ProcessAnimationLoad(_Mesh, pNode->GetChild(0), _index);
					return;
				}
			}
			break;
			case fbxsdk::FbxNodeAttribute::eMesh:
			{
				AnimationLoad(_Mesh, pNode, _index);
			}
			break;
			default:
				break;
			}
		}

		for (int n = 0; n < pNode->GetChildCount(); ++n)
		{
			ProcessAnimationLoad(_Mesh, pNode->GetChild(n), _index);
		}
	}
}

fbxsdk::FbxAMatrix GameEngineFBXAnimation::GetGeometryTransformation(fbxsdk::FbxNode* pNode)
{
	fbxsdk::FbxVector4 translation = pNode->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
	fbxsdk::FbxVector4 rotation = pNode->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	fbxsdk::FbxVector4 scale = pNode->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);

	return fbxsdk::FbxAMatrix(translation, rotation, scale);
}


bool GameEngineFBXAnimation::AnimationLoad(std::shared_ptr <GameEngineFBXMesh> _Mesh, fbxsdk::FbxNode* _Node, int AnimationIndex)
{
	FbxAnimStack* stack = Scene->GetSrcObject<FbxAnimStack>(AnimationIndex);
	// 이 씬의 애니메이션을 이 스택의 애니메이션으로 지정해준다.
	Scene->SetCurrentAnimationStack(stack);

	fbxsdk::FbxMesh* pCurrMesh = _Node->GetMesh();
	int deformerCount = pCurrMesh->GetDeformerCount();
	fbxsdk::FbxAMatrix geometryTransform = GetGeometryTransformation(_Node);

	// Dark Souls 3 geometry
	fbxsdk::FbxAMatrix D3AniTransform;
	float4 Degree = float4(0.f, 0.f, 90.f, 0.f);
	float4 Quat = Degree.EulerDegToQuaternion();
	D3AniTransform.SetQ(FbxQuaternion(Quat.X, Quat.Y, Quat.Z, Quat.W));
	D3AniTransform.SetS(FbxVector4(-1.0, 1.0, 1.0));

	geometryTransform *= D3AniTransform;

	fbxsdk::FbxTakeInfo* takeInfo = Scene->GetTakeInfo(AnimationDatas[AnimationIndex].AniName.c_str());
	fbxsdk::FbxTime start = takeInfo->mReferenceTimeSpan.GetStart();
	fbxsdk::FbxTime end = takeInfo->mReferenceTimeSpan.GetStop();
	fbxsdk::FbxTime::EMode timeMode = Scene->GetGlobalSettings().GetTimeMode();

	fbxsdk::FbxAMatrix currentTransformOffset;
	fbxsdk::FbxAMatrix localTransform;
	fbxsdk::FbxAMatrix globalTransform;

	fbxsdk::FbxCluster* pCurrCluster;
	std::string currJointName;
	Bone* pBone;

	fbxsdk::FbxAMatrix transformMatrix;
	fbxsdk::FbxAMatrix transformLinkMatrix;
	fbxsdk::FbxAMatrix globalBindposeInverseMatrix;

	fbxsdk::FbxLongLong fixIndex;

	std::string linkName;

	fbxsdk::FbxLongLong endTime;
	fbxsdk::FbxLongLong startTime;

	fbxsdk::FbxTime currTime;


	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		fbxsdk::FbxSkin* pCurrSkin = reinterpret_cast<fbxsdk::FbxSkin*>(pCurrMesh->GetDeformer(deformerIndex, fbxsdk::FbxDeformer::eSkin));
		if (nullptr == pCurrSkin)
		{
			continue;
		}

		unsigned int numOfClusters = pCurrSkin->GetClusterCount();

		endTime = end.GetFrameCount(timeMode);
		startTime = start.GetFrameCount(timeMode);

		FbxExAniData& CurAniData = AnimationDatas[AnimationIndex];
		CurAniData.EndTime = endTime;
		CurAniData.StartTime = startTime;
		CurAniData.TimeMode = timeMode;

		if (0 == CurAniData.AniFrameData.size())
		{
			return false;
		}

		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			pCurrCluster = pCurrSkin->GetCluster(clusterIndex);
			currJointName = pCurrCluster->GetLink()->GetName();
			pBone = _Mesh->FindBone(currJointName);

			pCurrCluster->GetTransformMatrix(transformMatrix);
			pCurrCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse();

			linkName = pCurrCluster->GetLink()->GetName();
			fbxsdk::FbxNode* pLinkNode = Scene->FindNodeByName(linkName.c_str());

			FbxExBoneFrame& Frame = CurAniData.AniFrameData[pBone->Index];
			Frame.BoneMatData.resize(endTime - startTime + 1);
			Frame.BoneIndex = pBone->Index;
			Frame.BoneParentIndex = pBone->ParentIndex;

			if (nullptr == pLinkNode)
			{
				continue;
			}

			for (fbxsdk::FbxLongLong i = startTime; i <= endTime; ++i)
			{
				fixIndex = i - startTime;

				FbxExBoneFrameData& FrameData = Frame.BoneMatData[fixIndex];

				currTime.SetFrame(fixIndex, timeMode);
				// 로
				currentTransformOffset = _Node->EvaluateGlobalTransform(currTime) * JointMatrix * geometryTransform;
				// 시간을 넣어주면 그때의 본의 행렬을 가져와 준다.
				// 커브 
				globalTransform = currentTransformOffset.Inverse() * pLinkNode->EvaluateGlobalTransform(currTime);

				localTransform.SetS(pLinkNode->EvaluateLocalScaling(currTime));
				localTransform.SetR(pLinkNode->EvaluateLocalRotation(currTime));
				localTransform.SetT(pLinkNode->EvaluateLocalTranslation(currTime));

				FrameData.Time = currTime.GetSecondDouble();
				FrameData.LocalAnimation = localTransform;
				FrameData.GlobalAnimation = globalTransform;
				FrameData.FrameMat = FbxMatTofloat4x4(FrameData.GlobalAnimation);
				FrameData.S = FbxVecTofloat4(FrameData.GlobalAnimation.GetS());
				FrameData.Q = FbxQuaternionTofloat4(FrameData.GlobalAnimation.GetQ());
				FrameData.T = FbxVecToTransform(FrameData.GlobalAnimation.GetT());
			}
		}
	}

	if (0 != deformerCount)
	{
		return true;
	}

	// return;

	{
		endTime = end.GetFrameCount(timeMode);
		startTime = start.GetFrameCount(timeMode);

		FbxExAniData& CurAniData = AnimationDatas[AnimationIndex];
		CurAniData.EndTime = endTime;
		CurAniData.StartTime = startTime;
		CurAniData.TimeMode = timeMode;


		std::vector<FBXNodeInfo> ALLNODE = CheckAllNode();


		_Mesh->AllBones;


		fbxsdk::FbxNode* pLinkNode = Scene->FindNodeByName(linkName.c_str());



		// ALLNODE[0].Node->GetAnimationEvaluator();

		//for (size_t i = 0; i < ALLNODE.size(); i++)
		//{

			//FbxAnimEvaluator* AnimEvaluator = ALLNODE[i].Node->GetAnimationEvaluator();

			//// ALLNODE[i].Node->GetAnimationInterval()

			//if (nullptr == AnimEvaluator)
			//{
			//	continue;
			//}

		for (size_t boneIndex = 0; boneIndex < CurAniData.AniFrameData.size(); boneIndex++)
		{
			FbxExBoneFrame& Frame = CurAniData.AniFrameData[boneIndex];
			Frame.BoneMatData.resize(endTime - startTime + 1);
			Frame.BoneIndex = static_cast<int>(boneIndex);
			Frame.BoneParentIndex = static_cast<int>(boneIndex);

			for (fbxsdk::FbxLongLong j = startTime; j <= endTime; ++j)
			{
				fixIndex = j - startTime;
				
				FbxExBoneFrameData& FrameData = Frame.BoneMatData[fixIndex];

				currTime.SetFrame(fixIndex, timeMode);
				// 로
				currentTransformOffset = _Node->EvaluateGlobalTransform(currTime) * JointMatrix * geometryTransform;
				// 시간을 넣어주면 그때의 본의 행렬을 가져와 준다.
				// 커브 

				fbxsdk::FbxNode* Node = _Node;

				globalTransform = currentTransformOffset.Inverse() * Node->EvaluateGlobalTransform(currTime);

				localTransform.SetS(Node->EvaluateLocalScaling(currTime));
				localTransform.SetR(Node->EvaluateLocalRotation(currTime));
				localTransform.SetT(Node->EvaluateLocalTranslation(currTime));

				FrameData.Time = currTime.GetSecondDouble();
				FrameData.LocalAnimation = localTransform;
				FrameData.GlobalAnimation = globalTransform;
				FrameData.FrameMat = FbxMatTofloat4x4(FrameData.GlobalAnimation);
				FrameData.S = FbxVecTofloat4(FrameData.GlobalAnimation.GetS());
				FrameData.Q = FbxQuaternionTofloat4(FrameData.GlobalAnimation.GetQ());
				FrameData.T = FbxVecToTransform(FrameData.GlobalAnimation.GetT());
			}
		}

		//}


		int a = 0;

	}




	return false;
}

void GameEngineFBXAnimation::Initialize()
{
	FBXInit(GetPath());
	CheckAnimation();
}

bool GameEngineFBXAnimation::CheckAnimation()
{
	std::string_view Path = GetPath();

	fbxsdk::FbxArray<FbxString*> AniNameArray;
	Scene->FillAnimStackNameArray(AniNameArray);

	if (0 == AniNameArray.Size())
	{
		return false;
	}

	AnimationDatas.resize(AniNameArray.Size());

	for (unsigned int i = 0; i < AnimationDatas.size(); i++)
	{
		AnimationDatas[i].AniName = GameEngineString::UTF8ToAnsi(AniNameArray[i]->Buffer());

		FbxTakeInfo* TakeInfo = Scene->GetTakeInfo(AnimationDatas[i].AniName.c_str());
		AnimationDatas[i].StartTime = TakeInfo->mReferenceTimeSpan.GetStart();
		AnimationDatas[i].EndTime = TakeInfo->mReferenceTimeSpan.GetStop();
		AnimationDatas[i].TimeMode = Scene->GetGlobalSettings().GetTimeMode();
		AnimationDatas[i].TimeStartCount = AnimationDatas[i].StartTime.GetFrameCount(AnimationDatas[i].TimeMode);

		if (0 >= AnimationDatas[i].TimeStartCount)
		{
			AnimationDatas[i].TimeStartCount *= (FbxLongLong)-1;
		}

		AnimationDatas[i].TimeEndCount = AnimationDatas[i].EndTime.GetFrameCount(AnimationDatas[i].TimeMode);
		AnimationDatas[i].FrameCount = AnimationDatas[i].TimeEndCount - AnimationDatas[i].TimeStartCount;

		AnimationDatas[i].FbxModeCount = (long long)fbxsdk::FbxTime::GetFrameRate(AnimationDatas[i].TimeMode);
		AnimationDatas[i].FbxModeRate = (double)fbxsdk::FbxTime::GetFrameRate(AnimationDatas[i].TimeMode);
	}

	for (int i = 0; i < AniNameArray.Size(); i++)
	{
		delete AniNameArray[i];
	}

	return true;
}