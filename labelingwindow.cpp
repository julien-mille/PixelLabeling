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

#include <sstream>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "labelingwindow.h"

using namespace std;

/**********************************************************************
*                          LabelingFRame                             *
**********************************************************************/

void LabelingFRame::mousePressEvent(QMouseEvent *event)
{
    if (pParentViewer!=NULL)
        pParentViewer->handleMousePress(event->x(), event->y(), event->button());
}

void LabelingFRame::mouseReleaseEvent(QMouseEvent *event)
{
    if (pParentViewer!=NULL)
        pParentViewer->handleMouseRelease(event->x(), event->y(), event->button());
}

void LabelingFRame::mouseMoveEvent(QMouseEvent *event)
{
    if (pParentViewer!=NULL)
        pParentViewer->handleMouseMove(event->x(), event->y());
}

void LabelingFRame::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (pParentViewer!=NULL)
    {
        int w = pParentViewer->qimgOutput.width();
        int h = pParentViewer->qimgOutput.height();

        painter.drawImage(QRect(QPoint(0,0), QSize(w*pParentViewer->scaleFactor, h*pParentViewer->scaleFactor)),
            pParentViewer->qimgOutput, QRect(QPoint(0,0), QSize(w, h)));
        painter.drawEllipse(QPoint(pParentViewer->piMouseCurrent.x, pParentViewer->piMouseCurrent.y),
                            (int)(pParentViewer->penRadius*pParentViewer->scaleFactor),
                            (int)(pParentViewer->penRadius*pParentViewer->scaleFactor));
    }
}

/**********************************************************************
*                          LabelingWindow                             *
**********************************************************************/

LabelingWindow::LabelingWindow()
{
    frame = new LabelingFRame(this);
    frame->setBackgroundRole(QPalette::Base);
    frame->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    frame->setMouseTracking(true);
    frame->pParentViewer = this;

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(frame);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();

    resize(500, 400);

    imageOpen = false;
    bLeftButtonPressed = false;
    bRightButtonPressed = false;
    bShiftKeyPressed = false;
    penRadius = 10;
    opacity = 50;
    currentLabel = 1;
    scaleFactor = 1.0;

    pDlgClasses = new DialogClassSelection(this);
    pDlgClasses->setWindowTitle("Class");
    pDlgClasses->pParentViewer = this;
    pDlgClasses->setModal(false);
    pDlgClasses->show();

    pDlgClasses->pListClasses->setCurrentRow(currentLabel-1);
}

void LabelingWindow::makeQImage()
{
    int x, y;
    unsigned char *pPixelOutput;
    const unsigned char *pPixelInput;

    qimgInput = QImage(imgInput.cols, imgInput.rows, QImage::Format_ARGB32);

    pPixelOutput = qimgInput.bits();
    for (y=0; y<imgInput.rows; y++)
	{
		pPixelInput = imgInput.ptr(y);
		if (imgInput.type()==CV_8U)
		{
            for (x=0; x<imgInput.cols; x++)
            {
                pPixelOutput[0] = pPixelOutput[1] = pPixelOutput[2] = *pPixelInput;
                pPixelOutput[3] = 0xff;
                pPixelOutput += 4;
                pPixelInput++;
            }
		}
		else if (imgInput.type()==CV_8UC3)
        {
            for (x=0; x<imgInput.cols; x++)
            {
                pPixelOutput[0] = pPixelInput[0];
                pPixelOutput[1] = pPixelInput[1];
                pPixelOutput[2] = pPixelInput[2];
                pPixelOutput[3] = 0xff;
                pPixelOutput += 4;
                pPixelInput+=3;
            }
        }
	}
}

