#include "OilPastelWindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	OilPastelWindow widget;
	widget.show();

	return app.exec();
}

