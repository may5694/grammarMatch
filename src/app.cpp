#include <QApplication>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPainter>
#include <QStyle>
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include <iomanip>
#include "app.hpp"
#include "imageoverlay.hpp"
#include "json.hpp"

using json = nlohmann::json;

App::App(QWidget* parent) : QWidget(parent) {
	init();
}

void App::keyReleaseEvent(QKeyEvent* e) {
	static int i = 0;

	switch (e->key()) {
	case Qt::Key_Escape:
		QApplication::quit();
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

	// Directory of images to look thru
	QLabel* dirLbl = new QLabel("Directory:", this);
	ctrlLayout->addWidget(dirLbl);
	QHBoxLayout* dirLayout = new QHBoxLayout;
	ctrlLayout->addLayout(dirLayout);
	dirLE = new QLineEdit(this);
	dirLayout->addWidget(dirLE);
	dirBtn = new QToolButton(this);
	dirBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
	dirLayout->addWidget(dirBtn);
	// Index label
	indexLbl = new QLabel("0 / 0", this);
	indexLbl->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	ctrlLayout->addWidget(indexLbl);
	// Prev and next images
	QHBoxLayout* prevNextLayout = new QHBoxLayout;
	ctrlLayout->addLayout(prevNextLayout);
	prevBtn = new QPushButton("<-  Prev", this);
	prevNextLayout->addWidget(prevBtn);
	nextBtn = new QPushButton("Next  ->", this);
	prevNextLayout->addWidget(nextBtn);
	// Skip invalid facades
	skipInvalidCB = new QCheckBox("Skip invalid", this);
	skipInvalidCB->setChecked(true);
	ctrlLayout->addWidget(skipInvalidCB);

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
	// Transparency
	transparencySlider = new QSlider(Qt::Horizontal, this);
	transparencySlider->setRange(0, 255);
	transparencySlider->setValue(128);
	dispLayout->addRow("Transparency", transparencySlider);
	// Brightness
	brightnessSlider = new QSlider(Qt::Horizontal, this);
	brightnessSlider->setRange(0, 255);
	brightnessSlider->setValue(128);
	dispLayout->addRow("Brightness", brightnessSlider);
	// x1
	x1Spin = new QSpinBox(this);
	x1Spin->setRange(0, 100);
	x1Spin->setValue(0);
	dispLayout->addRow("X1", x1Spin);
	// y1
	y1Spin = new QSpinBox(this);
	y1Spin->setRange(0, 100);
	y1Spin->setValue(0);
	dispLayout->addRow("Y1", y1Spin);
	// x2
	x2Spin = new QSpinBox(this);
	x2Spin->setRange(0, 100);
	x2Spin->setValue(0);
	dispLayout->addRow("X2", x2Spin);
	// y2
	y2Spin = new QSpinBox(this);
	y2Spin->setRange(0, 100);
	y2Spin->setValue(0);
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
	connect(dirLE, &QLineEdit::editingFinished, [=]() {
		// Do nothing if text was not modified
		if (!dirLE->isModified()) return;
		dirLE->setModified(false);

		// Attempt to read the directory
		readDir(dirLE->text());
	});
	connect(dirBtn, &QToolButton::clicked, [=]() {
		// Select an existing directory
		QString dirname = QFileDialog::getExistingDirectory(this,
			"Select directory", "",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		// Do nothing if user cancelled
		if (dirname.isNull()) return;

		// Set the lineEdit text
		dirLE->setText(dirname);

		// Attempt to read the directory
		readDir(dirname);
	});

	connect(prevBtn, &QPushButton::clicked, this, &App::prevFacade);
	connect(nextBtn, &QPushButton::clicked, this, &App::nextFacade);
	connect(skipInvalidCB, &QCheckBox::toggled, this, &App::updateIndexLabel);

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
	connect(transparencySlider, &QSlider::valueChanged, overlay, &ImageOverlay::setTransparency);
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

void App::clear() {
	topDir = fs::path();
	facadeInfo.clear();
	facadeIdx = 0;
	skipTotal = 0;

	updateIndexLabel();

	// Reset grammar params
	rowsSpin->setValue(0);
	colsSpin->setValue(0);
	relWSpin->setValue(0.5);
	relHSpin->setValue(0.5);
	doorsSpin->setValue(0);
	relDWSpin->setValue(0.5);
	relDHSpin->setValue(0.2);

	// Reset control settings
	x1Spin->setRange(0, 0);
	y1Spin->setRange(0, 0);
	x2Spin->setRange(0, 0);
	y2Spin->setRange(0, 0);
	x1Spin->setValue(0);
	y1Spin->setValue(0);
	x2Spin->setValue(0);
	y2Spin->setValue(0);

	rotateSlider->setValue(0);
	shearSlider->setValue(0);

	overlay->clear();
}

void App::readDir(QString path) {
	fs::path dirDir(path.toStdString());

	// Do nothing if the directory does not exist
	if (!fs::exists(dirDir)) return;
	// Do nothing if it's the same directory we're already using
	if (fs::equivalent(dirDir, topDir)) return;

	// Clear any current state
	clear();

	// Set the top-level directory to use and clear any existing facades
	topDir = dirDir;
	int skipIdx = 0;

	// Find all subdirectories named "metadata"
	fs::recursive_directory_iterator di(topDir), dend;
	for (; di != dend; ++di) {
		if (!fs::is_directory(di->path())) continue;
		if (di->path().filename().string() == "metadata") {
			di.disable_recursion_pending();

			fs::path metaDir = di->path();
			qDebug() << metaDir.string().c_str();

			// Iterate over all metadata files in this directory
			fs::directory_iterator ji(metaDir), jend;
			for (; ji != jend; ++ji) {
				// Skip if not a file or doesn't end in .json
				if (!fs::is_regular_file(ji->path())) continue;
				if (ji->path().extension().string() != ".json") continue;

				// Get path to corresponding image
				fs::path metaPath = ji->path();
				fs::path imagePath = metaDir.parent_path() / "image" / metaPath.filename();
				imagePath.replace_extension(".png");

				// Skip if image file doesn't exist
				if (!fs::exists(imagePath)) continue;

				// Get path to ground truth file
				fs::path truthPath = metaDir.parent_path() / "truth" / metaPath.filename();

				// Read whether facade has a valid grammar
				std::ifstream metaFile(metaPath);
				json metaJson;
				metaFile >> metaJson;
				bool valid = metaJson["valid"];

				// Skip if it's a roof
				if (metaJson["roof"]) continue;

				// Add facade to list
				FacadeInfo finfo;
				finfo.metaPath = metaPath;
				finfo.truthPath = truthPath;
				finfo.imagePath = imagePath;
				finfo.valid = valid;
				finfo.skipIdx = skipIdx;
				facadeInfo.push_back(finfo);

				if (valid) {
					skipIdx++;
					skipTotal++;
				}
			}
		}
	}

	if (facadeInfo.empty()) return;

	// Load the first facade
	facadeIdx = facadeInfo.size() - 1;
	nextFacade();
}

void App::nextFacade() {
	if (facadeInfo.empty()) return;

	// Get the next facade that is valid (if skipping invalid ones)
	facadeIdx = (facadeIdx + 1) % facadeInfo.size();
	while (skipInvalidCB->isChecked() && !facadeInfo[facadeIdx].valid)
		facadeIdx = (facadeIdx + 1) % facadeInfo.size();

	updateIndexLabel();

	loadFacade();
}

void App::prevFacade() {
	if (facadeInfo.empty()) return;

	// Get the previous facade that is valid (if skipping invalid ones)
	facadeIdx = (facadeIdx - 1 + facadeInfo.size()) % facadeInfo.size();
	while (skipInvalidCB->isChecked() && !facadeInfo[facadeIdx].valid)
		facadeIdx = (facadeIdx - 1 + facadeInfo.size()) % facadeInfo.size();

	updateIndexLabel();

	loadFacade();
}

void App::loadFacade() {
	QString imagename = QString::fromStdString(facadeInfo[facadeIdx].imagePath.string());

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

// Update the index display label
void App::updateIndexLabel() {
	if (facadeInfo.empty()) {
		indexLbl->setText("0 / 0");
		return;
	}

	if (skipInvalidCB->isChecked())
		indexLbl->setText(QString::number(facadeInfo[facadeIdx].skipIdx) + " / "
			+ QString::number(skipTotal));
	else
		indexLbl->setText(QString::number(facadeIdx) + " / "
			+ QString::number(facadeInfo.size()));
}
