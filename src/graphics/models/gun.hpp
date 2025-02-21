#ifndef GUN
#define GUN
#include "../objects/model.h"
#include "../../io/Camera.h"
#include "../../scene.h"
class Gun :public Model {

public:
	Gun(unsigned int maxNoInstances) : Model("gun", BoundTypes::AABB, maxNoInstances, CONST_INSTANCES | NO_TEX) {}

	void render(Shader shader,float deltaTime,Scene* scene, bool setModel=false) {
		glm::mat4 model = glm::mat4(1.0f);


		// ���������� ������� ������������ ������
		rb.pos = scene->getActiveCamera()->cameraPos +
			scene->getActiveCamera()->cameraFront * 0.5f -
			scene->getActiveCamera()->cameraUp * 0.25f;

		model = glm::translate(model, rb.pos);

		// ������� ������ cameraRight (������ �����/����)
		float theta = glm::acos(glm::dot(scene->getActiveCamera()->worldUp, scene->getActiveCamera()->cameraFront)); // � �������������� �������� �� ���� ������ �� ����� ���� ��������
		model = glm::rotate(model, glm::radians(90.0f) - theta, scene->getActiveCamera()->cameraRight);

		// ������� ������ cameraUp (������ �����/������)
		glm::vec2 front2d = glm::vec2(scene->getActiveCamera()->cameraFront.x, scene->getActiveCamera()->cameraFront.z);
		theta = glm::atan(front2d.y, front2d.x); // ���������� atan2 ��� ����������� ������� ����
		model = glm::rotate(model, -theta, scene->getActiveCamera()->worldUp);


		
	
		

		if (keyboard::Key(GLFW_KEY_SPACE))
		{
			std::cout <<"\n" << scene->getActiveCamera()->cameraFront.x << ' ' <<
				scene->getActiveCamera()->cameraFront.y << ' ' <<
				scene->getActiveCamera()->cameraFront.z << ' ' << "\n"<<
				scene->getActiveCamera()->cameraUp.x << ' ' <<
				scene->getActiveCamera()->cameraUp.y << ' ' <<
				scene->getActiveCamera()->cameraUp.z << ' ' << std::endl;
		}

		//model = glm::rotate(model, glm::radians(time * 10), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, size);
		
		//model =glm::rotate(model, Camera::defaultCamera.cameraFront, glm::vec3(1.0f, 0.0f, 0.0f))
		//scale
		shader.setMat4("model", model);

		Model::render(shader,deltaTime,scene, model);
	}



};
#endif // !GUN
