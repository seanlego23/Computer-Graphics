
#include "attachment.h"
#include "model.h"

bool attachment::compare(const attachment& a1, const attachment& a2) {
	if (a1.attachee->modelID < a2.attachee->modelID)
		return true;
	else if (a1.attachee->modelID == a2.attachee->modelID) {
		if (a1.attacher->modelID < a2.attacher->modelID)
			return true;
		else
			return false;
	} else
		return false;
}

std::queue<std::unique_ptr<attachmentEvent>> attachmentEvent::events;