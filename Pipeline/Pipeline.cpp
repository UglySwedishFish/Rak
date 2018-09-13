#include "Pipeline.h"
#include <iostream> 
#include <Mesh/meshselector.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"
Rak::Rendering::Combined::Pipeline::Pipeline()
{
}

void Rak::Rendering::Combined::Pipeline::Prepare(std::vector<Mesh::Model> _Models, Window & Window, Control::Camera & Camera)
{
	Core::PreparePostProcess();
	Deffered.Prepare(Window);
	PathTracing.Prepare(_Models, Camera, Window); 


	Models = _Models; 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	CompositingShader = Core::Shader("Shaders/Compositing/vert.glsl", "Shaders/Compositing/frag.glsl"); 
	PrepareMaterials(); 
	glClearColor(0., 0., 0., -1.f); 
	GUI.Init(Window); 

}

 

void Rak::Rendering::Combined::Pipeline::Run(Window & Window, Control::Camera & Camera)
{

	bool MouseInWindow = true; 
	bool Running = true;
	int SelectedMaterial = 0; 
	int SelectedMesh = 0; 
	//Window.CursorVisible(false); 
	sf::Clock GameClock; 
	sf::Event Event; 
	float T = 0.; 
	while (Running) {
		while (Window.GetRawWindow()->pollEvent(Event)) {
			ImGui::SFML::ProcessEvent(Event); 
			switch (Event.type)
			{
				case sf::Event::Closed:
					delete &Window;
					return;		
				case sf::Event::KeyPressed:
					switch (Event.key.code) {
					case sf::Keyboard::O:
						PathTracing.Time += 1.0;
						break;
					case sf::Keyboard::L:
						PathTracing.Time -= 1.0;
						break;
					case sf::Keyboard::Escape:
						if (MouseInWindow)
							MouseInWindow = false; 
						else
							return;
						break;
					case sf::Keyboard::Up:
						SelectedMaterial++; 
						if (SelectedMaterial == Models[0].MaterialData.Materials.size())
							SelectedMaterial = 0; 
						break; 
					case sf::Keyboard::Down:
						SelectedMaterial--;
						if (SelectedMaterial == -1)
							SelectedMaterial += Models[0].MaterialData.Materials.size();
						break;
					}
				case sf::Event::MouseButtonPressed: 
					switch (Event.mouseButton.button) {
					case sf::Mouse::Middle: 
						MouseInWindow = true; 
						break; 
					case sf::Mouse::Right:
						Mesh::Select(Window, Camera, Models,SelectedMesh, SelectedMaterial); 
						std::cout << SelectedMaterial << '\n'; 

						break; 
					}
				default:
					break;
			}		
		}
		Window.SetFrameTime(GameClock.getElapsedTime().asSeconds()); 
		ImGui::SFML::Update(*Window.GetRawWindow(), GameClock.getElapsedTime()); 
		GameClock.restart(); 
		T += Window.GetFrameTime(); 
		Window.SetFrameCount(Window.GetFrameCount()+1);
		Window.SetTimeOpened(Window.GetTimeOpened() + Window.GetFrameTime()); 
		Rak::Core::HandleInput(Camera, 2.0, 0.15,  Window, MouseInWindow, MouseInWindow);
		Camera.PrevView = Camera.View; 
		Camera.View = Rak::Core::ViewMatrix(Camera.Position, Camera.Rotation);
		
		DebugInfo DebugInfo; 
	
	//	if (MouseInWindow) {
			Deffered.DrawDeffered(Models, Camera, Window, DebugInfo);
			PathTracing.ComputeTrace(Window, Camera, Deffered, Models, DebugInfo);
	//	}

		//std::cout << SelectedMaterial << '\n';

		Compositing(Camera); 
	
		GUI.Draw(Window, DebugInfo, SelectedMaterial, Models[SelectedMesh], PathTracing, SelectedMesh);
		ImGui::SFML::Render(*Window.GetRawWindow());

		Window.GetRawWindow()->display(); 
	}
}

void Rak::Rendering::Combined::Pipeline::Compositing(Control::Camera & Camera)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	CompositingShader.Bind(); 

	glUniform1i(glGetUniformLocation(CompositingShader.ShaderID, "Lighting"), 0);
	glUniform1i(glGetUniformLocation(CompositingShader.ShaderID, "GUISystem"), 1);
	glUniform1i(glGetUniformLocation(CompositingShader.ShaderID, "Albedo"), 2);
	glUniform1i(glGetUniformLocation(CompositingShader.ShaderID, "Enviroment"), 3);

	glUniformMatrix4fv(glGetUniformLocation(CompositingShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(CompositingShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PathTracing.Temporal.ColorBuffers[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Deffered.DefferedFrameBuffer.ColorBuffers[0]);


	PathTracing.Enviroment.Bind(3); 

	Core::DrawPostProcessQuad(); 

	CompositingShader.UnBind(); 
}
