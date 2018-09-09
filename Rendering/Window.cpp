#include "window.h"

void Window::SetResolution(Vector2i Resolution)
{
	this->Resolution = Resolution; 
}

Vector2i Window::GetResolution()
{
	return Resolution; 
}

void Window::SetFullscreen(bool Fullscreen)
{
	this->Fullscreen = Fullscreen; 
}

bool Window::GetFullscreen()
{
	return Fullscreen; 
}

void Window::SetTitle(const char * Title)
{
	this->Title = Title; 
}

const char * Window::GetTitle()
{
	return Title; 
}

float Window::GetFrameTime()
{
	return FrameTime; 
}

void Window::SetFrameTime(float _FrameTime)
{
	FrameTime = _FrameTime; 
}

void Window::SetTimeOpened(float _TimeOpened)
{
	TimeOpened = _TimeOpened; 
}

float Window::GetTimeOpened()
{
	return TimeOpened;
}

void Window::SetFrameCount(int _FrameCount)
{
	FrameCount = _FrameCount; 
}

int Window::GetFrameCount()
{
	return FrameCount; 
}

void Window::CursorVisible(bool Visible)
{
//	RawWindow->setMouseCursorVisible(Visible); 
}

sf::RenderWindow * Window::GetRawWindow()
{
	return RawWindow;
}

Window::Window(Vector2i Resolution, bool Fullscreen)
{

	this->Resolution = Resolution; 
	this->Fullscreen = Fullscreen;
	this->FrameCount = 0; 

	sf::ContextSettings settings;
	settings.depthBits = 8;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 4;
	settings.minorVersion = 3;

	RawWindow = new sf::RenderWindow(sf::VideoMode(Resolution.x, Resolution.y), "Rak.", Fullscreen ? sf::Style::Fullscreen : sf::Style::None, settings);

	glewInit(); //prepare glew (needs to be AFTER window creation) 
}

Window::~Window()
{
	//RawWindow->close(); 
	//delete RawWindow; 
}
