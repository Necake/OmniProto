#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "Texture.h"

class Model
{
public:
	Model(const char* path)
	{
		loadModel(path);
	}
	Model()
	{

	}
	void draw(Shader shader);
	void drawInstanced(Shader shader, unsigned int numInstances);
	void getSpecs();
	std::vector<Mesh> meshes;
private:
	std::vector<TextureData> textures_loaded;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<TextureData> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
#endif