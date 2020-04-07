#ifndef DIALOGCLASSSELECTION_H
#define DIALOGCLASSSELECTION_H

#include <QtWidgets/QtWidgets>

class LabelingWindow;

class CDialogClassSelection : public QDialog
{
  public:
    int iCurrentClass;
    LabelingWindow *pParentViewer;
    QListWidget *pListClasses;

  public:
	CDialogClassSelection(QWidget *parent=0, Qt::WindowFlags f = 0);
	~CDialogClassSelection();

  public slots:
    void OnClassSelect(QListWidgetItem *);
};

#endif
