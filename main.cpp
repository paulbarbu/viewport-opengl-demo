#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

//TODO: instructions & help

struct viewport
{
    int aix, aiy, tix, tiy;
    float angleX, angleY, translateX, translateY;
    float zoom;

    float iAngleX, iAngleY, iTranslateX, iTranslateY;
    float iZoom;

    viewport()
    {
        aix = aiy = -1;
        tix = tiy = 0;
        init(0, 0, 0, 0, 1);
    }

    void init(float ax, float ay, float tx, float ty, float z)
    {
        angleX = iAngleX = ax;
        angleY = iAngleY = ay;
        translateX = iTranslateX = tx;
        translateY = iTranslateY = ty;
        zoom = iZoom = z;
    }

    void reset()
    {
        aix = aiy = -1;
        tix = tiy = 0;

        angleX = iAngleX;
        angleY = iAngleY;
        translateX = iTranslateX;
        translateY = iTranslateY;
        zoom = iZoom;
    }

} *activeViewport = NULL, viewports[4], phantomViewport;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int w, h;
float viewW, viewH, d;
bool locked = false;

GLfloat lightPosition[] = {-200, 200, 200, 1};

void setProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-viewW, viewW, -viewH, viewH, -d, d);
}

void display2DOverlay(bool show = true)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        setProjection();
        float lineWidth = d * 2.0/100.0;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);

        glLineWidth(lineWidth);

        if(show)
        {
            glColor3f(0, 1, 0);
        }
        else
        {
            glColor3f(0, 0, 0);
        }

        glPushMatrix();
            glBegin(GL_LINE_LOOP);
                glVertex3f(-viewW, viewH, 0); // top left
                glVertex3f(-viewW, -viewH, 0); // bottom right
                glVertex3f(viewW, -viewH, 0); // bottom left
                glVertex3f(viewW, viewH, 0); // top left
            glEnd();
        glPopMatrix();

    glPopAttrib();
}

void setActiveViewport(int x, int y)
{
    // clear the overlay from every viewport
    glViewport(0, h, w, h);
    glScissor(0, h, w, h);
    display2DOverlay(false);

    glViewport(w, h, w, h);
    glScissor(w, h, w, h);
    display2DOverlay(false);

    glViewport(0, 0, w, h);
    glScissor(0, 0, w, h);
    display2DOverlay(false);

    glViewport(w, 0, w, h);
    glScissor(w, 0, w, h);
    display2DOverlay(false);

    if(locked)
    {
        activeViewport = &phantomViewport;
        return;
    }

    if(x < 0 && y < 0)
    {
        activeViewport = NULL;
        printf("All viewports active\n");
        return;
    }

    if (x < w && y < h)
    {
        glViewport(0, h, w, h);
        glScissor(0, h, w, h);
        activeViewport = &viewports[0]; // top left
        printf("Top left viewport active\n");
        return;
    }

    if (x > w && y < h)
    {
        glViewport(w, h, w, h);
        glScissor(w, h, w, h);
        activeViewport = &viewports[1]; // top right
        printf("Top right viewport active\n");
        return;
    }

    if (x < w && y > h) // bottom left
    {
        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);
        activeViewport = &viewports[2];
        printf("Bottom left viewport active\n");
        return;
    }

    if (x > w && y > h)
    {
        glViewport(w, 0, w, h);
        glScissor(w, 0, w, h);
        activeViewport = &viewports[3]; // bottom right
        printf("Bottom right viewport active\n");
        return;
    }
}

bool initGL()
{
    GLfloat ambientLight[] = {0.2, 0.2, 0.2, 1};

    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    glClearColor(0, 0, 0, 1);

    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
        return false;
    }

    return true;
}

void display3DScene()
{
    GLfloat mat_diffuse[] = {1, 1, 1, 1};
    GLfloat mat_specular[] = {1, 1, 1, 1};
    GLfloat mat_shininess[] = {50};

    setProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPushMatrix();
        glScalef(activeViewport->zoom, activeViewport->zoom, activeViewport->zoom);
        glTranslatef(activeViewport->translateX, activeViewport->translateY, 0);
        glRotatef(activeViewport->angleX, 1, 0, 0);
        glRotatef(activeViewport->angleY, 0, 1, 0);

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

        glPushMatrix();
            glTranslatef(-100, 0, 0);
            glutSolidTeapot(20);
        glPopMatrix();

    glPopMatrix();
}


void display(bool overlay = true)
{
    display3DScene();
    if(overlay)
    {
        display2DOverlay();
    }

    glutSwapBuffers();
}