void LabelingWindow::drawLabels(int iSrcX, int iSrcY, int iSrcWidth, int iSrcHeight)
{
    const unsigned char *pPixelInitial;
    unsigned char *pPixelToDraw;
    const CLabelClassProperties *pProperties;
    cv::Point p;
    int i;

    if (qimgInput.width()!=qimgOutput.width() || qimgInput.height()!=qimgOutput.height()
        || qimgInput.format()!=qimgOutput.format())
        qimgOutput = QImage(qimgInput);

    int iSrcMaxX, iSrcMaxY;

    iSrcMaxX = min(labeling.cols-1, iSrcX+iSrcWidth-1);
    iSrcMaxY = min(labeling.rows-1, iSrcY+iSrcHeight-1);

    if (iSrcX<0)
        iSrcX = 0;
    if (iSrcY<0)
        iSrcY = 0;

    cv::Vec3i irgbClass, irgbInitial, irgbMix;

    for (p.y=iSrcY; p.y<=iSrcMaxY; p.y++)
    {
        pPixelInitial = qimgInput.bits() + p.y*qimgInput.bytesPerLine() + 4*iSrcX;
        pPixelToDraw = qimgOutput.bits() + p.y*qimgInput.bytesPerLine() + 4*iSrcX;

        for (p.x=iSrcX; p.x<=iSrcMaxX; p.x++)
        {
            unsigned int label = labeling.GetLabel(p);
            if (label!=0 && label<Labeling::vectClassesProperties.size())
            {
                pProperties = &(Labeling::vectClassesProperties[label]);
                irgbClass = (cv::Vec3i)pProperties->rgbClassColor;

                for (i=0; i<3; i++)
                    irgbInitial[i] = (int)(pPixelInitial[i]);
                irgbMix = ((int)opacity*irgbClass + (100-(int)opacity)*irgbInitial)/100;

                for (i=0; i<3; i++)
                    pPixelToDraw[i] = (unsigned char)(irgbMix[i]);

                pPixelToDraw[3] = 0xff;
            }
            else {
                for (i=0; i<4; i++)
                    pPixelToDraw[i] = pPixelInitial[i];
            }
            pPixelInitial+=4;
            pPixelToDraw+=4;
        }
    }
}

void LabelingWindow::handleMousePress(int x, int y, Qt::MouseButton button)
{
    if (button==Qt::LeftButton)
        bLeftButtonPressed = true;
    else if (button==Qt::RightButton)
        bRightButtonPressed = true;

    cv::Point piCenter;

    piCenter.x = (int)((float)x/scaleFactor);
    piCenter.y = (int)((float)y/scaleFactor);

    if (bLeftButtonPressed==true)
        labeling.SetLabelDisk(piCenter, penRadius, 0, currentLabel, bShiftKeyPressed);
    else if (bRightButtonPressed==true)
        labeling.SetLabelDisk(piCenter, penRadius, currentLabel, 0, bShiftKeyPressed);

    drawLabels(piCenter.x-penRadius, piCenter.y-penRadius, 2*penRadius+1, 2*penRadius+1);

    frame->update();
    updateTextStatusBar();
}

void LabelingWindow::handleMouseRelease(int x, int y, Qt::MouseButton button)
{
    if (button==Qt::LeftButton)
        bLeftButtonPressed = false;
    else if (button==Qt::RightButton)
        bRightButtonPressed = false;
}

void LabelingWindow::handleMouseMove(int x, int y)
{
    piMouseCurrent.x = x;
    piMouseCurrent.y = y;

    cv::Point piCenter;

    piCenter.x = (int)((float)x/scaleFactor);
    piCenter.y = (int)((float)y/scaleFactor);

    if (bLeftButtonPressed==true)
        labeling.SetLabelDisk(piCenter, penRadius, 0, currentLabel, bShiftKeyPressed);
    else if (bRightButtonPressed==true)
        labeling.SetLabelDisk(piCenter, penRadius, currentLabel, 0, bShiftKeyPressed);

    drawLabels(piCenter.x-penRadius, piCenter.y-penRadius, 2*penRadius+1, 2*penRadius+1);

    frame->update();
    updateTextStatusBar();
}

void LabelingWindow::updateImage()
{
    drawLabels(0, 0, imgInput.cols, imgInput.rows);
}

void LabelingWindow::updateTextStatusBar()
{
    stringstream streamStatus;

    streamStatus<<"Class = "<<currentLabel<<" \""<<Labeling::vectClassesProperties[currentLabel].className<<"\"";
    streamStatus<<"    Opacity = "<<opacity<<"\%";
    streamStatus<<"    Pen size = "<<penRadius;

    streamStatus<<"    Zoom = "<<(int)(scaleFactor*100.0f)<<"\%";

    cv::Point piMouseScaled;

    piMouseScaled = (cv::Point)((cv::Point2f)piMouseCurrent/scaleFactor);
    streamStatus<<"    (x,y)=("<<piMouseScaled.x<<","<<piMouseScaled.y<<")";

    cv::Rect rect(cv::Point(), labeling.size());

    if (rect.contains(piMouseScaled))
        streamStatus<<"    Current class = "<<labeling.GetLabel(piMouseScaled);

    statusBar()->showMessage(streamStatus.str().c_str());
}

void LabelingWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open image"), QDir::currentPath());
    if (!fileName.isEmpty())
    {
        strFilename = fileName.toLocal8Bit().constData();

        imgInput = cv::imread(strFilename.c_str(), cv::IMREAD_UNCHANGED);
        if (imgInput.data==nullptr)
        {
            cout<<"Failed to open "<<strFilename.c_str()<<endl;
            return;
        }

        if (imgInput.type()==CV_8UC4)
            cv::cvtColor(imgInput, imgInput, cv::COLOR_BGRA2BGR);

        imageOpen = true;
        labeling.Create(imgInput.size());
        scaleFactor = 1.0f;
        currentLabel = 1;
        makeQImage();
        updateImage();

        frame->resize((int)(imgInput.cols*scaleFactor), (int)(imgInput.rows*scaleFactor));
        frame->update();

        updateTextStatusBar();
        updateActions();
    }
}

void LabelingWindow::openLabel()
{
    QString labelFileName = QFileDialog::getOpenFileName(this, tr("Open label image"), QDir::currentPath());
    if (!labelFileName.isEmpty())
    {
        cv::Mat imgLabel;
        string strLabelFilename = labelFileName.toLocal8Bit().constData();

        imgLabel = cv::imread(strLabelFilename.c_str(), cv::IMREAD_ANYDEPTH);
        if (imgLabel.data!=nullptr)
        {
            labeling.InitFromLabelImage(imgLabel);

            updateImage();
            frame->update();
        }
        else cout<<"Cannot load image "<<strLabelFilename<<endl;
    }
}

void LabelingWindow::saveLabel()
{

    cv::Mat imgLabel, imgLabelRGB;
    string strLabelFilenamePrefix, strLabelFilename, strLabelFilenameRGB;

    labeling.MakeLabelImage(imgLabel);
    labeling.MakeLabelImageRGB(imgLabelRGB);

    size_t pos = strFilename.find_last_of('.');
    if (pos==string::npos)
        strLabelFilenamePrefix = strFilename;
    else
        strLabelFilenamePrefix = strFilename.substr(0, pos);

    strLabelFilename = strLabelFilenamePrefix + "_label.png";
    strLabelFilenameRGB = strLabelFilenamePrefix + "_label_RGB.png";

    if (cv::imwrite(strLabelFilename.c_str(), imgLabel)==true)
        cout<<"Saved label image "<<strLabelFilename<<endl;
    else
        cout<<"Cannot save label image "<<strLabelFilename<<endl;

    if (cv::imwrite(strLabelFilenameRGB.c_str(), imgLabelRGB)==true)
        cout<<"Saved RGB label image "<<strLabelFilenameRGB<<endl;
    else
        cout<<"Cannot save RGB label image "<<strLabelFilenameRGB<<endl;
}

void LabelingWindow::saveLabelAs()
{

    QString labelFileName = QFileDialog::getSaveFileName(this, tr("Save label image"), QDir::currentPath());
    if (!labelFileName.isEmpty())
    {
        cv::Mat imgLabel, imgLabelRGB;
        string strLabelFilenamePrefix, strLabelFilename, strLabelFilenameRGB;

        labeling.MakeLabelImage(imgLabel);
        labeling.MakeLabelImageRGB(imgLabelRGB);

        strLabelFilename = labelFileName.toLocal8Bit().constData();

        // Set extension to .png if not set
        if (strLabelFilename.substr(strLabelFilename.size()-4, 4)!=".png")
            strLabelFilename += ".png";

        size_t pos = strLabelFilename.find_last_of('.');
        if (pos==string::npos)
            strLabelFilenamePrefix = strLabelFilename;
        else
            strLabelFilenamePrefix = strLabelFilename.substr(0, pos);

        strLabelFilenameRGB = strLabelFilenamePrefix + "_RGB.png";

        if (cv::imwrite(strLabelFilename.c_str(), imgLabel)==true)
            cout<<"Saved label image "<<strLabelFilename<<endl;
        else
            cout<<"Cannot save label image "<<strLabelFilename<<endl;

        if (cv::imwrite(strLabelFilenameRGB.c_str(), imgLabelRGB)==true)
            cout<<"Saved RGB label image "<<strLabelFilenameRGB<<endl;
        else
            cout<<"Cannot save RGB label image "<<strLabelFilenameRGB<<endl;
    }
}

void LabelingWindow::zoomIn()
{
    scaleImage(2.0);
    updateTextStatusBar();
}

void LabelingWindow::zoomOut()
{
    if (scaleFactor>0.1)
    {
        scaleImage(0.5);
        updateTextStatusBar();
    }
}

void LabelingWindow::normalSize()
{
    frame->adjustSize();
    scaleFactor = 1.0;
    updateTextStatusBar();
}

void LabelingWindow::about()
{
    QMessageBox::about(this, tr("About PixelLabeling"),
    tr("<p>Copyright 2015-2020 Julien Mille</p>"));
}

