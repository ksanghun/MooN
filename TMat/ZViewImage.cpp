#include "stdafx.h"
#include "ZViewImage.h"
#include "ZDataManager.h"
#include "MainFrm.h"
#include "resource.h"

enum TIMEREVNT { _RENDER = 100, _ADDIMG, _SEARCHIMG, _MOVECAMANI, _UPDATE_PAGE};


CZViewImage::CZViewImage()
{
	m_addImgCnt = 0;
	m_isAnimation = true;
	m_mouseMode = 0;

	m_pSelectPageForCNS = NULL;
	m_bIsCutNSearchMode = false;
	
	mtSetPoint3D(&m_PO, 0.0f, 0.0f, 0.0f);	
	mtSetPoint3D(&m_PN, 0.0f, 0.0f, 1.0f);

	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL*3);
	m_CNSRectStart = m_CNSRectEnd;

}


CZViewImage::~CZViewImage()
{
}



void CZViewImage::InitGLview(int _nWidth, int _nHeight)
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(MAX_CAM_HIGHTLEVEL*2);
	m_cameraPri.InitializeCamera(MAX_CAM_FOV, 0, 0, m_lookAt, _nWidth, _nHeight);
	SetTimer(_RENDER, 30, NULL);
	SetTimer(_UPDATE_PAGE, 500, NULL);


	// Load Default PDF Texture //
	GLuint pdfTexid = 0;
	SINGLETON_TMat::GetInstance()->LoadImageTexture(L"./img/pdf.bmp", pdfTexid);
	SINGLETON_TMat::GetInstance()->SetPdfTexId(pdfTexid);
	
	glInitNames();
}

void CZViewImage::InitCamera()
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	
	m_cameraPri.SetInitLevelHeight(MAX_CAM_HIGHTLEVEL*2);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);
}

void CZViewImage::MouseWheel(short zDelta)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	float fLevelHeight = m_cameraPri.GetLevelHeight();
	float zoomValue = fLevelHeight*0.1f;
	if (zDelta > 0){ zoomValue = -zoomValue;}
	fLevelHeight += zoomValue;	

	if (fLevelHeight > MAX_CAM_HIGHTLEVEL*2){ fLevelHeight = MAX_CAM_HIGHTLEVEL*2; }
	if (fLevelHeight < MIN_CAM_HIGHTLEVEL){ fLevelHeight = MIN_CAM_HIGHTLEVEL; }

	m_cameraPri.SetInitLevelHeight(fLevelHeight);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);
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
		imgvec[m_addImgCnt]->LoadThumbImage(128);
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

	glPointSize(5);
	if (m_isAnimation){
		DrawBGPageAni();
	}
	else{
		DrawBGPage();
	}
	glPointSize(1);
	
	DrawCNSRect(0.0f, 0.99f, 0.1f, 1.0f);

	Render2D();
	SwapBuffers(m_CDCPtr->GetSafeHdc());
}

void CZViewImage::Render2D()
{

}

BEGIN_MESSAGE_MAP(CZViewImage, COGLWnd)
ON_WM_LBUTTONDOWN()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_MOUSEHOVER()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_MBUTTONDBLCLK()
ON_WM_LBUTTONDBLCLK()
ON_WM_SETCURSOR()
END_MESSAGE_MAP()


void CZViewImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	// move away from screen //
	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	m_CNSRectStart = m_CNSRectEnd;
	//=======================//

	SelectObject3D(point.x, point.y, 2, 2, 0);
	m_stratPnt = point;

	if (m_bIsCutNSearchMode){		
		m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectStart);
		m_CNSRectEnd = m_CNSRectStart;		
	}
	else{		
		m_mouseMode = 2;
		IDragMap(point.x, point.y, 0);		
	}

	SetCapture();
	COGLWnd::OnLButtonDown(nFlags, point);
}

void CZViewImage::DrawCNSRect(float r, float g, float b, float a)
{
	glLineWidth(2);
	glDisable(GL_DEPTH_TEST);
	glColor4f(r, g, b, a);
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_CNSRectStart.x, m_CNSRectStart.y, 0);
	glVertex3f(m_CNSRectStart.x, m_CNSRectEnd.y, 0);
	glVertex3f(m_CNSRectEnd.x, m_CNSRectEnd.y, 0);
	glVertex3f(m_CNSRectEnd.x, m_CNSRectStart.y, 0);
	glVertex3f(m_CNSRectStart.x, m_CNSRectStart.y, 0);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1);
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
	else if (nIDEvent == _MOVECAMANI){		
		//m_stratPnt.x += m_moveVec.x*SINGLETON_TMat::GetInstance()->GetAniAcceration(m_nAniCnt);
		//m_stratPnt.y += m_moveVec.y*SINGLETON_TMat::GetInstance()->GetAniAcceration(m_nAniCnt);
		//IDragMap(m_stratPnt.x, m_stratPnt.y, 1);
		float fDelta = SINGLETON_TMat::GetInstance()->GetAniAcceration(m_nAniCnt);

		m_lookAt.x = m_lookAt.x + m_AniMoveVec.x * fDelta;
		m_lookAt.y = m_lookAt.y + m_AniMoveVec.y * fDelta;
		m_lookAt.z = m_lookAt.z + m_AniMoveVec.z * fDelta;

		float levelheight = m_cameraPri.GetLevelHeight() - m_fAniMoveSca*fDelta;		
		m_cameraPri.SetInitLevelHeight(levelheight);
		m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);

		m_nAniCnt++;
		if (m_nAniCnt >= ANI_FRAME_CNT){
			KillTimer(_MOVECAMANI);
			m_mouseMode = 0;
			IDragMap(m_stratPnt.x, m_stratPnt.y, 2);
		}
	}
	else if (nIDEvent == _UPDATE_PAGE){
		SINGLETON_TMat::GetInstance()->UpdatePageStatus(m_cameraPri.GetEye());
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
	glPointSize(1);
}

