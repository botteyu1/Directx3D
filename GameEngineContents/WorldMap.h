#pragma once
#include <GameEngineCore/GameEngineActor.h>

// ���� :
class WorldMap : public GameEngineActor
{
public:
	// constrcuter destructer
	WorldMap();
	~WorldMap();

	// delete Function
	WorldMap(const WorldMap& _Other) = delete;
	WorldMap(WorldMap&& _Other) noexcept = delete;
	WorldMap& operator=(const WorldMap& _Other) = delete;
	WorldMap& operator=(WorldMap&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _Delta) override;
	void LevelEnd(class GameEngineLevel* _NextLevel) override;


	void SetSoundCol(const float4& _Pos, const float4& _Scale);

private:
	std::shared_ptr<GameContentsFBXRenderer> FBXRenderer;

	std::shared_ptr<class GameEnginePhysXTriMesh> TriMesh;

	std::shared_ptr<GameContentsFBXRenderer> FBXRenderer2;

	std::shared_ptr<class GameEnginePhysXTriMesh> TriMesh2;

	std::shared_ptr<GameContentsFBXRenderer> SkyRenderer;
	std::shared_ptr<GameContentsFBXRenderer> BackGroundRenderer;

	std::shared_ptr<GameEngineCollision>SoundCol;

	GameEngineSoundPlayer MapBGM;
	GameEngineSoundPlayer WindSound0;
	GameEngineSoundPlayer WindSound1;
	GameEngineSoundPlayer WindSound2;
	GameEngineSoundPlayer WindSound3;

	//// �ʵ�����
	//std::shared_ptr<GameEngineFBXMesh> MapMesh;
	//std::vector<class MapData> MapDatas;
	//std::vector<GameEngineTransform> Transdate;

};

