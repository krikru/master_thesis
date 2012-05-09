#ifndef VISWIDGET_H
#define VISWIDGET_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Qt includes
#include <QGLWidget>
#include <QTimer>

// Own includes
#include "watersystem.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

class viswidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit viswidget(QWidget *parent = 0);

public:
    /* Public methods */
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void set_system_to_visualize(watersystem* system);
    
signals:
    
public slots:

public:

private:
    void visualize_fvoctree(fvoctree *tree);

    /* Simple functions */
    void draw_pressure_deviation(octcell* cell);
    void draw_alpha(octcell* cell);
    void quick_draw_cell_face_velocities(octcell* cell);
    void quick_draw_cell_center_velocities(octcell* cell);
    void quick_mark_water_cell(octcell* cell);
    void quick_air_empty_cell(octcell* cell);
    void quick_draw_cell_water_level(octcell* cell);
    void quick_draw_cell(octcell* cell);
    void set_up_model_view_matrix(GLdouble scale_factor = 1);

    /* Recursive functions */
    void draw_pressure_deviations_recursively(octcell* cell);
    void draw_alpha_recursively(octcell* cell);
    void draw_cell_face_velocities_recursivelly(octcell* cell);
    void draw_cell_center_velocities_recursivelly(octcell* cell);
    void mark_water_cells_recursively(octcell* cell);
    void mark_air_cells_recursively(octcell* cell);
    void draw_water_level_recursively(octcell* cell);
    void visualize_leaf_cells_recursively(octcell* cell);
    void visualize_parent_cells_recursively(octcell* cell);
    void visualize_neighbor_connections_recursively(octcell* cell);
    void visualize_neighbor_connections_recursively(octcell* cell, uint neighbor_list_index);
    void visualize_finest_neighbor_connections_recursively(octcell* cell);

    static void init_neighbor_connection_colors();
    static void quick_set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    static void set_line_style(GLfloat width, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    static void quick_draw_circle(pfvec mid, pfvec e1, pfvec e2, uint num_lines);
    static void quick_draw_triangle(pftype x1, pftype y1, pftype z1, pftype x2, pftype y2, pftype z2,pftype x3, pftype y3, pftype z3);
    static void quick_draw_triangle(pfvec p1, pfvec p2, pfvec p3);
    static void quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz);
    static void quick_draw_line(pfvec p1, pfvec p2);
    static void quick_draw_arrow(pfvec start, pfvec arrow);
    static void draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a = 1);

    /* test functions (remove when finished testing) */
    void move_fvoctree(fvoctree *tree);
    void move_octcell(octcell *c);

private:
    /* Private member variables */
    GLdouble t;
    int gl_width;
    int gl_height;
    watersystem* system_to_visualize;

    /* Colors */
    static GLfloat* NEIGHBOR_CONNECTION_R;
    static GLfloat* NEIGHBOR_CONNECTION_G;
    static GLfloat* NEIGHBOR_CONNECTION_B;
    static GLfloat* NEIGHBOR_CONNECTION_A;
};

#endif // VISWIDGET_H
