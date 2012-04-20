#ifndef VISWIDGET_H
#define VISWIDGET_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Qt includes
#include <QGLWidget>
#include <QTimer>

// Own includes
#include "fvoctree.h"

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

    void quick_draw_cell(octcell* cell);
    void set_up_model_view_matrix(GLdouble scale_factor = 1);
    void visualize_leaf_cells_and_neighbor_connections_recursively(octcell* cell);
    void visualize_coarse_neighbor_connections_recursively(octcell* cell);
    void visualize_parent_cells_recursively(octcell* cell);
    void visualize_fvoctree(fvoctree *tree);

    /* test functions (remove when finished testing) */
    void move_fvoctree(fvoctree *tree);
    void move_octcell(octcell *c);
    
signals:
    
public slots:

public:

private:
    static inline void quick_set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    static inline void set_line_style(GLfloat width, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    static inline void quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz);
    static inline void quick_draw_line(pfvec p1, pfvec p2);
    static void draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLfloat  r, GLfloat g, GLfloat b, GLfloat a = 1);

private:
    /* Private member variables */
    QTimer frame_timer;
    int gl_width;
    int gl_height;
    GLdouble t; // Time
};

#endif // VISWIDGET_H
