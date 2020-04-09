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

#include "labelingwindow.h"
#include "dialogclassselection.h"

DialogClassSelection::DialogClassSelection(QWidget* parent, Qt::WindowFlags flags): QDialog( parent, flags )
{
    QGridLayout* mainGrid = new QGridLayout;
    QVBoxLayout* topLayout = new QVBoxLayout;
    unsigned int iClass;

    pListClasses = new QListWidget();
    for (iClass=0; iClass<Labeling::vectClassesProperties.size()-1; iClass++)
        pListClasses->addItem(QString(Labeling::vectClassesProperties[iClass+1].className.c_str()));

    pListClasses->setSelectionMode(QAbstractItemView::SingleSelection);

    topLayout->addWidget(pListClasses);

    mainGrid->addLayout(topLayout,0,0);
    setLayout(mainGrid);

    pParentViewer = NULL;

    connect(pListClasses, &QListWidget::itemClicked, this, &DialogClassSelection::OnClassSelect);
}

DialogClassSelection::~DialogClassSelection()
{
}

void DialogClassSelection::OnClassSelect(QListWidgetItem *)
{
    if (pParentViewer!=NULL)
    {
        pParentViewer->currentLabel = pListClasses->currentRow()+1;
        pParentViewer->updateTextStatusBar();
    }
}
