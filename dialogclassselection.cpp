#include "labelingwindow.h"
#include "dialogclassselection.h"

CDialogClassSelection::CDialogClassSelection(QWidget* parent, Qt::WindowFlags flags): QDialog( parent, flags )
{
    QGridLayout* mainGrid = new QGridLayout;
    QVBoxLayout* topLayout = new QVBoxLayout;
    unsigned int iClass;

    pListClasses = new QListWidget();
    for (iClass=0; iClass<CLabeling::vectClassesProperties.size()-1; iClass++)
        pListClasses->addItem(QString(CLabeling::vectClassesProperties[iClass+1].className.c_str()));

    pListClasses->setSelectionMode(QAbstractItemView::SingleSelection);

    topLayout->addWidget(pListClasses);

    mainGrid->addLayout(topLayout,0,0);
    setLayout(mainGrid);

    pParentViewer = NULL;

    connect(pListClasses, &QListWidget::itemClicked, this, &CDialogClassSelection::OnClassSelect);
}

CDialogClassSelection::~CDialogClassSelection()
{
}

void CDialogClassSelection::OnClassSelect(QListWidgetItem *)
{
    if (pParentViewer!=NULL)
    {
        pParentViewer->currentLabel = pListClasses->currentRow()+1;
        pParentViewer->updateTextStatusBar();
    }
}
