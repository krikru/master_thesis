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
// ENUMS
////////////////////////////////////////////////////////////////

enum SCALAR_PROPERTY {
    SP_NO_SCALAR_PROPERTY,
    SP_ALPHA,
    SP_WATER_VOLUME_COEFFICIENT,
    SP_AIR_VOLUME_COEFFICIENT,
    SP_TOTAL_VOLUME_COEFFICIENT,
    SP_PRESSURE,
    SP_PRESSURE_DEVIATION,
    SP_VELOCITY_DIVERGENCE,
    SP_FLOW_DIVERGENCE,
    NUM_SCALAR_PROPERTIES
};

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
    void set_scalar_property_to_visualize(uint property);

private:
    void visualize_fvoctree(const fvoctree *tree);
    void set_up_model_view_matrix(GLdouble scale_factor = 1);

    /* Simple functions */
    void draw_pressure                    (const octcell* cell);
    void draw_pressure_deviation          (const octcell* cell);
    void draw_water_vol_coeff             (const octcell *cell);
    void draw_air_vol_coeff               (const octcell* cell);
    void draw_total_vol_coeff             (const octcell* cell);
    void draw_alpha                       (const octcell *cell);
    void draw_velocity_divergence         (const octcell *cell);
    void draw_flow_divergence             (const octcell *cell);
    void quick_draw_cell_face_velocities  (const octcell* cell);
    void quick_draw_cell_center_velocity(const octcell *cell);
    void quick_mark_water_cell            (const octcell* cell);
    void quick_mark_air_cell             (const octcell *cell);
    void quick_draw_cell_water_level      (const octcell* cell);
    void quick_draw_cell                  (const octcell *cell);

    /* Recursive functions */
    void draw_pressure_recursively                        (const octcell* cell);
    void draw_pressure_deviations_recursively             (const octcell* cell);
    void draw_water_vol_coeff_recursively                 (const octcell *cell);
    void draw_air_vol_coeff_recursively                   (const octcell *cell);
    void draw_total_vol_coeff_recursively                 (const octcell *cell);
    void draw_alpha_recursively                           (const octcell *cell);
    void draw_velocity_divergence_recursively             (const octcell *cell);
    void draw_flow_divergence_recursively                 (const octcell *cell);
    void draw_cell_face_velocities_recursivelly           (const octcell* cell);
    void draw_cell_center_velocities_recursivelly         (const octcell *cell);
    void mark_water_cells_recursively                     (const octcell* cell);
    void mark_air_cells_recursively                       (const octcell* cell);
    void draw_water_level_recursively                     (const octcell* cell);
    void visualize_leaf_cells_recursively                 (const octcell *cell);
    void visualize_parent_cells_recursively               (const octcell* cell);
    void visualize_neighbor_connections_recursively       (const octcell *cell);
    void visualize_neighbor_connections_recursively       (const octcell* cell, uint neighbor_list_index);
    void visualize_finest_neighbor_connections_recursively(const octcell *cell);

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

private:
    /* Private non-static member variables */
    GLdouble t;
    int gl_width;
    int gl_height;
    watersystem* system_to_visualize;
    uint scalar_property_to_visualize;

private:
    /* Private static member variables */
    /* Colors */
    static GLfloat* NEIGHBOR_CONNECTION_R;
    static GLfloat* NEIGHBOR_CONNECTION_G;
    static GLfloat* NEIGHBOR_CONNECTION_B;
    static GLfloat* NEIGHBOR_CONNECTION_A;
};

#endif // VISWIDGET_H
