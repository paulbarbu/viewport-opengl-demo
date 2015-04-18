#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

//TODO: render whole tea set

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
bool locked = false, help = true;

GLuint scene;

void display2DOverlay(bool show = true)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-viewW, viewW, -viewH, viewH, -1, 1);

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

void displayCenterText(const char *s, float w)
{
    float fontH = glutBitmapHeight(GLUT_BITMAP_HELVETICA_12);

    int l = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)s);
    glRasterPos2f(w/2.0 - l/2.0, 0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)s);
    glTranslatef(0, fontH, 0);
}

void displayHelp()
{
    float width = w*2.0;
    float height = h*2.0;
    float lineWidth = d * 2.0/100.0;
    float paddingH = height * 10.0/100.0;
    float paddingW = width * 10.0/100.0;
    float fontH = glutBitmapHeight(GLUT_BITMAP_HELVETICA_18);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glViewport(0, 0, width, height);
        glOrtho(0, width, height, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_SCISSOR_TEST);

        glClear(GL_COLOR_BUFFER_BIT);

        glLineWidth(lineWidth);

        glColor3f(0.5, 1, 0.5);

        height -= paddingH;
        width -= paddingW;

        glPushMatrix();
            glBegin(GL_QUADS);
                glVertex3f(paddingW, paddingH, 0); // top left
                glVertex3f(paddingW, height, 0); // bottom right
                glVertex3f(width, height, 0); // bottom left
                glVertex3f(width, paddingH, 0); // top left
            glEnd();


            glPushMatrix();
                glTranslatef(paddingW, paddingH + fontH, 0);
                glColor3f(0, 0, 0);
                displayCenterText("HELP!", width-paddingW);
                displayCenterText("\n", width-paddingW);
                displayCenterText("\n", width-paddingW);
                displayCenterText("\n", width-paddingW);

                displayCenterText("Definition:", width-paddingW);
                displayCenterText("Current viewport - the viewport where the mouse is located when an action is performed", width-paddingW);

                displayCenterText("\n", width-paddingW);
                displayCenterText("Keybindings:", width-paddingW);
                displayCenterText("R - reset current viewport", width-paddingW);
                displayCenterText("SHIFT+R - reset all viewports", width-paddingW);
                displayCenterText("H - toggle this help screen", width-paddingW);
                displayCenterText("L - toggle a global viewport (its settings will be applied to all viewports)", width-paddingW);
                displayCenterText("ESC - exit", width-paddingW);

                displayCenterText("\n", width-paddingW);
                displayCenterText("Mouse:", width-paddingW);
                displayCenterText("Scroll up/down - zoom in/out on the current viewport", width-paddingW);
                displayCenterText("Left click + drag - translate the current viewport", width-paddingW);
                displayCenterText("Right click + drag - rotate the current viewport", width-paddingW);

            glPopMatrix();
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

    if(help)
    {
        activeViewport = NULL;
        return;
    }

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

void createScene()
{
    // http://devernay.free.fr/cours/opengl/materials.html
    GLfloat pearlAmbient[] = {0.25, 0.20725, 0.20725, 1};
    GLfloat pearlDiffuse[] = {1, 0.829, 0.829, 1};
    GLfloat pearlSpecular[] = {0.296648, 0.296648, 0.296648, 1};
    GLfloat pearlShininess[] = {0.088 * 128};

    GLfloat whiteRubberAmbient[] = {0.05, 0.05, 0.05, 1};
    GLfloat whiteRubberDiffuse[] = {0.5, 0.5, 0.5, 1};
    GLfloat whiteRubberSpecular[] = {0.7, 0.7, 0.7, 1};
    GLfloat whiteRubberShininess[] = {0.078125 * 128};

    GLfloat porcelainAmbient[] = {0.2, 0.2, 0.2, 1};
    GLfloat porcelainDiffuse[] = {1, 1, 1, 1};
    GLfloat porcelainSpecular[] = {1, 1, 1, 1};
    GLfloat porcelainShininess[] = {0.3 * 128};

    scene = glGenLists(1);
    glNewList(scene, GL_COMPILE);
        glMaterialfv(GL_FRONT, GL_AMBIENT, pearlAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, pearlDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, pearlSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, pearlShininess);

        glPushMatrix();
            glutSolidTeapot(20);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(100, 0, 0);
            glutSolidTeapot(20);
        glPopMatrix();

        glMaterialfv(GL_FRONT, GL_AMBIENT, porcelainAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, porcelainDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, porcelainSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, porcelainShininess);

        glPushMatrix();
            glTranslatef(100, 0, -100);
            glutSolidTeacup(20);
        glPopMatrix();

        glMaterialfv(GL_FRONT, GL_AMBIENT, whiteRubberAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteRubberDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, whiteRubberSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, whiteRubberShininess);
        glPushMatrix();
            glTranslatef(0, 0, -100);
            glutSolidTeaspoon(20);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(50, 0, -100);
            glutSolidTeaspoon(20);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-100, 0, 0);
            glutSolidTeapot(20);
        glPopMatrix();
    glEndList();
}

