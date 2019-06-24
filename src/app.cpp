#include <QApplication>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
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

App::App(QString startDir, QWidget* parent) : QWidget(parent) {
	init();
	if (!startDir.isNull()) {
		dirLE->setText(startDir);
		readDir(startDir);
	}
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
	// Path to facade
	pathLbl = new QLabel(this);
	ctrlLayout->addWidget(pathLbl);
	// Prev and next images
	QGridLayout* prevNextLayout = new QGridLayout;
	ctrlLayout->addLayout(prevNextLayout);
	prevBtn = new QPushButton("<-  Prev", this);
	prevNextLayout->addWidget(prevBtn, 0, 0);
	nextBtn = new QPushButton("Next  ->", this);
	prevNextLayout->addWidget(nextBtn, 0, 1);
	// Skip invalid facades
	skipInvalidCB = new QCheckBox("Skip invalid", this);
	skipInvalidCB->setChecked(true);
	prevNextLayout->addWidget(skipInvalidCB, 1, 0);
	// Save facade grammar
	saveBtn = new QPushButton("Save", this);
	prevNextLayout->addWidget(saveBtn, 1, 1);

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
	rotateSpin = new QDoubleSpinBox(this);
	rotateSpin->setRange(-90.0, 90.0);
	rotateSpin->setValue(0.0);
	rotateSpin->setSingleStep(0.5);
	dispLayout->addRow("Rotate", rotateSpin);
	// Shear
	shearSpin = new QDoubleSpinBox(this);
	shearSpin->setRange(-1.5, 1.5);
	shearSpin->setValue(0.0);
	shearSpin->setSingleStep(0.01);
	shearSpin->setDecimals(3);
	dispLayout->addRow("Shear", shearSpin);

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
	connect(saveBtn, &QPushButton::clicked, this, &App::saveFacade);

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
		QRect dispRect(QPoint(x1, y1), QPoint(x2, y2));
		overlay->setDispRect(dispRect);
	};
	connect(x1Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);
	connect(y1Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);
	connect(x2Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);
	connect(y2Spin, qOverload<int>(&QSpinBox::valueChanged), updateDispRect);

	connect(rotateSpin, qOverload<double>(&QDoubleSpinBox::valueChanged),
		overlay, &ImageOverlay::setRotation);
	connect(shearSpin, qOverload<double>(&QDoubleSpinBox::valueChanged),
		overlay, &ImageOverlay::setShear);
}

void App::clear() {
	topDir = fs::path();
	facadeInfo.clear();
	facadeIdx = 0;
	skipTotal = 0;
	pathLbl->setText("");

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

	rotateSpin->setValue(0.0);
	shearSpin->setValue(0.0);

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

				// Skip if size is null
				if (metaJson["size"][0].is_null() || metaJson["size"][1].is_null()) continue;

				// Add facade to list
				FacadeInfo finfo;
				finfo.metaPath = metaPath;
				finfo.truthPath = truthPath;
				finfo.imagePath = imagePath;
				finfo.valid = valid;
				finfo.skipIdx = skipIdx;
				finfo.size_x = metaJson["size"][0];
				finfo.size_y = metaJson["size"][1];
				finfo.crop_px = metaJson["crop"][2];
				finfo.crop_py = metaJson["crop"][3];
				if (valid) {
					finfo.chip_size_x = metaJson["chip_size"][0];
					finfo.chip_size_y = metaJson["chip_size"][1];
				} else {
					finfo.chip_size_x = finfo.size_x;
					finfo.chip_size_y = finfo.size_y;
				}
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
	facadeIdx = 0;
	while (skipInvalidCB->isChecked() && !facadeInfo[facadeIdx].valid)
		facadeIdx = (facadeIdx + 1) % facadeInfo.size();
	updateIndexLabel();
	loadFacade();
}

void App::nextFacade() {
	if (facadeInfo.empty()) return;

	saveFacade();

	// Get the next facade that is valid (if skipping invalid ones)
	facadeIdx = (facadeIdx + 1) % facadeInfo.size();
	while (skipInvalidCB->isChecked() && !facadeInfo[facadeIdx].valid)
		facadeIdx = (facadeIdx + 1) % facadeInfo.size();

	updateIndexLabel();

	loadFacade();
}

void App::prevFacade() {
	if (facadeInfo.empty()) return;

	saveFacade();

	// Get the previous facade that is valid (if skipping invalid ones)
	facadeIdx = (facadeIdx - 1 + facadeInfo.size()) % facadeInfo.size();
	while (skipInvalidCB->isChecked() && !facadeInfo[facadeIdx].valid)
		facadeIdx = (facadeIdx - 1 + facadeInfo.size()) % facadeInfo.size();

	updateIndexLabel();

	loadFacade();
}

