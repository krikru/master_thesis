
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <cmath>

// Widgets
#include "viswidget.h"

// OpenGL
//#include <GL/glut.h>

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

viswidget::viswidget(QWidget *parent) :
    QGLWidget(parent)
{
}


////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////

void viswidget::initializeGL()
{
    glClearColor(0.2, 0.2, 0.2, 1);
}

void viswidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    {
        glColor3f(1, 0, 0);
        glVertex3f(-0.5, -0.5, 1);
        glColor3f(0, 1, 0);
        glVertex3f( 0.5, -0.5, 1);
        glColor3f(0, 0, 1);
        glVertex3f( 0.0,  0.5, 1);
    }
    glEnd();
    draw_line(0, 0, 1, 1, 1, 2, 100, 120, 120, 120);
}

void viswidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //set_gl_perspective(45, GLdouble(w)/h, 0.01, 100);
    //set_gl_perspective(90, 1, 0.01, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt();
}

////////////////////////////////////////////////////////////////
// PUBLIC STATIC FUNCTIONS
////////////////////////////////////////////////////////////////

/* Replacement of the depricated function gluPerspective */
void viswidget::set_gl_perspective(GLdouble fovy,  GLdouble aspect,  GLdouble zNear,  GLdouble zFar)
{
    GLdouble fH_2 = tan(fovy * (M_PI / 360)) * zNear;
    GLdouble fW_2 = fH_2 * aspect;
    glFrustum(-fW_2, fW_2, -fH_2, fH_2, zNear, zFar);
}

////////////////////////////////////////////////////////////////
// PRIVATE STATIC FUNCTIONS
////////////////////////////////////////////////////////////////

void viswidget::set_line_style(GLfloat width, GLubyte  r, GLubyte g, GLubyte b, GLubyte a)
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* Smooth lines */
    //glEnable(GL_LINE_SMOOTH);
    glColor4ub(r, g, b, a);

    glLineWidth(width);
}

void viswidget::quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz)
{
    glBegin(GL_LINES);
#if 0
    glVertex2f(ax, ay);
    glVertex2f(bx, by);
#elif 0
    glVertex3f(ax, ay, az);
    glVertex3f(bx, by, bz);
#else
    if (az > 0 && bz > 0) {
        GLfloat af = 1/az;
        GLfloat bf = 1/bz;
        glVertex2f(ax*af, ay*af);
        glVertex2f(bx*bf, by*bf);
    }
    /* Don't render line otherwise */
#endif
    glEnd();
}

void viswidget::draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLubyte  r, GLubyte g, GLubyte b, GLubyte a)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    set_line_style(width, r, g, b, a);
    quick_draw_line(ax, ay, az, bx, by, bz);
    glPopAttrib();
}
