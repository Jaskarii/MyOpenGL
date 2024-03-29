#ifndef CAR_H
#define CAR_H
#include "Shader.h"
#include <string>
#include "Vector2.h"
#include "NeuralNetworkEigen.h"

struct InputSpace
{
    glm::vec2 nextPoint;
    glm::vec2 nextPointAfter;
    glm::vec2 roadDirection;
    glm::vec2 position;
    glm::vec2 direction;
    float distanceToNextPoint = 0;
    float angleOfNextIntersection = 0;
    float speed = 0.0f;
    float distanceFromRoad = 0.0f;
};

struct OutPuts
{
    float up = 0.0f;
    float down = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
};

struct CarVertex
{
    float posX = 0.0f;
    float posY = 0.0f;
    float dirX = 0.0f;
    float dirY = 1.0f;
    unsigned int isCamera = 0;
    float speed;
};

class Car
{
public:
    Car(float posX, float posY);
    ~Car();
    void Rotate(float angle);
    void Accelerate(float acc);
    void SetCrashed(bool crashed);
    bool IsCrashed();
    void Reset();
    CarVertex GetStatus();
    void SetCamera(bool isCam);
    InputSpace *getInputs();
    void GetAndHandleOutPuts(NeuralNetworkEigen *network, std::vector<float> *NNinputs);
    float calculateRelativeAngle();
    int CurrentPathIndex = 1;
    bool isTraining = true;
    bool hasAdvanced = true;
    int carIndex;

private:
    void PreRender();
    CarVertex carStatus;
    bool isCrashed = false;
    float prevDistance = 0;
    int previousPathIndex = 1;
    InputSpace inputs;
};
#endif // CAR_H