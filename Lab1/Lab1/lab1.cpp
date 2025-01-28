#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define M_PI 3.14159265358979323846

struct Vertex {
    float x, y, z;
};

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;
GLuint window, window2;
GLuint width = 600, height = 600;

Vertex	ociste = { 5.0, 5.0 , 60.0 };
//Vertex ociste = { 5.0, 5.0, 30.0 };
int currentPointOnBSpline = 0;
bool isDCM = false;

std::vector<Vertex> bSplinePoints;
std::vector<Vertex> tangentPoints;
std::vector<Vertex> secondDerivationPoints;

std::vector<Vertex> controlPoints = {
        {0.0, 0.0, 0.0}, {0, 10.0, 15.0}, {10.0, 10.0, 10.0},
        {0, 0, 20.0}, {0, 10.0, 25.0}, {10.0, 10.0, 30.0},
        {10.0, 0, 35.0}, {0,0, 40.0}, {0, 10.0, 45.0},
        {10.0, 10.0, 50.0}, {10.0, 0, 55.0}
};
/*
std::vector<Vertex> controlPoints = {
    {0.0, 0.0, 0.0}, {1.0, 2.0, 0.0}, {2.0, 3.0, 0.0},
    {3.0, 3.0, 0.0}, {4.0, 2.0, 0.0}, {5.0, 0.0, 0.0},
    {6.0, -1.0, 0.0}, {7.0, -1.5, 0.0}, {8.0, -1.0, 0.0}
};*/

glm::mat4 bSplineMatrix = glm::mat4(
    -1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f,
    3 / 6.0f, -6 / 6.0f, 3 / 6.0f, 0.0f,
    -3 / 6.0f, 0.0f, 3 / 6.0f, 0.0f,
    1 / 6.0f, 4 / 6.0f, 1 / 6.0f, 0.0f
);

glm::mat3x4 tangentMatrix = glm::mat3x4(
    -1 / 2.0f, 3 / 2.0f, -3 / 2.0f, 1 / 2.0f,
    2 / 2.0f, -4 / 2.0f, 2 / 2.0f, 0.0f,
    -1 / 2.0f, 0.0f, 1 / 2.0f, 0.0f
);

glm::mat2x4 secondDerivationMatrix = glm::mat2x4(
    -1.0f, 3.0f, -3.0f, 1.0f,
    1.0f, -2.0f, 1.0f, 0.0f
);

void loadOBJ(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Ne mogu otvoriti datoteku: " << path << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vertex vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (prefix == "f") {
            unsigned int v1, v2, v3;
            iss >> v1 >> v2 >> v3;
            indices.push_back(v1 - 1);
            indices.push_back(v2 - 1);
            indices.push_back(v3 - 1);
        }
    }

    file.close();
    std::cout << "Uspješno ucitano " << vertices.size() << " vrhova i " << indices.size() / 3 << " trokuta." << std::endl;
}

