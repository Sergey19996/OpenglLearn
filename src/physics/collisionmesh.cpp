#include "collisionmesh.h"
#include "collisionmodel.h"
#include "../algorithms/cmathematis/linalg.h"


// Line - plane intersection cases
// PI - is the plane containing the vectors P1P2 and P3
// CASE 0
// 
// line U1U2 lies in the plane PI
// CASE 1
// 
// no planar intersection
// t = (R != 0) / 0
// => intersection when
// !(tnum != 0 && tden ==0)
// =tnum == 0 || tden != 0 (DeMorgan)
// CASE 2
// 
// planar intersection, in between U1 and U2
// t = R / (R != 0) in the range [0,1]
// CASE 3
// planar intersection, outside of U1 and U2
// t = R / (R != 0) not in the range [0,1]



bool Face::collidesWidth(Face& face){
	// Преобразуем координаты так, чтобы точка P1 стала началом координат (origin)
	glm::vec3 P1 = this->mesh->points[this->i1]; // P1 - первая точка текущего треугольника (или грани)
	glm::vec3 P2 = this->mesh->points[this->i2] - P1; // P2 - вторая точка, преобразованная относительно P1
	glm::vec3 P3 = this->mesh->points[this->i3] - P1; // P3 - третья точка, преобразованная относительно P1

	// Создаем массив векторов, которые задают плоскость текущего треугольника
	glm::vec3 lines[3] = { // как будто бы треугольник в начале координат,где P1 в 0 
		P2,  // Вектор от P1 до P2 (P2 - P1)
		P3,  // Вектор от P1 до P3 (P3 - P1)
		P3 - P2  // Вектор от P2 до P3 (P3 - P2)
		// Этот вектор также можно интерпретировать как разность векторов P3 и P2
	};

	// Преобразуем координаты точек другой грани (face) относительно текущей системы координат (с началом в P1)
	glm::vec3 U1 = face.mesh->points[face.i1] - P1; // U1 - первая точка грани face, преобразованная относительно P1
	glm::vec3 U2 = face.mesh->points[face.i2] - P1; // U2 - первая точка грани face, преобразованная относительно P2
	glm::vec3 U3 = face.mesh->points[face.i3] - P1; // U3 - первая точка грани face, преобразованная относительно P3

	glm::vec3 sideOrigin[3] = { // точки которые мы будем проверять на коллизию U
		U1,
		U1,
		U2
	};
	glm::vec3 sides[3] = {   // направляющий вектор  D  для точек U
		U2 - U1,
		U3 - U1,
		U3 - U2,
	};

	// set P1 as the origin (zero vector)
	P1[0] = 0.0f; P1[1] = 0.0f; P1[2] = 0.0f;

	


	
	//placeholders
	float c1, c2;

	//iterate through each bounding line of the face
	//check intersections
	for (unsigned int i = 0; i < 3; i++) {
		//get intersection with this plane
		float t = 0;
		LinePlaneIntCase currentCase = linePlaneIntersection(P1, this->norm, sideOrigin[i], sides[i], t);


		switch (currentCase)
		{
		case LinePlaneIntCase::CASE0:
			//line in the plane, determine the intersection with the 3 bounding lines of this face
			for (int j = 0; j < 3; j++) {
				glm::mat3 m(lines[j], sides[i] * -1.0f, sideOrigin[i]); // делаем D -вектор движение напрвелным в противоположную сторону

				//do RREF
				rref(m);

				if (m[2][2] != 0.0f) {
						//no intersection with the lines
					continue;

				}
				c1 = m[0][2];
				c2 = m[1][2];

				if (0.0f <= c1 && c1 <= 1.0f &&
					0.0f <= c2 && c2 <= 1.0f) {
					return true;

				}

			}
			continue;
			 
			
		case LinePlaneIntCase::CASE1:
			// no intersection => no collision
			continue;


			
		case LinePlaneIntCase::CASE2:
			// intersection in the plane, in range 
			// determine if inside this triangle (bounded by P1, P2, P3)

			//get point of intersection
			glm::vec3 intersection = sideOrigin[i] + sides[i] * t;  // U + t * D

			if (faceContainsPoint(P2, P3, this->norm, intersection)) {
				return true;
			}



			continue;

		
		case LinePlaneIntCase::CASE3:  // пересечём в будущем но по вектору ещё долго нужно идти  U +t * D
			//intersection in the plane, out of range
			//no collision 
			continue;

			break;
		}

	}

    return false;
}

CollisionMesh::CollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices) : points(noPoints),faces(noFaces){
	//insert all points into list
	
	for (unsigned int i = 0; i < noPoints; i++){ 
		points[i] = { coordinates[i * 3 + 0], coordinates[i * 3 + 1], coordinates[i * 3 + 2] };
	}

	//calculate face normals
	for (unsigned int i = 0; i < noFaces; i++) {
		unsigned int i1 = indices[i * 3 + 0];
		unsigned int i2 = indices[i * 3 + 1];
		unsigned int i3 = indices[i * 3 + 2];


		glm::vec3 A = points[i2] - points[i1]; //A = P2 - P1  1 вектор для face
		glm::vec3 B = points[i3] - points[i1]; // B = p3 - p1 2 вектор для face
		glm::vec3 N = cross(A, B); // N = A * B

		faces[i] = {
			this,
			i1,i2,i3, // indies making up the triangles
			N,		  // normal
			N		  // transformed normal


		};
	}

}
