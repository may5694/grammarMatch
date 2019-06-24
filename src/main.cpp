#include <QApplication>
#include "app.hpp"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	QString startDir;
	if (argc >= 2)
		startDir = QString(argv[1]);

	// Create the application and show the window
	App a(startDir);
	a.show();

	// Run the event loop
	return app.exec();
}
