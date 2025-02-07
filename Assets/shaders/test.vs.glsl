
layout(std140) uniform Test{
 int i; // Простое поле типа int

    // Структура t2
    struct t2 {
        int d[5]; // Массив из 5 int
        float f;  // Одно поле типа float
        vec3 v[2]; // Массив из 2 vec3
    };
 
    // Структура t3
    struct t3 {
        mat3 m; // Матрица 3x3
        vec3 v; // Одно поле типа vec3
    }a[2];

}