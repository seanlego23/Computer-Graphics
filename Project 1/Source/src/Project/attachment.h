#pragma once

#include <memory>
#include <string>
#include <queue>

class model;

class attachment {
	model* attachee;
	model* attacher;

public:

	static bool compare(const attachment& a1, const attachment& a2);

	bool onTransform = false;

	attachment(model* attachTo, model* attach) : attachee(attachTo), attacher(attach) { }

	model* getAttachedModel() { return attacher; }

	model* getAttachedToModel() { return attachee; }
};

class attachmentEvent {
protected:

	const attachment* a;

	attachmentEvent(const attachment* a1) : a(a1) { }

public:

	static std::queue<std::unique_ptr<attachmentEvent>> events;

//	attachmentEvent() = delete;
//	attachmentEvent(attachmentEvent&) = delete;
//	attachmentEvent(attachmentEvent&&) = delete;

	virtual std::string getType() const = 0;
	virtual auto getFunctionCall() const -> void(model::*)(attachmentEvent*) = 0;

	const attachment* getAttachment() const { return a; }
	
};