#ifndef APP_HPP
#define APP_HPP

#include <QWidget>
#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPixmap>
#include <vector>
#include <experimental/filesystem>
#include "imageoverlay.hpp"

namespace fs = std::experimental::filesystem;

struct FacadeInfo {
	fs::path metaPath;		// Path to metadata
	fs::path truthPath;		// Path to ground truth
	fs::path imagePath;		// Path to image
	bool valid;				// Whether grammar is valid
	int skipIdx;			// Index of valid-only facades
	double size_x;			// Size of full facade in meters
	double size_y;
	double chip_size_x;		// Size of DN-crop in meters (from metadata)
	double chip_size_y;
};

class App : public QWidget {
	Q_OBJECT
public:
	App(QWidget* parent = NULL);

protected:
	void keyReleaseEvent(QKeyEvent* e);

private:
	// Internal state
	fs::path topDir;
	std::vector<FacadeInfo> facadeInfo;
	int facadeIdx;
	int skipTotal;	// Total number of facades minus invalid ones

	// File widgets
	QLineEdit* dirLE;
	QToolButton* dirBtn;
	QLabel* indexLbl;
	QPushButton* prevBtn;
	QPushButton* nextBtn;
	QPushButton* saveBtn;
	QCheckBox* skipInvalidCB;

	// Parameter widgets
	QLabel* grammarLbl;
	QSpinBox* rowsSpin;
	QSpinBox* colsSpin;
	QDoubleSpinBox* relWSpin;
	QDoubleSpinBox* relHSpin;
	QSpinBox* doorsSpin;
	QDoubleSpinBox* relDWSpin;
	QDoubleSpinBox* relDHSpin;

	// Display parameters
	QCheckBox* visibleCB;
	QSlider* transparencySlider;
	QSlider* brightnessSlider;
	QSpinBox* x1Spin;
	QSpinBox* y1Spin;
	QSpinBox* x2Spin;
	QSpinBox* y2Spin;
	QSlider* rotateSlider;
	QSlider* shearSlider;

	// Image and overlay display
	ImageOverlay* overlay;

	void init();
	void clear();
	void readDir(QString path);
	void nextFacade();
	void prevFacade();
	void saveFacade();
	void loadFacade();
	void updateGrammar();
	void updateIndexLabel();
};

#endif
