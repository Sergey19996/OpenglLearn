#include "ray.h"

#include "cmathematis/linalg.h"
#include <limits.h>

Ray::Ray(glm::vec3 origin, glm::vec3 d) : origin(origin), dir(d),invDir(1.0f){
	for (int i = 0; i < 3; i++) {
		invDir[i] = invDir[i] / dir[i];  // 1 / dir
	}
}

bool Ray::intersectsBoundingRegion(BoundingRegion br, float& tmin, float& tmax){
	if (br.type == BoundTypes::AABB) {
		////slab algortihm
		
		tmin = std::numeric_limits<float>::lowest();
		tmax = std::numeric_limits<float>::max();

		for (int i = 0; i < 3; i++) {
			float t1 = (br.min[i] - origin[i]) * invDir[i];  //  даёт коэффициенты для пересечения с min x,y,z
			float t2 = (br.max[i] - origin[i]) * invDir[i]; //  даёт коэффициенты для пересечения с min x,y,z

			tmin = std::fmax(tmin, std::fmin(t1, t2));
			tmax = std::fminf(tmax, std::fmax(t1, t2));


		}

		return (tmax >= tmin) && tmax >= 0.0f;
	}
	else{
		//ray -sphere collision
		//plug in line equation of ray into sphere equation
		//get quadratic equation for t, solve for roots

		glm::vec3 cp = origin - br.center; // vector from center br to origin 

		//calculate coefficients of the quadratic
		float a = magsq(dir); // dist без выведения корня
		float b = 2.0f * glm::dot(dir, cp); //произведение двух векторов удлинённое в 2 раза
		float c = magsq(cp) - br.radius * br.radius;

		//get discriminant
		float D = b * b - 4.0f * a * c;
		if (D < 0) {
			//no real root
			return false;
		}
		D = sqrtf(D); //извлекаем корень

		//get two roots using quadratic formula
		tmax = (-b + D) / (2.0f * a);
		tmin = ((-b - D) / (2.0f * a));

		return true;


	}

	
}

bool Ray::intersectsMesh(CollisionMesh* mesh, RigidBody* rb, float& t){
	bool intersects = false;
	


	for (Face& f : mesh->faces) { // Бежим по всем faceам в меше
		float tmp = -1.0f;

		//берём матрицу текущего инстансера и преборазуем точки в настоящие системы координат, только 0 находится в P1
		//что бы упрастить формулу AX + BY + CZ + D - мы убираем оффсет D, теперь формула будет   A * X + B * Y + C * Z - плоскость
		glm::vec3 P1 = mat4vec3mult(rb->model, mesh->points[f.i1]);
		glm::vec3 P2 = mat4vec3mult(rb->model, mesh->points[f.i2]) - P1;
		glm::vec3 P3 = mat4vec3mult(rb->model, mesh->points[f.i3]) - P1;

		glm::vec3 norm = rb->normalModel * f.norm; //f.norm - это нормаль до преобразований, rb->normalModel - это матрица c уёчтом изменений

		//луч
		glm::vec3 U1 = origin - P1; //origin - позицая источника луча в мире, получаем вектор от p1 к источнику
		
		LinePlaneIntCase intCase = linePlaneIntersection(glm::vec3(0.0f), norm, U1, dir, tmp); // получаем коэффийент tmp, origin + dir * tmp - дасат нам точку пересечения
		//Case 0 - vector lies in the plane
		//Case 2 - there is the planar intersection  0 > t > 1
		//Case 3 - intersection far than [0,1] or behind

		if ((char)intCase  >  1) {  //case 2 or case 3
			//intersection with the infinity plane at one point
			if (tmp < 0.0f || t < tmp) { // для луча мы не рассматриваем отрицательное пересечение
				//collision happens behind the origin
				//or have found a closer collision
				continue;
			}
			//get point of intersection
			glm::vec3 intersection = U1 + tmp * dir;
			if (faceContainsPoint(P2, P3, norm, intersection)) { //решает систему линейных уравнений
				intersects = true;
				t = fminf(t, tmp); //take min value between t and tmp


			 }
		}


	}


	return intersects;
}
