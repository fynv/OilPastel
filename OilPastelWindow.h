#pragma once

#include <QWidget>
#include "ui_OilPastelWindow.h"

class OilPastelWindow : public QWidget
{
	Q_OBJECT
public:
	OilPastelWindow();
	virtual ~OilPastelWindow();

private:
	Ui_OilPastelWindow m_ui;

private slots:
	void LightDirCenter();
	void LightDirUp();
	void LightDirDown();
	void LightDirLeft();
	void LightDirRight();
};

