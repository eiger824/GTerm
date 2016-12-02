#include <QApplication>
#include "gui.hpp"
using namespace gterm;
int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	GTerm gterm;
	return app.exec();
}
