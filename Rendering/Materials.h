#pragma once
#include <Rendering/Texture.h>
#include <vector>
#include <array>


namespace Rak {
	namespace Rendering {

		struct MaterialArray {
			unsigned int Resolution, ID; 
			void LoadMaterials(std::vector<std::string> MaterialNames, unsigned int Resolution); 
		};

		struct MaterialStructure {
			MaterialArray Albedo, DRM, Normal, DownScaledAlbedo; 
		};

		struct TextureStructure {
			unsigned int StartIndex, LengthOfIndex; 
			TextureStructure(unsigned int StartIndex = 0, unsigned int LengthOfIndex = 0) :
				StartIndex(StartIndex), LengthOfIndex(LengthOfIndex) {}
		};

		const std::array<TextureStructure, 17> TextureCategories = { TextureStructure(0,7), TextureStructure(7, 3), TextureStructure(10, 3), TextureStructure(13,7), 
																		TextureStructure(20, 3), TextureStructure(23, 4), TextureStructure(27, 3), TextureStructure(30, 6),
																			TextureStructure(36, 3), TextureStructure(39, 10), TextureStructure(59, 7),
																				TextureStructure(66, 15), TextureStructure(81, 3), TextureStructure(84, 3), TextureStructure(87,3), 
																					TextureStructure(90,10), TextureStructure(100, 5)};

		const std::array<const std::string, 17> CategoryNames = 
		{ "Bark", "Bricks", "Cloth", "Cobblestone", "Concrete", "Fabric", "Gravel", "Ground", "Leather", "Metal", "Painted Metal", "Planks", "Rock", "Stone Tiles", "Stones", "Tiles", "Wood"}; 

		enum Categories { BARK, BRICKS, CLOTH, COBBLESTONE, CONCRETE, FABRIC, GRAVEL, GROUND, LEATHER, METAL, PAINTED_METAL, PLANKS, ROCK, STONE_TILES, STONES, TILES, WOOD, CATEGORIES  };

		MaterialStructure GetMaterialStructure(); 
		void PrepareMaterials(); 

		void SetUniforms(unsigned int ShaderID, unsigned int StartingTexture); 
		void BindTextures(unsigned int StartingTexture); 
		void BindTextureLowRes(unsigned int Texture); 

	}
}