void LabelingWindow::createActions()
{
    openImageAct = new QAction(tr("&Open image..."), this);
    openImageAct->setShortcut(tr("Ctrl+O"));
    connect(openImageAct, &QAction::triggered, this, &LabelingWindow::openImage);

    openLabelAct = new QAction(tr("&Open label image..."), this);
    openLabelAct->setShortcut(tr("Ctrl+L"));
    openLabelAct->setEnabled(false);
    connect(openLabelAct, &QAction::triggered, this, &LabelingWindow::openLabel);

    saveLabelAct = new QAction(tr("&Save label image"), this);
    saveLabelAct->setShortcut(tr("Ctrl+S"));
    saveLabelAct->setEnabled(false);
    connect(saveLabelAct, &QAction::triggered, this, &LabelingWindow::saveLabel);

    saveLabelAsAct = new QAction(tr("&Save label image as..."), this);
    saveLabelAsAct->setShortcut(tr("Shift+Ctrl+S"));
    saveLabelAsAct->setEnabled(false);
    connect(saveLabelAsAct, &QAction::triggered, this, &LabelingWindow::saveLabelAs);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, &QAction::triggered, this, &LabelingWindow::close);

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, &QAction::triggered, this, &LabelingWindow::zoomIn);

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, &QAction::triggered, this, &LabelingWindow::zoomOut);

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, &QAction::triggered, this, &LabelingWindow::normalSize);

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, &QAction::triggered, this, &LabelingWindow::about);

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void LabelingWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openImageAct);

    fileMenu->addAction(openLabelAct);
    fileMenu->addAction(saveLabelAct);
    fileMenu->addAction(saveLabelAsAct);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

void LabelingWindow::updateActions()
{
    openLabelAct->setEnabled(imageOpen);
    saveLabelAct->setEnabled(imageOpen);
    saveLabelAsAct->setEnabled(imageOpen);

    zoomInAct->setEnabled(imageOpen);
    zoomOutAct->setEnabled(imageOpen);
    normalSizeAct->setEnabled(imageOpen);
}

void LabelingWindow::scaleImage(float factor)
{
    int newWidth, newHeight;

    scaleFactor *= factor;
    updateImage();

    newWidth = (int)(imgInput.cols*scaleFactor);
    newHeight = (int)(imgInput.rows*scaleFactor);

    frame->resize(newWidth, newHeight);

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 20.0);
    zoomOutAct->setEnabled(scaleFactor > 0.1);
}

void LabelingWindow::adjustScrollBar(QScrollBar *scrollBar, float factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void LabelingWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Shift)
    {
        bShiftKeyPressed = true;
    }
    else if (event->key()==Qt::Key_F1)
    {
        if (opacity>=10)
        {
            opacity-=10;
            updateImage();
            frame->update();
            updateTextStatusBar();
        }
    }
    else if (event->key()==Qt::Key_F2)
    {
        if (opacity<=90)
        {
            opacity+=10;
            updateImage();
            frame->update();
            updateTextStatusBar();
        }
    }
    else if (event->key()==Qt::Key_F3)
    {
        if (penRadius>2)
        {
            penRadius--;
            frame->update();
            updateTextStatusBar();
        }
    }
    else if (event->key()==Qt::Key_F4)
    {
        if (penRadius<99)
        {
            penRadius++;
            frame->update();
            updateTextStatusBar();
        }
    }
    else if (event->key()==Qt::Key_A)
    {
        if (currentLabel>1)
        {
            currentLabel--;
            if (pDlgClasses!=NULL)
                pDlgClasses->pListClasses->setCurrentRow(currentLabel-1);
            updateTextStatusBar();
        }
    }
    else if (event->key()==Qt::Key_Z)
    {
        if (currentLabel<Labeling::vectClassesProperties.size()-1)
        {
            currentLabel++;
            if (pDlgClasses!=NULL)
                pDlgClasses->pListClasses->setCurrentRow(currentLabel-1);
            updateTextStatusBar();
        }
    }
    else if (event->key()==Qt::Key_Space)
    {
        qimgOutput.save(QString("./capture.png"));
        cout<<"Saved to ./capture.png"<<endl;
    }
    else if (event->key()==Qt::Key_Delete)
    {
        if (bShiftKeyPressed==true)
        {
            labeling.Empty();
            cout<<"All pixels unlabeled"<<endl;
        }
        else {
            labeling.EmptyClass(currentLabel);
            cout<<"All pixels with label "<<currentLabel<<" unlabeled"<<endl;
        }
        updateImage();
        frame->update();
        updateTextStatusBar();
    }
}

void LabelingWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Shift)
    {
        bShiftKeyPressed = false;
    }
}
