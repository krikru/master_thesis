
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
        glClearColor(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, BACKGROUND_A);
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::initializeGL()", e, true);
    }
}

void viswidget::paintGL()
{
    static bool first_time_called = true;
    if (first_time_called) {
        first_time_called = false;
        t = 0;
        connect(&frame_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        frame_timer.start(FRAME_MS);
    }
    else {
        t += FRAME_MS/1000.0;
    }

    try
    {
        /* Generate tree */
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

        /* Set up perspective*/
        glViewport(0, 0, gl_width, gl_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
#if  NUM_DIMENSIONS == 3
        glTranslated(-1.0/6, -1.0/4, 0);
#endif
        gluPerspective(45, (GLdouble)gl_width/gl_height, 0.01, 100);

        set_up_model_view_matrix();

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
        gl_width  = w;
        gl_height = h;
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("viswidget::resizeGL()", e, true);
    }
}

void viswidget::quick_draw_cell(octcell* cell)
{
    pfvec r1 = cell->r;
    //TODO: Optimize
    pfvec r2 = 2*cell->cell_center() - cell->r;
#if    NUM_DIMENSIONS == 2
    quick_draw_line(r1[DIM_X], r1[DIM_Y], 0, r2[DIM_X], r1[DIM_Y], 0);
    quick_draw_line(r2[DIM_X], r1[DIM_Y], 0, r2[DIM_X], r2[DIM_Y], 0);
    quick_draw_line(r2[DIM_X], r2[DIM_Y], 0, r1[DIM_X], r2[DIM_Y], 0);
    quick_draw_line(r1[DIM_X], r2[DIM_Y], 0, r1[DIM_X], r1[DIM_Y], 0);
#elif  NUM_DIMENSIONS == 3
    quick_draw_line(r1[DIM_X], r1[DIM_Y], r1[DIM_Z], r2[DIM_X], r1[DIM_Y], r1[DIM_Z]);
    quick_draw_line(r1[DIM_X], r1[DIM_Y], r1[DIM_Z], r1[DIM_X], r2[DIM_Y], r1[DIM_Z]);
    quick_draw_line(r1[DIM_X], r1[DIM_Y], r1[DIM_Z], r1[DIM_X], r1[DIM_Y], r2[DIM_Z]);

    quick_draw_line(r2[DIM_X], r1[DIM_Y], r1[DIM_Z], r2[DIM_X], r2[DIM_Y], r1[DIM_Z]);
    quick_draw_line(r2[DIM_X], r1[DIM_Y], r1[DIM_Z], r2[DIM_X], r1[DIM_Y], r2[DIM_Z]);
    quick_draw_line(r1[DIM_X], r2[DIM_Y], r1[DIM_Z], r2[DIM_X], r2[DIM_Y], r1[DIM_Z]);
    quick_draw_line(r1[DIM_X], r2[DIM_Y], r1[DIM_Z], r1[DIM_X], r2[DIM_Y], r2[DIM_Z]);
    quick_draw_line(r1[DIM_X], r1[DIM_Y], r2[DIM_Z], r2[DIM_X], r1[DIM_Y], r2[DIM_Z]);
    quick_draw_line(r1[DIM_X], r1[DIM_Y], r2[DIM_Z], r1[DIM_X], r2[DIM_Y], r2[DIM_Z]);

    quick_draw_line(r1[DIM_X], r2[DIM_Y], r2[DIM_Z], r2[DIM_X], r2[DIM_Y], r2[DIM_Z]);
    quick_draw_line(r2[DIM_X], r1[DIM_Y], r2[DIM_Z], r2[DIM_X], r2[DIM_Y], r2[DIM_Z]);
    quick_draw_line(r2[DIM_X], r2[DIM_Y], r1[DIM_Z], r2[DIM_X], r2[DIM_Y], r2[DIM_Z]);
#endif
}

void viswidget::set_up_model_view_matrix(GLdouble scale_factor)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScaled(scale_factor, scale_factor, scale_factor);
#if  NUM_DIMENSIONS == 2
    gluLookAt(.5, .5, 1.5, .5, .5, 0, 0, 1, 0);
#elif  NUM_DIMENSIONS == 3
    // From front
    gluLookAt(1.0/3, -1.5, 1.0/3, 1.0/3, 0.5, 1.0/3, 0, 0, 1);
    //From below
    //gluLookAt(1.0/3, 1.0/3, -1.5, 1.0/3, 1.0/3, .5, 0, -1, 0);

    /* Rotate alpha degrees around the axis (ax, ay, az) through the point (x, y, z) */
    GLdouble w = 6;
    GLdouble alpha = w*t;
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
}

void viswidget::visualize_leaf_cells_and_neighbor_connections_recursively(octcell* cell)
{
#if DRAW_CELL_CUBES
    if (cell->is_leaf()) {
        quick_draw_cell(cell);
    }
#endif

    if (cell->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (cell->get_child(i)) {
                visualize_leaf_cells_and_neighbor_connections_recursively(cell->get_child(i));
            }
        }
    }

    /* Draw neighbor connections */
    nlnode* nn = cell->get_first_neighbor_list_node();
    if (DRAW_NEIGHBOR_CONNECTIONS && nn) {
        pfvec center1 = cell->cell_center();

        /* Draw everything but the middle of the connection  */
        quick_set_color(NEIGHBOR_CONNECTION_R, NEIGHBOR_CONNECTION_G, NEIGHBOR_CONNECTION_B, NEIGHBOR_CONNECTION_A);
        for (; nn; nn = nn->get_next_node()) {
            pfvec center2 = nn->v.n->cell_center();
#if  MARK_MIDDLE_OF_CONNECTION
            quick_draw_line(center1, center1 + (.5-MIDDLE_MARK_SIZE)*(center2-center1));
#else
            quick_draw_line(center1, .5*(center1 + center2));
#endif
        }

#if  MARK_MIDDLE_OF_CONNECTION
        /* Mark middle of connections */
        quick_set_color(MIDDLE_MARK_R, MIDDLE_MARK_G, MIDDLE_MARK_B, MIDDLE_MARK_A);
        for (nn = cell->get_first_neighbor_list_node(); nn; nn = nn->get_next_node()) {
            pfvec center2 = nn->v.n->cell_center();
            quick_draw_line(center1 + (.5-MIDDLE_MARK_SIZE)*(center2-center1), .5*(center1 + center2));
        }
#endif

#if DRAW_CELL_CUBES
        quick_set_color(LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
#endif
    }
}

void viswidget::visualize_parent_cells_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        return;
    }
    quick_draw_cell(cell);
    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            visualize_parent_cells_recursively(cell->get_child(i));
        }
    }
}

