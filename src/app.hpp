#ifndef APP_HPP
#define APP_HPP

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPixmap>
#include "imageoverlay.hpp"

class App : public QWidget {
	Q_OBJECT
public:
	App(QWidget* parent = NULL);

protected:
	void keyReleaseEvent(QKeyEvent* e);

private:
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
	void loadFacade(QString imagename);
	void updateGrammar();
};

#endif
