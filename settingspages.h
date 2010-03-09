#ifndef SETTINGSPAGES_H
#define SETTINGSPAGES_H

#include <QWidget>
#include <QMap>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QSettings;

class BrushPage : public QWidget
{
    Q_OBJECT

public:
    BrushPage(const QSettings &settings, QWidget *parent = 0);

    QColor color;
    QComboBox *styleCombo;
    QMap<QString, Qt::BrushStyle> stylesMap;

private slots:
    void colorClicked();

private:
    QLabel *colorLabel;
    QLineEdit *colorEdit;
    QPushButton *colorButton;
    QLabel *styleLabel;

    void fillMap();
};

class PenPage : public QWidget
{
    Q_OBJECT

public:
    PenPage(const QSettings &settings, QWidget *parent = 0);

    QColor color;
    QComboBox *capCombo;
    QMap<QString, Qt::PenCapStyle> capMap;
    QComboBox *joinCombo;
    QMap<QString, Qt::PenJoinStyle> joinMap;
    QDoubleSpinBox *limitSpin;
    QComboBox *penCombo;
    QMap<QString, Qt::PenStyle> penMap;
    QSpinBox *widthSpin;

private slots:
    void colorClicked();

private:
    QLineEdit *colorEdit;
    QLabel *colorLabel;
    QPushButton *colorButton;
    QLabel *capLabel;
    QLabel *joinLabel;
    QLabel *limitLabel;
    QLabel *penLabel;
    QLabel *widthLabel;

    void fillMaps();
};

#endif // SETTINGSPAGES_H
