#include <QApplication>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QToolButton>
#include <QPainter>
#include <QStyle>
#include "app.hpp"
#include "imageoverlay.hpp"

App::App(QWidget* parent) : QWidget(parent) {
	init();
}

void App::keyReleaseEvent(QKeyEvent* e) {
	static int i = 0;

	switch (e->key()) {
	case Qt::Key_Escape:
		QApplication::quit();
		break;
	case Qt::Key_Left:
		if (i++ % 2)
			loadFacade("0092.png");
		else
			loadFacade("0021.png");
		break;
	default:
		QWidget::keyReleaseEvent(e);
		break;
	}
}

void App::init() {
	setWindowTitle("grammarMatch");

	QHBoxLayout* topLayout = new QHBoxLayout;
	setLayout(topLayout);

	QWidget* ctrlWidget = new QWidget(this);
	ctrlWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	topLayout->addWidget(ctrlWidget);
	QVBoxLayout* ctrlLayout = new QVBoxLayout;
	ctrlWidget->setLayout(ctrlLayout);

	// Parameter widgets
	QGroupBox* paramsGroup = new QGroupBox("Params", this);
	ctrlLayout->addWidget(paramsGroup);
	QFormLayout* paramsLayout = new QFormLayout;
	paramsLayout->setLabelAlignment(Qt::AlignRight);
	paramsGroup->setLayout(paramsLayout);
	// Grammar display
	grammarLbl = new QLabel("0", this);
	paramsLayout->addRow("Grammar", grammarLbl);
	// Number of rows
	rowsSpin = new QSpinBox(this);
	rowsSpin->setRange(0, 100);
	rowsSpin->setValue(0);
	paramsLayout->addRow("Rows", rowsSpin);
	// Number of columns
	colsSpin = new QSpinBox(this);
	colsSpin->setRange(0, 100);
	colsSpin->setValue(0);
	paramsLayout->addRow("Cols", colsSpin);
	// Relative window width
	relWSpin = new QDoubleSpinBox(this);
	relWSpin->setRange(0.0, 1.0);
	relWSpin->setValue(0.5);
	relWSpin->setSingleStep(0.1);
	relWSpin->setDecimals(3);
	paramsLayout->addRow("Rel. width", relWSpin);
	// Relative window height
	relHSpin = new QDoubleSpinBox(this);
	relHSpin->setRange(0.0, 1.0);
	relHSpin->setValue(0.5);
	relHSpin->setSingleStep(0.1);
	relHSpin->setDecimals(3);
	paramsLayout->addRow("Rel. height", relHSpin);
	// Number of doors
	doorsSpin = new QSpinBox(this);
	doorsSpin->setRange(0, 100);
	doorsSpin->setValue(0);
	paramsLayout->addRow("Doors", doorsSpin);
	// Relative width of doors
	relDWSpin = new QDoubleSpinBox(this);
	relDWSpin->setRange(0.0, 1.0);
	relDWSpin->setValue(0.5);
	relDWSpin->setSingleStep(0.1);
	relDWSpin->setDecimals(3);
	paramsLayout->addRow("Rel. door width", relDWSpin);
	// Relative height of doors
	relDHSpin = new QDoubleSpinBox(this);
	relDHSpin->setRange(0.0, 1.0);
	relDHSpin->setValue(0.2);
	relDHSpin->setSingleStep(0.1);
	relDHSpin->setDecimals(3);
	paramsLayout->addRow("Rel. door height", relDHSpin);

	// Display parameters
	QGroupBox* dispGroup = new QGroupBox("Display", this);
	ctrlLayout->addWidget(dispGroup);
	QFormLayout* dispLayout = new QFormLayout;
	dispLayout->setLabelAlignment(Qt::AlignRight);
	dispGroup->setLayout(dispLayout);
	// Visible
	visibleCB = new QCheckBox("Visible", this);
	visibleCB->setChecked(true);
	dispLayout->addRow(visibleCB);
	// Brightness
	brightnessSlider = new QSlider(Qt::Horizontal, this);
	brightnessSlider->setRange(0, 255);
	brightnessSlider->setValue(128);
	dispLayout->addRow("Brightness", brightnessSlider);
	// x1
	x1Spin = new QSpinBox(this);
	x1Spin->setRange(0, 100);
	x1Spin->setValue(0);
	x1Spin->setSingleStep(5);
	dispLayout->addRow("X1", x1Spin);
	// y1
	y1Spin = new QSpinBox(this);
	y1Spin->setRange(0, 100);
	y1Spin->setValue(0);
	y1Spin->setSingleStep(5);
	dispLayout->addRow("Y1", y1Spin);
	// x2
	x2Spin = new QSpinBox(this);
	x2Spin->setRange(0, 100);
	x2Spin->setValue(100);
	x2Spin->setSingleStep(5);
	dispLayout->addRow("X2", x2Spin);
	// y2
	y2Spin = new QSpinBox(this);
	y2Spin->setRange(0, 100);
	y2Spin->setValue(100);
	y2Spin->setSingleStep(5);
	dispLayout->addRow("Y2", y2Spin);
	// Rotate
	rotateSlider = new QSlider(Qt::Horizontal, this);
	rotateSlider->setRange(-100, 100);
	rotateSlider->setValue(0);
	QToolButton* rotateReset = new QToolButton(this);
	rotateReset->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
	QHBoxLayout* rotateLayout = new QHBoxLayout;
	rotateLayout->addWidget(rotateSlider);
	rotateLayout->addWidget(rotateReset);
	dispLayout->addRow("Rotate", rotateLayout);
	// Shear
	shearSlider = new QSlider(Qt::Horizontal, this);
	shearSlider->setRange(-100, 100);
	shearSlider->setValue(0);
	QToolButton* shearReset = new QToolButton(this);
	shearReset->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
	QHBoxLayout* shearLayout = new QHBoxLayout;
	shearLayout->addWidget(shearSlider);
	shearLayout->addWidget(shearReset);
	dispLayout->addRow("Shear", shearLayout);

	ctrlLayout->addStretch();


	// Image and overlay view
	overlay = new ImageOverlay(this);
	topLayout->addWidget(overlay);

	// Connect signals and slots
	connect(rowsSpin, qOverload<int>(&QSpinBox::valueChanged), this, &App::updateGrammar);
	connect(colsSpin, qOverload<int>(&QSpinBox::valueChanged), this, &App::updateGrammar);
	connect(doorsSpin, qOverload<int>(&QSpinBox::valueChanged), this, &App::updateGrammar);

	// Update synth facade
	auto updateFacade = [=]() {
		Params p;
		p.rows = rowsSpin->value();
		p.cols = colsSpin->value();
		p.relWidth = relWSpin->value();
		p.relHeight = relHSpin->value();
		p.doors = doorsSpin->value();
		p.relDWidth = relDWSpin->value();
		p.relDHeight = relDHSpin->value();
		overlay->setParams(p);
	};
	connect(rowsSpin, qOverload<int>(&QSpinBox::valueChanged), updateFacade);
	connect(colsSpin, qOverload<int>(&QSpinBox::valueChanged), updateFacade);
	connect(relWSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), updateFacade);
	connect(relHSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), updateFacade);
	connect(doorsSpin, qOverload<int>(&QSpinBox::valueChanged), updateFacade);
	connect(relDWSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), updateFacade);
	connect(relDHSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), updateFacade);

	connect(visibleCB, &QCheckBox::toggled, overlay, &ImageOverlay::setOverlayVisible);
	connect(brightnessSlider, &QSlider::valueChanged, overlay, &ImageOverlay::setBrightness);

	// Update display rectangle
	auto updateDispRect = [=]() {
		int x1 = qMin(x1Spin->value(), x2Spin->value());
		int y1 = qMin(y1Spin->value(), y2Spin->value());
		int x2 = qMax(x1Spin->value(), x2Spin->value());
		int y2 = qMax(y1Spin->value(), y2Spin->value());
		QRect dispRect(QPoint(x1, y1), QPoint(x2 - 1, y2 - 1));
		overlay->setDispRect(dispRect);
	};
	connect(x1Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);
	connect(y1Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);
	connect(x2Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);
	connect(y2Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);

	connect(rotateSlider, &QSlider::valueChanged, [=](int value) {
		overlay->setRotation(value / 100.0 * 45.0);
	});
	connect(rotateReset, &QToolButton::clicked, [=]() { rotateSlider->setValue(0); });
	connect(shearSlider, &QSlider::valueChanged, [=](int value) {
		overlay->setShear(value / 100.0 * 0.5);
	});
	connect(shearReset, &QToolButton::clicked, [=]() { shearSlider->setValue(0); });
}

