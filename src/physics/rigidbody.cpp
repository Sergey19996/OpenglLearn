#include "rigidbody.h"



bool RigidBody::operator==(RigidBody rb)
{
	return instanceId == rb.instanceId;
}

bool RigidBody::operator==(std::string id)
{
	return instanceId == id;
}

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos) :
	modelId(modelId),size(size),mass(mass),pos(pos),velocity(0.0f),acceleration(0.0f),state(0)
{
}

void RigidBody::update(float deltaTime)
{
 	//    |                    | |     when acceleration > 0                   |  
	pos += velocity * deltaTime + 0.5f * acceleration * (deltaTime * deltaTime);
	velocity += acceleration * deltaTime; // ��� �� ������ �������� ���� �������� �� ���������


}

void RigidBody::applyForce(glm::vec3 force)
{
	acceleration += force / mass; // ��� �� ������� ��������� �� ���� 

}

void RigidBody::applyForce(glm::vec3 direction, float magnitude)
{
	//����������� �� ����� ���� ������ ������� ������������ ����  � ���� �����������
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
	velocity += force / mass * deltaTime; // ��� �� ������� ��������� �� ����   p = m * v  ��*�/c
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
