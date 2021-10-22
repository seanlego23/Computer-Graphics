#include "model.h"

unsigned long modelIDCounter = 0;

void model::attachModel(model* m) {
	attachedModels.insert({m->modelID, m});
}

void model::detachModel(model* m) {
	attachedModels.erase(m->modelID);
}