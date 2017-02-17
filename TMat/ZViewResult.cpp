#include "stdafx.h"
#include "ZViewResult.h"


CZViewResult::CZViewResult()
{
}


CZViewResult::~CZViewResult()
{
}


void CZViewResult::InitGLview(int _nWidth, int _nHeight)
{

}
void CZViewResult::MouseWheel(short zDelta)
{

}
void CZViewResult::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Render2D();
	SwapBuffers(m_CDCPtr->GetSafeHdc());
}
void CZViewResult::Render2D()
{

}