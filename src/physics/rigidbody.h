#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>
#include <string>

#define INSTANCE_DEAD  (unsigned char)0b00000001
#define INSTANCE_MOVED (unsigned char)0b00000010
class RigidBody {

public:

	unsigned char state;

	// mass in kg
	float mass;

	// pos in meters
	glm::vec3 pos;


	//m/s
	glm::vec3 velocity;

	//m/s^2
	glm::vec3 acceleration;
	
	glm::vec3 size;

	//rotation in Eueler angles
	glm::vec3 rotation;

	//model matrix
	glm::mat4 model;
	glm::mat3 normalModel;


	//ids for quick access to instance/model
	std::string modelId;
	std::string instanceId;

	bool operator==(RigidBody rb);
	bool operator==(std::string id);


	RigidBody(std::string modelId = "",
		glm::vec3 size = glm::vec3(1.0f),
		float mass = 1.0f,
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 rot = glm::vec3(0.0f));

	void update(float deltatime);

	void applyForce(glm::vec3 force);
	void applyForce(glm::vec3 direction, float magnitude);

	void applyAcceleration(glm::vec3 Acceleration);
	void applyAcceleration(glm::vec3 direction, float magnitude);

	void applyImpulse(glm::vec3 force, float deltaTime);
	void applyImpulse(glm::vec3 direction, float magnitude,float deltaTime);

	void transferEnergy(float joules,glm::vec3 direction);


};
#endif // !1
