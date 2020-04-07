/*
	labeling.cpp

	Copyright 2018 Julien Mille (julien.mille@insa-cvl.fr)

*/

#include <QtCore/QtCore>
#include "labelingwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	if (CLabeling::InitClassesProperties()==false)
        return -1;

    LabelingWindow LabelingWindow;
    LabelingWindow.setWindowTitle("Pixel labeling");
    LabelingWindow.show();

    return app.exec();
}
