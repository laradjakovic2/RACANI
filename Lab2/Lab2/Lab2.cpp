#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#define M_PI 3.14159265358979323846

struct Vertex {
    float x, y, z;
};

struct Particle {
    Vertex position;
    Vertex velocity;
    float life;
    float maxLife;
};

GLuint width = 1600, height = 800;
int deltaTime = 150;
float gravity = -9.8f;
Vertex attractionPoint = { 0.0f, 20.0f, 0.0f };

std::vector<Particle> particleSystem;

Vertex cloudS = { -20.0f, 10.0f, 0.0f };
Vertex cloudE = { 20.0f, 10.0f, 0.0f };

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(5.0);
}

void myReshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / height, 1.0, 100.0);
    gluLookAt(
        5.0, 5.0, 60.0,  // kamera
        0.0, 0.0, 0.0,  // srediste
        0.0, 1.0, 0.0   // smjer "gore"
    );
    glMatrixMode(GL_MODELVIEW);
}

void drawParticle(const Particle& p, int a) {
    float dx = p.position.x - attractionPoint.x;
    float dy = p.position.y - attractionPoint.y;
    float dz = p.position.z - attractionPoint.z;
    float distance = sqrt(dx * dx + dy * dy + dz * dz);

    //veća udaljenost -> veća prozirnost
    //normalizira alpha jer inace sve brzo prozirno
    float alpha = std::min(1.0f, distance /25);
    if (a == 0) {
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f - alpha);
    }
    else {
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f - alpha);
    }
    

    GLUquadric* quad = gluNewQuadric(); //za crtanje geometrije

    glPushMatrix();
    glTranslatef(p.position.x, p.position.y, p.position.z); //na poziciji cestice nacrtamo disk

    float innerRadius = 0.0f; //0=puni disk
    float outerRadius = 0.5f;
    gluDisk(quad, innerRadius, outerRadius, 30, 1);

    glPopMatrix();

    gluDeleteQuadric(quad);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const auto& p : particleSystem) {
        drawParticle(p, 0);
    }
    glutSwapBuffers();
}

Particle createParticle() {
    Particle p;

    //p.position = { 0.0f, 10.0f, 10.0f }; //svi krenu padat iz iste tocke

    // nasumicno odaberi iz linije oblaka
    float t = (float)(rand() % 1000) / 1000.0f; // između 0 i 1
    p.position.x = cloudS.x + t * (cloudE.x - cloudS.x);
    p.position.y = cloudS.y + t * (cloudE.y - cloudS.y);
    p.position.z = cloudS.z + t * (cloudE.z - cloudS.z);

    p.velocity = { 0.0f, -0.5f, 0.0f };
    p.life = 0.0f;
    p.maxLife = (float)(rand() % 5000 + 3000); //[3000, 5000+3000]
    return p;
}

void updateParticles(float dt) {
    for (auto& p : particleSystem) {
        //v=g*t
        p.velocity.y += gravity * dt;

        //s=v*t
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.position.z += p.velocity.z * dt;

        p.life += dt * 1000;
    }

    particleSystem.erase(
        std::remove_if(particleSystem.begin(), particleSystem.end(), [](const Particle& p) {
            return p.life >= p.maxLife;
            }),
        particleSystem.end()
    );

    while (particleSystem.size() < 50) {
        particleSystem.push_back(createParticle());
    }
}

void timer(int value) {
    updateParticles(deltaTime / 1000.0f); //ms->s
    glutPostRedisplay();
    glutTimerFunc(deltaTime, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Sustav cestica");

    initOpenGL();

    for (int i = 0; i < 100; ++i) {
        particleSystem.push_back(createParticle());
    }

    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutTimerFunc(deltaTime, timer, 0);
    glutMainLoop();

    return 0;
}
