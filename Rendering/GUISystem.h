#include "Pipeline/HybridRasterizing.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"
#include "Pipeline/HybridPathTracing.h"
#include "Rendering/Materials.h"
#include <array>

namespace Rak {
	namespace Rendering {
		namespace GUI {

			enum GuiElements {TOPBAR, WELCOMESCREEN, UITUTORIAL, SCENETUTORIAL, SAVINGTUTORIAL, RENDERINGTUTORIAL, WHATNOW, RENDERSETTINGS, MATERIALEDITOR, SIZE};
			
			struct GUIElement {
				bool Show = true; 
				virtual void Init(Window & Window) = 0; 
				virtual void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex) = 0;
			};

			struct WelcomeScreen : GUIElement {
				std::string WelcomeText; 
				sf::Texture Welcome; 
				void Init(Window & Window); 
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);
			};

			struct TopBar : GUIElement {
				void Init(Window & Window); 
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);
			};

			struct UITutorial : GUIElement {
				std::string Text;
				void Init(Window & Window);
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);
			};

			struct SceneTutorial : GUIElement {
				std::string Text;
				void Init(Window & Window);
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);
			};

			struct Rendersettings : GUIElement {

				float SkyLightPower; 
				float DirectionalLightPower; 

				void Init(Window & Window);
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);
			};

			struct MaterialEditor : GUIElement {

				int TextureCategory = 0; 
				std::array<std::vector<sf::Texture>, Categories::CATEGORIES> MaterialTextures; 

				void Init(Window & Window); 
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);

			};
			

			struct GUIPipeline {

				std::array<GUIElement*, GuiElements::SIZE> Elements; 

				ImFont * RaleWay; 
				ImFont * Default; 

				void Init(Window & Window); 
				void Draw(Window & Window, DebugInfo & DebugInfo, int & ActiveMaterial, Mesh::Model & ActiveModel, PathTracing::PathTracingPipeLine & PathTracing, int ActiveModelIndex);
			
			};
		}
	}
}