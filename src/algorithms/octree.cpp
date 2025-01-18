#include "octree.h"

void Octree::calculateBounds(BoundingRegion* out, Octant octant, BoundingRegion parentRegion)
{
    // Вычисляем центр родительского региона.
    // Центр разделяет пространство родительского региона на 8 октантов.
    glm::vec3 center = parentRegion.calculateCenter();

    // Проверяем, какой октант выбран, и на основе этого создаем соответствующий регион.
    if (octant == Octant::O1) {
        // Первый октант: от центра до максимальных координат родительского региона.
        out = new BoundingRegion(center, parentRegion.max);
    }
    else if (octant == Octant::O2) {
        // Второй октант: сдвиг от минимальной x и центральной y, z
        // до центральной x и максимальной y, z.
        out = new BoundingRegion(
            glm::vec3(parentRegion.min.x, center.y, center.z),
            glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O3) {
        // Третий октант: от минимальных x, y и центральной z 
        // до центральной x, y и максимальной z.
        out = new BoundingRegion(
            glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z),
            glm::vec3(center.x, center.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O4) {
        // Четвертый октант: от центральной x, минимальной y и центральной z
        // до максимальной x, y и z.
        out = new BoundingRegion(
            glm::vec3(center.x, parentRegion.min.y, center.z),
            glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z)
        );
    }
    else if (octant == Octant::O5) {
        // Пятый октант: от центра до максимальных координат по x, y,
        // но минимальных по z.
        out = new BoundingRegion(
            glm::vec3(center.x, center.y, parentRegion.min.z),
            glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z)
        );
    }
    else if (octant == Octant::O6) {
        // Шестой октант: от минимальных x, центральной y и минимальной z
        // до центральных x, y и z.
        out = new BoundingRegion(
            glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z),
            glm::vec3(center.x, parentRegion.max.y, center.z)
        );
    }
    else if (octant == Octant::O7) {
        // Седьмой октант: от минимальных координат до центра.
        out = new BoundingRegion(parentRegion.min, center);
    }
    else if (octant == Octant::O8) {
        // Восьмой октант: от центральной x, минимальных y и z
        // до максимальных координат.
        out = new BoundingRegion(
            glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z),
            glm::vec3(parentRegion.max.x, center.y, center.z)
        );
    }


}

Octree::node::node() : region(BoundTypes::AABB)
{
}

Octree::node::node(BoundingRegion bounds) : region(bounds)
{
}

Octree::node::node(BoundingRegion bounds, std::vector<BoundingRegion> objectList) : region(bounds)
{ 
    objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::node::build()
{
    if (treeBuilt && treeReady) {
        // Если дерево построено и готово, начинаем обновление.

        // Создаем список для хранения объектов, которые переместились со своего предыдущего положения.
        std::vector<BoundingRegion> movedObjects(objects.size());
        // TODO: Реализация логики определения перемещенных объектов.

        // Удаляем "мертвые" объекты, которые больше не существуют.
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            // Удалить объект, если он находится в списке удаленных объектов.
            // TODO: Реализация проверки "мертвых" объектов.
        }

        // Если у узла есть дочерние узлы, обновляем их.
        if (children != nullptr) {
            // Перебираем все активные октанты (дочерние узлы).
            for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++) {
                // Проверяем, активен ли текущий октант.
                if (States::isIndexActive(&flags, 0)) {
                    // Если октант активен и существует соответствующий дочерний узел.
                    if (children[i] != nullptr) {
                        // TODO: Реализация обновления дочернего узла.
                    }
                }
            }
        }

        // Обрабатываем объекты, которые переместились в новое место.
        BoundingRegion movedObj;
        while (movedObjects.size() != 0)
        {
            // Получаем первый перемещенный объект.
            movedObj = movedObjects[0];
            node* current = this; // Начинаем с текущего узла.

            // Поднимаемся вверх по дереву, пока не найдем узел, который полностью включает объект.
            while (!current->region.containsRegion(movedObj))
            {
                if (current->parent != nullptr) {
                    current = current->parent; // Переходим к родительскому узлу.
                }
                else {
                    break; // Если достигли корня дерева, выходим из цикла.
                }
            }

            // Удаляем первый объект из списка перемещенных объектов.
            movedObjects.erase(movedObjects.begin());

            // Удаляем объект из текущего узла.
            objects.erase(objects.begin() + List::getIndexOf<BoundingRegion>(objects, movedObj));

            // Вставляем объект в найденный подходящий узел.
            current->insert(movedObj);

            // TODO: Реализация обнаружения столкновений, если требуется.
        }

    }
    else {
        // Если дерево еще не готово, обрабатываем ожидающие объекты.
        if (queue.size() > 0) {
            processPending();
        }
    }
}
void Octree::node::update()
{
    if (treeBuilt && treeReady) {
        // Если дерево построено и готово, начинаем обновление.

        // Создаем список для хранения объектов, которые переместились со своего предыдущего положения.
        std::vector<BoundingRegion> movedObjects(objects.size());
        // TODO: Реализация логики определения перемещенных объектов.

        // Удаляем "мертвые" объекты, которые больше не существуют.
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            // Удалить объект, если он находится в списке удаленных объектов.
            // TODO: Реализация проверки "мертвых" объектов.
        }

        // Если у узла есть дочерние узлы, обновляем их.
        if (children != nullptr) {
            // Перебираем все активные октанты (дочерние узлы).
            for (unsigned char flags = activeOctants, i = 0; flags > 0; flags >>= 1, i++) {
                // Проверяем, активен ли текущий октант.
                if (States::isIndexActive(&flags, 0)) {
                    // Если октант активен и существует соответствующий дочерний узел.
                    if (children[i] != nullptr) {
                        // TODO: Реализация обновления дочернего узла.
                    }
                }
            }
        }

        // Обрабатываем объекты, которые переместились в новое место.
        BoundingRegion movedObj;
        while (movedObjects.size() != 0)
        {
            // Получаем первый перемещенный объект.
            movedObj = movedObjects[0];
            node* current = this; // Начинаем с текущего узла.

            // Поднимаемся вверх по дереву, пока не найдем узел, который полностью включает объект.
            while (!current->region.containsRegion(movedObj))
            {
                if (current->parent != nullptr) {
                    current = current->parent; // Переходим к родительскому узлу.
                }
                else {
                    break; // Если достигли корня дерева, выходим из цикла.
                }
            }

            // Удаляем первый объект из списка перемещенных объектов.
            movedObjects.erase(movedObjects.begin());

            // Удаляем объект из текущего узла.
            objects.erase(objects.begin() + List::getIndexOf<BoundingRegion>(objects, movedObj));

            // Вставляем объект в найденный подходящий узел.
            current->insert(movedObj);

            // TODO: Реализация обнаружения столкновений, если требуется.
        }

    }
    else {
        // Если дерево еще не готово, обрабатываем ожидающие объекты.
        if (queue.size() > 0) {
            processPending();
        }
    }
}
void Octree::node::processPending()
{
    // Если дерево не построено, добавляем объекты из очереди в список объектов узла.
    if (!treeBuilt) {
        // Переносим все объекты из очереди в список объектов узла
        while (queue.size() != 0) {
            objects.push_back(queue.front()); // Перенос объекта из очереди в список объектов
            queue.pop(); // Удаляем объект из очереди
        }
        // Строим дерево после того, как все объекты добавлены
        build();
    }
    else
    { // Если дерево уже построено, сразу вставляем объекты в соответствующие узлы
        while (queue.size() != 0)
        {
            insert(queue.front()); // Вставляем объект в дерево
            queue.pop(); // Удаляем объект из очереди
        }
    }
}

