#pragma once

#include <map>
#include <string>
#include <vector>

#include "model.h"
#include "SceneGraph.h"

struct objMesh {
	std::string name;
	int startingVertex;
};

struct vertIndices {
	int vi, ti, ni;
};

class ModelImporter {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vertexValues;
	std::vector<glm::vec2> texCoordValues;
	std::vector<glm::vec3> normalValues;
	std::vector<objMesh> meshes;
	std::map<std::string, unsigned int> textures;

public:

	ModelImporter();

	int getNumOfVertices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTextureCoordinates();
	std::vector<glm::vec3> getNormals();
	std::vector<objMesh> ModelImporter::getMeshes();

	void parseObj(const char* file);
	void parseMTL(const char* file);
};

class ObjModel : public model { 
	std::vector<objMesh> meshes;
	
public:

	ObjModel() = delete;

	ObjModel(Material* m, glm::mat4 xForm, const char* file);

	virtual ~ObjModel() { }

	virtual void ObjModel::render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

};

