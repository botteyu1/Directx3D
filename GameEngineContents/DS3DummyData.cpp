#include "PreCompile.h" 
#include "DS3DummyData.h"

#include "GameEngineCore/GameEngineFBXMesh.h"

#include "BaseActor.h"

DS3DummyData::DS3DummyData() 
{
}

DS3DummyData::~DS3DummyData() 
{
}

std::string DS3DummyData::GetExtName()
{
	return ".json";
}

std::string DS3DummyData::GetFullExtension()
{
	return ".flver.dummyData.json";
}

std::string DS3DummyData::GetFormatExt()
{
	return "DummyData";
}

std::string DS3DummyData::GetNameToID(int _ID)
{
	if (EMPTY_ID == _ID)
	{
		return std::string();
	}

	std::string IDName = std::string("c") + std::to_string(_ID);
	return IDName;
}

std::string DS3DummyData::GetFilePath(std::string_view _Name)
{
	GameEnginePath path;
	path.MoveParentToExistsChild("ContentsResources");
	path.MoveChild("ContentsResources");
	path.MoveChild("Mesh");
	path.MoveChild(_Name);
	return path.GetStringPath();
}

void DS3DummyData::LoadDummyData(int _ID)
{
	std::string IDName = GetNameToID(_ID);
	std::string Path = GetFilePath(IDName);
	if (Path.empty())
	{
		return;
	}

	GameEngineDirectory Dir(Path);
	std::vector<GameEngineFile> Files = Dir.GetAllFile({ DS3DummyData::GetExtName() });
	for (GameEngineFile& pFile : Files)
	{
		std::string Path = pFile.GetStringPath();
		std::string FullExt = DS3DummyData::GetFullExtension();
		if (std::string::npos != Path.find_last_of(FullExt))
		{
			DS3DummyData::Load(pFile);
		}
	}
}

void DS3DummyData::Load(GameEngineFile& _File)
{
	std::string FullName = _File.GetFileName();
	size_t StartNum = FullName.find_first_of('.');
	std::string Name = GameEngineString::ToUpperReturn(FullName.substr(0, StartNum));

	if (nullptr == Find(Name))
	{
		std::shared_ptr<DS3DummyData> Res = CreateRes(Name);
		Res->Init(_File.GetStringPath());
	}
}

std::map<int, DummyData> DS3DummyData::GetRefAllData(int _RefID) const
{
	std::multimap<int, DummyData>::const_iterator Iter_Lower = DummyDataMap.lower_bound(_RefID);
	std::multimap<int, DummyData>::const_iterator Iter_Upper = DummyDataMap.upper_bound(_RefID);

	std::multimap<int, DummyData>::const_iterator FindIter;

	size_t DataCount = DummyDataMap.count(_RefID);

	std::map<int, DummyData> ReturnDatas;

	for (FindIter = Iter_Lower; FindIter != Iter_Upper; ++FindIter)
	{
		const DummyData& pData = FindIter->second;
		ReturnDatas.insert(std::make_pair(pData.AttachBoneIndex, pData));
	}

	return ReturnDatas;
}

bool DS3DummyData::IsContainData(int _RefID) const
{
	return DummyDataMap.contains(_RefID);
}

const DummyData* DS3DummyData::GetDummyData(int _RefID) const
{
	if (false == IsContainData(_RefID))
	{
		MsgBoxAssert("데이터를 찾을 수 없었습니다.");
		static DummyData* ReturnType = nullptr;
		return ReturnType;
	}

	std::multimap<int, DummyData>::const_iterator FindIter = DummyDataMap.find(_RefID);
	const DummyData* pData = &FindIter->second;
	return pData;
}

const DummyData* DS3DummyData::GetDummyData(int _RefID, int _AttachBoneIndex) const
{
	std::multimap<int, DummyData>::const_iterator Iter_Lower = DummyDataMap.lower_bound(_RefID);
	std::multimap<int, DummyData>::const_iterator Iter_Upper = DummyDataMap.upper_bound(_RefID);

	std::multimap<int, DummyData>::const_iterator FindIter;

	for (FindIter = Iter_Lower; FindIter != Iter_Upper; ++FindIter)
	{
		const DummyData* pData = &FindIter->second;
		if (_AttachBoneIndex != pData->AttachBoneIndex)
		{
			continue;
		}

		return pData;
	}

	MsgBoxAssert("데이터를 찾을 수 없었습니다.");
	static DummyData* ReturnType = nullptr;
	return ReturnType;
}

std::vector<DummyData> DS3DummyData::GetDummyDatas() const
{
	std::vector<DummyData> Datas;
	Datas.reserve(DummyDataMap.size());
	for (const std::pair<int, DummyData>& Pair : DummyDataMap)
	{
		DummyData Data = Pair.second;
		Datas.push_back(Data);
	}

	return Datas;
}

void DS3DummyData::Init(std::string_view _Path)
{
	GameEngineFile File = GameEngineFile(_Path.data());
	std::string FileName = File.GetFileName();
	size_t ExtNum = FileName.find_first_of(".");
	FileName = FileName.substr(0, ExtNum);
	File.MoveParent();
	File.AppendPath(FileName);
	File.ChangeExtension(GetFormatExt());

	if (true == File.IsExits())
	{
		GameEngineSerializer Ser;

		File.Open(FileOpenType::Read, FileDataType::Binary);
		File.DataAllRead(Ser);

		AllDataRead(Ser);
		return;
	}

	DummyInit(_Path);

	GameEngineSerializer Ser;
	AllDataWrite(Ser);

	File.Open(FileOpenType::Write, FileDataType::Binary);
	File.Write(Ser);

	File.Close();
}

