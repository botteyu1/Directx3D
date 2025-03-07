#pragma once
#include <GameEngineCore/GameEngineActor.h>
#include "Object_BaseLadder.h"

class Object_Ladder1 : public Object_BaseLadder
{
public:
	// constrcuter destructer
	Object_Ladder1();
	~Object_Ladder1();

	// delete Function
	Object_Ladder1(const Object_Ladder1& _Other) = delete;
	Object_Ladder1(Object_Ladder1&& _Other) noexcept = delete;
	Object_Ladder1& operator=(const Object_Ladder1& _Other) = delete;
	Object_Ladder1& operator=(Object_Ladder1&& _Other) noexcept = delete;

	void CollisionOn();
	void CollisionOff();
	
protected:
	void Start() override;
	void Update(float _Delta) override;

private:
	std::shared_ptr<GameContentsFBXRenderer> FBXRenderer;
	std::shared_ptr<GameEngineCollision> CollisionTop;
	std::shared_ptr<GameEngineCollision> CollisionBottom;
	std::shared_ptr<GameEngineCollision> CollisionFront;
	std::shared_ptr<GameEngineCollision> CollisionBody;
	
};

