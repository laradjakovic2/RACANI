#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <string>
#define M_PI 3.14

float carX = 0.0f;
float carY = 0.5f;
float carZ = 0.0f;
float carAngle = 0.0f; //u stupnjevima ova var

float CARHEIGHT = 0.5f;
float CARWIDTH = 1.0f;
float CARLENGTH = 2.0f;
float speed = 0.5f;

const float ROAD_WIDTH = 4.0f;
const int ROAD_SEGMENTS = 100;
const float ROAD_LENGTH = 300.0f;
std::vector<std::pair<float, float>> roadPoints; //(x, z) koordinate tocke ceste

int lastMouseX;
bool isMouseActive = false;

bool carCrashDetected = false;
float collisionTime = 0.0f;

int offRoadCounter = 0;

void generateRoad() {
    roadPoints.clear();
    for (int i = 0; i <= ROAD_SEGMENTS; ++i) {
        float z = -i * (ROAD_LENGTH / ROAD_SEGMENTS);
        float x = sin(z * 0.1f) * 3.0f; // po sin fji racunaj x-zavoji
        roadPoints.push_back({ x, z });
    }
}

void drawRoad() {
    glColor3f(0.3f, 0.3f, 0.3f); //siva
    glBegin(GL_QUAD_STRIP);
    for (const auto& point : roadPoints) {
        float leftX = point.first - ROAD_WIDTH / 2;
        float rightX = point.first + ROAD_WIDTH / 2;
        float z = point.second;

        glVertex3f(leftX, 0.0f, z);
        glVertex3f(rightX, 0.0f, z);
    }
    glEnd();
}

void drawCar() {
    glPushMatrix();
    glTranslatef(carX, carY, carZ);
    glRotatef(carAngle, 0.0f, 1.0f, 0.0f);

    //karoserija
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(CARWIDTH, CARHEIGHT, CARLENGTH);
    glutSolidCube(1.0);
    glPopMatrix();

    //krov
    glColor3f(0.7f, 0.9f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.85f, 0.0f);
    glScalef(CARWIDTH*0.8f, CARHEIGHT*0.8f, CARLENGTH*0.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    //kotaci
    glColor3f(0.1f, 0.1f, 0.1f);
    float wheelZ = CARLENGTH / 2.0f;
    float wheelX = CARWIDTH / 2*0.8f;
    float wheelY = CARHEIGHT / 2 * 0.8f;

    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glPushMatrix();
            glTranslatef(i * wheelX, wheelY, j * wheelZ);
            glRotatef(90, 0.0f, 1.0f, 0.0f);
            glutSolidTorus(0.1, 0.2, 10, 20);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void drawCounter() {
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600); //ortogonalni prikaz

    glMatrixMode(GL_MODELVIEW);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(750, 550);

    std::string text = std::to_string(offRoadCounter);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawCarCrashMessage() {
    if (carCrashDetected) {
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600); // ortogonalni prikaz

        glMatrixMode(GL_MODELVIEW);

        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2i(350, 300);

        std::string text = "SUDAR!";
        for (char c : text) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

void keyboard(unsigned char key, int x, int y) {
    float radians = carAngle * M_PI / 180.0f; //u radijane

    switch (key) {
    case 'w':
        carX -= speed * sin(radians);
        carZ -= speed * cos(radians);
        break;
    case 's':
        carX += speed * sin(radians);
        carZ += speed * cos(radians);
        break;
    case 'a':
        carAngle += 2.0f;
        break;
    case 'd':
        carAngle -= 2.0f;
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if (isMouseActive) {
        int deltaX = x - lastMouseX;

        carAngle += deltaX * 0.1f; // skaliraj za slabiju osjetljivost

        lastMouseX = x;
        glutPostRedisplay();
    }
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMouseActive = true;
            lastMouseX = x;
        }
        else {
            isMouseActive = false;
        }
    }
}

void resetCarCrashMessage(int value) {
    carCrashDetected = false;
    glutPostRedisplay();
}

bool checkRoadZSegment(float roadPointZ) {
    return carZ >= roadPointZ - (ROAD_LENGTH / ROAD_SEGMENTS) / 2 && carZ <= roadPointZ + (ROAD_LENGTH / ROAD_SEGMENTS) / 2;
}

void checkCarCrash() {
    //krajnje tocke auta
    float leftSide = carX - CARWIDTH / 2;
    float rightSide = carX + CARWIDTH / 2;

    for (const auto& point : roadPoints) {
        if (checkRoadZSegment(point.second)) { //provjeri nalazi li se auto na tom segmentu ceste(dubina ceste)
            float leftEdgeRoad = point.first - ROAD_WIDTH / 2;
            float rightEdgeRoad = point.first + ROAD_WIDTH / 2;

            //provjeri rubove auta sa cestom
            if (leftSide < leftEdgeRoad || rightSide > rightEdgeRoad) {
                offRoadCounter++;
                carCrashDetected = true;
                glutTimerFunc(1000, resetCarCrashMessage, 0); //makni natpis

                carX = point.first; // vrati auto na sredinu ceste
                break;
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float cameraX = carX;
    float cameraY = 3.0f; //konstantna visina
    float cameraZ = carZ + 5.0f; //malo iza auta u prvom licu

    gluLookAt(
        cameraX, cameraY, cameraZ, // pozicija kamerea
        carX, carY, carZ, // cantar
        0.0f, 1.0f, 0.0f // gore malo (Y-os)
    );

    drawRoad();
    drawCar();
    drawCounter();
    drawCarCrashMessage();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void update(int value) {
    checkCarCrash();

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.8, 1.0, 1.0);

    generateRoad();
    carX = roadPoints[0].first;
    carZ = roadPoints[0].second;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Auto na Cesti");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}