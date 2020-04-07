#ifndef DIALOGPARAMS_H
#define DIALOGPARAMS_H

#include <QtWidgets/QtWidgets>

#include <vector>
#include <string>

class CDialogParams : public QDialog
{
  protected:
    std::vector<QLineEdit *> vectTextEdits;
    std::vector<QCheckBox *> vectCheckBoxes;

  public:
    std::vector<float> vectFloatValues;
    std::vector<int> vectIntValues;
    std::vector<bool> vectBoolValues;

  public:
	CDialogParams(const std::vector<float> &, const std::vector<std::string> &,
                  const std::vector<int> &, const std::vector<std::string> &,
                  const std::vector<bool> &, const std::vector<std::string> &,
               QWidget *parent=0, Qt::WindowFlags f = 0);
	~CDialogParams();

  public slots:
	void sl_accept();
    void sl_cancel();

  // signals:
};

#endif
