#pragma once
#include "GameEngineWindowTexture.h"
#include <functional>

// 설명 :
class GameEngineWindow
{
public:
	static std::function<LRESULT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)> MsgFunction;

	// constrcuter destructer
	GameEngineWindow();
	~GameEngineWindow();

	// delete Function
	GameEngineWindow(const GameEngineWindow& _Other) = delete;
	GameEngineWindow(GameEngineWindow&& _Other) noexcept = delete;
	GameEngineWindow& operator=(const GameEngineWindow& _Other) = delete;
	GameEngineWindow& operator=(GameEngineWindow&& _Other) noexcept = delete;

	void Open(const std::string& _Title, HINSTANCE _hInstance);

	static void MessageLoop(HINSTANCE _Inst, 
		std::function<void()> _Start,
		std::function<void()> _Update,
		std::function<void()> _Release
	);

	HDC GetHDC() 
	{
		return Hdc;
	}

	HWND GetHWND() const
	{
		return hWnd;
	}


	float4 GetScale() const
	{
		return Scale;
	}

	GameEngineWindowTexture* GetWindowBuffer() const
	{
		return WindowBuffer;
	}

	GameEngineWindowTexture* GetBackBuffer() const
	{
		return BackBuffer;
	}

	float4 GetMousePos();



	void SetPosAndScale(const float4& _Pos, const float4& _Scale);

	static void WindowLoopOff() 
	{
		IsWindowUpdate = false;
	}

	void ClearBackBuffer();
	void DoubleBuffering();

	void CursorOff();

	void SetMousePos(int _X, int _Y);

	static bool IsFocus() 
	{
		return IsFocusValue;
	}

	float GetDoubleBufferingCopyScaleRatio() const
	{
		return CopyRatio;
	}

	void SetDoubleBufferingCopyScaleRatio(float _Ratio) 
	{
		CopyRatio = _Ratio;
	}

	void AddDoubleBufferingCopyScaleRatio(float _Ratio)
	{
		CopyRatio += _Ratio;
	}

	float4 Get_Display_Size()
	{
		int iScreenCx = GetSystemMetrics(SM_CXSCREEN);
		int iScreenCy = GetSystemMetrics(SM_CYSCREEN);

		return { static_cast<float>(iScreenCx) , static_cast<float>(iScreenCy) };
	}

	static void ShowCursorOnOff()
	{
		static bool CursorValue = false;
		CursorValue = !CursorValue;
		ShowCursor(CursorValue);
	}

protected:

private:
	static bool IsWindowUpdate;
	static bool IsFocusValue;
	static HINSTANCE Instance;
	std::string Title = "";
	HWND hWnd = nullptr;

	float CopyRatio = 1.0f;

	float4 Scale;
	GameEngineWindowTexture* WindowBuffer = nullptr;

	GameEngineWindowTexture* BackBuffer = nullptr;

	// 2차원 배열 형식의 색깔들의 집합이 존재하고
	// 거기에 그림을 그리거나 수정할수 있는 권한을 HDC
	HDC Hdc = nullptr;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void InitInstance();
	void MyRegisterClass();
};