void App::loadFacade(QString imagename) {
	overlay->openImage(imagename);
	QRect rect = overlay->imageRect();

	// Reset grammar params
	rowsSpin->setValue(0);
	colsSpin->setValue(0);
	relWSpin->setValue(0.5);
	relHSpin->setValue(0.5);
	doorsSpin->setValue(0);
	relDWSpin->setValue(0.5);
	relDHSpin->setValue(0.2);

	// Update control settings
	int margin = qMax(rect.width(), rect.height()) * 0.25;
	x1Spin->setRange(-margin, rect.width() - 1 + margin);
	y1Spin->setRange(-margin, rect.height() - 1 + margin);
	x2Spin->setRange(-margin, rect.width() - 1 + margin);
	y2Spin->setRange(-margin, rect.height() - 1 + margin);
	x1Spin->setValue(0);
	y1Spin->setValue(0);
	x2Spin->setValue(rect.width() - 1);
	y2Spin->setValue(rect.height() - 1);

	rotateSlider->setValue(0);
	shearSlider->setValue(0);
}

// Update the grammar label with the current grammar
void App::updateGrammar() {
	int rows = rowsSpin->value();
	int cols = colsSpin->value();
	int doors = doorsSpin->value();

	if (rows < 1 || cols < 1)
		grammarLbl->setText("0");

	else if (rows == 1 && cols > 1 && doors < 1)
		grammarLbl->setText("3");

	else if (rows == 1 && cols > 1 && doors >= 1)
		grammarLbl->setText("4");

	else if (rows > 1 && cols == 1 && doors < 1)
		grammarLbl->setText("5");

	else if (rows > 1 && cols == 1 && doors >= 1)
		grammarLbl->setText("6");

	else if (doors < 1)
		grammarLbl->setText("1");

	else if (doors >= 1)
		grammarLbl->setText("2");
}
