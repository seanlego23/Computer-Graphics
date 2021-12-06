#include "Light.h"
#include "renderer.h"
#include "SceneGraph.h"

#define PAD_FLOAT(n) float pad ## n;

struct shaderLight {
	float pos[3];	PAD_FLOAT(1)
	float dir[3];	PAD_FLOAT(2)
	float color[3]; PAD_FLOAT(3)
	float atten[3]; PAD_FLOAT(4)
	float ambInt;
	float diffInt;
	float cod;
	float coa;
	float sa;
	int type;		
	PAD_FLOAT(5) PAD_FLOAT(6)
};

void treeNode::traverse(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) { 
	vMat *= mMat;

	for (renderer* r : group)
		r->render(vMat, pMat, deltaTime, sg);

	for (treeNode* c : children)
		c->traverse(vMat, pMat, deltaTime, sg);
}

SceneGraph::SceneGraph(Camera c) : tree(glm::mat4(1.0f), nullptr), camera(c) {
	currentNode = &tree;

	//nLights = Light::lights.size();

	//glGenBuffers(1, &uboLights);
	//glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	////68 bytes per light object, up to 32 lights, plus 4 bytes for # of lights
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(shaderLight) * 32 + sizeof(int), NULL, GL_STATIC_DRAW);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int), &nLights);

	//for (unsigned int i = 0; i < nLights; i++) {
	//	shaderLight lightData{};
	//	std::shared_ptr<Light> light = Light::lights[i];

	//	glm::vec3 lightColor = light->getColor();

	//	lightData.type = light->getTypeID();
	//	lightData.color[0] = lightColor.r;
	//	lightData.color[1] = lightColor.g;
	//	lightData.color[2] = lightColor.b;
	//	lightData.ambInt = light->getAmbientIntensity();
	//	lightData.diffInt = light->getDiffuseIntensity();

	//	switch (lightData.type) {
	//	case DIRECTIONAL_LIGHT_ID:
	//	{
	//		glm::vec3 lightDir = std::dynamic_pointer_cast<DirectionalLight>(light)->getDirection();
	//		lightData.dir[0] = lightDir.x;
	//		lightData.dir[1] = lightDir.y;
	//		lightData.dir[2] = lightDir.z;
	//		break;
	//	}
	//	case POINT_LIGHT_ID:
	//	case SPOT_LIGHT_ID:
	//	{
	//		std::shared_ptr<PointLight> pointLight = std::dynamic_pointer_cast<PointLight>(light);
	//		glm::vec3 lightPos = pointLight->getPosition();
	//		glm::vec3 lightAtt = pointLight->getAttenuation();
	//		lightData.pos[0] = lightPos.x;
	//		lightData.pos[1] = lightPos.y;
	//		lightData.pos[2] = lightPos.z;
	//		lightData.atten[0] = lightAtt.x;
	//		lightData.atten[1] = lightAtt.y;
	//		lightData.atten[2] = lightAtt.z;
	//		lightData.cod = pointLight->getMaxDistance();
	//		if (lightData.type == POINT_LIGHT_ID)
	//			break;

	//		std::shared_ptr<SpotLight> spotLight = std::dynamic_pointer_cast<SpotLight>(light);
	//		glm::vec3 lightDir = spotLight->getDirection();
	//		lightData.dir[0] = lightDir.x;
	//		lightData.dir[1] = lightDir.y;
	//		lightData.dir[2] = lightDir.z;
	//		lightData.coa = spotLight->getCutOffAngle();
	//		lightData.sa = spotLight->getSmoothingAngle();
	//		break;
	//	}
	//	}

	//	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(int) + i * sizeof(shaderLight), sizeof(shaderLight), &lightData);
	//}
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights);
}

void SceneGraph::render(double deltaTime, RenderPass rp) {
	glm::mat4 pMat = camera.projection() * glm::lookAt(camera.position, camera.target, camera.up);
	pass = rp;

	if (rp == RenderPass::GEOMETRY) { //update lights
		//nLights = Light::lights.size();
		//TODO: Update lights when ImGui interface is setup
	}

	tree.traverse(glm::mat4(1.0f), pMat, deltaTime, this);
}
