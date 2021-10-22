#pragma once

#include <map>

#include "renderer.h"

extern unsigned long modelIDCounter;

class model : public renderer { 

protected:

	std::map<unsigned long, model*> attachedModels;

public:

	const unsigned long modelID;

	model() : modelID(++modelIDCounter) { }

	virtual void update() { }

	void attachModel(model* m);

	void detachModel(model* m);

};

