#include <QApplication>
#include "app.hpp"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	// Create the application and show the window
	App a;
	a.show();

	// Run the event loop
	return app.exec();
}
