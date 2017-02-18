#include "stdafx.h"
#include "ZViewImage.h"
#include "ZDataManager.h"
#include "MainFrm.h"

enum TIMEREVNT { _RENDER = 100, _ADDIMG, _SEARCHIMG };
#define MAX_CAM_HIGHTLEVEL 2000
#define MIN_CAM_HIGHTLEVEL 1000
#define MAX_CAM_FOV 45.0f


CZViewImage::CZViewImage()
{
	m_addImgCnt = 0;
	m_isAnimation = true;
	m_mouseMode = 0;
}


CZViewImage::~CZViewImage()
{
}


void CZViewImage::InitGLview(int _nWidth, int _nHeight)
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(MAX_CAM_HIGHTLEVEL);
	m_cameraPri.InitializeCamera(MAX_CAM_FOV, 0, 0, m_lookAt, _nWidth, _nHeight);
	SetTimer(_RENDER, 30, NULL);
}

void CZViewImage::InitCamera()
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(MAX_CAM_HIGHTLEVEL);
}

void CZViewImage::MouseWheel(short zDelta)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	float fLevelHeight = m_cameraPri.GetLevelHeight();
	float zoomValue = fLevelHeight*0.1f;
	if (zDelta > 0){ zoomValue = -zoomValue;}
	fLevelHeight += zoomValue;	

	if (fLevelHeight > MAX_CAM_HIGHTLEVEL){ fLevelHeight = MAX_CAM_HIGHTLEVEL; }
	if (fLevelHeight < MIN_CAM_HIGHTLEVEL){ fLevelHeight = MIN_CAM_HIGHTLEVEL; }

	m_cameraPri.SetInitLevelHeight(fLevelHeight);
}

void CZViewImage::ProcGenerateThumbnail()
{
	m_addImgCnt = 0;
	SetTimer(_ADDIMG, 10, NULL);
}

void CZViewImage::GenerateThumbnail()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	_vecPageObj imgvec = SINGLETON_TMat::GetInstance()->GetImgVec();

	int cnt = 0;
	for (; m_addImgCnt < imgvec.size(); m_addImgCnt++){
		imgvec[m_addImgCnt]->LoadPageImage(DEFAULT_PAGE_SIZE);
		cnt++;
		if (cnt > 10)
			break;
	}	

	if (m_addImgCnt >= imgvec.size()){
		KillTimer(_ADDIMG);
	}

	float complete = (float)m_addImgCnt / (float)imgvec.size();
	CString str;
	str.Format(_T("Generating thumbnails.....%d"), int(complete * 100));
	str += _T("%");
	str += _T(" completed.");

	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	pM->AddOutputString(str, true);
}
void CZViewImage::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_isAnimation){
		DrawBGPageAni();
	}
	else{
		DrawBGPage();
	}
	
	Render2D();
	SwapBuffers(m_CDCPtr->GetSafeHdc());
}

void CZViewImage::Render2D()
{

}BEGIN_MESSAGE_MAP(CZViewImage, COGLWnd)
ON_WM_LBUTTONDOWN()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_MOUSEHOVER()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CZViewImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mouseMode = 2;
	IDragMap(point.x, point.y, 0);
	SetCapture();

	COGLWnd::OnLButtonDown(nFlags, point);
}


void CZViewImage::OnSize(UINT nType, int cx, int cy)
{
	COGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	m_rectWidth = cx;
	m_rectHeight = cy;
	m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0.0f, 0.0f);
}


void CZViewImage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == _RENDER){
		Render();
	}
	else if (nIDEvent == _ADDIMG){
		GenerateThumbnail();
	}
	COGLWnd::OnTimer(nIDEvent);
}

void CZViewImage::DrawBGPageAni()
{
	_vecPageObj::iterator iter = SINGLETON_TMat::GetInstance()->GetVecImageBegin();
	for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++){
		(*iter)->DrawThumbNail(0.3f);
		(*iter)->RotatePos(1.0f);
	}
}

void CZViewImage::DrawBGPage()
{
	_vecPageObj::iterator iter = SINGLETON_TMat::GetInstance()->GetVecImageBegin();
	for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++){
		(*iter)->DrawThumbNail(0.3f);
	}
}

void CZViewImage::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	COGLWnd::OnMouseHover(nFlags, point);
}


void CZViewImage::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_mouseMode == 2)		// Move
	{
		if ((point.x>0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight))
		{
			IDragMap(point.x, point.y, 1);
		}
	}
	COGLWnd::OnMouseMove(nFlags, point);
}


void CZViewImage::IDragMap(int x, int y, short sFlag)
{
	POINT3D curPos, prePos, transPos, ptLookAt;
	BOOL res = FALSE;
	int dx = 0, dy = 0;


	switch (sFlag){
	case 0:		// DOWN
		m_dragOper.init();
		m_dragOper.IsDrag = true;
		m_dragOper.px = x;
		m_dragOper.py = y;
		break;

	case 1:		// MOVE
		if (m_dragOper.IsDrag){

			curPos = m_cameraPri.ScreenToWorld(x, y);
			prePos = m_cameraPri.ScreenToWorld(m_dragOper.px, m_dragOper.py);
			transPos = prePos - curPos;
			ptLookAt = m_cameraPri.GetLookAt();

			ptLookAt.x += transPos.x;
			//ptLookAt.y += transPos.y;

			m_dragOper.px = x;
			m_dragOper.py = y;

			//	m_DemProj->ProjectPoint(&ptLookAt, &m_cameraPri.m_currentBlockid);
			m_cameraPri.SetModelViewMatrix(ptLookAt, 0, 0);
			m_lookAt = ptLookAt;
			//	UpdateCamera(ptLookAt, 0,0);
		}
		break;

	case 2:		// UP
		ReleaseCapture();
		m_dragOper.init();
		break;
	}




}

void CZViewImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mouseMode = 0;
	IDragMap(point.x, point.y, 2);

	COGLWnd::OnLButtonUp(nFlags, point);
}