void render()
{
    // update all viewports
    if(activeViewport == NULL && !locked)
    {
        // top left
        glViewport(0, h, w, h);
        glScissor(0, h, w, h);
        activeViewport = &viewports[0];
        display(false);

        // top right
        glViewport(w, h, w, h);
        glScissor(w, h, w, h);
        activeViewport = &viewports[1];
        display(false);

        // bottom left
        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);
        activeViewport = &viewports[2];
        display(false);

        // bottom right
        glViewport(w, 0, w, h);
        glScissor(w, 0, w, h);
        activeViewport = &viewports[3];
        display(false);

        activeViewport = NULL;
    }
    else if(locked)
    {
        glViewport(0, 0, w*2, h*2);
        glScissor(0, 0, w*2, h*2);
        activeViewport = &phantomViewport;
        display(false);
    }
    else
    {
        display();
    }
}

void resize(int width, int height)
{
    printf("Resize: w=%d\th=%d\n", width, height);
    w = width/2;
    h = height/2;

    if (w <= h)
    {
        d = w/2.0;
        viewW = d;
        viewH = d*(float)h/(float)w;

    }
    else
    {
        d = h/2.0;
        viewW = d*(float)w/(float)h;
        viewH = d;
    }

    activeViewport = NULL; // all viewports shall now be updated
}

void mousePress(int button, int state, int x, int y)
{
    printf("mouse: %d\tx: %d\ty: %d\n", button, x, y);

    setActiveViewport(x, y);

    switch(button)
    {
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN)
            {
                activeViewport->aix = x;
                activeViewport->aiy = y;
                printf("Right click down\n");
            }
            else
            {
                printf("Right up: dx: %d\tdy: %d\n", activeViewport->aix-x, activeViewport->aiy-y);
                activeViewport->aix = -1;
                activeViewport->aiy = -1;
            }
            break;
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN)
            {
                activeViewport->tix = x;
                activeViewport->tiy = y;
                printf("Left click down\n");
            }
            else
            {
                printf("Left up: dx: %d\tdy: %d\n", activeViewport->tix-x, activeViewport->tiy-y);
                activeViewport->tix = 0;
                activeViewport->tiy = 0;
            }
            break;
        case 3: // scroll up
            activeViewport->zoom += 0.1;
            render();
            printf("scroll up\n");
            break;
        case 4: // scroll down
            if(activeViewport->zoom > 0.1)
            {
                activeViewport->zoom -= 0.1;
                render();
            }
            printf("scroll down\n");
            break;
    }
}

void mouseMove(int x, int y)
{
    bool doRender = false;

    if(activeViewport->aix != -1 && activeViewport->aiy != -1)
    {
        activeViewport->angleX += y-activeViewport->aiy; // moving the mouse up/down (iy & y) rotates the scene on the X axis
        activeViewport->angleY += x-activeViewport->aix; // moving the mouse left/right (ix & x) rotates the scene on the Y axis
        activeViewport->aix = x;
        activeViewport->aiy = y;
        doRender = true;
    }

    if(activeViewport->tix != 0 && activeViewport->tiy != 0)
    {
        activeViewport->translateY += activeViewport->tiy-y; // moving the mouse up/down (iy & y) translated the scene on the X axis
        activeViewport->translateX += x-activeViewport->tix; // moving the mouse left/right (ix & x) translates the scene on the Y axis
        activeViewport->tix = x;
        activeViewport->tiy = y;
        doRender = true;
    }

    if(doRender) render();
}

void keyPress(unsigned char key, int x, int y)
{
    printf("Key press: %d at x: %d\ty: %d\n", key, x, y);
    printf("Modifiers: %d\n", glutGetModifiers());

    setActiveViewport(x, y);

    switch(key)
    {
        case 'R':
            activeViewport = NULL;

            for(int i=0; i<4; i++)
            {
                viewports[i].reset();
            }
            phantomViewport.reset();

            render();
            break;
        case 'r':
            activeViewport->reset();
            render();
            break;
        case 'l':
            locked = !locked;

            printf("Locked: %d\n", locked);
            activeViewport = NULL;

            if(!locked)
            {
                for(int i=0; i<4; ++i)
                {
                    viewports[i].angleX = phantomViewport.angleX;
                    viewports[i].angleY = phantomViewport.angleY;
                    viewports[i].translateX = phantomViewport.translateX;
                    viewports[i].translateY = phantomViewport.translateY;
                    viewports[i].zoom = phantomViewport.zoom;
                }
            }

            render();
            break;
        case 27: //ESC
            glutLeaveMainLoop();
            break;
    }
}

int main(int argc, char* args[])
{
    viewports[0].init(45, 45, 0, 0, 1);
    viewports[2].init(0, -90, 0, 0, 1);
    viewports[3].init(-90, 0, 0, 0, 1);

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
    glutMouseFunc(mousePress);
    glutDisplayFunc(render);
    glutReshapeFunc(resize);

    glutMainLoop();

    return 0;
}
