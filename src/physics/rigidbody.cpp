#include "rigidbody.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

bool RigidBody::operator==(RigidBody rb)
{
	return instanceId == rb.instanceId;
}

bool RigidBody::operator==(std::string id)
{
	return instanceId == id;
}



RigidBody::RigidBody(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos,glm::vec3 rot) :
	modelId(modelId),size(size),mass(mass),pos(pos),rotation(rot),velocity(0.0f),acceleration(0.0f),state(0){
	update(0.0f);

	//calculate rotation matrix
	glm::mat4 rotMat = glm::toMat4(glm::quat(rot));

	//model = trans * rot * scale   = T * R * S
	model = glm::translate(glm::mat4(1.0f), pos); // M  = I * T
	model = model * rotMat; // M = M * R = T * R
	model = glm::scale(model, size); // M = M * S = T * R * S

	normalModel = glm::transpose(glm::inverse(glm::mat3(model)));

}

void RigidBody::update(float deltaTime)
{
 	//    |                    | |     when acceleration > 0                   |  
	pos += velocity * deltaTime + 0.5f * acceleration * (deltaTime * deltaTime);
	velocity += acceleration * deltaTime; // что бы учесть линейный рост скорость от ускарения


}

void RigidBody::applyForce(glm::vec3 force)
{
	acceleration += force / mass; // тут мы находим ускорение из силы 

}

void RigidBody::applyForce(glm::vec3 direction, float magnitude)
{
	//направление на длину даст вектор который олицетворяет силу  в этом направлении
	applyForce(direction * magnitude);
}

void RigidBody::applyAcceleration(glm::vec3 Acceleration)
{
	acceleration += Acceleration;
}

void RigidBody::applyAcceleration(glm::vec3 direction, float magnitude)
{
	applyAcceleration(direction * magnitude);
}

void RigidBody::applyImpulse(glm::vec3 force, float deltaTime)
{
	velocity += force / mass * deltaTime; // тут мы находим ускорение из силы   p = m * v  кг*м/c
}

void RigidBody::applyImpulse(glm::vec3 direction, float magnitude, float deltaTime)
{
	applyImpulse(direction * magnitude, deltaTime);
}

void RigidBody::transferEnergy(float joules, glm::vec3 direction)
{
	if (joules == 0){
		return;
	}
	// comes from formula : KE = 1/2 * m * v^2
	glm::vec3 deltaV = sqrt(2 * abs(joules) / mass) * direction;

	velocity += joules > 0 ? deltaV : -deltaV;

}