void CZViewImage::DrawBGPage()
{
	_vecPageObj::iterator iter = SINGLETON_TMat::GetInstance()->GetVecImageBegin();
	glPointSize(2);
	for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++){
		(*iter)->DrawThumbNail(0.3f);
	}
	glPointSize(1);

}

void CZViewImage::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	COGLWnd::OnMouseHover(nFlags, point);
}


void CZViewImage::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture()){
		if (m_bIsCutNSearchMode){
			m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectEnd);
		}
		else{
			int xDelta = point.x - m_stratPnt.x;
			int yDelta = point.y - m_stratPnt.y;

			if (xDelta*xDelta > yDelta*yDelta){
				m_moveVec.x = 1.0f;			m_moveVec.y = 0;
			}
			else{
				m_moveVec.y = 1.0f;			m_moveVec.x = 0;
			}
			if (m_mouseMode == 2){		// MOVE
				if ((point.x > 0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight)){
					IDragMap(point.x, point.y, 1);
				}
			}
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

			ptLookAt.x += transPos.x*m_moveVec.x;
			ptLookAt.y += transPos.y*m_moveVec.y;

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
	if (m_bIsCutNSearchMode){
	}
	else{
		m_mouseMode = 0;
		IDragMap(point.x, point.y, 2);
	}

	ReleaseCapture();
	COGLWnd::OnLButtonUp(nFlags, point);
}


void CZViewImage::DrawImageByOrderForPicking()
{
	_vecPageObj::iterator iter = SINGLETON_TMat::GetInstance()->GetVecImageBegin();
	int id = 0;
	for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++){
		glPushName(id);
		(*iter)->DrawForPicking();
		glPopName();
		id++;
	}
}

int CZViewImage::SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode)
{
	if (m_pSelectPageForCNS){
		m_pSelectPageForCNS->SetSelection(false);
		m_pSelectPageForCNS = NULL;
	}

	GLuint selectBuff[1024];
	memset(&selectBuff, 0, sizeof(GLuint) * 1024);

	GLint hits, viewport[4];
	hits = 0;

	glSelectBuffer(1024, selectBuff);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glRenderMode(GL_SELECT);
	
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, 2, 2, viewport);
	gluPerspective(m_cameraPri.m_Cntfovy, (float)viewport[2] / (float)viewport[3], m_cameraPri.GetNearPlane(), m_cameraPri.GetFarPlane());

	//m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	glMatrixMode(GL_MODELVIEW);
	DrawImageByOrderForPicking();

	hits = glRenderMode(GL_RENDER);
	if (hits>0)
	{
		m_pSelectPageForCNS = SINGLETON_TMat::GetInstance()->GetPageByOrderID(selectBuff[3]);
		if(m_pSelectPageForCNS)
			m_pSelectPageForCNS->SetSelection(true);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return hits;
}

void CZViewImage::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	COGLWnd::OnMButtonDblClk(nFlags, point);
}

void CZViewImage::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pSelectPageForCNS){

		KillTimer(_MOVECAMANI);
		m_nAniCnt = 0;
		m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
		m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE-200;
		
		SetTimer(_MOVECAMANI, 20, NULL);		
	}
	COGLWnd::OnLButtonDblClk(nFlags, point);
}


void CZViewImage::EnableCutSearchMode(bool IsEnable) 
{ 
	m_bIsCutNSearchMode = IsEnable; 
}

BOOL CZViewImage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsCutNSearchMode){
		SetCursor(LoadCursor(NULL, IDC_CROSS));
	//	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	//	m_CNSRectStart = m_CNSRectEnd;
		return TRUE;
	}
	else{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}
	return COGLWnd::OnSetCursor(pWnd, nHitTest, message);
}


void CZViewImage::MovePrePage()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250){
		SelectObject3D(10, m_rectHeight*0.5f, 2, 2, 0);
		if (m_pSelectPageForCNS){

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE-200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}
void CZViewImage::MoveNextPage()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250){
		SelectObject3D(m_rectWidth - 10, m_rectHeight*0.5f, 2, 2, 0);
		if (m_pSelectPageForCNS){

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE-200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}


void CZViewImage::MoveNextUp()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250){
		SelectObject3D(m_rectWidth*0.5f, 10, 2, 2, 0);
		if (m_pSelectPageForCNS){

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}

void CZViewImage::MoveNextDown()
{
	if (m_cameraPri.GetLevelHeight() < DEFAULT_PAGE_SIZE + 250){
		SelectObject3D(m_rectWidth*0.5f, m_rectHeight-10, 2, 2, 0);
		if (m_pSelectPageForCNS){

			KillTimer(_MOVECAMANI);
			m_nAniCnt = 0;
			m_AniMoveVec = m_pSelectPageForCNS->GetPos() - m_lookAt;
			m_fAniMoveSca = m_cameraPri.GetLevelHeight() - DEFAULT_PAGE_SIZE - 200;

			SetTimer(_MOVECAMANI, 20, NULL);
		}
	}
}

RECT2D CZViewImage::GetSelectedAreaForCNS()
{
	if (m_pSelectPageForCNS){
		RECT2D selRect = m_pSelectPageForCNS->ConvertVec3DtoImgateCoord(m_CNSRectStart, m_CNSRectEnd);
		return selRect;
	}
	return RECT2D(0, 0, 0, 0);
}