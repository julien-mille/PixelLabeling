/*
Copyright 2015-2020 Julien Mille

This file is part of the PixelLabeling source code package.

PixelLabeling is free software: you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

PixelLabeling is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU General Public License,
and a copy of the GNU Lesser General Public License, along with
PixelLabeling. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LABELING_WINDOW_H
#define LABELING_WINDOW_H

#include <QtWidgets/QtWidgets>
#include "labeling.h"
#include "dialogclassselection.h"

class LabelingWindow;

class LabelingFRame : public QFrame
{
  public:
    LabelingFRame(QWidget *pParent=0):QFrame(pParent)
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

/* The LabelingWindow is inspired from the ImageViewer Qt example */
class LabelingWindow : public QMainWindow
{
    friend class DialogClassSelection;
    friend class LabelingFRame;

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
    void about();

    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(float factor);
    void adjustScrollBar(QScrollBar *scrollBar, float factor);

    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);

    LabelingFRame *frame;
    DialogClassSelection *pDlgClasses;
    QScrollArea *scrollArea;
    float scaleFactor;

    std::string strFilename;

    cv::Mat imgInput;
    QImage qimgInput, qimgOutput;

    Labeling labeling;
    cv::Point piMouseCurrent;

    bool imageOpen;
    bool bLeftButtonPressed;
    bool bRightButtonPressed;
    bool bShiftKeyPressed;

    unsigned int penRadius;
    unsigned int opacity;
    unsigned int currentLabel;

    QAction *openImageAct;
    QAction *openLabelAct;
    QAction *saveLabelAct;
    QAction *saveLabelAsAct;

    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
};

#endif
