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

#ifndef DIALOGCLASSSELECTION_H
#define DIALOGCLASSSELECTION_H

#include <QtWidgets/QtWidgets>

class LabelingWindow;

class DialogClassSelection : public QDialog
{
  public:
    int iCurrentClass;
    LabelingWindow *pParentViewer;
    QListWidget *pListClasses;

  public:
	DialogClassSelection(QWidget *parent=0, Qt::WindowFlags f = 0);
	~DialogClassSelection();

  public slots:
    void OnClassSelect(QListWidgetItem *);
};

#endif
