#include "Model.h"


Model::Model()
{

}

void Model::RenderModel()
{
	for (size_t i = 0; i < MeshList.size(); i++)
	{
		unsigned int MaterialIndex = MeshToTexture[i];

		// Verify the Index is within array bounds, before checking if a valid result exists at the index
		if (MaterialIndex < TextureList.size() && TextureList[MaterialIndex])
		{
			TextureList[MaterialIndex]->UseTexture();
		}

		MeshList[i]->RenderMesh();
	}
}

void Model::LoadModel(const std::string& FileName)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(FileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!Scene)
	{
		printf("Model (%s) failed to load: %s", FileName, Importer.GetErrorString());
		return;
	}

	LoadNode(Scene->mRootNode, Scene);
	LoadMaterials(Scene);
}

void Model::ClearModel()
{
	for (size_t i = 0; i < MeshList.size(); i++)
	{
		if (MeshList[i])
		{
			// Safe delete
			delete MeshList[i];
			MeshList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < TextureList.size(); i++)
	{
		if (TextureList[i])
		{
			// Safe delete
			delete TextureList[i];
			TextureList[i] = nullptr;
		}
	}
}

void Model::LoadNode(aiNode* Node, const aiScene* Scene)
{
	for (size_t i = 0; i < Node->mNumMeshes; i++)
	{
		LoadMesh(Scene->mMeshes[Node->mMeshes[i]], Scene);
	}

	for (size_t i = 0; i < Node->mNumChildren; i++)
	{
		LoadNode(Node->mChildren[i], Scene);
	}
}

void Model::LoadMesh(aiMesh* LoadMesh, const aiScene* Scene)
{
	std::vector<GLfloat> Vertices;
	std::vector<unsigned int> Indices;

	// Add vertices to vertice list
	for (size_t i = 0; i < LoadMesh->mNumVertices; i++)
	{
		// Load our vertex x,y,z for the model
		Vertices.insert(Vertices.end(), { LoadMesh->mVertices[i].x,LoadMesh->mVertices[i].y,LoadMesh->mVertices[i].z });
		if (LoadMesh->mTextureCoords[0])
		{
			// Load our texture U and V coordinates
			Vertices.insert(Vertices.end(), { LoadMesh->mTextureCoords[0][i].x,LoadMesh->mTextureCoords[0][i].y });
		}
		else  // Handle no texture case
		{
			Vertices.insert(Vertices.end(), { 0.0f, 0.0f });
		}
		// Load our normals for Nx, Ny, Nz
		Vertices.insert(Vertices.end(), { -LoadMesh->mNormals[i].x, -LoadMesh->mNormals[i].y, -LoadMesh->mNormals[i].z });
	}

	// Add faces to Indices list
	for (size_t i = 0; i < LoadMesh->mNumFaces; i++)
	{
		aiFace Face = LoadMesh->mFaces[i];
		for (size_t j = 0; j < Face.mNumIndices; j++)
		{
			Indices.push_back(Face.mIndices[j]);
		}
	}

	// Instantiate the Mesh & store the texture details
	Mesh* NewMesh = new Mesh();
	NewMesh->CreateMesh(&Vertices[0], &Indices[0], Vertices.size(), Indices.size());
	MeshList.push_back(NewMesh);
	MeshToTexture.push_back(LoadMesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* Scene)
{
	TextureList.resize(Scene->mNumMaterials);

	for (size_t i = 0; i < Scene->mNumMaterials; i++)
	{
		aiMaterial* Material = Scene->mMaterials[i];

		TextureList[i] = nullptr;

		if (Material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString TexturePath;
			if (Material->GetTexture(aiTextureType_DIFFUSE, 0, &TexturePath) == AI_SUCCESS)
			{
				int Index = std::string(TexturePath.data).rfind("\\");
				std::string RawFilename = std::string(TexturePath.data).substr(Index + 1);
				printf("Texture %s loaded successfully!\n", RawFilename);

				std::string TexturePath = std::string("Textures/") + RawFilename;

				TextureList[i] = new Texture(TexturePath.c_str());

				if (!TextureList[i]->LoadTexture())
				{
					printf("Failed to Load Texture at: %s !\n", TexturePath);

					// Safe Delete
					delete TextureList[i]; 
					TextureList[i] = nullptr;
				}
			}
		}

		// Fallback texture
		if (!TextureList[i])
		{
			TextureList[i] = new Texture("Textures/plain.png");
			TextureList[i]->LoadTexture();
		}
	}
}

Model::~Model()
{
	ClearModel();
}