void DS3DummyData::DummyInit(std::string_view _Path)
{
	GameEngineFile File = GameEngineFile(_Path.data());
	File.Open(FileOpenType::Read, FileDataType::Text);

	GameEngineSerializer Bin;
	File.DataAllRead(Bin);

	std::string DataString = Bin.GetDataPtr<const char*>();

	std::string UnitStartString("{");
	std::string UnitEndString("\"Unk34\":");

	size_t Offset = 0;
	size_t UnitLen = DataString.find(UnitEndString) - DataString.find_first_of(UnitStartString);
	size_t EndStringLen = UnitEndString.length();
	size_t EndPoint = DataString.find_first_of(']');

	while (true)
	{
		size_t StartNum = DataString.find(UnitStartString, Offset);
		bool bDone = StartNum + UnitLen > EndPoint;
		if (std::string::npos == StartNum || bDone)
		{
			break;
		}

		size_t EndNum = DataString.find(UnitEndString, Offset) + EndStringLen;
		std::string NumberString = std::string(DataString, StartNum, EndNum - StartNum);
		Interpret(NumberString);

		Offset = EndNum;
	}
}

void DS3DummyData::Interpret(std::string_view _Data)
{
	DummyData NewData;

	NewData.Position = InterpretType<float4>(_Data, "Position");
	NewData.Position.X *= -1.0f;
	NewData.Position.Z *= -1.0f;
	NewData.Forward = InterpretType<float4>(_Data, "Forward");
	NewData.Forward.X *= -1.0f;
	NewData.Forward.Z *= -1.0f;
	NewData.Upward = InterpretType<float4>(_Data, "Upward");
	NewData.Upward.X *= -1.0f;
	NewData.Upward.Z *= -1.0f;
	NewData.ReferenceID = InterpretType<int>(_Data, "ReferenceID");
	NewData.ParentBoneIndex = InterpretType<int>(_Data, "ParentBoneIndex");
	NewData.AttachBoneIndex = InterpretType<int>(_Data, "AttachBoneIndex");
	NewData.Flag1 = InterpretType<bool>(_Data, "Flag1");

	SetLocalMatrix(NewData);
	
	CreateData(NewData);
}

void DS3DummyData::SetLocalMatrix(DummyData& _DummyPolyData)
{
	std::string Name = std::string(GetName().data()) + ".fbx";
	const std::shared_ptr<GameEngineFBXMesh>& Mesh = GameEngineFBXMesh::Find(Name);
	if (nullptr == Mesh)
	{
		MsgBoxAssert("메쉬 데이터를 로드하지 않고 사용할 수 없는 기능입니다.");
		return;
	}

	const float4 WorldPos = _DummyPolyData.Position;
	const float4 WorldForward = _DummyPolyData.Forward;
	const float4 WorldUpward = _DummyPolyData.Upward;
	const int AttachIndex = _DummyPolyData.AttachBoneIndex;

	float4 DPStartPos = float4::ZERO;
	float4 DPStartRot = float4::ZERONULL;
	if (-1 != AttachIndex)
	{
		const JointPos& MeshJointData = Mesh->FindBoneToIndex(AttachIndex)->BonePos;
		DPStartPos = MeshJointData.GlobalTranslation;
		DPStartRot = MeshJointData.GlobalRotation;
	}

	float4x4 GlobalDummyPolyMatrix = DirectX::XMMatrixLookAtLH(WorldPos.DirectXVector, (WorldPos + WorldForward).DirectXVector, WorldUpward.DirectXVector);
	float4 WDPS;
	float4 WDPQ;
	float4 WDPT;
	GlobalDummyPolyMatrix.Decompose(WDPS, WDPQ, WDPT);

	float4 mDPS = float4::ONE;
	float4 mDPQ = DPStartRot * DirectX::XMQuaternionInverse(WDPQ.DirectXVector);
	float4 mDPT = -(DPStartPos + WDPT);
	float4 RmDPT = mDPT;
	RmDPT.Z *= -1.0f;
	RmDPT = RmDPT.VectorRotationToDegZReturn(-90.0f);

	float4x4 mDPMat;
	float4x4 RmDPMat;
	float4x4 mAffine;
	mDPMat.Compose(mDPS, mDPQ, mDPT);
	RmDPMat.Compose(mDPS, mDPQ, RmDPT);

	_DummyPolyData.Offset = mDPT;
	_DummyPolyData.Quaternion = WDPQ;
	_DummyPolyData.Local = mDPMat;
	_DummyPolyData.Local_ReversePos = RmDPMat;
	_DummyPolyData.Local_NotPos = mAffine.Affine(mDPS, mDPQ, mDPT);
}

void DS3DummyData::CreateData(const DummyData& _Data)
{
	std::pair Pair = std::make_pair(_Data.ReferenceID, _Data);
	DummyDataMap.insert(Pair);
}

void DS3DummyData::AllDataRead(class GameEngineSerializer& _Ser)
{
	int Size = 0;
	_Ser >> Size;

	for (int i = 0; i < Size; i++)
	{
		DummyData NewData;
		NewData.Read(_Ser);
		CreateData(NewData);
	}
}

void DS3DummyData::AllDataWrite(class GameEngineSerializer& _Ser)
{
	int Size = static_cast<int>(DummyDataMap.size());
	_Ser << Size;

	for (const std::pair<int, DummyData>& Pair : DummyDataMap)
	{
		DummyData Data = Pair.second;
		Data.Write(_Ser);
	}
}