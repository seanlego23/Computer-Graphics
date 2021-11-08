#pragma once

#include <string>
#include <set>

#include "attachment.h"
#include "renderer.h"

class transformEvent : public attachmentEvent {

	glm::mat4 oldXForm;
	glm::mat4 newXForm;

protected:
	
	static const std::string type;

public:

	transformEvent(const attachment* a, glm::mat4 oldModelMat, glm::mat4 newModelMat) : attachmentEvent(a), 
		oldXForm(oldModelMat), newXForm(newModelMat) { }

	virtual std::string getType() const { return type; }

	virtual auto getFunctionCall() const -> void(model::*)(attachmentEvent*);

	glm::mat4 getOldXForm() { return oldXForm; }

	glm::mat4 getNewXForm() { return newXForm; }
};

extern unsigned long modelIDCounter;

class model : public renderer { 
protected:

	std::set<attachment, bool(*)(const attachment&, const attachment&)> attachedModels;

public:

	const unsigned long modelID;

	model() : modelID(++modelIDCounter), attachedModels(&attachment::compare) { }

	model(const model& rhs) : renderer(rhs), modelID(++modelIDCounter), attachedModels(&attachment::compare) {	}

	virtual ~model() { }

	virtual void update() { }

	attachment* attachModel(model* m);

	bool detachModel(model* m);

	virtual void onAttachmentEvent(attachmentEvent* event) { }

	virtual void onTransformEvent(attachmentEvent* event) { }

	attachment* getModelAttachment(model* m);

	attachment* getModelAttachment(unsigned long id);

	virtual void OnTransform(glm::mat4 oldModel, glm::mat4 newModel) { 
		for (const attachment& a : attachedModels) {
			if (a.onTransform)
				attachmentEvent::events.push(std::unique_ptr<attachmentEvent>(new transformEvent(&a, oldModel, newModel)));
		}
	}

};