#include "OilPastelWindow.h"


OilPastelWindow::OilPastelWindow()
{
	m_ui.setupUi(this);
	connect(m_ui.btnLightDirCenter, SIGNAL(clicked()), SLOT(LightDirCenter()));
	connect(m_ui.btnLightDirUp, SIGNAL(clicked()), SLOT(LightDirUp()));
	connect(m_ui.btnLightDirDown, SIGNAL(clicked()), SLOT(LightDirDown()));
	connect(m_ui.btnLightDirLeft, SIGNAL(clicked()), SLOT(LightDirLeft()));
	connect(m_ui.btnLightDirRight, SIGNAL(clicked()), SLOT(LightDirRight()));

}

OilPastelWindow::~OilPastelWindow()
{
	
}

void OilPastelWindow::LightDirCenter()
{
	m_ui.canvas->m_light_dir = { 0.0f, 0.0f, 1.0f };
	m_ui.canvas->update();
	
}

void OilPastelWindow::LightDirUp()
{
	m_ui.canvas->m_light_dir[1] += 0.2f;
	m_ui.canvas->update();
}

void OilPastelWindow::LightDirDown()
{
	m_ui.canvas->m_light_dir[1] -= 0.2f;
	m_ui.canvas->update();
}

void OilPastelWindow::LightDirLeft()
{
	m_ui.canvas->m_light_dir[0] -= 0.2f;
	m_ui.canvas->update();
}

void OilPastelWindow::LightDirRight()
{
	m_ui.canvas->m_light_dir[0] += 0.2f;
	m_ui.canvas->update();
}