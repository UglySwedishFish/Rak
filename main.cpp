/*

Rak² is a hybrid path-tracer built for one purpose - highly realistic rendering in real time*

TODO: 
	-	the core engine (done) 
	-	deffered rasterizing (aka the part that makes it hybrid, and not entirely path tracing) (done) 
	-	The tracing function (done) 
	-	Github page? (done)
	-	Spatial split BVH (similar aproach to the production-ready cycles renderer) 
	-	Woop ray/triangle intersection 
	-	Some basic hand optimization (hand-written kernels etc) 
	-	logo (I know I have great priorities) 
	-	everything else (who cares) 
	-	potential integration with blender? (no lol) 

Planned features
	-	Ridiciously fast path tracing (hopefully) 
	-	Spatiotemporal filtering (simple at first but I have a paper stored somewhere) (really needed since I have a budget of 1 sample per pixel) 
	-	Non-path traced giant data sets (essentially designed for smaller entities that are going to be instanced, like grass for instance) 
	-	Dynamic sky with volumetric clouds (ported over from fEngine) 
	-	Fast & simple volumetric lighting (helps with realism) (does not utilize path tracing) 
	-	Physically based rendering (would be a shame if I didn't use it with pathtracing lol) 


References: 
	https://www.nvidia.com/object/nvidia_research_pub_012.html
	http://www.sven-woop.de/papers/2004-GH-SaarCOR.pdf (simplified aproach because every triangle consists of the verticies (0,0,0) (1,0,0) and (0,1,0) while the ray is transformed to
	//unit triangle space
	http://blog.hvidtfeldts.net/index.php/2012/10/image-based-lighting/ fantastic tutorial about equirectangular maps (i;e most hdrs) 
	
	* 30 samples per second at 720p, 150k tris. (1 sample per pixel) 
*/

#include <Pipeline\Pipeline.h> 
#include <iostream> 
#include <Mesh/BVH.h>




int main() {
	Window Screen;
	Screen.SetTitle("Rak."); 
	Screen = Window(Vector2i(1920, 1080), false);
	
	
	Rak::Control::Camera Camera = Rak::Control::Camera(70.0,0.01,1000.0,Vector3f(0.122498, 1.76084, 7.83415), Vector3f(0.), Screen); //fov, znear, zfar, position, rotation, display 

	srand(time(0)); 

	std::vector<Rak::Rendering::Mesh::Model> Models{ Rak::Rendering::Mesh::Model("Test.obj") };
	for (int i = 0; i < Models[0].MaterialData.Materials.size(); i++) {
		Models[0].MaterialData.Materials[i].AlbedoMultiplier = Vector3f(1.);
		Models[0].MaterialData.Materials[i].ValueTwo = Vector3f(0.,0.,0.);
		Models[0].MaterialData.Materials[i].Texture = 0;
	}
	Models[0].MaterialData.UpdateMaterials(); 

	Rak::Rendering::Combined::Pipeline Pipeline; //the pipeline itself 
	Pipeline.Prepare(Models, Screen, Camera); 
	Pipeline.Run(Screen, Camera); 
}
	


/*
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(640, 480), "ImGui + SFML = <3");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		//ImGui::ShowTestWindow();

		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		ImGui::End();

		window.clear();
		window.draw(shape);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
*/