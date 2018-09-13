#pragma once

#include <vector> 
#include <unordered_map> 
#include <Dependencies\DependenciesMesh.h> 
#include <Rendering\Texture.h> 
#include <Rendering\Shader.h> 
#include <Core\Camera.h> 

namespace Rak {
	namespace Rendering {
		namespace Mesh {
			enum Material_Texture_Types { TEXTURE_ALBEDOMAP, TEXTURE_NORMALMAP, TEXTURE_SPECULARMAP, TEXTURE_ROUGHNESSMAP, TEXTURE_PARALLAXMAP, TEXTURE_AOMAP };

			struct Mesh { //note; this is NOT a model. It is only a collection of the data required to construct a model 

				struct MeshEntry {
					unsigned int NumIndices;
					unsigned int BaseVertex;
					unsigned int BaseIndex;
					unsigned int MaterialIndex;
					inline MeshEntry() :
						NumIndices(0),
						BaseVertex(0),
						BaseIndex(0),
						MaterialIndex(0)
					{}
				};


				std::vector<glm::vec3> Vertices;
				std::vector<glm::vec3> TextureCoordinates;
				std::vector<glm::vec3> Normals;
				std::vector<glm::vec3> Tangents;
				std::vector<unsigned int> Indices;
				std::vector<MeshEntry> MeshEntries;
				std::vector<unsigned int> Materials;
				void ClearVectors();
				~Mesh();

			};

			struct Material {
				int Texture;
				Vector3f AlbedoMultiplier;
				Vector3f ValueTwo;
				float MaterialZoom = 1.f; 
				int WorkFlow = 0; 
				inline Material(int tex = 88, Vector3f c = Vector3f(1.)) : Texture(tex), AlbedoMultiplier(c), ValueTwo(0.) {}
			};

			struct MeshMaterials {
				std::vector<Material> Materials;
				unsigned int MaterialTexture;
				void PrepareMaterials();
				void UpdateMaterials();
				~MeshMaterials();
			};

			bool InitMaterials(const aiScene* pScene, const std::string& Filename, MeshMaterials &Materials);
			bool InitMesh(const aiMesh * aiMesh, Mesh & mesh);

			void LoadMeshData(const char * file, Mesh & Mesh, MeshMaterials & Materials);


			struct Model {
				Mesh ModelData; //where all of the mesh data is stored
				MeshMaterials MaterialData; //where all of the material data is stored

				unsigned int Vao, Vbo[5];
				inline Model(Mesh ModelData, MeshMaterials MaterialData) :
					Vao(),
					Vbo{},
					ModelData(ModelData),
					MaterialData(MaterialData)
				{}
				inline Model(const char *file) :
					Vao(),
					Vbo{},
					ModelData(Mesh()),
					MaterialData(MeshMaterials())
				{
					LoadMeshData(file, ModelData, MaterialData);
					PrepareForRendering();
				}

				inline Model() :
					Vao(),
					Vbo{},
					ModelData(Mesh()),
					MaterialData(MeshMaterials())
				{}

				void SetAlbedoMultiplier(unsigned int index, Vector3f Multiplier);
				void SetTexture(unsigned int index, unsigned int texture);
				void PrepareForRendering();
			};

			void DrawModel(Model & Model, Core::Shader Shader, Control::Camera Camera, Window Window);
		}
	}
}