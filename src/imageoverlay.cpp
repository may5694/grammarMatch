#include "imageoverlay.hpp"
#include <QPainter>

ImageOverlay::ImageOverlay(QWidget* parent) : QWidget(parent) {
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	m_ovisible = true;
	m_transparency = 128;
	m_brightness = 128;
	m_rotation = 0.0;
	m_shear = 0.0;
}

QSize ImageOverlay::minimumSizeHint() const {
	// At minimum we must see the entire image
	return pixmap.size();
}

QSize ImageOverlay::sizeHint() const {
	// It's possible to draw outside of image, so we should see that if we can
	int maxDim = qMax(pixmap.width(), pixmap.height()) * 1.5;
	return QSize(qMax(maxDim, 600), qMax(maxDim, 400));
}

void ImageOverlay::setOverlayVisible(bool visible) {
	m_ovisible = visible;
	update();
}
void ImageOverlay::setTransparency(int transparency) {
	m_transparency = qMin(qMax(transparency, 0), 255);
	update();
}
void ImageOverlay::setBrightness(int brightness) {
	m_brightness = qMin(qMax(brightness, 0), 255);
	update();
}
void ImageOverlay::setDispRect(QRect dispRect) {
	m_dispRect = dispRect;
	update();
}
void ImageOverlay::setRotation(double rotation) {
	m_rotation = rotation;
	update();
}
void ImageOverlay::setShear(double shear) {
	m_shear = shear;
	update();
}
void ImageOverlay::setParams(Params params) {
	m_params = params;
	update();
}

// Clear any existing image
void ImageOverlay::clear() {
	pixmap = QPixmap();
	// Nothing else will be drawn so it doesn't matter
}

// Open the specified image file
void ImageOverlay::openImage(QString imagename) {
	pixmap = QPixmap(imagename);
	m_dispRect = pixmap.rect().adjusted(0, 0, -1, -1);

	updateGeometry();
	update();
}

void ImageOverlay::paintEvent(QPaintEvent* event) {
	// Draw nothing if we don't have a valid pixmap
	if (pixmap.isNull()) return;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Set origin to image top-left
	painter.translate(
		(width() - pixmap.width()) / 2,
		(height() - pixmap.height()) / 2);

	painter.drawPixmap(0, 0, pixmap);

	// If not visible, don't draw anything else
	if (!m_ovisible) return;

	// Go to rotated frame of reference
	painter.translate(
		m_dispRect.x() + m_dispRect.width() / 2.0,
		m_dispRect.y() + m_dispRect.height() / 2.0);
	painter.rotate(m_rotation);
	painter.shear(0.0, m_shear);
	painter.translate(
		-m_dispRect.x() - m_dispRect.width() / 2.0,
		-m_dispRect.y() - m_dispRect.height() / 2.0);

	// Draw the rect
	QPen pen(Qt::red);
	pen.setWidth(0);
	painter.setPen(pen);
	painter.drawRect(m_dispRect);

	// Go to normalized facade coordinates
	painter.translate(m_dispRect.x(), m_dispRect.y());
	painter.scale(m_dispRect.width(), m_dispRect.height());

	// Skip if invalid grammar
	if (m_params.rows <= 0 || m_params.cols <= 0) return;

	QColor winColor(m_brightness, m_brightness, m_brightness, m_transparency);
	painter.setBrush(QBrush(winColor));

	// Draw doors
	if (m_params.doors > 0) {
		painter.save();
		painter.translate(0.0, 1.0 - m_params.relDHeight);
		painter.scale(1.0, m_params.relDHeight);

		for (int d = 0; d < m_params.doors; d++) {
			painter.save();
			painter.translate((double)d / m_params.doors, 0.0);
			painter.scale(1.0 / m_params.doors, 1.0);

			QRectF doorRect((1.0 - m_params.relDWidth) / 2.0, 0.0, m_params.relDWidth, 1.0);
			painter.drawRect(doorRect);

			painter.restore();
		}

		painter.restore();
		painter.scale(1.0, 1.0 - m_params.relDHeight);
	}

	// Draw windows
	for (int r = 0; r < m_params.rows; r++) {
		painter.save();
		painter.translate(0.0, (double)r / m_params.rows);
		painter.scale(1.0, 1.0 / m_params.rows);

		for (int c = 0; c < m_params.cols; c++) {
			painter.save();
			painter.translate((double)c / m_params.cols, 0.0);
			painter.scale(1.0 / m_params.cols, 1.0);

			QRectF winRect(
				(1.0 - m_params.relWidth) / 2.0,
				(1.0 - m_params.relHeight) / 2.0,
				m_params.relWidth, m_params.relHeight);
			painter.drawRect(winRect);

			painter.restore();
		}

		painter.restore();
	}
}
