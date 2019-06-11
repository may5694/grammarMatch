#ifndef IMAGEOVERLAY_HPP
#define IMAGEOVERLAY_HPP

#include <QWidget>
#include <QPixmap>
#include <QRect>

// Grammar parameters
struct Params {
	int rows;
	int cols;
	double relWidth;
	double relHeight;
	int doors;
	double relDWidth;
	double relDHeight;

	Params() : rows(0), cols(0), relWidth(0.0), relHeight(0.0), doors(0),
		relDWidth(0.0), relDHeight(0.0) {}
};

class ImageOverlay : public QWidget {
	Q_OBJECT
public:
	ImageOverlay(QWidget* parent = NULL);

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

	QRect imageRect() const { return pixmap.rect(); }

public slots:
	void openImage(QString imagename);
	void setOverlayVisible(bool visible);
	void setBrightness(int brightness);
	void setDispRect(QRect dispRect);
	void setRotation(double rotation);
	void setShear(double shear);
	void setParams(Params params);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	QPixmap pixmap;			// Facade image
	bool m_ovisible;		// Whether overlay is visible
	int m_brightness;		// Brightness of windows
	QRect m_dispRect;		// Display rectangle
	double m_rotation;		// Rotation in degrees
	double m_shear;			// Shear amount
	Params m_params;		// Grammar parameters
};

#endif
