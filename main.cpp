#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int aix = -1, aiy = -1, tix = 0, tiy = 0;
float angleX = 0, angleY = 0, translateY = 0, translateX = 0;
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

    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glPushMatrix();
        //TODO: reset these per viewport
        glScalef(zoom, zoom, zoom);
        glTranslatef(translateX, translateY, 0);
        glRotatef(angleX, 1, 0, 0);
        glRotatef(angleY, 0, 1, 0);

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
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN)
            {
                aix = x;
                aiy = y;
                printf("Right click down\n");
            }
            else
            {
                printf("Right up: dx: %d\tdy: %d\n", aix-x, aiy-y);
                aix = -1;
                aiy = -1;
            }
            break;
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN)
            {
                tix = x;
                tiy = y;
                printf("Left click down\n");
            }
            else
            {
                printf("Left up: dx: %d\tdy: %d\n", tix-x, tiy-y);
                tix = 0;
                tiy = 0;
            }
            break;
        case 3: // scroll up
            zoom += 0.1;
            render();
            printf("scroll up\n");
            break;
        case 4: // scroll down
            zoom -= 0.1;
            render();
            printf("scroll down\n");
            break;
    }
}

void mouseMove(int x, int y)
{
    bool doRender = false;
    if(aix != -1 && aiy != -1)
    {
        angleX += y-aiy; // moving the mouse up/down (iy & y) rotates the scene on the X axis
        angleY += x-aix; // moving the mouse left/right (ix & x) rotates the scene on the Y axis
        aix = x;
        aiy = y;
        doRender = true;
    }

    if(tix != 0 && tiy != 0)
    {
        translateY += tiy-y; // moving the mouse up/down (iy & y) translated the scene on the X axis
        translateX += x-tix; // moving the mouse left/right (ix & x) translates the scene on the Y axis
        tix = x;
        tiy = y;
        doRender = true;
    }

    if(doRender) render();
}

void keyPress(unsigned char key, int x, int y)
{
    printf("Key press: %c at x: %d\ty: %d", key, x, y);
    switch(key)
    {
        case 'r':
            angleY = 0;
            angleX = 0;
            zoom = 1;
            translateY = 0;
            translateX = 1;
            render();
            break;
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

    glutKeyboardFunc(keyPress);
    glutMotionFunc(mouseMove);
    glutMouseFunc(handleMouse);
    glutDisplayFunc(render);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}
