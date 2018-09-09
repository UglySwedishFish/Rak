#pragma once
#include <Dependencies\DependenciesRendering.h> 
#include <Dependencies\DependenciesMath.h> 

class Window {
	Vector2i Resolution; 
	bool Fullscreen; 
	sf::RenderWindow * RawWindow; 
	const char * Title; 
	float FrameTime; 
	float TimeOpened; 
	int FrameCount; 
	public: 
	void SetResolution(Vector2i Resolution); 
	Vector2i GetResolution(); 

	void SetFullscreen(bool Fullscreen); 
	bool GetFullscreen(); 

	void SetTitle(const char * Title); 
	const char * GetTitle(); 

	void SetFrameTime(float FrameTime);
	float GetFrameTime(); 

	void SetTimeOpened(float TimeOpened); 
	float GetTimeOpened();

	void SetFrameCount(int FrameCount); 
	int GetFrameCount(); 

	void CursorVisible(bool Visible); 

	sf::RenderWindow * GetRawWindow(); 

	inline Window() : Resolution(0), Fullscreen(false), RawWindow(nullptr), Title("Rak²"), FrameCount(0) {}
	Window(Vector2i Resolution, bool Fullscreen); 
	~Window(); 
}; 