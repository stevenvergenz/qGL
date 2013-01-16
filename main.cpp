#include <QtGui/QApplication>
#include "glwidget.h"

using namespace std;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);    
	GLWidget w;
	w.show();

	return a.exec();
}
