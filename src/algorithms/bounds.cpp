#include "bounds.h"
// Конструктор для инициализации типа региона
BoundingRegion::BoundingRegion(BoundTypes type) : type(type) {}

// Конструктор для сферы, задаём центр и радиус
BoundingRegion::BoundingRegion(glm::vec3 center, float radius) :
    type(BoundTypes::SPHERE), center(center), radius(radius) {}

// Конструктор для AABB, задаём минимальные и максимальные точки
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max) :
    type(BoundTypes::AABB), min(min), max(max) {}

// Метод для вычисления центра региона
glm::vec3 BoundingRegion::calculateCenter() {
    // Если это AABB, центр — среднее арифметическое между min и max
    // Если это сфера, возвращается её центр
    return (type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

// Метод для вычисления размеров региона
glm::vec3 BoundingRegion::calculateDimensions() {
    // Для AABB — разность между max и min   // его истенные размеры (бокса) или вектор который описывает пространство бокса 
    // Для сферы — диаметр (2 * радиус) одинаковый по всем осям
    return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

// Метод для проверки, содержится ли точка внутри региона
bool BoundingRegion::containspoint(glm::vec3 point) {
    if (type == BoundTypes::AABB) {
        // Для AABB точка должна быть больше или равна min и меньше или равна max по всем осям
        return (point.x >= min.x) && (point.x <= max.x) &&
            (point.y >= min.y) && (point.y <= max.y) &&
            (point.z >= min.z) && (point.z <= max.z);
    }
    else {
        // Для сферы расстояние от точки до центра должно быть меньше или равно радиусу
        float distSquare = 0.0f;
        for (int i = 0; i < 3; i++) {
            // Вычисляем квадрат расстояния (x^2 + y^2 + z^2)
            distSquare += (center[i] - point[i]) * (center[i] - point[i]);
        }
        return distSquare <= (radius * radius);
    }
}

// Метод для проверки, полностью ли регион `br` содержится внутри текущего региона
bool BoundingRegion::containsRegion(BoundingRegion br) {
    if (br.type == BoundTypes::AABB) {
        // Для AABB: текущий регион должен содержать обе точки `min` и `max` другого региона
        return containspoint(br.min) && containspoint(br.max);
    }
    else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
        // Для двух сфер: расстояние между их центрами + радиус вложенной сферы 
        // должно быть меньше радиуса текущей сферы
        return glm::length(center - br.center) + br.radius <= radius;
    }
    else {
        // Если текущий регион — AABB, а вложенный регион — сфера
        if (!containspoint(br.center)) {
            // Если центр сферы за пределами AABB, возвращаем `false`
            return false;
        }

        // Проверяем расстояние от центра сферы до каждой стороны AABB
        for (int i = 0; i < 3; i++) {
            // Если центр сферы слишком близко к любой грани AABB
            // (меньше радиуса сферы), значит, сфера выходит за границы AABB
            if (abs(max[i] - br.center[i]) < br.radius || abs(br.center[i] - min[i]) < br.radius) {
                return false;
            }
        }
    }

    // Если ни одно из условий выше не нарушено, регион содержится
    return true;
}

 //частично пересекает элемент
bool BoundingRegion::intersectsWith(BoundingRegion br)
{// overlap on all axes
    if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
        //both boxes  //  calculateDimensions() возвращает истеный размер коробки 
        glm::vec3 rad = calculateDimensions() / 2.0f;  // radius of this box;
        glm::vec3 radBr = br.calculateDimensions() / 2.0f; // radius of br

        glm::vec3 center = calculateCenter();           //center of this box
        glm::vec3 centerBr = br.calculateCenter();      //center of other box

        glm::vec3 dist = abs(center - centerBr);

        for (int i = 0; i < 3; i++)
        {
            if (dist[i] > rad[i] + radBr[i]) {
                //no overlap on this axis
                return false; // Если расстояние больше суммы радиусов по одной из осей, пересечения нет.

            }
        }
        //failed to prove wrong on each axes
        return true;
    }
    else if (type == BoundTypes::SPHERE && br.type ==BoundTypes::SPHERE)
    {  // both spheres
        return glm::length(center - br.center) < (radius + br.radius);
    }
    else if(type == BoundTypes::SPHERE)
    {  //this is a sphere  br is a box
        
        //determinate if sphere is above top, below bottom,etc
        // find the distance(squared) to the closet plane
        float distSquared = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            if (center[i] < br.min[i]) {  //Центр сферы находится "за границей" коробки, перед её минимальной гранью.
                // Центр сферы за пределами минимальной границы коробки по оси i.
                distSquared += (br.min[i] - center[i]) * (br.min[i] - center[i]);

            }
            else if (center[i] > br.max[i])
            {   // Центр сферы за пределами максимальной границы коробки по оси i.
                distSquared += (center[i] - br.max[i]) * (center[i] - br.max[i]);

            }
            // Если центр сферы между min и max, ничего не добавляем.

        }
        return distSquared < (radius * radius);  // Если квадрат расстояния меньше квадрата радиуса, сфера пересекает коробку.
    }
    else
    {//this is a box , br is a sphere
        //call algorithm for br (defined in preceding if block)
        return br.intersectsWith(*this);  //просто свичаем как будто бы мы сфера а br box

    }


    //failed to prove wrong on each axis
	return false;
}

bool BoundingRegion::operator==(BoundingRegion br){
    if (type != br.type) {
        return false;
    }
    if (type == BoundTypes::AABB) {

        return min == br.min && max == br.max;
    }
    else
    {
        return center == br.center && radius ==br.radius;
    }
    return true;
}
