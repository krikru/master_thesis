
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
    connect(&frame_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    frame_timer.start(16);
}


////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////

void viswidget::initializeGL()
{
    try
    {
        glClearColor(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, BACKGROUND_A);
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

#if 0
        BEGIN_TAKE_TIME("Moving octree... ");
        move_fvoctree(tree);
        END_TAKE_TIME();
#endif

#if (NUM_DIRECTIONS == 3)
        /* Rotate alpha degrees around the axis (ax, ay, az) through the point (x, y, z) */
        GLdouble alpha = 0.1;
        GLdouble ax = .5;
        GLdouble ay = 0;
        GLdouble az = 1;
        GLdouble x = 0.5;
        GLdouble y = 0.5;
        GLdouble z = 0.5;
        glTranslated( x,  y,  z);
        glRotated(alpha, ax, ay, az);
        glTranslated(-x, -y, -z);
#endif

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
#if  NUM_DIRECTIONS == 3
        glTranslated(-1.0/6, -1.0/4, 0);
#endif
        gluPerspective(45, (GLdouble)w/h, 0.01, 100);


        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
#if  NUM_DIRECTIONS == 2
        gluLookAt(.5, .5, -1.5, .5, .5, 0, 0, 1, 0);
#elif  NUM_DIRECTIONS == 3
        // From front
        gluLookAt(1.0/3, -1.5, 1.0/3, 1.0/3, 0.5, 1.0/3, 0, 0, 1);
        //From below
        //gluLookAt(1.0/3, 1.0/3, -1.5, 1.0/3, 1.0/3, .5, 0, -1, 0);
#endif
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::resizeGL()", e, true);
    }
}

void viswidget::visualize_octcell_recursively(octcell *cell, bool recursively)
{
#if DRAW_CELL_CUBES
    pftype x1 = cell->x;
    pftype x2 = x1 + cell->s;
    pftype y1 = cell->y;
    pftype y2 = y1 + cell->s;
#if  NUM_DIRECTIONS == 3
    pftype z1 = cell->z;
    pftype z2 = z1 + cell->s;
#endif

    if (DRAW_PARENT_CELLS && cell->has_child_array()) {
        quick_set_color(PARENT_CUBE_R, PARENT_CUBE_G, PARENT_CUBE_B, PARENT_CUBE_A);
    }

    if (DRAW_PARENT_CELLS || cell->is_leaf()) {
#if    NUM_DIRECTIONS == 2
        quick_draw_line(x1, y1, 0, x2, y1, 0);
        quick_draw_line(x2, y1, 0, x2, y2, 0);
        quick_draw_line(x2, y2, 0, x1, y2, 0);
        quick_draw_line(x1, y2, 0, x1, y1, 0);
#elif  NUM_DIRECTIONS == 3
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
#endif
    }

    if (DRAW_PARENT_CELLS && cell->has_child_array()) {
        quick_set_color(LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
    }
#endif

    if (recursively && cell->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (cell->get_child(i)) {
                visualize_octcell_recursively(cell->get_child(i), recursively);
            }
        }
    }

    nlnode* nn = cell->get_first_neighbor_list_node();
    if (DRAW_NEIGHBOR_CONNECTIONS && nn) {
        quick_set_color(NEIGHBOR_CONNECTION_R, NEIGHBOR_CONNECTION_G, NEIGHBOR_CONNECTION_B, NEIGHBOR_CONNECTION_A);
        pfvec center1 = cell->cell_center();
        for (; nn; nn = nn->get_next_node()) {
            pfvec center2 = nn->v.n->cell_center();
            quick_draw_line(center1, .5*(center1 + center2));
        }
#if DRAW_CELL_CUBES
        quick_set_color(LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
#endif
    }

}

void viswidget::visualize_fvoctree(fvoctree *tree)
{
    if (tree->root) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
#if DRAW_CELL_CUBES
        set_line_style(1, LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
#else
        set_line_style(1, NEIGHBOR_CONNECTION_R, NEIGHBOR_CONNECTION_G, NEIGHBOR_CONNECTION_B, NEIGHBOR_CONNECTION_A);
#endif
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
    if (c->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (c->get_child(i)) {
                move_octcell(c->get_child(i));
            }
        }
    }
}

////////////////////////////////////////////////////////////////
// PRIVATE STATIC FUNCTIONS
////////////////////////////////////////////////////////////////

void viswidget::quick_set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    glColor4f(r, g, b, a);
}

void viswidget::set_line_style(GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a)
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
    quick_set_color(r, g, b, a);

    glLineWidth(width);
}

void viswidget::quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz)
{
    glBegin(GL_LINES);
    glVertex3f(ax, ay, az);
    glVertex3f(bx, by, bz);
    glEnd();
}

void viswidget::quick_draw_line(pfvec p1, pfvec p2)
{
#if    NUM_DIRECTIONS == 2
    quick_draw_line(p1.e[DIR_X], p1.e[DIR_Y], 0,
                    p2.e[DIR_X], p2.e[DIR_Y], 0);
#elif  NUM_DIRECTIONS == 3
    quick_draw_line(p1.e[DIR_X], p1.e[DIR_Y], p1.e[DIR_Z],
                    p2.e[DIR_X], p2.e[DIR_Y], p2.e[DIR_Z]);
#endif
}

void viswidget::draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    set_line_style(width, r, g, b, a);
    quick_draw_line(ax, ay, az, bx, by, bz);
    glPopAttrib();
}
