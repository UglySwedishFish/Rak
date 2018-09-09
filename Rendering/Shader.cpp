#include "Shader.h"
#include <Dependencies\DependenciesRendering.h> 
#include <string> 
#include <fstream> 
#include <Core\Error.h> 
#include <iostream> 


unsigned int Rak::Rendering::Core::LoadShader(unsigned int ShaderType, const char * File)
{
	std::string source; //all the shader code
	std::ifstream file(File); 

	if (!file.is_open()) {
		throwerror(static_cast<std::string>("Failed to open file: ") + File); 
		return 0; //-1 hurr durr
	}


	while (!file.eof()) {
		char line[500]; 
		file.getline(line, 500); 
		std::string Line = line; 
		if (line[0] == '#' && line[1] == 'd') {
			char id[100];
			int i = sscanf(reinterpret_cast<char*>(line), "#dirincl %s", id);

			if (i != 0) { //probably shouldn't be copying any code from here, this is a strictly "it works" brute force way to do it 
				std::string Dir = "shaders/" + static_cast<std::string>(id);

				std::ifstream IncludeFile(Dir);

				if (!IncludeFile.is_open())
					throwerror(static_cast<std::string>("Failed to open included file: ") + id + " in shader: " + File);
				else
					while (!IncludeFile.eof()) {
						char IncludeLine[500];

						IncludeFile.getline(IncludeLine, 500);

						source += IncludeLine + '\n';

					}

			}
			else {
				throwerror(static_cast<std::string>("Failed to find command: ") + id + " in shader: " + File);

			}
		}
		else
			source += Line + '\n'; 
	}
	

	GLuint id = glCreateShader(ShaderType); //compile the shader 

	const char* csource = source.c_str();
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glShaderSource(id, 1, &csource, NULL);
	glCompileShader(id);
	char error[15000];
	glGetShaderInfoLog(id, 15000, NULL, error);


	throwerror(error); 
	std::string s = error;

		std::cout << "Shader: " << File << " compiled with either errors or warnings" << std::endl;

	return id;
}

Rak::Rendering::Core::Shader::Shader(const char * vertex, const char * fragment) :
	VertexShader(LoadShader(GL_VERTEX_SHADER, vertex)),
	FragmentShader(LoadShader(GL_FRAGMENT_SHADER, fragment)),
	ShaderID(glCreateProgram())
{
	glAttachShader(ShaderID, VertexShader);
	glAttachShader(ShaderID, FragmentShader); 
	glLinkProgram(ShaderID);
	glUseProgram(ShaderID);
}	

Rak::Rendering::Core::Shader::Shader() : 
	VertexShader(NULL),
	FragmentShader(NULL),
	ShaderID(NULL)
{
}

void Rak::Rendering::Core::Shader::Bind()
{
	glUseProgram(ShaderID); 
}

void Rak::Rendering::Core::Shader::UnBind()
{
	glUseProgram(NULL); 
}

Rak::Rendering::Core::Shader::~Shader()
{
	//TODO: free up shader
}
