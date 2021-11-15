#include "model.h"

const std::string transformEvent::type = "transformEvent";

auto transformEvent::getFunctionCall() const -> void(model::*)(attachmentEvent*) { 
	return &model::onTransformEvent;
}

unsigned long modelIDCounter = 0;

attachment* model::attachModel(model* m) {
	/*return &const_cast<attachment&>(attachedModels.emplace(this, m).first);*/
	return nullptr;
}

bool model::detachModel(model* m) {
	//for (auto& it = attachedModels.begin(); it != attachedModels.end(); it++) {
	//	if ((*it).getAttachedModel()->modelID == m->modelID) {
	//		attachedModels.erase(it);
	//		return true;
	//	}
	//}
	return false;
}

attachment* model::getModelAttachment(model* m) {
	return nullptr;
}

attachment* model::getModelAttachment(unsigned long id) {
	return nullptr;
}
