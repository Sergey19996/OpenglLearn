#include "collisionmesh.h"
#include "collisionmodel.h"
#include "../algorithms/cmathematis/matrix.h"


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

#define CASE0 (char)0
#define CASE1 (char)1
#define CASE2 (char)2
#define CASE3 (char)3

char linePlaneIntersection(vec P1, vec norm, vec U1, vec D, float& t) {  // U1 - точка, D(side) - направляющий вектор точки U1,  P1 - любая точка на плоскости 
	//calculate the parametr t of the line { U1 + D * t} at the point of intersection
	// t =	(N dot U1P1) / (N dot U1 U2)

	vec U1P1 = vecSubtract(P1, U1);  // (P1 - U1) //под формулу пересечения прямой с плоскотью

	//calculate numerator and denominator
	// t = N * (P1 - U)
	//     ------------
	//       N * D
	float tnum = dot(norm, U1P1); // числитель   // нормаль и дистанция от точки к плоскасти 
	float tden = dot(norm, D); // Знаменатель    нормаль и направление точки
	if (tden == 0.0f){ // возвращает косинус 0 - то есть направление точки !!перпендикулярно!! нормали плоскати p1(направление линии параллельно плокости)
	//нет смысла дальше считать
		return tnum == 0.0f ? CASE0 : CASE1; // если dot кос 0 = точка на плоскости? да : линия параллельна плоскости (cos 1.0 = 0 градусов параллельны(сонаправлены) векторы) 
		// когда направвление точно парраллельно плейну(перпендикулярно нормали) если получился 0 в tnum - значит точка на плоскасти, есл нет значит не на плоскасти

	}
	else { // исключив параллельное напревление точки 

	float t = tnum / tden;  // если t = 0 - мы на плоскости, если t > 0 - пересечение впереди по направлению D, t < 0 - позади по направлению D
	return t >= 0.0f && t <= 1.0f ? CASE2 : CASE3;

	}




}

bool Face::collidesWidth(Face& face){
	// Преобразуем координаты так, чтобы точка P1 стала началом координат (origin)
	vec P1 = this->mesh->points[this->i1]; // P1 - первая точка текущего треугольника (или грани)
	vec P2 = vecSubtract(this->mesh->points[this->i2], P1); // P2 - вторая точка, преобразованная относительно P1
	vec P3 = vecSubtract(this->mesh->points[this->i3], P1); // P3 - третья точка, преобразованная относительно P1

	// Создаем массив векторов, которые задают плоскость текущего треугольника
	vec lines[3] = { // как будто бы треугольник в начале координат,где P1 в 0 
		P2,  // Вектор от P1 до P2 (P2 - P1)
		P3,  // Вектор от P1 до P3 (P3 - P1)
		vecSubtract(P3, P2)  // Вектор от P2 до P3 (P3 - P2)
		// Этот вектор также можно интерпретировать как разность векторов P3 и P2
	};

	// Преобразуем координаты точек другой грани (face) относительно текущей системы координат (с началом в P1)
	vec U1 = vecSubtract(face.mesh->points[face.i1], P1); // U1 - первая точка грани face, преобразованная относительно P1
	vec U2 = vecSubtract(face.mesh->points[face.i2], P1); // U2 - первая точка грани face, преобразованная относительно P2
	vec U3 = vecSubtract(face.mesh->points[face.i3], P1); // U3 - первая точка грани face, преобразованная относительно P3

	vec sideOrigin[3] = { // точки которые мы будем проверять на коллизию U
		U1,
		U1,
		U2
	};
	vec sides[3] = {   // направляющий вектор  D  для точек U
		vecSubtract(U2, U1),
		vecSubtract(U3, U1),
		vecSubtract(U3, U2),
	};

	// set P1 as the origin (zero vector)
	P1.elements[0] = 0.0f; P1.elements[1] = 0.0f; P1.elements[2] = 0.0f;

	printVec(P1);
	printVec(P2);
	printVec(P3);
	printVec(U1);
	printVec(U2);
	printVec(U3);



	
	//placeholders
	float c1, c2;

	//iterate through each bounding line of the face
	//check intersections
	for (unsigned int i = 0; i < 3; i++) {
		//get intersection with this plane
		float t = 0;
		char currentCase = linePlaneIntersection(P1, this->norm, sideOrigin[i], sides[i], t);


		switch (currentCase)
		{
		case CASE0:
			//line in the plane, determine the intersection with the 3 bounding lines of this face
			for (int j = 0; j < 3; j++) {
				mat m = newColMat(3, 3, lines[j], vecScalarMultiplication(sides[i], -1.0f), sideOrigin[i]); // делаем D -вектор движение напрвелным в противоположную сторону

				//do RREF
				rref(&m);

				if (m.elements[2][2] != 0.0f) {
						//no intersection with the lines
					continue;

				}
				c1 = m.elements[0][2];
				c2 = m.elements[1][2];

				if (0.0f <= c1 && c1 <= 1.0f &&
					0.0f <= c2 && c2 <= 1.0f) {
					return true;

				}

			}
			continue;
			 
			
		case CASE1:
			// no intersection => no collision
			continue;


			
		case CASE2:
			// intersection in the plane, in range 
			// determine if inside this triangle (bounded by P1, P2, P3)

			//get point of intersection
			vec intersection = vecAdd(sideOrigin[i], vecScalarMultiplication(sides[i], t));  // U + t * D

			//represent the intersection point as a linear combination
			mat m = newColMat(3, 4, P2, P3, this->norm, intersection);

			// do RREF
			rref(&m);


			//obtain the coefficients for the linear combinations
			// c3 ~= 0.0 becouse the point is in the plane
			c1 = m.elements[0][3];
			c2 = m.elements[1][3];
			// cпросить у Ai
			if (c1 >= 0.0f && c2 >= 0.0f && c1 + c2 <= 1.0f) {
				return true;


			}

			continue;

		
		case CASE3:  // пересечём в будущем но по вектору ещё долго нужно идти  U +t * D
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
		points[i] = newVector(3, coordinates[i * 3 + 0], coordinates[i * 3 + 1], coordinates[i * 3 + 2]);
	}

	//calculate face normals
	for (unsigned int i = 0; i < noFaces; i++) {
		unsigned int i1 = indices[i * 3 + 0];
		unsigned int i2 = indices[i * 3 + 1];
		unsigned int i3 = indices[i * 3 + 2];


		vec A = vecSubtract(points[i2], points[i1]); //A = P2 - P1  1 вектор для face
		vec B = vecSubtract(points[i3], points[i1]); // B = p3 - p1 2 вектор для face
		vec N = cross(A, B); // N = A * B

		faces[i] = {
			this,
			i1,i2,i3, // indies making up the triangles
			N,		  // normal
			N		  // transformed normal


		};
	}

}
