#pragma once
#include <GameEngineCore/GameEngineActor.h>

// 설명 :
class TitleLogo : public GameEngineActor
{
	enum class eLogoState
	{
		None,
		BanDai,
		FromSoft,
		DarkSoul,
		AnyButton,
		EndLogo,
		Done,
	};

	enum class eButtonState
	{
		None,
		ButtonActor,
		Done,
	};

	enum ButtonType
	{
		GameStart,
		GameExit,
	};

public:
	// constructer destructer
	TitleLogo();
	~TitleLogo();

	// delete Function
	TitleLogo(const TitleLogo& _Other) = delete;
	TitleLogo(TitleLogo&& _Other) noexcept = delete;
	TitleLogo& operator = (const TitleLogo& _Other) = delete;
	TitleLogo& operator = (TitleLogo&& _Other) noexcept = delete;

	void Ending();

protected:
	void Start() override;
	void Update(float _Delta) override;
	void Release() override;

	// 로고 State
	void Start_EndLogo(GameEngineState* _State);

	void Update_BanDai(float _Delta, GameEngineState* _State);
	void Update_FromSoft(float _Delta, GameEngineState* _State);
	void Update_DarkSoul(float _Delta, GameEngineState* _State);
	void Update_AnyButton(float _Delta, GameEngineState* _State);
	void Update_EndLogo(float _Delta, GameEngineState* _State);

	void End_BanDai(GameEngineState* _State);
	void End_FromSoft(GameEngineState* _State);
	void End_DarkSoul(GameEngineState* _State);
	void End_AnyButton(GameEngineState* _State);
	void End_EndLogo(GameEngineState* _State);

	void Start_Done(GameEngineState* _State);

	// 버튼 State
	void Start_ButtonActor(GameEngineState* _State);
	void Update_ButtonActor(float _Delta, GameEngineState* _State);
	void End_ButtonActor(GameEngineState* _State);

	// 다크소울로고 이미지 크기변경
	void DarkLogoAddScale(const float4& _Value);
private:
	std::shared_ptr<class GameEngineSpriteRenderer> DarkBack = nullptr;
	std::shared_ptr<class GameEngineSpriteRenderer> BanDaiNamco_Logo = nullptr;
	std::shared_ptr<class GameEngineSpriteRenderer> FromSoft_Logo = nullptr;
	std::shared_ptr<class GameEngineSpriteRenderer> DarkSouls_Logo = nullptr;
	std::shared_ptr<class GameEngineSpriteRenderer> Inc_Logo = nullptr;

	std::shared_ptr<class GameEngineSpriteRenderer> AnyButtonBack = nullptr;
	std::shared_ptr<class GameEngineUIRenderer> AnyButtonFont = nullptr;

	void ButtonCreate();
	void ButtonOperation();
	void ButtonFlash(float _Delta, GameEngineState* _State); // 깜빡이는
	void SkipButton(GameEngineState* _State, eLogoState _StateName); // E
	std::vector<std::shared_ptr<class TitleButton>> Buttons;
	std::shared_ptr<TitleButton> ButtonActor;
	int ButtonCheckNum = 0;

	float FontScale = 20.0f;
	bool AnyBack = false;
	bool AnyBackColor = false;
	bool FontEnter = false;

	GameEngineState Logostate;
	GameEngineState ButtonState;

};

