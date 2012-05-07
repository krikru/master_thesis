
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
#include "base_float_vec3.h"

////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

typedef base_float_vec3<GLfloat> color3;

////////////////////////////////////////////////////////////////
// STATIC VARIABLES
////////////////////////////////////////////////////////////////

GLfloat* viswidget::NEIGHBOR_CONNECTION_R = 0;
GLfloat* viswidget::NEIGHBOR_CONNECTION_G = 0;
GLfloat* viswidget::NEIGHBOR_CONNECTION_B = 0;
GLfloat* viswidget::NEIGHBOR_CONNECTION_A = 0;

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

viswidget::viswidget(QWidget *parent) :
    QGLWidget(parent)
{
    /* Init member variables */
    system_to_visualize = 0;

    init_neighbor_connection_colors();
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
        //connect(&frame_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        //frame_timer.start(FRAME_MS);
        t = 0;
    }
    else {
        t += FRAME_MS*.001;
    }

    try
    {
        /* Generate tree */
        double t1, t2;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!system_to_visualize) {
            /* Nothing to draw */
            return;
        }

#if  0
        BEGIN_TAKE_TIME("Moving octree... ");
        move_fvoctree(system_to_visualize->get_water());
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
        visualize_fvoctree(system_to_visualize->get_water());
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

void viswidget::set_system_to_visualize(watersystem* system)
{
    system_to_visualize = system;
}

////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////


void viswidget::draw_pressure_deviation(octcell* cell)
{
    /* Calculate color */
    /*
     * In rising order: White, blue, cyan, green, yellow, red, black
     */
    const uint NUM_TRANSITIONS = 6;
    const color3 colors[] = {color3(1, 1, 1),
                             color3(0, 0, 1),
                             color3(0, 1, 1),
                             color3(0, 1, 0),
                             color3(1, 1, 0),
                             color3(1, 0, 0),
                             color3(0, 0, 0)};
    pftype q = NUM_TRANSITIONS * (cell->p / (P_G * P_WATER_DENSITY) - (SURFACE_HEIGHT - cell->get_cell_center().e[VERTICAL_DIMENSION]) + 0.5);
    q = MIN(MAX(q, 0), NUM_TRANSITIONS);
    uint idx1 = uint(q);
    uint idx2 = MIN(idx1 + 1, NUM_TRANSITIONS);
    q -= idx1;
    color3 c = (1-q)*colors[idx1] + q*colors[idx2];
#if  DEBUG
    if (q < 0) {
        throw domain_error("q < 0");
    }
    if (q > 1) {
        throw domain_error("q < 0");
    }
    if (idx1 < 0) {
        throw domain_error("idx1 < 0");
    }
    if (idx1 > NUM_TRANSITIONS) {
        throw domain_error("idx1 > NUM_TRANSITIONS");
    }
    if (idx2 < 0) {
        throw domain_error("idx2 < 0");
    }
    if (idx2 > NUM_TRANSITIONS) {
        throw domain_error("idx2 > NUM_TRANSITIONS");
    }
#endif
    //quick_set_color(c[0], c[1], c[2], 1);
    quick_set_color(c[0], c[1], c[2], cell->get_alpha());

#if    NUM_DIMENSIONS == 2
    /* Vertices */
    pfvec p00 = cell->r;
    pfvec p01 = p00;
    p01[DIM_X] += cell->s;
    pfvec p10 = p00;
    p10[DIM_Y] += cell->s;
    pfvec p11 = p10 + p01 - p00;

    /* Draw triangle*/
    quick_draw_triangle(p00, p01, p10);
    quick_draw_triangle(p11, p10, p01);

#elif  NUM_DIMENSIONS == 3
    /* Don't draw pressure */
#endif
}

void viswidget::quick_mark_water_cell(octcell* cell)
{
#if    NUM_DIMENSIONS == 2
    pftype rad = 0.4 * cell->s;
    pfvec e_x, e_y;
    e_x.e[DIM_X] = 1;
    e_y.e[DIM_Y] = 1;
    quick_draw_circle(cell->get_cell_center(), rad* e_y, rad * e_x, NUM_LINES_IN_CIRCLES);
#elif  NUM_DIMENSIONS == 3
#endif
}

void viswidget::quick_air_empty_cell(octcell* cell)
{
#if    NUM_DIMENSIONS == 2
    pftype x0 = cell->r.e[DIM_X];
    pftype x1 = x0 + cell->s;
    pftype y0 = cell->r.e[DIM_Y];
    pftype y1 = y0 + cell->s;
    quick_draw_line(x0, y0, 0, x1, y1, 0);
    quick_draw_line(x0, y1, 0, x1, y0, 0);
#elif  NUM_DIMENSIONS == 3
#endif
}

