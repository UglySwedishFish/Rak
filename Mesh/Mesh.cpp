#include "Mesh.h"
#include <Dependencies\DependenciesRendering.h> 
#include <iostream> 

void Rak::Rendering::Mesh::Mesh::ClearVectors()
{
}

Rak::Rendering::Mesh::Mesh::~Mesh()
{
	ClearVectors(); 
}

void Rak::Rendering::Mesh::MeshMaterials::PrepareMaterials()
{
	glGenTextures(1, &MaterialTexture); 
	glBindTexture(GL_TEXTURE_1D, MaterialTexture); 
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_1D, 0);
	UpdateMaterials(); 
}

void Rak::Rendering::Mesh::MeshMaterials::UpdateMaterials()
{
	std::vector<Vector3f> Data(Materials.size()*2); 

	for (int Material = 0; Material < Data.size(); Material+=2) {
		Data[Material] = Materials[Material/2].AlbedoMultiplier; 

		switch (Materials[Material / 2].WorkFlow) {
		case 0:
			Data[Material + 1] = glm::vec3(Materials[Material / 2].MaterialZoom, glm::intBitsToFloat(Materials[Material / 2].Texture), -1.f);
			break;
		case 1:
			Data[Material + 1] = glm::vec3(0.,Materials[Material / 2].ValueTwo.y, Materials[Material / 2].ValueTwo.z);
			break; 
		case 2:
			Data[Material + 1] = glm::vec3(0.f, Materials[Material / 2].ValueTwo.y, -20.f);
			break;
		}
	}

	glBindTexture(GL_TEXTURE_1D, MaterialTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, Materials.size()*2, 0, GL_RGB, GL_FLOAT, &Data[0]);
	glBindTexture(GL_TEXTURE_1D, 0);
}

Rak::Rendering::Mesh::MeshMaterials::~MeshMaterials()
{
}

bool Rak::Rendering::Mesh::InitMaterials(const aiScene * pScene, const std::string & Filename, MeshMaterials & Materials)
{
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;



}

const aiVector3D ai_Null(0.0, 0.0, 0.0);

bool Rak::Rendering::Mesh::InitMesh(const aiMesh * aiMesh, Mesh & mesh)
{
	for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
		const aiVector3D *VertexPos = &(aiMesh->mVertices[i]);
		const aiVector3D *VertexNormal = &(aiMesh->mNormals[i]);
		const aiVector3D* VertexTextureCoordinate = aiMesh->HasTextureCoords(0) ? &(aiMesh->mTextureCoords[0][i]) : &aiVector3D(0);
		const aiVector3D* VertexTangent = aiMesh->HasTangentsAndBitangents() ? &(aiMesh->mTangents[i]) : &aiVector3D(0);

		mesh.Vertices.push_back(glm::vec3(VertexPos->x, VertexPos->y, VertexPos->z));
		mesh.Normals.push_back(glm::vec3(VertexNormal->x, VertexNormal->y, VertexNormal->z));


		//calculate texture coordinate: 

		Vector3f Norm = glm::normalize(Vector3f(VertexNormal->x, VertexNormal->y, VertexNormal->z)); 
		Vector3f Tang = glm::normalize(Vector3f(VertexTangent->x, VertexTangent->y, VertexTangent->z));

		Vector3f Tangent = glm::normalize(Tang - glm::dot(Tang, Norm) * Norm);
		Vector3f Bitangent = glm::normalize(glm::cross(Tang, Norm));
		Matrix3f TBN = Matrix3f(Tangent, Bitangent, Norm);

		Vector3f NewTextureCoordinate = Vector3f(VertexPos->x, VertexPos->y, VertexPos->z) * TBN; 

		mesh.TextureCoordinates.push_back(Vector3f(NewTextureCoordinate.x, NewTextureCoordinate.y, 0.f));
		mesh.Tangents.push_back(glm::vec3(VertexTangent->x, VertexTangent->y, VertexTangent->z));
		mesh.Materials.push_back(aiMesh->mMaterialIndex);
	}

	for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
		const aiFace& Face = aiMesh->mFaces[i];
		if (Face.mNumIndices == 3) { //if it isn't a triangle, skip this face
			mesh.Indices.push_back(Face.mIndices[0]);
			mesh.Indices.push_back(Face.mIndices[1]);
			mesh.Indices.push_back(Face.mIndices[2]);
		}
		else {
			mesh.Indices.push_back(Face.mIndices[0]);
			mesh.Indices.push_back(Face.mIndices[0]);
			mesh.Indices.push_back(Face.mIndices[0]);
		}
	}
	return true;
}

