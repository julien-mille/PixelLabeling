#ifndef LABELING_WINDOW_H
#define LABELING_WINDOW_H

#include <QtWidgets/QtWidgets>
#include "labeling.h"
#include "dialogclassselection.h"

class LabelingWindow;

class MyFrame : public QFrame
{
  public:
    MyFrame(QWidget *pParent=0):QFrame(pParent)
    {
        pParentViewer = NULL;
    }

    LabelingWindow *pParentViewer;

  protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
};

class LabelingWindow : public QMainWindow
{
    friend class CDialogClassSelection;
    friend class MyFrame;

  public:
    LabelingWindow();

    void handleMousePress(int, int, Qt::MouseButton);
    void handleMouseRelease(int, int, Qt::MouseButton);
    void handleMouseMove(int, int);
    void updateImage();
    void updateTextStatusBar();
    void makeQImage();
    void drawLabels(int iSrcX, int iSrcY, int iSrcWidth, int iSrcHeight);

  private:
    void openImage();
    void openLabel();
    void saveLabel();
    void saveLabelAs();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();

    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(float factor);
    void adjustScrollBar(QScrollBar *scrollBar, float factor);

    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void closeEvent(QCloseEvent *);

    MyFrame *frame;
    CDialogClassSelection *pDlgClasses;
    QScrollArea *scrollArea;
    float scaleFactor;

    std::string strFilename;

    cv::Mat imgInput;
    enum {NONE, IMAGE} iInputType;
    QImage qimgInput, qimgOutput;

    CLabeling labeling;
        // cv::Point piMouseLastPress; // Position at which the mouse was last pressed (coordinates of the QLabel)
    cv::Point piMouseCurrent;

    // enum {MOUSEMODE_INIT, MOUSEMODE_DRAW, MOUSEMODE_DISPLACE} iMouseMode;
    // enum {INITMODE_CIRCLE, INITMODE_ELLIPSE, INITMODE_LINE} iInitMode;

    bool bLeftButtonPressed;
    bool bRightButtonPressed;
    bool bShiftKeyPressed;
    int iPenRadius;
    int iOpacity;
    unsigned int currentLabel; // For multiple region

    QAction *openImageAct;
    QAction *openLabelAct;
    QAction *saveLabelAct;
    QAction *saveLabelAsAct;

    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
};

#endif
