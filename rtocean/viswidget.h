#ifndef VISWIDGET_H
#define VISWIDGET_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Qt includes
#include <QGLWidget> //<QtOpenGL/QGLWidget>

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

    void visualize_octcell(octcell *cell, bool recursively);
    void visualize_fvoctree(fvoctree *tree);
    
signals:
    
public slots:

public:


private:
    static inline void set_line_style(GLfloat width, GLubyte  r, GLubyte g, GLubyte b, GLubyte a);
    static inline void quick_draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz);
    static void draw_line(GLfloat ax, GLfloat ay, GLfloat az, GLfloat bx, GLfloat by, GLfloat bz, GLfloat width, GLubyte  r, GLubyte g, GLubyte b, GLubyte a = 255);
};

#endif // VISWIDGET_H