void viswidget::quick_draw_cell_water_level(octcell* cell)
{
#if    NUM_DIMENSIONS == 2
    pfvec p0 = cell->r;
    p0.e[VERTICAL_DIMENSION] += cell->get_alpha() * cell->s;
    pfvec p1 = p0;
    p1.e[HORIZONTAL_DIMENSION1] += cell->s;
    quick_draw_line(p0, p1);
#elif  NUM_DIMENSIONS == 3
    pfvec p00 = cell->r;
    p00.e[VERTICAL_DIMENSION] += cell->alpha * cell->s;
    pfvec p01 = p00;
    p01.e[HORIZONTAL_DIMENSION1] += cell->s;
    pfvec p10 = p00;
    p10.e[HORIZONTAL_DIMENSION2] += cell->s;
    pfvec p11 = p10 + p01 - p00;
    quick_draw_line(p00, p01);
    quick_draw_line(p01, p11);
    quick_draw_line(p11, p10);
    quick_draw_line(p10, p00);
#endif
}

void viswidget::quick_draw_cell(octcell* cell)
{
    pfvec r1 = cell->r;
    //TODO: Optimize
    pfvec r2 = 2*cell->get_cell_center() - cell->r;
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

void viswidget::visualize_leaf_cells_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
#if  DRAW_ONLY_SURFACE_CELLS
        if (!cell->surface_cell) {
            return;
        }
#endif
        quick_draw_cell(cell);
        return;
    }

    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            visualize_leaf_cells_recursively(cell->get_child(i));
        }
    }
}


void viswidget::draw_pressure_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        draw_pressure_deviation(cell);
        return;
    }

    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            draw_pressure_recursively(cell->get_child(i));
        }
    }
}

void viswidget::mark_water_cells_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        if (cell->is_water_cell()) {
            quick_mark_water_cell(cell);
        }
        return;
    }

    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            mark_water_cells_recursively(cell->get_child(i));
        }
    }
}

void viswidget::mark_air_cells_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        if (cell->is_air_cell()) {
            quick_air_empty_cell(cell);
        }
        return;
    }

    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            mark_air_cells_recursively(cell->get_child(i));
        }
    }
}

void viswidget::draw_water_level_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        if (cell->is_mixed_cell()) {
            quick_draw_cell_water_level(cell);
        }
        return;
    }

    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            draw_water_level_recursively(cell->get_child(i));
        }
    }
}

void viswidget::visualize_parent_cells_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        return;
    }
    else {
        quick_draw_cell(cell);
    }

    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            visualize_parent_cells_recursively(cell->get_child(i));
        }
    }
}

void viswidget::visualize_neighbor_connections_recursively(octcell* cell)
{
#if  VISUALIZE_ONLY_FINEST_NEIGHBOR_CONNECTIONS
    quick_set_color(FINEST_NEIGHBOR_CONNECTION_R, FINEST_NEIGHBOR_CONNECTION_G, FINEST_NEIGHBOR_CONNECTION_B, FINEST_NEIGHBOR_CONNECTION_A);
    visualize_finest_neighbor_connections_recursively(cell);
#else
    for (uint i = 0; i < NUM_NEIGHBOR_LISTS; i++) {
        quick_set_color(NEIGHBOR_CONNECTION_R[i],
                        NEIGHBOR_CONNECTION_G[i],
                        NEIGHBOR_CONNECTION_B[i],
                        NEIGHBOR_CONNECTION_A[i]);
        visualize_neighbor_connections_recursively(cell, i);
    }
#endif
}

