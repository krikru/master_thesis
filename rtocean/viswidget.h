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

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void visualize_octcell_recursively(octcell *cell, bool recursively);
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
};

#endif // VISWIDGET_H
