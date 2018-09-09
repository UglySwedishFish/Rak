#pragma once
#include <Dependencies\DependenicesRasterizing.h> 

struct DebugInfo {
	float DefferedTime;
	float TraceTime; 
	float TemporalTime; 
	float LightCombinerTime; 
	float ShadowPassTime; 
	float SpatialTime; 
};

namespace Rak {
	namespace Rendering {
		namespace Rasterizer {
			class DefferedPipeLine {
				Core::Shader Deffered; 
			public:
				Core::MultiPassFrameBufferObject DefferedFrameBuffer;
				void DrawDeffered(std::vector<Mesh::Model> & BaseModels, Control::Camera & Camera, Window & Window, DebugInfo & DebugInfo);
				void Prepare(Window & Screen); 
				DefferedPipeLine();
			}; 
		}
	}
}