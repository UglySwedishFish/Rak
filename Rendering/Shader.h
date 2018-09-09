#pragma once
namespace Rak {
	namespace Rendering {
		namespace Core {
			unsigned int LoadShader(unsigned int ShaderType, const char *File); 

			struct Shader {
				unsigned int VertexShader; 
				unsigned int FragmentShader; 
				unsigned int ShaderID; 

				Shader(const char * vertex, const char * fragment); 
				Shader(); 
				void Bind(); 
				void UnBind(); 
				~Shader(); 
			}; 
		}
	}
}