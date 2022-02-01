#pragma once

#include <type_traits>
#include <array>

#include <glm/glm.hpp>

#include "Line.h"
#include "Material.h"
#include "model.h"
#include "shader_s.h"

class BezierCurve : public Line {
public:

	typedef std::pair<float*, unsigned int> (*calcFnc)(glm::vec3, glm::vec3);

private:

	glm::vec3 start;
	glm::vec3 end;

	float *controlPoints = nullptr;
	float precision;
	int numOfControlPoints = 2;
	calcFnc calcPointsFnc;

	long factorial(long i) {
		if (i == 1)
			return 1;
		else
			return i * factorial(i - 1);
	}

	float nChooseR(int n, int r) {
		if (r == 0 || r == n)
			return 1;
		else if (r == 1 || r == n - 1)
			return n;
		return factorial(n) / (factorial(n-r) * factorial(r));
	}

	void calculateLines();

public:

	BezierCurve() = delete;

	BezierCurve(std::shared_ptr<Material> m, glm::mat4 xForm, std::string name, glm::vec3 startPoint, glm::vec3 endPoint, float prec, calcFnc calcPoints);

	virtual ~BezierCurve() {
		delete[] controlPoints;
	}

	virtual glm::vec3 getStartPoint() { return start; }

	virtual glm::vec3 getEndPoint() { return end; }

	virtual void update(double deltaTime) {
		delete[] controlPoints;
		std::pair<float*, unsigned int> data = calcPointsFnc(start, end);
		controlPoints = data.first;
		calculateLines();

		Line::update(deltaTime);
	}

	virtual void setStartPoint(glm::vec3 pStart) {
		start = pStart;

		//BezierCurve::update();
	}

	virtual void setEndPoint(glm::vec3 pEnd) {
		end = pEnd;

		//BezierCurve::update();
	}

	float getPrecision() { return precision; }

	void setPrecision(float value) {
		precision = value;
		numOfPoints = 1.0f / precision + 1.0f;
		
		delete[] points;
		points = new float[(long long)numOfPoints * 3];
		calculateLines();

		//Line::update();
	}

	unsigned int getNumOfControlPoints() { return numOfControlPoints; }

	virtual float length() {
		float l = 0;
		for (int i = 0; i < this->numOfPoints - 1; i++) {
			glm::vec3 p1(points[i*3], points[i*3 + 1], points[i*3 + 2]);
			glm::vec3 p2(points[i*3 + 3], points[i*3 + 4], points[i*3 + 5]);
			l += glm::length(p2 - p1);
		}
		return l;
	}

	virtual std::pair<glm::vec3, glm::vec3> getPointOnLine(float progress);
	
	virtual void render(glm::mat4 vMat, glm::mat4 pMat, double deltaTime, SceneGraph* sg);

};

std::pair<float*, unsigned int> eightPointRectangle(glm::vec3 start, glm::vec3 end);
std::pair<float*, unsigned int> fourPointRectangle(glm::vec3 start, glm::vec3 end);
