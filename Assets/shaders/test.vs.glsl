
layout(std140) uniform Test{
 int i; // ������� ���� ���� int

    // ��������� t2
    struct t2 {
        int d[5]; // ������ �� 5 int
        float f;  // ���� ���� ���� float
        vec3 v[2]; // ������ �� 2 vec3
    };
 
    // ��������� t3
    struct t3 {
        mat3 m; // ������� 3x3
        vec3 v; // ���� ���� ���� vec3
    }a[2];

}