void initOpenGL() {
    //glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

Vertex calculateBSplinePoint(float t, int segment) {
    glm::vec4 T = glm::vec4(t * t * t, t * t, t, 1.0f);

    glm::mat4 R = glm::mat4(
        glm::vec4(controlPoints[segment].x, controlPoints[segment].y, controlPoints[segment].z, 0.0f),
        glm::vec4(controlPoints[segment + 1].x, controlPoints[segment + 1].y, controlPoints[segment + 1].z, 0.0f),
        glm::vec4(controlPoints[segment + 2].x, controlPoints[segment + 2].y, controlPoints[segment + 2].z, 0.0f),
        glm::vec4(controlPoints[segment + 3].x, controlPoints[segment + 3].y, controlPoints[segment + 3].z, 0.0f)
    );

    glm::vec4 resultVec = T * glm::transpose(bSplineMatrix) * glm::transpose(R);

    Vertex result;
    result.x = resultVec.x;
    result.y = resultVec.y;
    result.z = resultVec.z;

    return result;
}

Vertex calculateTangentPoint(float t, int segment) {
    glm::vec3 T = glm::vec3(t * t, t, 1.0f);

    glm::mat4 R = glm::mat4(
        glm::vec4(controlPoints[segment].x, controlPoints[segment].y, controlPoints[segment].z, 0.0f),
        glm::vec4(controlPoints[segment + 1].x, controlPoints[segment + 1].y, controlPoints[segment + 1].z, 0.0f),
        glm::vec4(controlPoints[segment + 2].x, controlPoints[segment + 2].y, controlPoints[segment + 2].z, 0.0f),
        glm::vec4(controlPoints[segment + 3].x, controlPoints[segment + 3].y, controlPoints[segment + 3].z, 0.0f)
    );

    glm::vec4 resultVec = T * glm::transpose(tangentMatrix) * glm::transpose(R);

    Vertex result;
    result.x = resultVec.x;
    result.y = resultVec.y;
    result.z = resultVec.z;

    return result;
}

Vertex calculateSecondDerivationPoint(float t, int segment) {
    glm::vec2 T = glm::vec2(t, 1.0f);

    glm::mat4 R = glm::mat4(
        glm::vec4(controlPoints[segment].x, controlPoints[segment].y, controlPoints[segment].z, 0.0f),
        glm::vec4(controlPoints[segment + 1].x, controlPoints[segment + 1].y, controlPoints[segment + 1].z, 0.0f),
        glm::vec4(controlPoints[segment + 2].x, controlPoints[segment + 2].y, controlPoints[segment + 2].z, 0.0f),
        glm::vec4(controlPoints[segment + 3].x, controlPoints[segment + 3].y, controlPoints[segment + 3].z, 0.0f)
    );

    glm::vec4 resultVec = T * glm::transpose(secondDerivationMatrix) * glm::transpose(R);

    Vertex result;
    result.x = resultVec.x;
    result.y = resultVec.y;
    result.z = resultVec.z;

    return result;
}

void calculateBSplineData() {
    for (int i = 0; i < controlPoints.size() - 3; ++i) { //svaki segment definiran sa 4 točke
        for (float t = 0; t <= 1; t += 0.01f) { // Parametar t ide od 0 do 1
            //putanja
            Vertex p = calculateBSplinePoint(t, i);
            bSplinePoints.push_back(p);

            //orijentacije objekta
            Vertex tp = calculateTangentPoint(t, i);
            tangentPoints.push_back(tp);

            Vertex sdp = calculateSecondDerivationPoint(t, i);
            secondDerivationPoints.push_back(sdp);
        }
    }
}

void drawBSpline() {
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);

    for (const Vertex p : bSplinePoints) {
        glVertex3f(p.x, p.y, p.z);
    }

    glEnd();
}

void drawControlPoints() {
    glColor3f(0.0, 0.0, 1.0);
    glPointSize(5.0);
    glBegin(GL_POINTS);

    for (const Vertex p : controlPoints) {
        glVertex3f(p.x, p.y, p.z);
    }

    glEnd();
}

