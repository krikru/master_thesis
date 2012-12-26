#ifndef VISWIDGET_H
#define VISWIDGET_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <iostream>
#include <fstream>

// Qt includes
#include <QString>
#include <QGLWidget>
#include <QTimer>

// Own includes
#include "watersystem.h"
#include "base_float_vec3.h"

////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

typedef base_float_vec3<GLfloat> color3;

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

class viswidget : public QGLWidget
{
    Q_OBJECT

private:
    /* Private non-static member variables */
    GLdouble t;
    int gl_width;
    int gl_height;
    watersystem* system_to_visualize;
    uint scalar_property_to_visualize;

private:
    /* Private static member variables */
    std::ofstream tikz_file;
    bool drawing_tikz_image;
    GLfloat tikz_line_opacity;
    bool tikz_line_color_black;
    /* Colors */
    static GLfloat* NEIGHBOR_CONNECTION_R;
    static GLfloat* NEIGHBOR_CONNECTION_G;
    static GLfloat* NEIGHBOR_CONNECTION_B;
    static GLfloat* NEIGHBOR_CONNECTION_A;
    /* Scalar property names */

public:
    /* Constructor */
    explicit viswidget(QWidget *parent = 0);

    /* Public methods */
    void save_screen_as_tikz_picture(QString file_name);
    void set_system_to_visualize(watersystem* system);
    void set_scalar_property_to_visualize(uint property);
    uint get_scalar_property_to_visualize() const;

protected:
    /* Protected methods */
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    
signals:
    
public slots:

private:
    /* Complex functions */
    void visualize_fvoctree(const fvoctree *tree);
    void set_up_model_view_matrix(GLdouble scale_factor = 1);

    /* Simple drawing functions */
    void draw_pressure                  (const octcell* cell);
    void draw_pressure_deviation        (const octcell* cell);
    void draw_water_vol_coeff           (const octcell* cell);
    void draw_air_vol_coeff             (const octcell* cell);
    void draw_total_vol_coeff           (const octcell* cell);
    void draw_alpha                     (const octcell* cell);
    void draw_velocity_divergence       (const octcell* cell);
    void draw_flow_divergence           (const octcell* cell);
    void quick_draw_cell_face_velocities(const octcell* cell);
    void quick_draw_cell_center_velocity(const octcell* cell);
    void quick_mark_water_cell          (const octcell* cell);
    void quick_mark_air_cell            (const octcell* cell);
    void quick_draw_cell_water_level    (const octcell* cell);
    void quick_draw_cell                (const octcell* cell);

    /* Recursive drawing functions */
    void draw_pressure_recursively                        (const octcell* cell);
    void draw_pressure_deviations_recursively             (const octcell* cell);
    void draw_water_vol_coeff_recursively                 (const octcell* cell);
    void draw_air_vol_coeff_recursively                   (const octcell* cell);
    void draw_total_vol_coeff_recursively                 (const octcell* cell);
    void draw_alpha_recursively                           (const octcell* cell);
    void draw_velocity_divergence_recursively             (const octcell* cell);
    void draw_flow_divergence_recursively                 (const octcell* cell);
    void draw_cell_face_velocities_recursivelly           (const octcell* cell);
    void draw_cell_center_velocities_recursivelly         (const octcell* cell);
    void mark_water_cells_recursively                     (const octcell* cell);
    void mark_air_cells_recursively                       (const octcell* cell);
    void draw_water_level_recursively                     (const octcell* cell);
    void visualize_leaf_cells_recursively                 (const octcell* cell);
    void visualize_parent_cells_recursively               (const octcell* cell);
    void visualize_neighbor_connections_recursively       (const octcell* cell);
    void visualize_neighbor_connections_recursively       (const octcell* cell, uint neighbor_list_index);
    void visualize_finest_neighbor_connections_recursively(const octcell* cell);

    // Miscellaneous functions
    static void init_neighbor_connection_colors();

    // More or less drawing primitives
    void start_tikz_picture(QString file_name);
    void end_tikz_picture();
    void quick_set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void set_line_style(GLfloat width, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void quick_draw_circle(pfvec mid, pfvec e1, pfvec e2, uint num_lines);
    void quick_fill_cell_2d(const octcell *cell, color3 color, GLfloat alpha = 1);
    void quick_draw_triangle(pftype x1, pftype y1, pftype z1, pftype x2, pftype y2, pftype z2,pftype x3, pftype y3, pftype z3);
    void quick_draw_triangle(pfvec p1, pfvec p2, pfvec p3);
    void quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz);
    void quick_draw_line(pfvec p1, pfvec p2);
    void quick_draw_arrow(pfvec start, pfvec arrow);
    void draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a = 1);
};

#endif // VISWIDGET_H