void Rak::Rendering::Mesh::LoadMeshData(const char * file, Mesh & Model, MeshMaterials & Materials)
{
	Assimp::Importer ObjectImporter;

	const aiScene * Scene = ObjectImporter.ReadFile(file, aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (Scene) {

		Mesh Mesh; 

		Mesh.MeshEntries.resize(Scene->mNumMeshes);


		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		// Count the number of vertices and indices
		for (unsigned int i = 0; i < Mesh.MeshEntries.size(); i++) {
			Mesh.MeshEntries[i].MaterialIndex = Scene->mMeshes[i]->mMaterialIndex;
			Mesh.MeshEntries[i].NumIndices = Scene->mMeshes[i]->mNumFaces * 3;
			Mesh.MeshEntries[i].BaseVertex = NumVertices;
			Mesh.MeshEntries[i].BaseIndex = NumIndices;

			NumVertices += Scene->mMeshes[i]->mNumVertices;
			NumIndices += Mesh.MeshEntries[i].NumIndices;
		}

		// Reserve space in the vectors for the vertex attributes and indices
		Mesh.Vertices.reserve(NumVertices);
		Mesh.Normals.reserve(NumVertices);
		Mesh.TextureCoordinates.reserve(NumVertices);
		Mesh.Tangents.reserve(NumVertices);
		Mesh.Indices.reserve(NumIndices);

		for (unsigned int i = 0; i < Mesh.MeshEntries.size(); i++) {
			const aiMesh* aiMesh = Scene->mMeshes[i];
			InitMesh(aiMesh, Mesh);
		}
		
		int CurrentIndicie = 0; 

		std::cout << "Meshentry size: " << Mesh.MeshEntries.size() << '\n'; 

		for (int MeshEntry = 0; MeshEntry < Mesh.MeshEntries.size(); MeshEntry++) {
			
			Materials.Materials.push_back(Material()); 

			std::cout << MeshEntry << '\n';

			for (int Indicie = 0; Indicie < Mesh.MeshEntries[MeshEntry].NumIndices; Indicie++) {

				Model.Vertices.push_back(Mesh.Vertices[CurrentIndicie]); 
				Model.Normals.push_back(Mesh.Normals[CurrentIndicie]);
				Model.Tangents.push_back(Mesh.Tangents[CurrentIndicie]);
				Model.TextureCoordinates.push_back(Vector3f(Mesh.TextureCoordinates[CurrentIndicie].x, Mesh.TextureCoordinates[CurrentIndicie].y, static_cast<float>(MeshEntry) / 100.f));
				Model.Indices.push_back(CurrentIndicie++); 
				
			}
			
		}



		Materials.PrepareMaterials(); 

		
	}
	else {
		std::cout << "Failed to load model: " << file << " (Error: " << ObjectImporter.GetErrorString() << ") \n";
	}
}

void Rak::Rendering::Mesh::DrawModel(Model & _Model, Core::Shader _Shader, Control::Camera _Camera, Window _Window)
{

	glUniform1i(glGetUniformLocation(_Shader.ShaderID, "Materials"), 0); 

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_1D, _Model.MaterialData.MaterialTexture); 

	glBindVertexArray(_Model.Vao);

	glDrawElements(GL_TRIANGLES, _Model.ModelData.Vertices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

void Rak::Rendering::Mesh::Model::SetAlbedoMultiplier(unsigned int index, Vector3f Multiplier)
{
	MaterialData.Materials[index].AlbedoMultiplier = Multiplier; 
	MaterialData.UpdateMaterials(); 
}

void Rak::Rendering::Mesh::Model::SetTexture(unsigned int index, unsigned int texture)
{
	MaterialData.Materials[index].Texture = texture;
//	MaterialData.UpdateMaterials();
}

void Rak::Rendering::Mesh::Model::PrepareForRendering()
{
	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);
	glGenBuffers(5, Vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.Vertices[0]) * ModelData.Vertices.size(), &ModelData.Vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.TextureCoordinates[0]) * ModelData.TextureCoordinates.size(), &ModelData.TextureCoordinates[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.Normals[0]) * ModelData.Normals.size(), &ModelData.Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, Vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelData.Tangents[0]) * ModelData.Tangents.size(), &ModelData.Tangents[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Vbo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ModelData.Indices[0]) * ModelData.Indices.size(), &ModelData.Indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}
