#include "texture.h"
#include <Dependencies\DependenciesRendering.h> 
#include <Rendering/FrameBuffer.h>
#include <iostream>

Rak::Rendering::Core::Texture2D::~Texture2D()
{

}

Rak::Rendering::Core::Texture2D::Texture2D(unsigned int id, glm::ivec2 res) :
	ID(id), resolution(res)
{

}

void Rak::Rendering::Core::Texture2D::Bind(unsigned int target)
{
	glActiveTexture(GL_TEXTURE0 + target); 
	glBindTexture(GL_TEXTURE_2D,ID); 
}

Rak::Rendering::Core::Texture3D::~Texture3D()
{

}

Rak::Rendering::Core::Texture3D::Texture3D(unsigned int id, glm::ivec3 res) : 
	ID(id), resolution(res)
{

}

void Rak::Rendering::Core::Texture3D::Bind(unsigned int target)
{
	glActiveTexture(GL_TEXTURE0 + target);
	glBindTexture(GL_TEXTURE_3D, ID);
}

Rak::Rendering::Core::Texture2D Rak::Rendering::Core::LoadTexture2D(const char * name, bool linear, bool mipmaps)
{
	unsigned int id;
	sf::Image Image;

	if (Image.loadFromFile(name)) {

		Image.flipVertically();
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? (mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Image.getSize().x, Image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.getPixelsPtr());

		glBindTexture(GL_TEXTURE_2D, 0);
		return Texture2D(id, glm::ivec2(Image.getSize().x, Image.getSize().y));
	}
	else {
		return Texture2D();
		throw std::exception((std::string("Failed to load texture: ") + name).c_str());
	}
}

Rak::Rendering::Core::Texture3D Rak::Rendering::Core::EmptyTexture3D(glm::ivec3 resolution, bool linear, bool mipmaps)
{
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_3D, id);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, linear ? (mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST));
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, resolution.x, resolution.y, resolution.z, 0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_3D, 0);

	return Texture3D(id, resolution);
}

#define sfgetpxl(p,i) float(i == 0 ? p.r : i == 1 ? p.g : i == 2 ? p.b : p.a) / 255.; 

Rak::Rendering::Core::Texture3D Rak::Rendering::Core::LoadTexture3D(const char * name, bool linear, bool mipmaps)
{
	unsigned int id;
	sf::Image Image;



	if (Image.loadFromFile(name)) {

		//find the actual resolution 

		unsigned int pixelCount = Image.getSize().x * Image.getSize().y;
		unsigned int res = cbrt(pixelCount);

		std::vector<float> Pixels;
		Pixels.resize(res*res*res * 4);
		for (int x = 0; x < res; x++) {
			for (int y = 0; y < res; y++) {
				for (int z = 0; z < res; z++) {

					sf::Color pixel = Image.getPixel(x*res + z, y);



					for (int color = 0; color < 4; color++) {
						Pixels[(z*(res*res) + y*res + x) * 4 + color] = sfgetpxl(pixel, color);
					}
				}
			}
		}

		Image.flipVertically();
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);
		if (mipmaps)
			glGenerateMipmap(GL_TEXTURE_3D);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, linear ? (mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST));
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, res, res, res, 0, GL_RGBA, GL_FLOAT, Pixels.data());

		glBindTexture(GL_TEXTURE_3D, 0);


		return Texture3D(id, glm::ivec3(res));

	}
	else {



		return Texture3D();
		throw std::exception((std::string("Failed to load texture: ") + name).c_str());
	}
}





Rak::Rendering::Core::TextureCubeMap::TextureCubeMap(unsigned int id, glm::ivec2 res) : 
	ID(id), Resolution(res)
{

}

void Rak::Rendering::Core::TextureCubeMap::Bind(unsigned int target)
{
	glActiveTexture(GL_TEXTURE0 + target);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

Rak::Rendering::Core::TextureCubeMap Rak::Rendering::Core::LoadHDRI(const char * name, bool linear, bool mipmaps, Core::Shader & EquirectangularToCubeMapShader)
{
	Texture2D RawHDRIMap = LoadTexture2D(name, linear, false);
	//get a somewhat accurate resolution to use for cubemap: 
	unsigned int PixelCount = RawHDRIMap.resolution.x * RawHDRIMap.resolution.y;

	unsigned int Resolution = sqrt(static_cast<float>(PixelCount) / 6.); //obtain a resolution
	unsigned int ResolutionPower2 = pow(2, round(log2(Resolution) + .25)); //force it to be a power of 2

	unsigned int TemporaryFrameBuffer, TemporaryRenderBuffer, FinalImage;

	glGenFramebuffers(1, &TemporaryFrameBuffer);
	glGenRenderbuffers(1, &TemporaryRenderBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, TemporaryFrameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, TemporaryRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ResolutionPower2, ResolutionPower2);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, TemporaryRenderBuffer);

	glGenTextures(1, &FinalImage);
	glBindTexture(GL_TEXTURE_CUBE_MAP, FinalImage);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			ResolutionPower2, ResolutionPower2, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);

	glViewport(0, 0, ResolutionPower2, ResolutionPower2); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, TemporaryFrameBuffer);

	EquirectangularToCubeMapShader.Bind();

	glUniformMatrix4fv(glGetUniformLocation(EquirectangularToCubeMapShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(CubeProjection));
	glUniform1i(glGetUniformLocation(EquirectangularToCubeMapShader.ShaderID, "EquirectangularMap"), 0);
	RawHDRIMap.Bind(0);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(EquirectangularToCubeMapShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(CubeViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, FinalImage, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Core::DrawPostProcessCube(); // renders a 1x1 cube
	}

	EquirectangularToCubeMapShader.UnBind();

	return TextureCubeMap(FinalImage, Vector2i(ResolutionPower2));
}

