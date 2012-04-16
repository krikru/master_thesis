
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <cmath>
#include <time.h>
#include <iostream>
using std::cout;
using std::endl;

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
        static fvoctree* tree = 0;
        double t1, t2;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (tree == 0) {
            BEGIN_TAKE_TIME("Generating octree... ");
            tree = new fvoctree(0, 0);
            END_TAKE_TIME();
        }

        BEGIN_TAKE_TIME("Moving octree... ");
        move_fvoctree(tree);
        END_TAKE_TIME();


        BEGIN_TAKE_TIME("Visualizing octree... ");
        visualize_fvoctree(tree);
        END_TAKE_TIME();
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
        glTranslated(-1.0/6, -1.0/4, 0);
        gluPerspective(45, (GLdouble)w/h, 0.01, 100);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(1.0/3, -1.5, 1.0/3, 1.0/3, 0.5, 1.0/3, 0, 0, 1);
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::resizeGL()", e, true);
    }
}

void viswidget::visualize_octcell_recursively(octcell *cell, bool recursively)
{
    pftype x1 = cell->x;
    pftype y1 = cell->y;
    pftype z1 = cell->z;
    pftype x2 = x1 + cell->s;
    pftype y2 = y1 + cell->s;
    pftype z2 = z1 + cell->s;

    if (DRAW_PARENT_CELLS && cell->has_child_array()) {
        //set_line_style(1, 128, 128, 128, 255);
        glColor4ub(128, 128, 128, 255);
    }

    if (DRAW_PARENT_CELLS || cell->is_leaf()) {
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
    }

    if (DRAW_PARENT_CELLS && cell->has_child_array()) {
        //set_line_style(1, 255, 255, 255, 255);
        glColor4ub(255, 255, 255, 255);
    }

    if (recursively && cell->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (cell->c[i]) {
                visualize_octcell_recursively(cell->c[i], recursively);
            }
        }
    }
}

void viswidget::visualize_fvoctree(fvoctree *tree)
{
    if (tree->root) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        set_line_style(1, 255, 255, 255, 255);
        visualize_octcell_recursively(tree->root, true);
        glPopAttrib();
    }


}

// TODO: Remove move_fvoctree() and move_octcell().
void viswidget::move_fvoctree(fvoctree *tree)
{
    if (tree->root) {
        move_octcell(tree->root);
    }
}

void viswidget::move_octcell(octcell *c)
{
    c->x += 0.01;
    if (c->c) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (c->c[i]) {
                move_octcell(c->c[i]);
            }
        }
    }
}

////////////////////////////////////////////////////////////////
// PRIVATE STATIC FUNCTIONS
////////////////////////////////////////////////////////////////

void viswidget::set_line_style(GLfloat width, GLubyte  r, GLubyte g, GLubyte b, GLubyte a)
{
    glDisable(GL_TEXTURE_2D);
#if  DRAW_SMOOTH_LINES
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
#else
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
#endif
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
