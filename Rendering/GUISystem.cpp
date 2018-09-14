#include "GUISystem.h"
#include <fstream>



//TODO: Add cross-platform support for opening a page in web browser 

#ifdef _WIN32 //for windows

#include <Windows.h>

#define OpenURL(url) ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL)

#else 

#ifdef __linux__ //for linux

#define OpenURL(url) std::cout << "Sorry, opening urls is not currently supported on your platform. The URL request was: " << url << '\n'; 

#else

#define OpenURL(url) std::cout << "Sorry, opening urls is not currently supported on your platform. The URL request was: " << url << '\n'; 

#ifdef __APPLE__ //for OSX

#define OpenURL(url) std::cout << "Sorry, opening urls is not currently supported on your platform. The URL request was: " << url << '\n'; 

#else //other os, just dont do anything

#define OpenURL(url) std::cout << "Sorry, opening urls is not currently supported on your platform. The URL request was: " << url << '\n'; 

#endif

#endif

#endif

#define ClearTutorials() for(int i=1;i<7;i++) {if(Elements[i]!=nullptr) {Elements[i]->Show=false;}}




namespace Rak {
	namespace Rendering {
		namespace GUI {
			void GUIPipeline::Init(Window & Window)
			{
				ImGuiIO io; 
				RaleWay = io.Fonts->AddFontFromFileTTF("GUI/Raleway.ttf", 16.f);
				Default = io.Fonts->AddFontDefault();

				ImGui::SFML::Init(*Window.GetRawWindow());

				Elements[TOPBAR] = new TopBar; 
				Elements[WELCOMESCREEN] = new WelcomeScreen; 
				Elements[UITUTORIAL] = new UITutorial; 
				Elements[SCENETUTORIAL] = new SceneTutorial;
				Elements[RENDERSETTINGS] = new Rendersettings; 
				Elements[MATERIALEDITOR] = new MaterialEditor; 

				for (auto Element : Elements) {
					if(Element!=nullptr)
						Element->Init(Window);
				}
			}
			void GUIPipeline::Draw(Window & Window, DebugInfo & DebugInfo, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{
				#ifdef RAK_DEBUG
					ImGui::Begin("Debug info:", 0, ImGuiWindowFlags_NoResize); 
				
					ImGui::Button(("Deferred Time: " + std::to_string(DebugInfo.DefferedTime)).c_str()); 
					ImGui::Button(("LightCombiner Time: " + std::to_string(DebugInfo.LightCombinerTime)).c_str());
					ImGui::Button(("ShadowPass Time: " + std::to_string(DebugInfo.ShadowPassTime)).c_str());
					ImGui::Button(("Spatial Time: " + std::to_string(DebugInfo.SpatialTime)).c_str());
					ImGui::Button(("Temporal Time: " + std::to_string(DebugInfo.TemporalTime)).c_str());
					ImGui::Button(("Trace Time: " + std::to_string(DebugInfo.TraceTime)).c_str());
				
					ImGui::End();
				#endif

					ImGui::PushFont(RaleWay);

					for (auto Element : Elements) {
						if(Element!=nullptr) 
							if(Element->Show) 
								Element->Draw(Window, Elements, ActiveMaterial, ActiveModel, PathTracing, ActiveModelIndex); 
					}
				
					ImGui::PopFont();


			}

			void TopBar::Init(Window & Window)
			{
			}
			void TopBar::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{
				ImVec2 Pos = ImVec2(0, 0);
				ImGui::SetNextWindowPos(Pos);
				ImGui::SetNextWindowSizeConstraints(ImVec2(Window.GetResolution().x, 0.), ImVec2(Window.GetResolution().x, 2000.));

				bool Open = true;

				ImGui::BeginMainMenuBar();
				
				ImGui::Text("Rak 0.1a	"); 

				if (ImGui::BeginMenu("Help"))
				{

					//Todo: finish up all tutorials. (maybe make it a bit more sophisticated)
					if (ImGui::MenuItem("Welcome To Rak")) { ClearTutorials();  Elements[WELCOMESCREEN]->Show = true; }
					if (ImGui::MenuItem("Getting used to the user interface")) { ClearTutorials(); Elements[UITUTORIAL]->Show = true;  }
					if (ImGui::MenuItem("Creating your first scene")) { ClearTutorials(); Elements[SCENETUTORIAL]->Show = true; }
					if (ImGui::MenuItem("Saving your work")) { ClearTutorials();  }
					if (ImGui::MenuItem("Rendering your first scene")) { ClearTutorials();  }
					if (ImGui::MenuItem("What now?")) { ClearTutorials();  }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Github"))
				{
					if (ImGui::MenuItem("Documentation")) { OpenURL("https://github.com/UglySwedishFish/Rak"); }
					if (ImGui::MenuItem("Source code")) { OpenURL("https://github.com/UglySwedishFish/Rak"); }
					if (ImGui::MenuItem("Releases")) { OpenURL("https://github.com/UglySwedishFish/Rak"); }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("File"))
				{

					if (ImGui::MenuItem("Import", "Ctrl+I")) {  }
					if (ImGui::MenuItem("Open", "Ctrl+O")) {  }
					if (ImGui::MenuItem("Save", "Ctrl+S")) {  }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Add"))
				{
					if (ImGui::MenuItem("Camera")) {  }
					if (ImGui::MenuItem("Point light")) {  }
					if (ImGui::MenuItem("Spot light")) {  }
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Scene"))
				{
					if (ImGui::MenuItem("Set Sky")) {}
					if (ImGui::MenuItem("Render settings")) { ClearTutorials();  Elements[RENDERSETTINGS]->Show = true; }
					if (ImGui::MenuItem("Material editor")) { ClearTutorials();  Elements[MATERIALEDITOR]->Show = true; }
					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();

				ImGui::CloseCurrentPopup();

				//ImGui::End(); 

			}

			void WelcomeScreen::Init(Window & Window)
			{
				std::ifstream File("GUI/WelcomeMessage.txt"); 

				std::string TempLine; 
				while (std::getline(File, TempLine)) {
					WelcomeText += TempLine + '\n'; 
				}

				Welcome.loadFromFile("GUI/Welcome.png"); 
				
				
				

			}
			void WelcomeScreen::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{
				

				ImGui::SetNextWindowSizeConstraints(ImVec2(1000., 600.), ImVec2(1000., 600.));
				
				
				


				ImGui::Begin("Welcome To Rak.", &Show);


				ImGui::Image(Welcome); 

				ImGui::Text(WelcomeText.c_str()); 

				if (ImGui::Button("Got it, I am ready to start using rak.")) {
					Show = false; 
				}
				if (ImGui::Button("I am new here. Could I get some help?")) {
					Show = false; 
					Elements[UITUTORIAL]->Show = true; 
				}
				
				ImGui::End(); 
			}
			
			void UITutorial::Init(Window & Window)
			{
				std::ifstream File("GUI/UiTutorial.txt");

				std::string TempLine;
				while (std::getline(File, TempLine)) {
					Text += TempLine + '\n';
				}

				Show = false; 

			}
			void UITutorial::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE>& Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{
				ImGui::SetNextWindowSizeConstraints(ImVec2(1000., 600.), ImVec2(1000., 600.));

				ImGui::Begin("Getting used to the user interface", &Show); 

				ImGui::Text(Text.c_str());

				ImGui::End(); 
			}
			void SceneTutorial::Init(Window & Window)
			{
				std::ifstream File("GUI/SceneTutorial.txt");

				std::string TempLine;
				while (std::getline(File, TempLine)) {
					Text += TempLine + '\n';
				}

				Show = false;
			}
			void SceneTutorial::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE>& Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{
				ImGui::SetNextWindowSizeConstraints(ImVec2(1000., 600.), ImVec2(1000., 600.));

				ImGui::Begin("Creating your first scene", &Show);

				ImGui::Text(Text.c_str());

				ImGui::End();
			}
			void Rendersettings::Init(Window & Window)
			{
				Show = false; 
			}
			void Rendersettings::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE>& Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{

				ImGui::SetNextWindowSizeConstraints(ImVec2(400., 0.), ImVec2(400., 1000.));

				ImGui::Begin("Render setting", &Show, ImGuiWindowFlags_NoResize); 

				ImGui::SliderFloat("Sky light power", &SkyLightPower, 0., 100.); 
				ImGui::SliderFloat("Directional light power", &DirectionalLightPower, 0., 100.);

				ImGui::End(); 
			}

			const char * Material_WorkFlows[3] = { "Premade material", "Custom lambert material", "Custom emmisive material" }; 

			void MaterialEditor::Init(Window & Window)
			{
				//Load material textures 

				for (int Index = 0; Index < MaterialTextures.size(); Index++) {
					auto & Vector = MaterialTextures[Index]; 

					for (int Texture = 0; Texture < TextureCategories[Index].LengthOfIndex; Texture++) {
						std::string File = "Materials/LowResolution/" + CategoryNames[Index] + ' ' + std::to_string(Texture + 1) + "/Albedo.jpg"; 

						sf::Texture TemporaryTexture; 
						TemporaryTexture.loadFromFile(File); 

						Vector.push_back(TemporaryTexture); 

					}

				}

			}

			const char * CategoryNamesCChar[] = { "Bark", "Bricks", "Cloth", "Cobblestone", "Concrete", "Fabric", "Gravel", "Ground", "Leather", "Metal", "Painted Metal", "Planks", "Rock", "Stone Tiles", "Stones", "Tiles", "Wood" };

			void MaterialEditor::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE>& Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex)
			{

				ImGui::Begin("Material Edtior", &Show);


				ImGui::Text("Material workflow: "); 
				ImGui::ListBox("", &ActiveModel.MaterialData.Materials[ActiveMaterial].WorkFlow, Material_WorkFlows, 3); 
	
				bool Update = false; 
				
				if (ActiveModel.MaterialData.Materials[ActiveMaterial].WorkFlow == 0) {

					if (ImGui::SliderInt("Texture", &ActiveModel.MaterialData.Materials[ActiveMaterial].Texture, 0, 80)) {
						Update = true; 
						
					}
					if (ImGui::SliderFloat("Texture Zoom", &ActiveModel.MaterialData.Materials[ActiveMaterial].MaterialZoom, 0.f, 25.f)) {
						Update = true;
					}

					ImGui::Text("Texture category: "); 



					ImGui::SliderInt(CategoryNames[TextureCategory].c_str(), &TextureCategory, 0, 16);

					for (int Texture = 0; Texture < MaterialTextures[TextureCategory].size(); Texture++) {
						//ImGui::BeginMenuBar(); 
						for (int OtherTexture = Texture; OtherTexture < std::min(int(MaterialTextures[TextureCategory].size()), Texture + 4); OtherTexture++) {
							sf::Vector2f Size(128., 128.);
							
							if (ImGui::ImageButton(MaterialTextures[TextureCategory][OtherTexture], Size)) {

								int TextureAddon = 0;

								for (int NewCat = TextureCategory - 1; NewCat != -1; NewCat--) {
									TextureAddon += TextureCategories[NewCat].LengthOfIndex;
								}


								ActiveModel.MaterialData.Materials[ActiveMaterial].Texture = TextureAddon + Texture;
								Update = true;
							}
							Texture++;
						}
						//ImGui::EndMenuBar(); 
						

					}


				}

				else if (ActiveModel.MaterialData.Materials[ActiveMaterial].WorkFlow == 1) {

					if (ImGui::SliderFloat("Roughness", &ActiveModel.MaterialData.Materials[ActiveMaterial].ValueTwo.y, 0., 1.)) {
						Update = true; 
					}
					if (ImGui::SliderFloat("Metalness", &ActiveModel.MaterialData.Materials[ActiveMaterial].ValueTwo.z, 0., 1.)) {
						Update = true; 
					}

					float Color[3] = { ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier.x,
										ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier.y,
										ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier.z};

					if (ImGui::ColorPicker3("Albedo", Color)) {
						Update = true; 
					}

					for (int i = 0; i < 3; i++)
						ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier[i] = Color[i]; 

				}
				
				else if (ActiveModel.MaterialData.Materials[ActiveMaterial].WorkFlow == 2) {

					if (ImGui::SliderFloat("Emission", &ActiveModel.MaterialData.Materials[ActiveMaterial].ValueTwo.y, 0., 25.)) {
						Update = true; 
					}

					float Color[3] = { ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier.x,
						ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier.y,
						ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier.z };

					if (ImGui::ColorPicker3("Emission color", Color)) {
						Update = true; 
					}

					for (int i = 0; i < 3; i++)
						ActiveModel.MaterialData.Materials[ActiveMaterial].AlbedoMultiplier[i] = Color[i];

				}

				ImGui::End(); 

				if (Update) {
					PathTracing.WrappedModels[ActiveModelIndex].MaterialData.Materials[ActiveMaterial] = ActiveModel.MaterialData.Materials[ActiveMaterial];
					ActiveModel.MaterialData.UpdateMaterials(); 
					PathTracing.UpdateMaterials(); 
					PathTracing.First = true; 
				}

			}
		}
	}
}