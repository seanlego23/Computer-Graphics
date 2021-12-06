#pragma once

#include <string>

class Object {
	std::string name;
	bool dirty = false;

	static unsigned int objectCount;

public:

	Object() : name("Object" + std::to_string(objectCount++)) { }

	Object(std::string n) : name(n) { 
		++objectCount;
	}

	Object(const Object& rhs) : name(rhs.name + " - Copy") {
		++objectCount;
	}

	virtual ~Object() { }

	inline std::string getName() { return name; }

	inline bool isDirty() { return dirty; }

	inline void setDirty() { dirty = true; }

	inline void resetDirty() { dirty = false; }
};