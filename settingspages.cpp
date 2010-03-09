#include "settingspages.h"
#include <QtGui>

BrushPage::BrushPage(const QSettings &settings, QWidget *parent) : QWidget(parent)
{
    fillMap();

    QBrush brush;
    brush.setColor(settings.value("brush/color", brush.color()).value<QColor>());
    brush.setStyle(stylesMap.value(settings.value("brush/style", "No brush pattern").toString()));

    colorLabel = new QLabel(tr("Brush color:"));
    colorEdit = new QLineEdit;
    colorButton = new QPushButton(tr("Color..."));

    colorLabel->setBuddy(colorEdit);
    colorEdit->setDisabled(true);
    colorEdit->setMaximumWidth(30);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(colorClicked()));

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorEdit);
    colorLayout->addWidget(colorButton);
    colorLayout->addStretch();    

    color = brush.color();
    colorEdit->setPalette(QPalette(color));
    colorEdit->setAutoFillBackground(true);

    styleLabel = new QLabel(tr("Brush style:"));
    styleCombo = new QComboBox;

    styleLabel->setBuddy(styleCombo);
    styleCombo->addItems(stylesMap.keys());

    QHBoxLayout *styleLayout = new QHBoxLayout;
    styleLayout->addWidget(styleLabel);
    styleLayout->addWidget(styleCombo);
    styleLayout->addStretch();

    QString styleText = stylesMap.key(brush.style());
    for (int i = 0; i < styleCombo->count(); i++) {
        if (styleCombo->itemText(i) == styleText) {
            styleCombo->setCurrentIndex(i);
            break;
        }
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(colorLayout);
    mainLayout->addLayout(styleLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

void BrushPage::fillMap()
{
    stylesMap.insert(tr("No brush pattern"), Qt::NoBrush);
    stylesMap.insert(tr("Uniform color"), Qt::SolidPattern);
    stylesMap.insert(tr("Extremely dense brush pattern"), Qt::Dense1Pattern);
    stylesMap.insert(tr("Very dense brush pattern"), Qt::Dense2Pattern);
    stylesMap.insert(tr("Somewhat dense brush pattern"), Qt::Dense3Pattern);
    stylesMap.insert(tr("Half dense brush pattern"), Qt::Dense4Pattern);
    stylesMap.insert(tr("Somewhat sparse brush pattern"), Qt::Dense5Pattern);
    stylesMap.insert(tr("Very sparse brush pattern"), Qt::Dense6Pattern);
    stylesMap.insert(tr("Extremely sparse brush pattern"), Qt::Dense7Pattern);
    stylesMap.insert(tr("Horizontal lines"), Qt::HorPattern);
    stylesMap.insert(tr("Vertical lines"), Qt::VerPattern);
    stylesMap.insert(tr("Crossing horizontal and vertical lines"), Qt::CrossPattern);
    stylesMap.insert(tr("Backward diagonal lines"), Qt::BDiagPattern);
    stylesMap.insert(tr("Forward diagonal lines"), Qt::FDiagPattern);
    stylesMap.insert(tr("Crossing diagonal lines"), Qt::DiagCrossPattern);
}

void BrushPage::colorClicked()
{
    color = QColorDialog::getColor();
    if (color.isValid()) {
        colorEdit->setPalette(QPalette(color));
        colorEdit->setAutoFillBackground(true);
    }
}

PenPage::PenPage(const QSettings &settings, QWidget *parent) : QWidget(parent)
{
    fillMaps();

    QPen pen;
    pen.setColor(settings.value("pen/color", pen.color()).value<QColor>());
    pen.setCapStyle(capMap.value(settings.value("pen/capStyle", "Square cap").toString()));
    pen.setJoinStyle(joinMap.value(settings.value("pen/joinStyle", "Round join").toString()));
    pen.setMiterLimit(settings.value("pen/miterLimit", pen.miterLimit()).toDouble());
    pen.setStyle(penMap.value(settings.value("pen/style", "Plain line").toString()));
    pen.setWidth(settings.value("pen/width", pen.width()).toInt());

    colorLabel = new QLabel(tr("Brush color:"));
    colorEdit = new QLineEdit;
    colorButton = new QPushButton(tr("Color..."));

    colorLabel->setBuddy(colorEdit);
    colorEdit->setDisabled(true);
    colorEdit->setMaximumWidth(30);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(colorClicked()));

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorEdit);
    colorLayout->addWidget(colorButton);
    colorLayout->addStretch();    

    color = pen.color();
    colorEdit->setPalette(QPalette(color));
    colorEdit->setAutoFillBackground(true);

    capLabel = new QLabel(tr("Pen cap style:"));
    capCombo = new QComboBox;

    capLabel->setBuddy(capCombo);
    capCombo->addItems(capMap.keys());

    QHBoxLayout *capLayout = new QHBoxLayout;
    capLayout->addWidget(capLabel);
    capLayout->addWidget(capCombo);
    capLayout->addStretch();

    QString capText = capMap.key(pen.capStyle());
    for (int i = 0; i < capCombo->count(); i++) {
        if (capCombo->itemText(i) == capText) {
            capCombo->setCurrentIndex(i);
            break;
        }
    }

    joinLabel = new QLabel(tr("Pen join style:"));
    joinCombo = new QComboBox;

    joinLabel->setBuddy(joinCombo);
    joinCombo->addItems(joinMap.keys());

    QHBoxLayout *joinLayout = new QHBoxLayout;
    joinLayout->addWidget(joinLabel);
    joinLayout->addWidget(joinCombo);
    joinLayout->addStretch();

    QString joinText = joinMap.key(pen.joinStyle());
    for (int i = 0; i < joinCombo->count(); i++) {
        if (joinCombo->itemText(i) == joinText) {
            joinCombo->setCurrentIndex(i);
            break;
        }
    }

    limitLabel = new QLabel(tr("Miter limit:"));
    limitSpin = new QDoubleSpinBox;

    limitLabel->setBuddy(limitSpin);

    QHBoxLayout *limitLayout = new QHBoxLayout;
    limitLayout->addWidget(limitLabel);
    limitLayout->addWidget(limitSpin);
    limitLayout->addStretch();

    limitSpin->setValue(pen.miterLimit());

    penLabel = new QLabel(tr("Pen style:"));
    penCombo = new QComboBox;

    penLabel->setBuddy(penCombo);
    penCombo->addItems(penMap.keys());

    QHBoxLayout *penLayout = new QHBoxLayout;
    penLayout->addWidget(penLabel);
    penLayout->addWidget(penCombo);
    penLayout->addStretch();

    QString styleText = penMap.key(pen.style());
    for (int i = 0; i < penCombo->count(); i++) {
        if (penCombo->itemText(i) == styleText) {
            penCombo->setCurrentIndex(i);
            break;
        }
    }

    widthLabel = new QLabel(tr("Pen width:"));
    widthSpin = new QSpinBox;

    widthLabel->setBuddy(widthSpin);

    QHBoxLayout *widthLayout = new QHBoxLayout;
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(widthSpin);
    widthLayout->addStretch();

    widthSpin->setValue(pen.width());

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(colorLayout);
    mainLayout->addLayout(capLayout);
    mainLayout->addLayout(joinLayout);
    mainLayout->addLayout(limitLayout);
    mainLayout->addLayout(penLayout);
    mainLayout->addLayout(widthLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

void PenPage::fillMaps()
{
    capMap.insert(tr("Flat cap"), Qt::FlatCap);
    capMap.insert(tr("Square cap"), Qt::SquareCap);
    capMap.insert(tr("Round cap"), Qt::RoundCap);

    joinMap.insert(tr("Miter join"), Qt::MiterJoin);
    joinMap.insert(tr("Bevel join"), Qt::BevelJoin);
    joinMap.insert(tr("Round join"), Qt::RoundJoin);

    penMap.insert(tr("No line"), Qt::NoPen);
    penMap.insert(tr("Plain line"), Qt::SolidLine);
    penMap.insert(tr("Dashes"), Qt::DashLine);
    penMap.insert(tr("Dots"), Qt::DotLine);
    penMap.insert(tr("Alternate dots and dashes"), Qt::DashDotLine);
    penMap.insert(tr("One dash, two dots"), Qt::DashDotDotLine);
}

void PenPage::colorClicked()
{
    color = QColorDialog::getColor();
    if (color.isValid()) {
        colorEdit->setPalette(QPalette(color));
        colorEdit->setAutoFillBackground(true);
    }
}
