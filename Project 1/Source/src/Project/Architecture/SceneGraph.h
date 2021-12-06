#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

#include "Camera.h"
#include "Light.h"

class SceneGraph;
class renderer;

class treeNode {

	glm::mat4 mMat;
	std::vector<renderer*> group;
	std::vector<treeNode*> children;
	treeNode* parent;

public:

	treeNode(glm::mat4 m, treeNode* p) : mMat(m), parent(p) { }

	~treeNode() {
		for (treeNode* c : children) {
			c->~treeNode();
			delete c;
		}
	}

	void setModelMatrix(glm::mat4 m) {
		mMat = m;
	}

	void addRenderer(renderer* r) {
		group.push_back(r);
	}

	treeNode* addChild(glm::mat4 m) {
		children.push_back(new treeNode(m, this));
		return children.back();
	}

	treeNode* getParent() {
		return parent;
	}

	void traverse(glm::mat4 vMat, glm::mat4 perspective, double deltaTime, SceneGraph* sg);

};

class SceneGraph {

	treeNode tree;
	treeNode* currentNode;

	//unsigned int nLights;

public:

	enum class RenderPass {
		FORWARD,
		GEOMETRY,
		LIGHTING,
		POST,
	};

	Camera camera;
	RenderPass pass = RenderPass::FORWARD;
	float exposure = 3.0f;

	//unsigned int uboLights;

	SceneGraph(Camera c);

	void setModelMatrix(glm::mat4 m) {
		currentNode->setModelMatrix(m);
	}

	void addRenderer(renderer* r) {
		currentNode->addRenderer(r);
	}

	treeNode* addChild(glm::mat4 m) {
		currentNode = currentNode->addChild(m);
		return currentNode;
	}

	treeNode* getParent() {
		currentNode = currentNode->getParent();
		return currentNode;
	}

	void render(double deltaTime, RenderPass rp);

};