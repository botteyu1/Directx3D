#pragma once

class GlobalValue
{
public:
	static constexpr const char* OptimusFont = "OptimusPrincepsSemiBold";

	static float4 GetWinScale();

protected:

private:
	// constructer destructer
	GlobalValue();
	~GlobalValue();

	// delete function
	GlobalValue(const GlobalValue& _Ohter) = delete;
	GlobalValue(GlobalValue&& _Ohter) noexcept = delete;
	GlobalValue& operator=(const GlobalValue& _Other) = delete;
	GlobalValue& operator=(GlobalValue&& _Other) noexcept = delete;

};