void viswidget::visualize_neighbor_connections_recursively(octcell* cell, uint neighbor_list_index)
{

    if (cell->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (cell->get_child(i)) {
                visualize_neighbor_connections_recursively(cell->get_child(i), neighbor_list_index);
            }
        }
    }

    /* Draw neighbor connections */
    nlnode* node = cell->neighbor_lists[neighbor_list_index].get_first_node();
    if (node) {
        pfvec center1 = cell->get_cell_center();

        /* Draw everything but the middle of the connection  */
        for (; node; node = node->get_next_node()) {
            pfvec center2 = node->v.n->get_cell_center();
            pfvec diff = center2 - center1;
#if  RANDOMIZE_NEIGHBOR_CONNECTION_MIDPOINTS
            pfvec ovec = diff.random_equal_lenth_orthogonal_vector();
            diff += ovec * uniform(0, NEIGHBOR_CONNECTION_MIDPOINT_RANDOMIZATION);
#endif
#if  MARK_MIDDLE_OF_CONNECTION
            quick_draw_line(center1, center1 + (.5-MIDDLE_MARK_SIZE)*diff);
#else
            quick_draw_line(center1, center1 +  .5                  *diff);
#endif
        }

#if  MARK_MIDDLE_OF_CONNECTION
        /* Mark middle of connections */
        quick_set_color(MIDDLE_MARK_R, MIDDLE_MARK_G, MIDDLE_MARK_B, MIDDLE_MARK_A);
        for (node = cell->neighbor_lists[neighbor_list_index].get_first_node(); node; node = node->get_next_node()) {
            pfvec center2 = node->v.n->cell_center();
            quick_draw_line(center1 + (.5-MIDDLE_MARK_SIZE)*(center2-center1), .5*(center1 + center2));
        }
        quick_set_color(NEIGHBOR_CONNECTION_R[neighbor_list_index],
                        NEIGHBOR_CONNECTION_G[neighbor_list_index],
                        NEIGHBOR_CONNECTION_B[neighbor_list_index],
                        NEIGHBOR_CONNECTION_A[neighbor_list_index]);
#endif
    }
}

void viswidget::visualize_finest_neighbor_connections_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
            if (cell->get_child(i)) {
                visualize_finest_neighbor_connections_recursively(cell->get_child(i));
            }
        }
        return;
    }

    /* Draw neighbor connections to leaf cells */
    pfvec center1 = cell->get_cell_center();
    nlset leaf_set;

    /* Draw everything but the middle of the connection  */
    leaf_set.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    leaf_set.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF ]);
    leaf_set.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL    ]);
    for (nlnode* node = leaf_set.get_first_node(); node; node = leaf_set.get_next_node()) {
        pfvec center2 = node->v.n->get_cell_center();
        pfvec diff = center2 - center1;
#if  RANDOMIZE_NEIGHBOR_CONNECTION_MIDPOINTS
        pfvec ovec = diff.random_equal_lenth_orthogonal_vector();
        diff += ovec * uniform(0, NEIGHBOR_CONNECTION_MIDPOINT_RANDOMIZATION);
#endif
#if  MARK_MIDDLE_OF_CONNECTION
        quick_draw_line(center1, center1 + (.5-MIDDLE_MARK_SIZE)*diff);
#else
        quick_draw_line(center1, center1 +  .5                  *diff);
#endif
    }

#if  MARK_MIDDLE_OF_CONNECTION
    /* Mark middle of connections */
    quick_set_color(MIDDLE_MARK_R, MIDDLE_MARK_G, MIDDLE_MARK_B, MIDDLE_MARK_A);
    leaf_set.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    leaf_set.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF ]);
    leaf_set.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL    ]);
    for (nlnode* node = leaf_set.get_first_node(); node; node = leaf_set.get_next_node()) {
        pfvec center2 = node->v.n->cell_center();
        quick_draw_line(center1 + (.5-MIDDLE_MARK_SIZE)*(center2-center1), .5*(center1 + center2));
    }
    quick_set_color(FINEST_NEIGHBOR_CONNECTION_R, FINEST_NEIGHBOR_CONNECTION_G, FINEST_NEIGHBOR_CONNECTION_B, FINEST_NEIGHBOR_CONNECTION_A);
#endif
}