// Write ground truth parameters
void App::saveFacade() {
	if (facadeInfo.empty()) return;

	json truthJson;
	// Write display grammar params
	int grammar = grammarLbl->text().toInt();
	truthJson["dispParams"]["grammar"] = grammar;
	truthJson["dispParams"]["rows"] = rowsSpin->value();
	truthJson["dispParams"]["cols"] = colsSpin->value();
	truthJson["dispParams"]["relWidth"] = relWSpin->value();
	truthJson["dispParams"]["relHeight"] = relHSpin->value();
	truthJson["dispParams"]["doors"] = doorsSpin->value();
	truthJson["dispParams"]["relDWidth"] = relDWSpin->value();
	truthJson["dispParams"]["relDHeight"] = relDHSpin->value();

	// Write display variables
	truthJson["disp"]["x1"] = x1Spin->value();
	truthJson["disp"]["y1"] = y1Spin->value();
	truthJson["disp"]["x2"] = x2Spin->value();
	truthJson["disp"]["y2"] = y2Spin->value();
	truthJson["disp"]["rotate"] = rotateSpin->value();
	truthJson["disp"]["shear"] = shearSpin->value();

	// Write params relative to chip-size
	double rect_width_px = qAbs(x2Spin->value() - x1Spin->value()) + 1;
	double rect_height_px = qAbs(y2Spin->value() - y1Spin->value()) + 1;
	double image_width_px = facadeInfo[facadeIdx].crop_px;
	double image_height_px = facadeInfo[facadeIdx].crop_py;
	double image_width_m = facadeInfo[facadeIdx].size_x;
	double image_height_m = facadeInfo[facadeIdx].size_y;
	double chip_width_m = facadeInfo[facadeIdx].chip_size_x;
	double chip_height_m = facadeInfo[facadeIdx].chip_size_y;
	truthJson["params"]["valid"] = (grammar != 0);
	truthJson["params"]["grammar"] = grammar;

	// Write zeros for all params if invalid grammar
	if (grammar == 0) {
		truthJson["params"]["rows"] = 0;
		truthJson["params"]["cols"] = 0;
		truthJson["params"]["relativeWidth"] = 0.0;
		truthJson["params"]["relativeHeight"] = 0.0;
		truthJson["params"]["doors"] = 0;
		truthJson["params"]["relativeDWidth"] = 0.0;
		truthJson["params"]["relativeDHeight"] = 0.0;
	} else {
		// If grid grammar, write rows cols relative to metadata chip size
		if (grammar == 1 || grammar == 2) {
			truthJson["params"]["rows"] = rowsSpin->value() / rect_height_px
				* image_height_px / image_height_m * chip_height_m;
			truthJson["params"]["cols"] = colsSpin->value() / rect_width_px
				* image_width_px / image_width_m * chip_width_m;
		// If vertical windows, write 1 row
		} else if (grammar == 3 || grammar == 4) {
			truthJson["params"]["rows"] = 1;
			truthJson["params"]["cols"] = colsSpin->value() / rect_width_px
				* image_width_px / image_width_m * chip_width_m;
		// If horizontal windows, write 1 column
		} else if (grammar == 5 || grammar == 6) {
			truthJson["params"]["rows"] = rowsSpin->value() / rect_height_px
				* image_height_px / image_height_m * chip_height_m;
			truthJson["params"]["cols"] = 1;
		}
		truthJson["params"]["relativeWidth"] = relWSpin->value();
		truthJson["params"]["relativeHeight"] = relHSpin->value();

		// If there are doors, write door params (relative to chip size)
		if (grammar == 2 || grammar == 4 || grammar == 6) {
			truthJson["params"]["doors"] = doorsSpin->value() / rect_width_px
				* image_width_px / image_width_m * chip_width_m;
			truthJson["params"]["relativeDWidth"] = relDWSpin->value();
			truthJson["params"]["relativeDHeight"] = relDHSpin->value() * rect_height_px
				* image_height_m / image_height_px / chip_height_m;
		// No doors, write zeros for door params
		} else {
			truthJson["params"]["doors"] = 0;
			truthJson["params"]["relativeDWidth"] = 0.0;
			truthJson["params"]["relativeDHeight"] = 0.0;
		}
	}

	// Create ground truth directory if it doesn't exist
	fs::path truthPath = facadeInfo[facadeIdx].truthPath;
	fs::path truthDir = truthPath.parent_path();
	if (!fs::exists(truthDir))
		fs::create_directory(truthDir);

	// Write output json
	std::ofstream truthFile(truthPath);
	truthFile << std::setw(4) << truthJson;
}

// Load facade image and ground truth params if they exist
void App::loadFacade() {
	QString imagename = QString::fromStdString(facadeInfo[facadeIdx].imagePath.string());

	overlay->openImage(imagename);
	QRect rect = overlay->imageRect();

	// Get path relative to the top path
	fs::path metaPath = facadeInfo[facadeIdx].metaPath;
	fs::path relPath;
	auto metaIt = metaPath.begin();
	for (auto topIt = topDir.begin(); topIt != topDir.end(); ++metaIt, ++topIt);
	for (; metaIt != metaPath.end(); ++metaIt)
		relPath /= *metaIt;
	pathLbl->setText(QString::fromStdString(relPath.string()));

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

	rotateSpin->setValue(0.0);
	shearSpin->setValue(0.0);

	// Load ground truth data if it exists
	fs::path truthPath = facadeInfo[facadeIdx].truthPath;
	if (!fs::exists(truthPath)) return;

	std::ifstream truthFile(truthPath);
	json truthJson;
	truthFile >> truthJson;

	rowsSpin->setValue(truthJson["dispParams"]["rows"]);
	colsSpin->setValue(truthJson["dispParams"]["cols"]);
	relWSpin->setValue(truthJson["dispParams"]["relWidth"]);
	relHSpin->setValue(truthJson["dispParams"]["relHeight"]);
	doorsSpin->setValue(truthJson["dispParams"]["doors"]);
	relDWSpin->setValue(truthJson["dispParams"]["relDWidth"]);
	relDHSpin->setValue(truthJson["dispParams"]["relDHeight"]);

	x1Spin->setValue(truthJson["disp"]["x1"]);
	y1Spin->setValue(truthJson["disp"]["y1"]);
	x2Spin->setValue(truthJson["disp"]["x2"]);
	y2Spin->setValue(truthJson["disp"]["y2"]);
	rotateSpin->setValue(truthJson["disp"]["rotate"]);
	shearSpin->setValue(truthJson["disp"]["shear"]);
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
