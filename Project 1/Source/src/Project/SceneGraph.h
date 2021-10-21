#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

#include "Camera.h"
#include "renderer.h"

class SceneGraph;

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

	void traverse(glm::mat4 vMat, glm::mat4 perspective, double deltaTime);

};

class SceneGraph {

	treeNode tree;
	treeNode* currentNode;

public:

	Camera camera;

	SceneGraph(Camera c) : tree(glm::mat4(1.0f), nullptr), camera(c) {
		currentNode = &tree;
	}

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

	void render(double deltaTime) {
		glm::mat4 pMat = camera.projection() * glm::lookAt(camera.position, camera.target, camera.up);

		tree.traverse(glm::mat4(1.0f), pMat, deltaTime);
	}

};