bool initGL()
{
    GLfloat ambientLight[] = {1, 1, 1, 1};
    GLfloat lightPosition[] = {-200, 200, 200, 1};

    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glClearColor(0, 0, 0, 1);

    createScene();

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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float zoomFactor = 1;
    if(activeViewport)
    {
        zoomFactor = activeViewport->zoom;
    }

    glOrtho(-viewW*zoomFactor, viewW*zoomFactor, -viewH*zoomFactor, viewH*zoomFactor, -200, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        glTranslatef(activeViewport->translateX, activeViewport->translateY, 0);
        glRotatef(activeViewport->angleX, 1, 0, 0);
        glRotatef(activeViewport->angleY, 0, 1, 0);

        glCallList(scene);
    glPopMatrix();
}


void display(bool overlay = true)
{
    if(help) // if help is enabled, just display it
    {
        displayHelp();
    }
    else
    {
        // otherwise display the 3D scene and (maybe) the current viewport's overlay
        display3DScene();
        if(overlay)
        {
            display2DOverlay();
        }
    }

    glutSwapBuffers();
}

void render()
{
    if(help) // if help is enabled, there's no need to do anything else
    {
        display();
    }
    else if(activeViewport == NULL && !locked) // update all viewports
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
    else if(locked) // update the phantom viewport (whole screen)
    {
        glViewport(0, 0, w*2, h*2);
        glScissor(0, 0, w*2, h*2);
        activeViewport = &phantomViewport;
        display(false);
    }
    else // update the active viewport (part of screen)
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

            if(activeViewport->zoom > 0.1)
            {
                activeViewport->zoom -= 0.1;
                glutPostRedisplay();
            }
            printf("scroll up\n");
            break;
        case 4: // scroll down
                activeViewport->zoom += 0.1;
                glutPostRedisplay();
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

    if(doRender)
    {
        glutPostRedisplay();
    }
}

void keyPress(unsigned char key, int x, int y)
{
    printf("Key press: %d at x: %d\ty: %d\n", key, x, y);
    printf("Modifiers: %d\n", glutGetModifiers());

    setActiveViewport(x, y);

    //only use the keyboard after the help screen is closed
    if(help && key != 'h' && key != 'H' && key != 27)
    {
        return;
    }

    switch(key)
    {
        case 'R': // reset all viewports
            activeViewport = NULL;

            for(int i=0; i<4; i++)
            {
                viewports[i].reset();
            }
            phantomViewport.reset();

            glutPostRedisplay();
            break;
        case 'r': // reset the current viewport
            activeViewport->reset();
            glutPostRedisplay();
            break;
        case 'l':
            locked = !locked;

            printf("Locked: %d\n", locked);
            activeViewport = NULL;

            if(!locked)
            {
                // upon unlocking, copy the settings to every viewport
                for(int i=0; i<4; ++i)
                {
                    viewports[i].angleX = phantomViewport.angleX;
                    viewports[i].angleY = phantomViewport.angleY;
                    viewports[i].translateX = phantomViewport.translateX;
                    viewports[i].translateY = phantomViewport.translateY;
                    viewports[i].zoom = phantomViewport.zoom;
                }
            }

            glutPostRedisplay();
            break;
        case 'h':
        case 'H':
            help = !help;
            printf("Help: %d\n", help);

            if(help)
            {
                // no interaction possible during help screen
                activeViewport = NULL;
                glutMotionFunc(NULL);
                glutMouseFunc(NULL);
            }
            else
            {
                //enable interaction
                glutMotionFunc(mouseMove);
                glutMouseFunc(mousePress);
            }

            glutPostRedisplay();
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
    glutMotionFunc(NULL);
    glutMouseFunc(NULL);
    glutDisplayFunc(render);
    glutReshapeFunc(resize);

    glutMainLoop();

    return 0;
}