void viswidget::visualize_fvoctree(fvoctree *tree)
{
    if (tree->root) {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
#if  DRAW_CELL_CUBES
#if  !(TEST_DEPTH && DRAW_CHILD_CELLS_FIRST_IF_DEPTH_TESTING) && DRAW_PARENT_CELLS
        set_line_style(1, PARENT_CUBE_R, PARENT_CUBE_G, PARENT_CUBE_B, PARENT_CUBE_A);
        set_up_model_view_matrix(PARENT_CUBE_DIST_SCALEING);
        visualize_parent_cells_recursively(tree->root);
#endif
        set_line_style(1, LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
#else
        set_line_style(1, NEIGHBOR_CONNECTION_R, NEIGHBOR_CONNECTION_G, NEIGHBOR_CONNECTION_B, NEIGHBOR_CONNECTION_A);
#endif
        set_up_model_view_matrix();
        visualize_leaf_cells_and_neighbor_connections_recursively(tree->root);
#if  DRAW_CELL_CUBES && TEST_DEPTH && DRAW_CHILD_CELLS_FIRST_IF_DEPTH_TESTING && DRAW_PARENT_CELLS
        set_line_style(1, PARENT_CUBE_R, PARENT_CUBE_G, PARENT_CUBE_B, PARENT_CUBE_A);
        set_up_model_view_matrix(PARENT_CUBE_DIST_SCALEING);
        visualize_parent_cells_recursively(tree->root);
#endif
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
    c->r[DIM_X] += 0.01;
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
#if  TEST_DEPTH
    glEnable(GL_DEPTH_TEST);
#endif
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
#if    NUM_DIMENSIONS == 2
    quick_draw_line(p1.e[DIM_X], p1.e[DIM_Y], 0,
                    p2.e[DIM_X], p2.e[DIM_Y], 0);
#elif  NUM_DIMENSIONS == 3
    quick_draw_line(p1.e[DIM_X], p1.e[DIM_Y], p1.e[DIM_Z],
                    p2.e[DIM_X], p2.e[DIM_Y], p2.e[DIM_Z]);
#endif
}

void viswidget::draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    set_line_style(width, r, g, b, a);
    quick_draw_line(ax, ay, az, bx, by, bz);
    glPopAttrib();
}