void drawObject() {
    glScalef(4.0f, 4.0f, 4.0f);
    glBegin(GL_TRIANGLES);

    for (unsigned int i = 0; i < indices.size(); i += 3) {
        Vertex v1 = vertices[indices[i]];
        Vertex v2 = vertices[indices[i + 1]];
        Vertex v3 = vertices[indices[i + 2]];

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

void myReshape(int w, int h)
{
    width = w; height = h;
    //glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION); //matrica projekcije
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);				   
    gluPerspective(45.0, (float)width / height, 1, 100); // kut pogleda, x/y, prednja i straznja ravnina odsjecanja
    //gluPerspective(45.0, 1.0, 1.0, 100.0);
    gluLookAt(
        ociste.x, ociste.y, ociste.z,  // ozicija kamere (x, y, z)
        5.0, 5.0, 25.0, // točka na koju kamera gleda (središte vaših točaka)
        0.0, 1.0, 0.0 // smjer prema gore
    );
    glMatrixMode(GL_MODELVIEW); //aktivirana matrica modela
}

void handleTranslationAndRotation() {
    //-----------------------translacija
    Vertex currentPosition = bSplinePoints[currentPointOnBSpline];
    glTranslatef(currentPosition.x, currentPosition.y, currentPosition.z);
    //-----------------------rotacija
    Vertex currentTangent = tangentPoints[currentPointOnBSpline];
    
    glm::vec3 s(0.0f, 0.0f, 1.0f); // defaultna početna točka
    glm::vec3 e(currentTangent.x, currentTangent.y, currentTangent.z);
    float magS = glm::length(s);
    float magE = glm::length(e);

    glm::vec3 rotationAxis = glm::cross(s, e);

    float angle = acos(glm::dot(s, e) / (magE*magS)) * 180.0f / M_PI;

    if (currentPointOnBSpline == 0 || currentPointOnBSpline == 1) {
        std::cout << "Rotacija:(" << angle << ", " << rotationAxis.x << ", " << rotationAxis.y << ", " << rotationAxis.z << ")" << std::endl;
    }
    glRotatef(angle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
}

glm::mat3 handleRotationDCM() {
    //-----------------------rotacija
    Vertex currentTangent = tangentPoints[currentPointOnBSpline];
    Vertex secondDerivation = secondDerivationPoints[currentPointOnBSpline];

    glm::vec3 w(currentTangent.x, currentTangent.y, currentTangent.z); //tangenta
    w = glm::normalize(w);
    glm::vec3 a(secondDerivation.x, secondDerivation.y, secondDerivation.z);
    glm::vec3 u = glm::normalize(glm::cross(w, a)); //normala = prvaDerivacija x drugaDerivacija
    glm::vec3 v = glm::normalize(glm::cross(w, u)); //binormala = w x u

    glm::mat3 R = glm::mat3(
        glm::vec3(w.x, u.x, v.x),
        glm::vec3(w.y, u.y, v.y),
        glm::vec3(w.z, u.z, v.z)        
    );
    R = glm::transpose(R);

    return glm::inverse(R);

    //ovo je rotacija jedne točke
    /*Vertex localPoint = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotatedPoint = R * glm::vec3(localPoint.x, localPoint.y, localPoint.z);

    Vertex result;
    result.x = rotatedPoint.x;
    result.y = rotatedPoint.y;
    result.z = rotatedPoint.z;*/

    //glRotatef(angle, resultVec.x, resultVec.y, resultVec.z);
    //-----------------------translacija
    //glTranslatef(resultVec.x, resultVec.y, resultVec.z);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);

    //točke su uvijek na instoj poziciji - ne radimo trans i rot
    drawControlPoints();
    drawBSpline();

    if (isDCM) {
        // ------------------------------------ - DCM racunanje
        glm::mat3 R_inverse = handleRotationDCM();
        GLfloat* matrix = new GLfloat[16];
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                matrix[i * 4 + j] = R_inverse[j][i];
            }
        }
        matrix[3] = 0.0f;
        matrix[7] = 0.0f;
        matrix[11] = 0.0f;
        matrix[12] = 0.0f;
        matrix[13] = 0.0f;
        matrix[14] = 0.0f;
        matrix[15] = 1.0f;
        //--------------------------------------------end

        glPushMatrix();
        //--------------------------------prvo translairaj
        Vertex currentPosition = bSplinePoints[currentPointOnBSpline];
        glTranslatef(currentPosition.x, currentPosition.y, currentPosition.z);
        //----------------------rotacija
        glMultMatrixf(matrix);

        drawObject();

        glPopMatrix();
    }
    else {
        glPushMatrix();
        if (currentPointOnBSpline < bSplinePoints.size()) {
            handleTranslationAndRotation();
        }

        drawObject();

        glPopMatrix();
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void updatePosition(int value) {
    currentPointOnBSpline++;
    if (currentPointOnBSpline >= bSplinePoints.size()) {
        currentPointOnBSpline = 0;
    }

    glutTimerFunc(15, updatePosition, 0);
}

int main(int argc, char** argv) {
    loadOBJ("../f16.obj");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    calculateBSplineData(); //obavezno pozvati prije crtanje jer su mi tu svi podaci

    window = glutCreateWindow("Display");

    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutTimerFunc(15, updatePosition, 0);

    initOpenGL();

    glutMainLoop();

    return 0;
}