
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <cmath>

// Widgets
#include "viswidget.h"

// OpenGL
#include <GL/glu.h>

// Own includes
#include "message_handler.h"

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
    try
    {
        glClearColor(0.2, 0.2, 0.2, 1);
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::initializeGL()", e, true);
    }
}

void viswidget::paintGL()
{
    try
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
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

        draw_line(0, 0, 0, 1, 1, 0, 100, 120, 120, 120);
#else
        fvoctree tree;
        octcell *c = tree.root = new octcell(1, -1.0/3, -1.0/3, 0);
        c->refine();
        c = c->c[octcell::child_index(0, 1, 0)];
        c->refine();
        c = c->c[octcell::child_index(0, 0, 0)];
        c->refine();
        c = c->c[octcell::child_index(0, 1, 0)];
        c->refine();
        for (int i = 0; i < 2; i++) {
            c = c->c[octcell::child_index(0, 1, 0)];
            c->refine();
        }


        visualize_fvoctree(&tree);
#endif
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::paintGL()", e, true);
    }
}

void viswidget::resizeGL(int w, int h)
{
    try
    {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, 1, 0.01, 100);
        //gluPerspective(90, 1, 0.01, 100);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 2.5, 0, 0, 0, 0, 1, 0);
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::resizeGL()", e, true);
    }
}

void viswidget::visualize_octcell(octcell *cell, bool recursively)
{
    pftype x1 = cell->x;
    pftype y1 = cell->y;
    pftype z1 = cell->z;
    pftype x2 = x1 + cell->s;
    pftype y2 = y1 + cell->s;
    pftype z2 = z1 + cell->s;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    if (cell->has_child_array()) {
        set_line_style(1, 128, 128, 128, 255);
    }
    else {
        set_line_style(1, 255, 255, 255, 255);
    }

    quick_draw_line(x1, y1, z1, x2, y1, z1);
    quick_draw_line(x1, y1, z1, x1, y2, z1);
    quick_draw_line(x1, y1, z1, x1, y1, z2);

    quick_draw_line(x2, y1, z1, x2, y2, z1);
    quick_draw_line(x2, y1, z1, x2, y1, z2);
    quick_draw_line(x1, y2, z1, x2, y2, z1);
    quick_draw_line(x1, y2, z1, x1, y2, z2);
    quick_draw_line(x1, y1, z2, x2, y1, z2);
    quick_draw_line(x1, y1, z2, x1, y2, z2);

    quick_draw_line(x1, y2, z2, x2, y2, z2);
    quick_draw_line(x2, y1, z2, x2, y2, z2);
    quick_draw_line(x2, y2, z1, x2, y2, z2);

    glPopAttrib();

    if (recursively && cell->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (cell->c[i]) {
                visualize_octcell(cell->c[i], recursively);
            }
        }
    }
}

void viswidget::visualize_fvoctree(fvoctree *tree)
{
    if (tree->root) {
        visualize_octcell(tree->root, true);
    }
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
    glEnable(GL_LINE_SMOOTH);
    glColor4ub(r, g, b, a);

    glLineWidth(width);
}

void viswidget::quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz)
{
    glBegin(GL_LINES);
    glVertex3f(ax, ay, az);
    glVertex3f(bx, by, bz);
    glEnd();
}

void viswidget::draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLubyte  r, GLubyte g, GLubyte b, GLubyte a)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    set_line_style(width, r, g, b, a);
    quick_draw_line(ax, ay, az, bx, by, bz);
    glPopAttrib();
}
