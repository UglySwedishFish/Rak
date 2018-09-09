#include "Materials.h"
#include <Dependencies/DependenciesRendering.h>
#include <iostream>

Rak::Rendering::MaterialStructure CurrentStructure;

const int TextureCount = 95; 

void Rak::Rendering::MaterialArray::LoadMaterials(std::vector<std::string> MaterialNames, unsigned int Resolution)
{

	glGenTextures(1, &ID); 

	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);


	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Resolution, Resolution, MaterialNames.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	for (int i = 0; i < MaterialNames.size(); i++) {
		sf::Image Image; 
		Image.loadFromFile(MaterialNames[i]); 

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			0, 0, i,
			Resolution, Resolution, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			Image.getPixelsPtr()); //super straight forward 

		
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0); 

 
}

Rak::Rendering::MaterialStructure Rak::Rendering::GetMaterialStructure()
{
	return CurrentStructure;
}

void Rak::Rendering::PrepareMaterials()
{

	std::vector<std::string> Albedo, Normal, DRM, LowResAlbedo; 

	for (int Category = 0; Category < TextureCategories.size(); Category++) {
		for (int Texture = 0; Texture < TextureCategories[Category].LengthOfIndex; Texture++) {
			
			std::string SubDirectory = CategoryNames[Category] + ' ' + std::to_string(Texture + 1) + '/'; 

			std::string DirectoryFullRes = "Materials/FullResolution/" + SubDirectory; 
			std::string DirectoryLowRes = "Materials/LowResolution/" + SubDirectory; 

			Albedo.push_back(DirectoryFullRes + "Albedo.jpg"); 
			Normal.push_back(DirectoryFullRes + "Normal.jpg");
			DRM.push_back(DirectoryFullRes + "DRM.png");
			LowResAlbedo.push_back(DirectoryLowRes + "Albedo.jpg");

		}
	}

	std::cout << Albedo.size() << '\n'; 

	CurrentStructure.Albedo.LoadMaterials(Albedo, 1024); 
	CurrentStructure.Normal.LoadMaterials(Normal, 1024);
	CurrentStructure.DRM.LoadMaterials(DRM, 1024);
	CurrentStructure.DownScaledAlbedo.LoadMaterials(LowResAlbedo, 256);

}

std::array<const char*, 3> TextureNames = { "Albedo", "DRM", "Normal" }; 

void Rak::Rendering::SetUniforms(unsigned int ShaderID, unsigned int StartingTexture)
{
	for (int i = 0; i < 3; i++)
		glUniform1i(glGetUniformLocation(ShaderID, TextureNames[i]), StartingTexture + i); 
}

void Rak::Rendering::BindTextures(unsigned int StartingTexture)
{

	for (int i = 0; i < 3; i++) {
		glActiveTexture(GL_TEXTURE0 + StartingTexture + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, i == 0 ? CurrentStructure.Albedo.ID : i == 1 ? CurrentStructure.DRM.ID : CurrentStructure.Normal.ID); 
	}
}

void Rak::Rendering::BindTextureLowRes(unsigned int Texture)
{
	glActiveTexture(GL_TEXTURE0 + Texture); 
	glBindTexture(GL_TEXTURE_2D_ARRAY, CurrentStructure.DownScaledAlbedo.ID);

}