void viswidget::visualize_fvoctree(fvoctree *tree)
{
    if (!tree->root) {
        set_line_style(50, 1, 0, 0, 1);
        quick_draw_line(0, 0, 0, 1, 1, 1);
        quick_draw_line(1, 0, 0, 0, 1, 1);
        quick_draw_line(0, 1, 0, 1, 0, 1);
        quick_draw_line(0, 0, 1, 1, 1, 0);
        return;
    }
    glPushAttrib(GL_ALL_ATTRIB_BITS);
#if DRAW_PRESSURE
    set_up_model_view_matrix(PRESSURE_DISTANCE_SCALING);
    draw_pressure_recursively(tree->root);
#endif
#if DRAW_WATER_LEVEL
    set_up_model_view_matrix();
    set_line_style(LINE_WIDTH, SURFACE_R, SURFACE_G, SURFACE_B, SURFACE_A);
    draw_water_level_recursively(tree->root);
#endif
#if  DRAW_CELL_CUBES
#if  !(TEST_DEPTH && DRAW_CHILD_CELLS_FIRST_IF_DEPTH_TESTING) && DRAW_PARENT_CELLS && !DRAW_ONLY_SURFACE_CELLS
    /* Draw parent cells */
    set_line_style(LINE_WIDTH, PARENT_CUBE_R, PARENT_CUBE_G, PARENT_CUBE_B, PARENT_CUBE_A);
    set_up_model_view_matrix(PARENT_CUBE_DIST_SCALING);
    visualize_parent_cells_recursively(tree->root);
    /* Draw leaf cells */
    set_line_style(LINE_WIDTH, LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
    set_up_model_view_matrix();
    visualize_leaf_cells_recursively(tree->root);
#else
    /* Draw leaf cells */
    set_line_style(LINE_WIDTH, LEAF_CUBE_R, LEAF_CUBE_G, LEAF_CUBE_B, LEAF_CUBE_A);
    set_up_model_view_matrix(LEAF_CUBE_DIST_SCALING);
    visualize_leaf_cells_recursively(tree->root);
#if DRAW_PARENT_CELLS && !DRAW_ONLY_SURFACE_CELLS
    /* Draw parent cells */
    set_line_style(LINE_WIDTH, PARENT_CUBE_R, PARENT_CUBE_G, PARENT_CUBE_B, PARENT_CUBE_A);
    set_up_model_view_matrix(PARENT_CUBE_DIST_SCALING);
    visualize_parent_cells_recursively(tree->root);
#endif // DRAW_PARENT_CELLS
#endif // Parent/leaf cell order
#endif // DRAW_CELL_CUBES

#if  MARK_BULK_CELLS
    /* Mark bulk cells */
    set_line_style(BULK_CELL_MARK_LINE_WIDTH, BULK_CELL_MARK_R, BULK_CELL_MARK_G, BULK_CELL_MARK_B, BULK_CELL_MARK_A);
    set_up_model_view_matrix(BULK_CELL_MARK_SCALING);
    mark_water_cells_recursively(tree->root);
#endif

#if  MARK_EMPTY_CELLS
    /* Mark empty cells */
    set_line_style(EMPTY_CELL_MARK_LINE_WIDTH, EMPTY_CELL_MARK_R, EMPTY_CELL_MARK_G, EMPTY_CELL_MARK_B, EMPTY_CELL_MARK_A);
    set_up_model_view_matrix(EMPTY_CELL_MARK_SCALING);
    mark_air_cells_recursively(tree->root);
#endif

#if  DRAW_NEIGHBOR_CONNECTIONS
    set_up_model_view_matrix(NEIGHBOR_CONNECTIONS_DIST_SCALING);
    visualize_neighbor_connections_recursively(tree->root);
#endif
    glPopAttrib();
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

void viswidget::init_neighbor_connection_colors()
{
#if  !VISUALIZE_ONLY_FINEST_NEIGHBOR_CONNECTIONS
    NEIGHBOR_CONNECTION_R = new GLfloat[NUM_NEIGHBOR_LISTS];
    NEIGHBOR_CONNECTION_G = new GLfloat[NUM_NEIGHBOR_LISTS];
    NEIGHBOR_CONNECTION_B = new GLfloat[NUM_NEIGHBOR_LISTS];
    NEIGHBOR_CONNECTION_A = new GLfloat[NUM_NEIGHBOR_LISTS];
    for (uint i = 0; i < NUM_NEIGHBOR_LISTS; i++) {
        NEIGHBOR_CONNECTION_R[i] = NEIGHBOR_CONNECTION_G[i] = NEIGHBOR_CONNECTION_B[i] = NEIGHBOR_CONNECTION_A[i] = 0;
    }

    NEIGHBOR_CONNECTION_R[NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF] = LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_R;
    NEIGHBOR_CONNECTION_G[NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF] = LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_G;
    NEIGHBOR_CONNECTION_B[NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF] = LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_B;
    NEIGHBOR_CONNECTION_A[NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF] = LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_A;

    NEIGHBOR_CONNECTION_R[NL_LOWER_LEVEL_OF_DETAIL_LEAF    ] = LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_R    ;
    NEIGHBOR_CONNECTION_G[NL_LOWER_LEVEL_OF_DETAIL_LEAF    ] = LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_G    ;
    NEIGHBOR_CONNECTION_B[NL_LOWER_LEVEL_OF_DETAIL_LEAF    ] = LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_B    ;
    NEIGHBOR_CONNECTION_A[NL_LOWER_LEVEL_OF_DETAIL_LEAF    ] = LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_A    ;

    NEIGHBOR_CONNECTION_R[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF ] = SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_R ;
    NEIGHBOR_CONNECTION_G[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF ] = SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_G ;
    NEIGHBOR_CONNECTION_B[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF ] = SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_B ;
    NEIGHBOR_CONNECTION_A[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF ] = SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_A ;

    NEIGHBOR_CONNECTION_R[NL_SAME_LEVEL_OF_DETAIL_LEAF     ] = SAME_LOD_LEAF_NEIGHBOR_CONNECTION_R     ;
    NEIGHBOR_CONNECTION_G[NL_SAME_LEVEL_OF_DETAIL_LEAF     ] = SAME_LOD_LEAF_NEIGHBOR_CONNECTION_G     ;
    NEIGHBOR_CONNECTION_B[NL_SAME_LEVEL_OF_DETAIL_LEAF     ] = SAME_LOD_LEAF_NEIGHBOR_CONNECTION_B     ;
    NEIGHBOR_CONNECTION_A[NL_SAME_LEVEL_OF_DETAIL_LEAF     ] = SAME_LOD_LEAF_NEIGHBOR_CONNECTION_A     ;

    NEIGHBOR_CONNECTION_R[NL_HIGHER_LEVEL_OF_DETAIL        ] = HIGHER_LOD_NEIGHBOR_CONNECTION_R        ;
    NEIGHBOR_CONNECTION_G[NL_HIGHER_LEVEL_OF_DETAIL        ] = HIGHER_LOD_NEIGHBOR_CONNECTION_G        ;
    NEIGHBOR_CONNECTION_B[NL_HIGHER_LEVEL_OF_DETAIL        ] = HIGHER_LOD_NEIGHBOR_CONNECTION_B        ;
    NEIGHBOR_CONNECTION_A[NL_HIGHER_LEVEL_OF_DETAIL        ] = HIGHER_LOD_NEIGHBOR_CONNECTION_A        ;

#if  DEBUG
    for (uint i = 0; i < NUM_NEIGHBOR_LISTS; i++) {
        if (!NEIGHBOR_CONNECTION_A[i]) {
            throw logic_error("All neighbor connection colors were not initialized");
        }
    }
#endif
#endif
}

void viswidget::quick_set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    static bool blending_enabled = false;
    if (a < 1 && !blending_enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        blending_enabled = true;
    }
    GLfloat back_light = 0;
    r = r*a + back_light*(1-a);
    g = g*a + back_light*(1-a);
    b = b*a + back_light*(1-a);
    glColor4f(r, g, b, a);
}

void viswidget::set_line_style(GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a)
{
    static bool blending_enabled = false;
    if (a < 1 && !blending_enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        blending_enabled = true;
    }
    glDisable(GL_TEXTURE_2D);
#if  TEST_DEPTH
    glEnable(GL_DEPTH_TEST);
#endif
#if  DRAW_SMOOTH_LINES
    glEnable(GL_LINE_SMOOTH);
#else
    glDisable(GL_LINE_SMOOTH);
#endif
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    quick_set_color(r, g, b, a);

    glLineWidth(width);
}

void viswidget::quick_draw_circle(pfvec mid, pfvec e1, pfvec e2, uint num_lines)
{
   glBegin(GL_LINE_LOOP);
   for (uint i = 0; i < num_lines; i++) {
       pftype angle = i * M_2PI / num_lines;
       pfvec r = mid + cos(angle)*e1 + sin(angle)*e2;
#if    NUM_DIMENSIONS == 2
       glVertex3f(r.e[DIM_X], r.e[DIM_Y], 0);
#elif  NUM_DIMENSIONS == 3
       glVertex3f(r.e[DIM_X], r.e[DIM_Y], r.e[DIM_Z]);
#endif
   }
   glEnd();
}

void viswidget::quick_draw_triangle(pftype x1, pftype y1, pftype z1,
                                    pftype x2, pftype y2, pftype z2,
                                    pftype x3, pftype y3, pftype z3)
{
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glEnd();
}

void viswidget::quick_draw_triangle(pfvec p1, pfvec p2, pfvec p3)
{
#if    NUM_DIMENSIONS == 2
    quick_draw_triangle(p1[DIM_X], p1[DIM_Y], 0,
                        p2[DIM_X], p2[DIM_Y], 0,
                        p3[DIM_X], p3[DIM_Y], 0);
#elif  NUM_DIMENSIONS == 3
    quick_draw_triangle(p1[DIM_X], p1[DIM_Y], p1[DIM_Z],
                        p2[DIM_X], p2[DIM_Y], p2[DIM_Z],
                        p3[DIM_X], p3[DIM_Y], p3[DIM_Z]);
#endif
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


