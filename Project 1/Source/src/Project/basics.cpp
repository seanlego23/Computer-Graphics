#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

#include <random>

struct myEvent {
	float time;
	int thing;
};

using namespace std;

constexpr auto dimx = 3840u, dimy = 2160u;

float imageBuff[dimx][dimy][3] = {};

void attenuate(glm::vec3& color);

int myTexture() {
	std::minstd_rand lcg(glfwGetTime());
	std::chi_squared_distribution<double> intensityDist(3);
	std::chi_squared_distribution<double> colorPicker(2);

	constexpr double twoPi = glm::pi<double>() * 2.0;
	constexpr double halfPi = glm::pi<double>() * 0.5;
	
	constexpr double scaleX = dimx / twoPi;
	constexpr double scaleY = dimy / glm::pi<double>();

	for (int i = 0; i < 5000; i++) {
		double yaw = std::fmod(std::log(lcg()), twoPi);
		double pitch = (lcg() < lcg.modulus / 2 ? -1 : 1) * std::fmod(std::log(lcg()), halfPi);

		unsigned int intensity = intensityDist(lcg) + 1;
		unsigned int colorChoice = colorPicker(lcg);
		
		//Keeps stars away from edges
		int x = (int)glm::clamp(std::round(yaw * scaleX), 3.0, dimx - 4.0);
		int y = (int)glm::clamp(std::round(pitch * scaleY + dimy / 2), 3.0, dimy - 4.0);

		glm::vec3 color;
		//See the following website for a visual of the chi-squared distribution with dof=2
		//https://en.cppreference.com/w/cpp/numeric/random/chi_squared_distribution
		switch (colorChoice % 7) {
		case 0:
			color = glm::vec3(1.0f, 1.0f, 0.0f); //yellow
			break;
		case 1:
			color = glm::vec3(1.0f); //white
			break;
		case 2:
			color = glm::vec3(1.0f, 0.0f, 0.0f); //red
			break;
		case 3:
			color = glm::vec3(0.7f, 0.7f, 1.0f); //blueish white
			break;
		case 4:
			color = glm::vec3(0.0f, 0.0f, 1.0f); //blue
			break;
		case 5:
			color = glm::vec3(1.0f, 0.325f, 0.286f); //red orange
			break;
		case 6:
			color = glm::vec3(1.0f, 1.0f, 0.5f); //yellowish white
			break;
		}

		float intensityFactor = std::sqrt(intensity) / std::sqrt(7.0);
		color *= intensityFactor;

		imageBuff[x][y][0] += color.r; imageBuff[x][y][1] += color.g; imageBuff[x][y][2] += color.b;

		attenuate(color);
		if (glm::length(color) == 0.0)
			continue;

		imageBuff[x+1][y][0] += color.r; imageBuff[x+1][y][1] += color.g; imageBuff[x+1][y][2] += color.b;
		imageBuff[x][y+1][0] += color.r; imageBuff[x][y+1][1] += color.g; imageBuff[x][y+1][2] += color.b;
		imageBuff[x-1][y][0] += color.r; imageBuff[x-1][y][1] += color.g; imageBuff[x-1][y][2] += color.b;
		imageBuff[x][y-1][0] += color.r; imageBuff[x][y-1][1] += color.g; imageBuff[x][y-1][2] += color.b;

		attenuate(color);
		if (glm::length(color) == 0.0)
			continue;

		imageBuff[x+2][y][0] += color.r; imageBuff[x+2][y][1] += color.g; imageBuff[x+2][y][2] += color.b;
		imageBuff[x+1][y+1][0] += color.r; imageBuff[x+1][y+1][1] += color.g; imageBuff[x+1][y+1][2] += color.b;
		imageBuff[x][y+2][0] += color.r; imageBuff[x][y+2][1] += color.g; imageBuff[x][y+2][2] += color.b;
		imageBuff[x-1][y+1][0] += color.r; imageBuff[x-1][y+1][1] += color.g; imageBuff[x-1][y+1][2] += color.b;
		imageBuff[x-2][y][0] += color.r; imageBuff[x-2][y][1] += color.g; imageBuff[x-2][y][2] += color.b;
		imageBuff[x-1][y-1][0] += color.r; imageBuff[x-1][y-1][1] += color.g; imageBuff[x-1][y-1][2] += color.b;
		imageBuff[x][y-2][0] += color.r; imageBuff[x][y-2][1] += color.g; imageBuff[x][y-2][2] += color.b;
		imageBuff[x+1][y-1][0] += color.r; imageBuff[x+1][y-1][1] += color.g; imageBuff[x+1][y-1][2] += color.b;

		attenuate(color);
		if (glm::length(color) == 0.0)
			continue;

		imageBuff[x+3][y][0] += color.r; imageBuff[x+3][y][1] += color.g; imageBuff[x+3][y][2] += color.b;
		imageBuff[x+2][y+1][0] += color.r; imageBuff[x+2][y+1][1] += color.g; imageBuff[x+2][y+1][2] += color.b;
		imageBuff[x+1][y+2][0] += color.r; imageBuff[x+1][y+2][1] += color.g; imageBuff[x+1][y+2][2] += color.b;
		imageBuff[x][y+3][0] += color.r; imageBuff[x][y+3][1] += color.g; imageBuff[x][y+3][2] += color.b;
		imageBuff[x-1][y+2][0] += color.r; imageBuff[x-1][y+2][1] += color.g; imageBuff[x-1][y+2][2] += color.b;
		imageBuff[x-2][y+1][0] += color.r; imageBuff[x-2][y+1][1] += color.g; imageBuff[x-2][y+1][2] += color.b;
		imageBuff[x-3][y][0] += color.r; imageBuff[x-3][y][1] += color.g; imageBuff[x-3][y][2] += color.b;
		imageBuff[x-2][y-1][0] += color.r; imageBuff[x-2][y-1][1] += color.g; imageBuff[x-2][y-1][2] += color.b;
		imageBuff[x-1][y-2][0] += color.r; imageBuff[x-1][y-2][1] += color.g; imageBuff[x-1][y-2][2] += color.b;
		imageBuff[x][y-3][0] += color.r; imageBuff[x][y-3][1] += color.g; imageBuff[x][y-3][2] += color.b;
		imageBuff[x+1][y-2][0] += color.r; imageBuff[x+1][y-2][1] += color.g; imageBuff[x+1][y-2][2] += color.b;
		imageBuff[x+2][y-1][0] += color.r; imageBuff[x+2][y-1][1] += color.g; imageBuff[x+2][y-1][2] += color.b;
	}

	return 0;
}

void attenuate(glm::vec3& color) {
	static const glm::vec3 attenuation = glm::vec3(0.25f);

	color -= attenuation;
	if (color.x < 0.0f)
		color.x = 0.0f;
	if (color.y < 0.0f)
		color.y = 0.0f;
	if (color.z < 0.0f)
		color.z = 0.0f;
}