#include "Pipeline/HybridRasterizing.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"
#include <array>

namespace Rak {
	namespace Rendering {
		namespace GUI {

			enum GuiElements {TOPBAR, WELCOMESCREEN, UITUTORIAL, FIRSTSCENETUTORIAL, SAVINGTUTORIAL, RENDERINGTUTORIAL, WHATNOW, SIZE};
			
			struct GUIElement {
				bool Show = true; 
				virtual void Init(Window & Window) = 0; 
				virtual void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements) = 0;
			};

			struct WelcomeScreen : GUIElement {
				std::string WelcomeText; 
				sf::Texture Welcome; 
				void Init(Window & Window); 
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements);
			};

			struct TopBar : GUIElement {
				void Init(Window & Window); 
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements);
			};

			struct UITutorial : GUIElement {
				std::string Text;
				void Init(Window & Window);
				void Draw(Window & Window, std::array<GUIElement*, GuiElements::SIZE> &Elements);
			};
			

			struct GUIPipeline {

				std::array<GUIElement*, GuiElements::SIZE> Elements; 

				ImFont * RaleWay; 
				ImFont * Default; 

				void Init(Window & Window); 
				void Draw(Window & Window, DebugInfo & DebugInfo);
			
			};
		}
	}
}