#include "GUISystem.h"
#include <fstream>




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
				

				for (auto Element : Elements) {
					if(Element!=nullptr)
						Element->Init(Window);
				}
			}
			void GUIPipeline::Draw(Window & Window, DebugInfo & DebugInfo)
			{
				#ifdef RAK_DEBUG
					ImGui::Begin("Debug info:"); 
				
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
								Element->Draw(Window, Elements); 
					}
				
					ImGui::PopFont();


			}

			void TopBar::Init(Window & Window)
			{
			}
			void TopBar::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements)
			{
				ImVec2 Pos = ImVec2(0, 0);
				ImGui::SetNextWindowPos(Pos);
				ImGui::SetNextWindowSizeConstraints(ImVec2(Window.GetResolution().x, 0.), ImVec2(Window.GetResolution().x, 2000.));

				bool Open = true;

				ImGui::BeginMainMenuBar();

				if (ImGui::BeginMenu("Help"))
				{
					if (ImGui::MenuItem("Welcome To Rak")) { ClearTutorials();  Elements[WELCOMESCREEN]->Show = true; }
					if (ImGui::MenuItem("Getting used to the user interface")) { ClearTutorials(); Elements[UITUTORIAL]->Show = true;  }
					if (ImGui::MenuItem("Creating your first scene")) { ClearTutorials(); /* Do stuff */ }
					if (ImGui::MenuItem("Saving your work")) { ClearTutorials(); /* Do stuff */ }
					if (ImGui::MenuItem("Rendering your first scene")) { ClearTutorials(); /* Do stuff */ }
					if (ImGui::MenuItem("What now?")) { ClearTutorials(); /* Do stuff */ }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Github"))
				{
					if (ImGui::MenuItem("Documentation")) { OpenURL("http://www.google.com"); }
					if (ImGui::MenuItem("Source code")) { /* Do stuff */ }
					if (ImGui::MenuItem("Releases")) { /* Do stuff */ }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Import", "Ctrl+I")) { /* Do stuff */ }
					if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Do stuff */ }
					if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Add"))
				{
					if (ImGui::MenuItem("Camera")) { /* Do stuff */ }
					if (ImGui::MenuItem("Point light")) { /* Do stuff */ }
					if (ImGui::MenuItem("Spot light")) { /* Do stuff */ }
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
			void WelcomeScreen::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements)
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
			void UITutorial::Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE>& Elements)
			{
				ImGui::SetNextWindowSizeConstraints(ImVec2(1000., 600.), ImVec2(1000., 600.));

				ImGui::Begin("Getting used to the user interface", &Show); 

				ImGui::Text(Text.c_str());

				ImGui::End(); 
			}
		}
	}
}