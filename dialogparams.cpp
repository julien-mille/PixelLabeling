#include "dialogparams.h"

using namespace std;

CDialogParams::CDialogParams(const vector<float> &floatValues, const vector<string> &floatNames,
                             const vector<int> &intValues, const vector<string> &intNames,
                             const vector<bool> &boolValues, const vector<string> &boolNames,
                             QWidget* parent, Qt::WindowFlags flags): QDialog( parent, flags )
{
    QGridLayout* mainGrid = new QGridLayout;
    QVBoxLayout* topLayout = new QVBoxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QPushButton* btn;
    char strValue[50];
    unsigned int iValue;

    vectFloatValues = floatValues;
    vectIntValues = intValues;
    vectBoolValues = boolValues;

    for (iValue=0; iValue<vectFloatValues.size(); iValue++)
    {
        hboxLayout = new QHBoxLayout();
        label = new QLabel(floatNames[iValue].c_str());

        vectTextEdits.push_back(NULL);
        vectTextEdits.back() = new QLineEdit;

        sprintf(strValue, "%.03f", vectFloatValues[iValue]);
        vectTextEdits[iValue]->setText(strValue);

        hboxLayout->addWidget(label);
        hboxLayout->addWidget(vectTextEdits.back());
        topLayout->addLayout(hboxLayout);
    }

    for (iValue=0; iValue<vectIntValues.size(); iValue++)
    {
        hboxLayout = new QHBoxLayout();
        label = new QLabel(intNames[iValue].c_str());

        vectTextEdits.push_back(NULL);
        vectTextEdits.back() = new QLineEdit;

        sprintf(strValue, "%d", vectIntValues[iValue]);
        vectTextEdits.back()->setText(strValue);

        hboxLayout->addWidget(label);
        hboxLayout->addWidget(vectTextEdits.back());
        topLayout->addLayout(hboxLayout);
    }

    for (iValue=0; iValue<vectBoolValues.size(); iValue++)
    {
        vectCheckBoxes.push_back(NULL);
        vectCheckBoxes[iValue] = new QCheckBox(boolNames[iValue].c_str());
        vectCheckBoxes[iValue]->setCheckable(true);
        vectCheckBoxes[iValue]->setChecked(vectBoolValues[iValue]);
        topLayout->addWidget(vectCheckBoxes[iValue]);
    }

    btn = new QPushButton("OK");
    connect(btn, SIGNAL(clicked()), this, SLOT(sl_accept()));
    topLayout->addWidget(btn);

    btn = new QPushButton("Cancel");
    connect(btn, SIGNAL(clicked()), this, SLOT(sl_cancel()));

    topLayout->addWidget(btn);

    mainGrid->addLayout(topLayout,0,0);
    setLayout(mainGrid);
}

CDialogParams::~CDialogParams()
{
}

void CDialogParams::sl_accept()
{
    unsigned int iValue;
    QString stringValue;

    for (iValue=0; iValue<vectFloatValues.size(); iValue++)
    {
        stringValue = vectTextEdits[iValue]->text();
        vectFloatValues[iValue] = stringValue.toFloat();
    }

    for (iValue=0; iValue<vectIntValues.size(); iValue++)
    {
        stringValue = vectTextEdits[iValue + vectFloatValues.size()]->text();
        vectIntValues[iValue] = stringValue.toInt();
    }

    for (iValue=0; iValue<vectBoolValues.size(); iValue++)
        vectBoolValues[iValue] = vectCheckBoxes[iValue]->isChecked();

    QDialog::accept();
}

void CDialogParams::sl_cancel()
{
    QDialog::reject();
}
