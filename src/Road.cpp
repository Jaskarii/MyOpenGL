#include "Road.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../Libraries/include/glm/gtx/fast_square_root.hpp"

static std::vector<glm::vec2> roadPoints;

float calculateRelativeAngle(glm::vec2 v1, glm::vec2 v2)
{
	float dot = glm::dot(v1, v2);
	dot = std::max(-1.0f, std::min(1.0f, dot));

	float angle = std::acos(dot); // Angle in radians

	// Determine the direction of the turn
	float crossProduct = v1.x * v2.y - v1.y * v2.x;
	if (crossProduct < 0)
	{
		angle = -angle; // Turn right
	}
	return angle;
}

void Road::GenerateRandomPoints()
{
	roadPoints.push_back(glm::vec2(0.0f, -100.0f));
	roadPoints.push_back(glm::vec2(0.0f, 300.0f));
	roadPoints.push_back(glm::vec2(500.0f, 650.0f));
	roadPoints.push_back(glm::vec2(-400.0f, 1000.0f));
	roadPoints.push_back(glm::vec2(200.0f, 1400.0f));

	const float min_val = -600.0f;
	const float max_val = 600.0f;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	float yPos = 1800.0f;
	for (size_t i = 0; i < 1000; i++)
	{
		int random_int = std::rand();
		// Convert to a float between 0 and 1 and scale it to the desired range
		float random_float = static_cast<float>(random_int) / RAND_MAX;	 // Between 0 and 1
		random_float = min_val + random_float * (max_val - min_val);	 // Scale to the desired range
		float random_floaty = static_cast<float>(random_int) / RAND_MAX; // Between 0 and 1
		random_floaty = 250.0f + random_floaty * (500.0f - 50.0f);		 // Scale to the desired range
		yPos += random_floaty;
		roadPoints.push_back(glm::vec2(random_float, yPos));
	}
}

Road::Road()
{
	int numVertices = 500000; // Assuming each vertex is a Vector2 (x, y)
	positions = new glm::vec2[numVertices];
	indices = new unsigned int[1000000];
	for (size_t i = 0; i < roadPoints.size(); i++)
	{
		SetNewPath(roadPoints[i]);
	}
}

Road::~Road()
{
}

void Road::Render(glm::mat4 MVP)
{
	va->Bind();
	ibo->Bind();
	shader->Bind();
	shader->SetUniformMat4f("MVP", MVP);

	if (indicesCount > 1)
	{
		glDrawElements(GL_LINES, indicesCount, GL_UNSIGNED_INT, nullptr);
	}
}

void Road::UpdateCarStatus(Car *car, std::vector<float> *NNinputs)
{
	InputSpace *inputs = car->getInputs();

	if (car->IsCrashed())
	{
		return;
	}

	int carIndex = car->carIndex;
	int currentIndex = car->CurrentPathIndex;
	float distance1 = DistanceToLineSegment(inputs->position, positions[currentIndex], positions[currentIndex - 1]);
	float distance2 = DistanceToLineSegment(inputs->position, positions[currentIndex], positions[currentIndex + 1]);

	float absDistance1 = std::abs(distance1);
	float absDistance2 = std::abs(distance2);
	float distFromRoad = 0;
	if ((absDistance2 < width))
	{
		car->CurrentPathIndex++;
		currentIndex++;
		InitNewPathSegment(car, NNinputs, currentIndex);
		distFromRoad = distance2 / 100.0f;
	}
	else
	{
		distFromRoad = distance1 / 100.0f;
	}

	float distancetoNextPoint = glm::distance(inputs->position, positions[currentIndex]);

	car->SetCrashed(std::min(absDistance1, absDistance2) > width);
	inputs->position += glm::fastNormalize(inputs->direction) * inputs->speed;
	float relativeAngle = car->calculateRelativeAngle();
	relativeAngle = relativeAngle / 3.0f;
	relativeAngle = std::max(relativeAngle, -1.0f);
	relativeAngle = std::min(relativeAngle, 1.0f);

	// hasAdvanced > 0.5f;
	distancetoNextPoint = std::min(1.0f, distancetoNextPoint / 600.0f);
	distFromRoad *= 0.01f;

	distFromRoad = std::max(distFromRoad, -1.0f);
	distFromRoad = std::min(distFromRoad, 1.0f);

	NNinputs->at(5 * carIndex + 1) = relativeAngle;
	NNinputs->at(5 * carIndex + 2) = distancetoNextPoint;
	NNinputs->at(5 * carIndex + 3) = distFromRoad;
	NNinputs->at(5 * carIndex + 4) = inputs->speed / 6.0f;
}

void Road::InitNewPathSegment(Car *car, std::vector<float> *NNinputs, int ctPathIndex)
{
	InputSpace *inputs = car->getInputs();
	int carIndex = car->carIndex;
	glm::vec2 roadDir;
	roadDir.x = positions[ctPathIndex].x - positions[ctPathIndex - 1].x;
	roadDir.y = positions[ctPathIndex].y - positions[ctPathIndex - 1].y;
	inputs->roadDirection = glm::normalize(roadDir);
	inputs->nextPoint.x = positions[ctPathIndex].x;
	inputs->nextPoint.y = positions[ctPathIndex].y;
	inputs->nextPointAfter.x = positions[ctPathIndex + 1].x;
	inputs->nextPointAfter.y = positions[ctPathIndex + 1].y;
	glm::vec2 nextRoadDir;
	nextRoadDir.x = positions[ctPathIndex + 1].x - positions[ctPathIndex].x;
	nextRoadDir.y = positions[ctPathIndex + 1].y - positions[ctPathIndex].y;
	inputs->distanceToNextPoint = glm::distance(inputs->position, positions[ctPathIndex]);
	float relativeCornerAngle = calculateRelativeAngle(glm::normalize(roadDir), glm::normalize(nextRoadDir));
	relativeCornerAngle = relativeCornerAngle / 3.0f;
	relativeCornerAngle = std::max(relativeCornerAngle, -1.0f);
	relativeCornerAngle = std::min(relativeCornerAngle, 1.0f);
	NNinputs->at(5 * carIndex) = relativeCornerAngle;
	inputs->angleOfNextIntersection = relativeCornerAngle;
}

glm::vec2 *Road::getPositionArray()
{
	return positions;
}

void Road::InitBuffers()
{
	vbo = new VertexBuffer(positions, 1000000 * sizeof(float));
	ibo = new IndexBuffer(indices, 1000000);
	shader = new Shader("../shaders/Road.glsl");
	shader->Bind();
	shader->SetUniform1f("Width", width);

	va = new Vertexarray();
	VertexBufferLayout layout;
	layout.PushFloat(2);
	va->AddBuffer(*vbo, layout);
}

void Road::SetNewPath(glm::vec2 pos)
{
	// Update positions
	positions[positionCount] = pos;
	positionCount++; // Increment by the size of one position (x and y)

	// Update position buffer data
	// glBufferSubData(GL_ARRAY_BUFFER, 0, positionCount * 2 * sizeof(float), positions);

	int prevElementCount = indicesCount;
	// Update indices
	if (vertexCount > 1)
	{
		indices[indicesCount] = vertexCount - 1; // Assuming 0-based index
		indicesCount++;
	}

	indices[indicesCount] = vertexCount;
	indicesCount++;

	// glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesCount * sizeof(unsigned int), indices);

	vertexCount++;
}