bool Octree::node::insert(BoundingRegion obj)
{
    // Условия завершения вставки: если объект не помещается в текущий узел
    glm::vec3 dimensions = region.calculateDimensions(); // Вычисляем размеры текущего региона

    // Если узел пустой или размеры слишком маленькие для дальнейшего деления
    if (objects.size() == 0 || dimensions.x < MIN_BOUNDS || dimensions.y < MIN_BOUNDS || dimensions.z < MIN_BOUNDS)
    {
        objects.push_back(obj); // Добавляем объект в текущий узел
        return true;
    }

    // Если объект не помещается в текущий регион, передаем его родительскому узлу
    if (!region.containsRegion(obj)) {
        return parent == nullptr ? false : parent->insert(obj);
    }

    // Создаем регионы для дочерних узлов, если они еще не созданы
    BoundingRegion octans[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (children[i] != nullptr) {
            octans[i] = children[i]->region; // Если дочерний узел существует, используем его регион
        }
        else
        {
            calculateBounds(&octans[i], (Octant)(1 << i),region); // Вычисляем границы для дочернего узла
        }
    }

    // Находим дочерний регион, который полностью содержит объект
    bool found = false;
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octans[i].containsRegion(obj)) {
            // Если дочерний узел существует, вставляем объект в него
            if (children[i] != nullptr) {
                return children[i]->insert(obj);
            }
            else {
                // Создаем новый узел для дочернего региона и добавляем объект
                children[i] = new node(octans[i], { obj });
                States::activateIndex(&activeOctants, i); // Отмечаем дочерний узел как активный
                return true;
            }
        }
    }

    // Если ни один дочерний узел не подошел, добавляем объект в текущий узел
    objects.push_back(obj);
    return true;
}

void Octree::node::destroy()
{
    // Очищаем дочерние узлы, если они существуют
    if (children != nullptr) {
        for (int flags = activeOctants, i = 0; flags > 0; flags >> 1, i++)
        {
            // Если дочерний узел активен
            if (States::isIndexActive(&flags, 0)) {
                // Если дочерний узел существует, рекурсивно очищаем его
                if (children[i] != nullptr) {
                    children[i]->destroy(); // Удаляем дочерний узел
                    children[i] = nullptr; // Обнуляем указатель
                }
            }
        }
    }
    // Очищаем текущий узел
    objects.clear(); // Очищаем список объектов в текущем узле
    // Очищаем очередь, если она не пуста
    while (queue.size() != 0)
    {
        queue.pop(); // Удаляем объекты из очереди
    }
}