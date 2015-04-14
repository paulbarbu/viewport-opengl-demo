#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int ix = -1, iy = -1;
float angleX = 0, angleY = 0;
float zoom = 1.0;

GLfloat position[] = { -200.0, 200.0, 200.0, 1.0 };

void initLights()
{
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
}

bool initGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    initLights();

    glClearColor(0.f, 0.f, 0.f, 1.f);

    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
        return false;
    }

    return true;
}

void render()
{
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //TODO: reset this rotation per viewport
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    glScalef(zoom, zoom, zoom);

    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glutSolidTeapot(20);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(100, 0, 0);
        glutSolidTeapot(20);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w <= h)
    {
        float d = w/2.0;
        glOrtho(-d, d, -d*(float)h/(float)w, d*(float)h/(float)w, -d, d);
    }
    else
    {
        float d = h/2.0;
        glOrtho(-d*(float)w/(float)h, d*(float)w/(float)h, -d, d, -d, d);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void handleMouse(int button, int state, int x, int y)
{
    printf("mouse: %d\tx: %d\ty: %d\n", button, x, y);
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN)
            {
                ix = x;
                iy = y;
                printf("Left click down\n");
            }
            else
            {
                printf("Left up: dx: %d\tdy: %d\n", ix-x, iy-y);
                angleX = 0;
                angleY = 0;
                ix = -1;
                iy = -1;
            }
            break;
        case 3: // scroll up
            zoom = 1.1;
            render();
            zoom = 1;
            printf("scroll up\n");
            break;
        case 4: // scroll down
            zoom = 0.9;
            render();
            zoom = 1;
            printf("scroll down\n");
            break;
    }
}

void mouseMove(int x, int y)
{
    if(ix != -1 && iy != -1)
    {
        angleX = y-iy; // moving the mouse up/down (iy & y) rotates the scene on the X axis
        angleY = x-ix; // moving the mouse left/right (ix & x) rotates the scene on the Y axis
        render();
        ix = x;
        iy = y;
    }
}

int main(int argc, char* args[])
{
    glutInit(&argc, args);

    glutInitContextVersion(2, 1);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Barbu Paul - Gheorghe 2015");

    if(!initGL())
    {
        printf("Unable to initialize graphics library!\n");
        return 1;
    }

    glutMotionFunc(mouseMove);
    glutMouseFunc(handleMouse);
    glutDisplayFunc(render);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}
