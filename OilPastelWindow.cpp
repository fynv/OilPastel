#include "OilPastelWindow.h"


OilPastelWindow::OilPastelWindow()
{
	m_ui.setupUi(this);
	m_ui.canvas->setFixedSize(m_ui.canvas->m_image_width, m_ui.canvas->m_image_height);

	m_ui.edit_r->setText(QString::number(m_ui.canvas->stroke_color[0]));
	m_ui.edit_g->setText(QString::number(m_ui.canvas->stroke_color[1]));
	m_ui.edit_b->setText(QString::number(m_ui.canvas->stroke_color[2]));
	m_ui.edit_opacity->setText(QString::number(m_ui.canvas->stroke_color[3]));
	m_ui.edit_radius->setText(QString::number(m_ui.canvas->stroke_radius));
	m_ui.edit_grain->setText(QString::number(m_ui.canvas->stroke_grain_size));
	m_ui.edit_rough->setText(QString::number(m_ui.canvas->stroke_roughness));
	m_ui.edit_hard->setText(QString::number(m_ui.canvas->stroke_hardness));

	connect(m_ui.btnLightDirCenter, SIGNAL(clicked()), SLOT(LightDirCenter()));
	connect(m_ui.btnLightDirUp, SIGNAL(clicked()), SLOT(LightDirUp()));
	connect(m_ui.btnLightDirDown, SIGNAL(clicked()), SLOT(LightDirDown()));
	connect(m_ui.btnLightDirLeft, SIGNAL(clicked()), SLOT(LightDirLeft()));
	connect(m_ui.btnLightDirRight, SIGNAL(clicked()), SLOT(LightDirRight()));

	connect(m_ui.edit_r, SIGNAL(editingFinished()), SLOT(EditR()));
	connect(m_ui.edit_g, SIGNAL(editingFinished()), SLOT(EditG()));
	connect(m_ui.edit_b, SIGNAL(editingFinished()), SLOT(EditB()));
	connect(m_ui.edit_opacity, SIGNAL(editingFinished()), SLOT(EditA()));
	connect(m_ui.edit_radius, SIGNAL(editingFinished()), SLOT(EditRadius()));
	connect(m_ui.edit_grain, SIGNAL(editingFinished()), SLOT(EditGrain()));
	connect(m_ui.edit_rough, SIGNAL(editingFinished()), SLOT(EditRough()));
	connect(m_ui.edit_hard, SIGNAL(editingFinished()), SLOT(EditHard()));
	

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

void OilPastelWindow::EditR()
{
	m_ui.canvas->stroke_color[0] = m_ui.edit_r->text().toFloat();
}

void OilPastelWindow::EditG()
{
	m_ui.canvas->stroke_color[1] = m_ui.edit_g->text().toFloat();
}

void OilPastelWindow::EditB()
{
	m_ui.canvas->stroke_color[2] = m_ui.edit_b->text().toFloat();
}

void OilPastelWindow::EditA()
{
	m_ui.canvas->stroke_color[3] = m_ui.edit_opacity->text().toFloat();
}

void OilPastelWindow::EditRadius()
{
	m_ui.canvas->stroke_radius = m_ui.edit_radius->text().toFloat();
}

void OilPastelWindow::EditGrain()
{
	m_ui.canvas->stroke_grain_size = m_ui.edit_grain->text().toFloat();
}

void OilPastelWindow::EditRough()
{
	m_ui.canvas->stroke_roughness = m_ui.edit_rough->text().toFloat();
}

void OilPastelWindow::EditHard()
{
	m_ui.canvas->stroke_hardness = m_ui.edit_hard->text().toFloat();
}
