#include "renderer.h"
#include "SceneGraph.h"

void treeNode::traverse(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg) { 
	vMat *= mMat;

	for (renderer* r : group)
		r->render(vMat, pMat, deltaTime, sg);

	for (treeNode* c : children)
		c->traverse(vMat, pMat, deltaTime, sg);
}
