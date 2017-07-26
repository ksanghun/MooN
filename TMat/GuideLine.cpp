#include "stdafx.h"
#include "GuideLine.h"

CGuideLine::CGuideLine()
{
//	mtSetPoint2D(&m_increment, 0, 0);
	mtSetPoint2D(&m_sPnt, 0, 0);
	mtSetPoint2D(&m_ePnt, 0, 0);
	mtSetPoint2D(&m_sDPnt, 0, 0);
	mtSetPoint2D(&m_eDPnt, 0, 0);

}


CGuideLine::~CGuideLine()
{
}

void CGuideLine::Init(float _r, float _g, float _b, int _size, int _type)
{
	mtSetPoint3D(&m_color, _r, _g, _b);


	if (_type == 0){ // horizental bar
		mtSetPoint2D(&m_texcoord[0], 1.0f, 0.0f);
		mtSetPoint2D(&m_vecButton[0], 0.0f, _size*0.5f);

		mtSetPoint2D(&m_texcoord[1], 0.0f, 0.0f);
		mtSetPoint2D(&m_vecButton[1], -_size, _size*0.5f);

		mtSetPoint2D(&m_texcoord[2], 0.0f, 1.0f);
		mtSetPoint2D(&m_vecButton[2], -_size, -_size*0.5f);

		mtSetPoint2D(&m_texcoord[3], 1.0f, 1.0f);
		mtSetPoint2D(&m_vecButton[3], 0.0f, -_size*0.5f);
	}


	else{		// vertical bar
		//mtSetPoint2D(&m_texcoord[0], 1.0f, 0.0f);
		//mtSetPoint2D(&m_texcoord[1], 0.0f, 0.0f);		
		//mtSetPoint2D(&m_texcoord[2], 0.0f, 1.0f);
		//mtSetPoint2D(&m_texcoord[3], 1.0f, 1.0f);
		
		//mtSetPoint2D(&m_vecButton[0], _size*0.5f, 0.0f);
		//mtSetPoint2D(&m_vecButton[1], _size*0.5f, _size);
		//mtSetPoint2D(&m_vecButton[2], -_size*0.5f, _size);
		//mtSetPoint2D(&m_vecButton[3], -_size*0.5f, 0.0f);

		mtSetPoint2D(&m_texcoord[0], 0.0f, 1.0f);
		mtSetPoint2D(&m_texcoord[1], 1.0f, 1.0f);
		mtSetPoint2D(&m_texcoord[2], 1.0f, 0.0f);
		mtSetPoint2D(&m_texcoord[3], 0.0f, 0.0f);
		
		
		mtSetPoint2D(&m_vecButton[0], _size*0.5f, -_size);
		mtSetPoint2D(&m_vecButton[1], _size*0.5f, 0.0f);		
		mtSetPoint2D(&m_vecButton[2], -_size*0.5f, 0.0f);
		mtSetPoint2D(&m_vecButton[3], -_size*0.5f, -_size);
	}

	
}

void CGuideLine::DrawLine()
{
	//glVertex2f(m_sDPnt.x+m_increment.x, m_sDPnt.y+m_increment.y);
	//glVertex2f(m_eDPnt.x+m_increment.x, m_eDPnt.y+m_increment.y);

	//glVertex2f(m_sDPnt.x,  m_sDPnt.y);
	//glVertex2f(m_eDPnt.x,  m_eDPnt.y);

	glVertex2f(m_sPnt.x, m_sPnt.y);
	glVertex2f(m_ePnt.x, m_ePnt.y);
}

POINT2D CGuideLine::GetCurrPos()
{
	POINT2D res;
//	mtSetPoint2D(&res, m_sPnt.x + m_increment.x, m_sPnt.y + m_increment.y);
	mtSetPoint2D(&res, m_sPnt.x, m_sPnt.y);
	return res;
}
void CGuideLine::DrawButtions(float r, float g, float b)
{
	glBindTexture(GL_TEXTURE_2D, m_texId);
	glPushMatrix();
//	glTranslatef(m_sDPnt.x + m_increment.x, m_sDPnt.y + m_increment.y, 0.0f);
	glTranslatef(m_sPnt.x, m_sPnt.y, 0.0f);

	glColor3f(r, g, b);
	glBegin(GL_QUADS);

	glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
	glVertex2f(m_vecButton[0].x, m_vecButton[0].y);

	glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
	glVertex2f(m_vecButton[1].x, m_vecButton[1].y);

	glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
	glVertex2f(m_vecButton[2].x, m_vecButton[2].y);

	glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
	glVertex2f(m_vecButton[3].x, m_vecButton[3].y);

	glEnd();


	//glColor3f(0, 0, 0);
	//glBegin(GL_QUADS);
	//glVertex2f(m_vecButton[0].x, m_vecButton[0].y);
	//glVertex2f(m_vecButton[1].x, m_vecButton[1].y);
	//glVertex2f(m_vecButton[2].x, m_vecButton[2].y);
	//glVertex2f(m_vecButton[0].x, m_vecButton[0].y);
	//glEnd();

	glPopMatrix();

}

void CGuideLine::SetIncrement(float _dx, float _dy)
{
	//m_increment.x += _dx;
	//m_increment.y += _dy;

	m_sPnt.x += _dx;
	m_sPnt.y += _dy;

	m_ePnt.x += _dx;
	m_ePnt.y += _dy;
}