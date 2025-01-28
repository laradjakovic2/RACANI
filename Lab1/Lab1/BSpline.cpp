#include <GL/glut.h>
#include <vector>

struct Point1 {
    float x, y, z;
};

std::vector<Point1> controlPoints = {
        {0.0, 0.0, 0.0}, {0, 10.0, 15.0}, {10.0, 10.0, 10.0},
        {0, 0, 20.0}, {0, 10.0, 25.0}, {10.0, 10.0, 30.0},
        {10.0, 0, 35.0}, {0,0, 40.0}, {0, 10.0, 45.0},
        {10.0, 10.0, 50.0}, {10.0, 0, 55.0}
};
/*
float B0(float t) { return (1 - t) * (1 - t) * (1 - t) / 6.0f; }
float B1(float t) { return (3 * t * t * t - 6 * t * t + 4) / 6.0f; }
float B2(float t) { return (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0f; }
float B3(float t) { return t * t * t / 6.0f; }
*/
// Funkcija za generiranje B-spline krivulje
/*
void drawBSplineCurve1() {
    glColor3f(1.0, 0.0, 0.0); // Boja krivulje
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < controlPoints.size() - 3; ++i) { // Svaki segment definiran sa 4 točke
        for (float t = 0; t <= 1; t += 0.01f) { // Parametar t ide od 0 do 1
            float x = B0(t) * controlPoints[i].x + B1(t) * controlPoints[i + 1].x +
                B2(t) * controlPoints[i + 2].x + B3(t) * controlPoints[i + 3].x;
            float y = B0(t) * controlPoints[i].y + B1(t) * controlPoints[i + 1].y +
                B2(t) * controlPoints[i + 2].y + B3(t) * controlPoints[i + 3].y;
            float z = B0(t) * controlPoints[i].z + B1(t) * controlPoints[i + 1].z +
                B2(t) * controlPoints[i + 2].z + B3(t) * controlPoints[i + 3].z;

            glVertex3f(x, y, z); // Dodavanje točke krivulje
        }
    }

    glEnd();
}
*/
void drawControlPoints1() {
    glColor3f(0.0, 0.0, 1.0); // Boja točaka
    glPointSize(5.0);
    glBegin(GL_POINTS);

    for (const auto& point : controlPoints) {
        glVertex3f(point.x, point.y, point.z);
    }

    glEnd();
}

// Postavljanje kamere
void setupCamera1() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0); // Širina perspektive, omjer stranica, blizina, daljina

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        15.0, 20.0, 80.0,  // Pozicija kamere (x, y, z)
        5.0, 5.0, 25.0,    // Točka na koju kamera gleda (središte vaših točaka)
        0.0, 1.0, 0.0      // Smjer prema gore
    );
}

// Funkcija za prikaz
void display1() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setupCamera1();

    // Crtamo kontrolne točke
    drawControlPoints1();

    // Crtamo B-spline krivulju
    //drawBSplineCurve1();

    glutSwapBuffers();
    glutPostRedisplay();
}

void initOpenGL1() {
    //glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

/*
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("B-spline");

    initOpenGL1();
    glutDisplayFunc(display1);

    glutMainLoop();

    return 0;
}*/