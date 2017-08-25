#include "stdafx.h"
#include "ExtractView.h"
#include "TMatView.h"


CExtractView::CExtractView()
{
	memset(&m_LogFont, 0, sizeof(m_LogFont));
	m_LogFont.lfCharSet = ANSI_CHARSET;
	m_LogFont.lfHeight = -18;
	m_LogFont.lfWidth = 0;
//	m_LogFont.lfWeight = FW_BOLD;

	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));


	m_pImg = NULL;
	m_eTexId = 0;
	m_MatImg = NULL;

	m_selGuideLineId = -1;
	m_bCameraMove = true;

	m_bUserEditMode = __EDIT_NONE;

	mtSetPoint3D(&m_PO, 0.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_PN, 0.0f, 0.0f, 1.0f);
	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	m_CNSRectStart = m_CNSRectEnd;
}


CExtractView::~CExtractView()
{
	m_MatImg.release();
	m_MatOImg.release();
	if (m_pImg){
		delete m_pImg;
		m_pImg = NULL;
	}
	//if (m_pSelectcImg){
	//	delete m_pSelectcImg;
	//}
}


void CExtractView::InitCamera()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(5000 * 2);
	m_cameraPri.InitializeCamera(45.0f, 0, 0, m_lookAt, m_rectWidth, m_rectHeight);
}

void CExtractView::InitGLview(int _nWidth, int _nHeight)
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(5000 * 2);
	m_cameraPri.InitializeCamera(45.0f, 0, 0, m_lookAt, _nWidth, _nHeight);

	m_rectWidth = _nWidth;
	m_rectHeight = _nHeight;
	

	GLuint tid = Load4ChannelImage("./img/arrow_green.tiff");

	m_guideLine[0].SetButtonTexId(tid);
	m_guideLine[1].SetButtonTexId(tid);
	m_guideLine[2].SetButtonTexId(tid);
	m_guideLine[3].SetButtonTexId(tid);

	glInitNames();

//	SetTimer(200, 50, NULL);

	if (m_OCRMng.InitOCRMng() == false){
		TRACE("OCR Error");
	}

}


GLuint CExtractView::Load4ChannelImage(char* sz)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	GLuint nTexId = 0;
	//USES_CONVERSION;
	//char* sz = T2A(strPath);

	IplImage *pimg = cvLoadImage(sz, CV_LOAD_IMAGE_UNCHANGED);
	if (pimg){
		//	cvShowImage(sz, pimg);
		cvCvtColor(pimg, pimg, CV_BGRA2RGBA);

		// glupload Image - Thumnail image=======================================================//
		glGenTextures(1, &nTexId);
		glBindTexture(GL_TEXTURE_2D, nTexId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexImage2D(GL_TEXTURE_2D, 0, 4, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, pimg->width, pimg->height, GL_RGBA, GL_UNSIGNED_BYTE, pimg->imageData);
		//glTexImage2D(GL_TEXTURE_2D,
		//	0,
		//	GL_RGBA,
		//	pimg->width,
		//	pimg->height,
		//	0,
		//	GL_RGBA,
		//	GL_UNSIGNED_BYTE,
		//	pimg->imageData
		//	);
		//======================================================================================//
		cvReleaseImage(&pimg);
	}
	return nTexId;
}


void CExtractView::MouseWheel(short zDelta)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	float fLevelHeight = m_cameraPri.GetLevelHeight();
	float zoomValue = fLevelHeight*0.1f + MIN_CAM_HIGHTLEVEL;
	if (zDelta > 0){ zoomValue = -zoomValue; }
	fLevelHeight += zoomValue;

	//	if (fLevelHeight > MAX_CAM_HIGHTLEVEL*15){ fLevelHeight = MAX_CAM_HIGHTLEVEL*15; }
	if (fLevelHeight < MIN_CAM_HIGHTLEVEL){ fLevelHeight = MIN_CAM_HIGHTLEVEL; }

	m_cameraPri.SetInitLevelHeight(fLevelHeight);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);

	m_posSearchBox = m_cameraPri.ScreenToWorld(0, m_rectHeight);
	Render();
}

void CExtractView::DrawGuideLines()
{
	// Draw Guide Line=====================//
	glPushMatrix();
	glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
	glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < 4; i++){
		m_guideLine[i].DrawButtions(1.0f, 1.0f, 1.0f);
	}
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0f, 0.0f, 0.0f);
	glLineStipple(2, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++){
		m_guideLine[i].DrawLine();
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glPopMatrix();
	//====================================//
}

void CExtractView::Render()
{
	m_posSearchBox = m_cameraPri.ScreenToWorld(0, m_rectHeight);


	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

//	glClearColor(0.1f, 0.2f, 0.0f, 1.0f);
	glClearColor(0.99f, 0.99f, 0.99f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_pImg){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_eTexId);
		m_pImg->DrawWithoutTexID(1.0f);
		glDisable(GL_TEXTURE_2D);


		DrawExtractions();
		
		

		//glLineWidth(2);
		//std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
		//if (ptexBox->size() > 0){
		//	// Draw detected position //
		//	glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		//	glPushMatrix();
		//	glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
		//	glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

		//	//if (m_bIsNear){		
		//	
		//	for (int i = 0; i < ptexBox->size(); i++){

		//		RECT2D rect;
		//		rect.set((*ptexBox)[i].textbox.x, (*ptexBox)[i].textbox.x + (*ptexBox)[i].textbox.width,
		//			(*ptexBox)[i].textbox.y, (*ptexBox)[i].textbox.y + (*ptexBox)[i].textbox.height);
		//		
		//		glColor4f(0.0f, 1.0f, 0.0f, 0.99f);
		//		if ((*ptexBox)[i].pNextBox!=NULL){
		//			glColor4f(1.0f, 0.0f, 0.0f, 0.99f);
		//		}

		//		if ((*ptexBox)[i].IsBig){
		//			glColor4f(0.0f, 0.0f, 1.0f, 0.99f);
		//		}		

		//		glBegin(GL_LINE_STRIP);
		//		glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
		//		glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
		//		glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
		//		glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
		//		glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
		//		glEnd();

		//	}			
		//	glPopMatrix();
		//}
	}

	DrawGuideLines();
//	DrawSearchRect();

	DrawCNSRect(1.0f, 0.0f, 0.0f, 1.0f);

	glLineWidth(1);
	
	SwapBuffers(m_CDCPtr->GetSafeHdc());
}


void CExtractView::DrawExtractions()
{
	
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	if (ptexBox->size() > 0){
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
		glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

		//if (m_bIsNear){		
		glLineWidth(1);
		POINT3D tPos;
		RECT2D rect;
		for (int i = 0; i < ptexBox->size(); i++){

			
			rect.set((*ptexBox)[i].textbox.x, (*ptexBox)[i].textbox.x + (*ptexBox)[i].textbox.width,
				(*ptexBox)[i].textbox.y, (*ptexBox)[i].textbox.y + (*ptexBox)[i].textbox.height);


			// Draw Text //
			glColor4f(0.0f, 0.0f, 1.0f, 0.99f);
			if ((*ptexBox)[i].textType==1)
				glColor4f(1.0f, 0.0f, 1.0f, 0.99f);
			mtSetPoint3D(&tPos, (rect.x1 + rect.x2)*0.5f, m_pImg->GetImgHeight() - rect.y1 + 5, 1.0f);
			gl_DrawText(tPos, (*ptexBox)[i].strCode, m_LogFont, 1, m_pBmpInfo, m_CDCPtr);



			if ((*ptexBox)[i].IsSelected==false){
				glColor4f(0.0f, 1.0f, 0.0f, 0.99f);
			//	if ((*ptexBox)[i].pNextBox != NULL)		glColor4f(1.0f, 0.0f, 0.0f, 0.99f);			

				//if ((*ptexBox)[i].IsBig)				glColor4f(0.0f, 0.0f, 1.0f, 0.99f);				

				//if ((*ptexBox)[i].IsAmbig)				glColor4f(1.0f, 0.0f, 0.0f, 0.99f);

				if ((*ptexBox)[i].fConfi < 0.70f)		glColor4f(1.0f, 0.0f, 0.0f, 0.99f);

				glBegin(GL_LINE_STRIP);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

				
				
			}
			else{

				glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
				glBegin(GL_QUADS);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

				glColor4f(1.0f, 0.0f, 0.0f, 0.99f);
				glLineStipple(2, 0xAAAA);
				glEnable(GL_LINE_STIPPLE);

				glBegin(GL_LINE_STRIP);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

				glDisable(GL_LINE_STIPPLE);

			}
		}
		glLineWidth(1);
		glPopMatrix();
	}

	else{
		DrawExtractedLine();
	}
}


void CExtractView::DrawExtractionsForPick()
{
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	if (ptexBox->size() > 0){
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
		glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

		//if (m_bIsNear){		

		for (int i = 0; i < ptexBox->size(); i++){
			glPushName(i + 1000);
			RECT2D rect;
			rect.set((*ptexBox)[i].textbox.x, (*ptexBox)[i].textbox.x + (*ptexBox)[i].textbox.width,
				(*ptexBox)[i].textbox.y, (*ptexBox)[i].textbox.y + (*ptexBox)[i].textbox.height);

			glBegin(GL_QUADS);
			glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
			glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
			glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
			glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
			glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
			glEnd();
			glPopName();
		}
		glPopMatrix();
	}

	else{
		DrawExtractedLineForPick();
	}
}

void CExtractView::DrawSearchRect()
{
	glPushMatrix();
	

	
//	glTranslatef(-m_pImg->GetImgWidth()*0.6f, +m_pImg->GetImgHeight()*0.6f, 0.0f);
//	glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
	glTranslatef(m_posSearchBox.x+5, m_posSearchBox.y+5, 0.0f);
	glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
	

	

	RECT2D rect = m_Extractor.GetSearchingRect();
	RECT2D averRect = m_Extractor.GetAverRect();

	glColor4f(1.0f, 1.0f, 0.0f, 0.99f);
	glBegin(GL_LINE_STRIP);
	glVertex3f(rect.x1, rect.y1, 0.0f);
	glVertex3f(rect.x1, rect.y2, 0.0f);
	glVertex3f(rect.x2, rect.y2, 0.0f);
	glVertex3f(rect.x2, rect.y1, 0.0f);
	glVertex3f(rect.x1, rect.y1, 0.0f);
	glEnd();


	glPushMatrix();
	glTranslatef(rect.width + 5, 0, 0);

	glColor4f(0.0f, 1.0f, 0.0f, 0.99f);
	glBegin(GL_LINE_STRIP);
	glVertex3f(averRect.x1, averRect.y1, 0.0f);
	glVertex3f(averRect.x1, averRect.y2, 0.0f);
	glVertex3f(averRect.x2, averRect.y2, 0.0f);
	glVertex3f(averRect.x2, averRect.y1, 0.0f);
	glVertex3f(averRect.x1, averRect.y1, 0.0f);
	glEnd();
	glPopMatrix();


	glPopMatrix();

}

void CExtractView::Render2D()
{

}BEGIN_MESSAGE_MAP(CExtractView, COGLWnd)
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_SETCURSOR()
END_MESSAGE_MAP()


void CExtractView::OnSize(UINT nType, int cx, int cy)
{
	COGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	m_rectWidth = cx;
	m_rectHeight = cy;
	m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0.0f, 0.0f);

	
}


void CExtractView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 100){
		Render();
	}

	COGLWnd::OnTimer(nIDEvent);
}

void CExtractView::SetExtractImage(CZPageObject* _pImg, RECT2D cutRect)
{ 
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	m_pSelectcImg = _pImg;

	m_Extractor.InitExtractor();

	float levelheight = 700;
	m_cameraPri.SetInitLevelHeight(levelheight);
//	m_cameraPri.SetModelViewMatrix(pImg->GetPos(), 0, 0);

	m_posSearchBox = m_cameraPri.ScreenToWorld(0, m_rectHeight);


	if (m_eTexId != 0){
		glDeleteTextures(1, &m_eTexId);
		m_eTexId = 0;
	}
	m_cutRect = cutRect;
	if (m_pImg){
		delete m_pImg;
		m_pImg = NULL;
	}
	m_pImg = new CZPageObject;

	// Add Image Data //
//	m_pImg->SetName(L"", L"", L"", 0, 0);

	m_pImg->SetName(_pImg->GetPath(), _pImg->GetPName(), _pImg->GetName(), _pImg->GetPCode(), _pImg->GetCode());
	POINT3D pos;
	mtSetPoint3D(&pos, 0.0f, 0.0f, 0.0f);
	m_pImg->SetPosition(pos);





	IplImage* pCut = NULL;
	m_strImgPath = _pImg->GetPath();
	

	USES_CONVERSION;
	char* sz = T2A(_pImg->GetPath());
	IplImage *pimg = NULL;

	CString str = PathFindExtension(_pImg->GetPath());
	if ((str == L".pdf") || (str == L".jpg") || (str == L".JPG") || (str == L".jpeg")){

		pimg = SINGLETON_TMat::GetInstance()->LoadPDFImage(_pImg->GetPath(), 4);

		pCut = cvCreateImage(cvSize(cutRect.width, cutRect.height), pimg->depth, pimg->nChannels);
		cvSetImageROI(pimg, cvRect(cutRect.x1, cutRect.y1, cutRect.width, cutRect.height));		// posx, posy = left - top
		cvCopy(pimg, pCut);


		if (pCut){
			m_pImg->SetSize(pCut->width, pCut->height, DEFAULT_PAGE_SIZE);
			glGenTextures(1, &m_eTexId);
			glBindTexture(GL_TEXTURE_2D, m_eTexId);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pCut->width, pCut->height, GL_RGBA, GL_UNSIGNED_BYTE, pCut->imageData);
		}
	}
	else{
		pimg = cvLoadImage(sz);
		pCut = cvCreateImage(cvSize(cutRect.width, cutRect.height), pimg->depth, pimg->nChannels);
		cvSetImageROI(pimg, cvRect(cutRect.x1, cutRect.y1, cutRect.width, cutRect.height));		// posx, posy = left - top
		cvCopy(pimg, pCut);


		if (pCut){
			m_pImg->SetSize(pCut->width, pCut->height, DEFAULT_PAGE_SIZE);

			cvCvtColor(pimg, pimg, CV_BGR2RGB);
			glGenTextures(1, &m_eTexId);
			glBindTexture(GL_TEXTURE_2D, m_eTexId);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pCut->width, pCut->height, GL_RGB, GL_UNSIGNED_BYTE, pCut->imageData);
		}
	}

	m_pImg->m_fImgCols = pCut->width;
	m_pImg->m_fImgRows = pCut->height;
	
	m_MatImg.release();
	m_MatOImg.release();

	
	m_MatImg = cv::cvarrToMat(pCut);
	m_MatImg.copyTo(m_MatOImg);
	cvtColor(m_MatImg, m_MatImg, CV_BGR2GRAY);
	

	cvReleaseImage(&pimg);
	cvReleaseImage(&pCut);

	
	float fSize = 25.0f/m_pImg->GetfXScale();

	m_guideLine[0].Init(0.0f, 1.0f, 0.0f, fSize, 0);
	m_guideLine[1].Init(0.0f, 1.0f, 0.0f, fSize, 0);
	m_guideLine[2].Init(0.0f, 1.0f, 0.0f, fSize, 1);
	m_guideLine[3].Init(1.0f, 0.0f, 0.0f, fSize, 1);

	m_guideLine[0].SetStartPnt(0.0f, m_MatImg.rows, 0.0f);
	m_guideLine[0].SetEndPnt(m_MatImg.cols, m_MatImg.rows, 0.0f);

	m_guideLine[1].SetStartPnt(0.0f, 0.0f, 0.0f);
	m_guideLine[1].SetEndPnt(m_MatImg.cols, 0.0f, 0.0f);

	m_guideLine[2].SetStartPnt(0.0f, 0.0f, 0.0f);
	m_guideLine[2].SetEndPnt(0.0f, m_MatImg.rows, 0.0f);

	m_guideLine[3].SetStartPnt(m_MatImg.cols, 0.0f, 0.0f);
	m_guideLine[3].SetEndPnt(m_MatImg.cols, m_MatImg.rows, 0.0f);


//	ProcExtractTextBoundary();
	Render();
}

void CExtractView::ContractImage(cv::Mat& img)
{
//	cv::imshow("Before-binaryMat", img);
	for (int y = 1; y < img.rows - 1; y++){
		for (int x = 1; x < img.cols - 1; x++){
			int pId = (y - 1)*img.step + x;
			int id = y*img.step + x;
			int nId = (y + 1)*img.step + x;

			if (img.data[id] < 128){		// black
				if (img.data[nId] > 128){
					img.data[id] = 255;
				}
			}
			//if (img.data[id] > 128){		// black
			//	if (img2.data[pId] < 128){
			//		img2.data[pId] = 255;
			//	}
			//}

			//pId = (y)*img.step + x - 1;
			//id = y*img.step + x;
			//nId = (y)*img.step + x + 1;

			//if (img.data[id] < 128){		// black
			//	if (img.data[nId] > 128){
			//		img.data[id] = 255;
			//	}
			//}

		}
	}	//}	
}

void CExtractView::ProcExtractTextBoundary()
{
	std::vector<cv::Rect> textbox;
	//cv::Mat tImg;
	cv::Mat img2;
	m_MatImg.copyTo(img2);


	//cv::Mat img2(m_MatImg.size(), m_MatImg.type());
	////Apply thresholding
	//cv::threshold(m_MatImg, img2, 200, 255, cv::THRESH_BINARY);
	//cv::imshow("THRESH_BINARY", img2);
	//img2.copyTo(tImg);
//	m_MatImg.copyTo(img2);
	
//	ContractImage(img2);
//	ContractImage(img2);

//	cv::imshow("ContractImage", img2);



	m_Extractor.getContours(img2, m_MatImg, textbox);
	img2.release();

	




	//for (int i = 0; i < ptexBox.size(); i++){
	//	if (ptexBox[i].IsAmbig){
	//		cv::rectangle(img2, ptexBox[i].textbox, cv::Scalar(0, 0, 0), 1, 8, 0);
	//	}
	//	else{
	//		cv::rectangle(img2, ptexBox[i].textbox, cv::Scalar(0, 0, 0), 2, 8, 0);
	//	}
	//}
//	cv::imshow("Extraction", img2);


}

void CExtractView::ChangeXExpand(int _d)
{ 
	m_Extractor.ChangeXExpand(_d);
//	ProcExtractTextBoundary();
//	Render();
}
void CExtractView::ChangeYExpand(int _d)
{ 
	m_Extractor.ChangeYExpand(_d); 
//	ProcExtractTextBoundary();
//	Render();
}

void CExtractView::GroupingExtractions()
{
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	std::map<int, int> matchRes;
	float fTh = 0.75;
	for (int i = 0; i < ptexBox->size(); i++){
		float fD = 0.0f;
		for (int j = 0; j < ptexBox->size(); j++){

			if (i == j) continue;
			if ((*ptexBox)[j].IsMatched == true) continue;
			
			fD = MatchingCutImgs((*ptexBox)[i].pcutImg, (*ptexBox)[j].pcutImg);
			if (fD > fTh){
				(*ptexBox)[j].IsMatched = true;
			}
		}
	}
}

void CExtractView::CutNSearchExtractions()
{

	// Make Cut image for List Ctrl ===================================//
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 0);
	//	ptexBox = m_Extractor.GetTextBoxes();
	for (int i = 0; i < ptexBox->size(); i++){

		if ((*ptexBox)[i].pcutImg != NULL){
			cvReleaseImage(&(*ptexBox)[i].pcutImg);
			(*ptexBox)[i].pcutImg = NULL;
		}

		cv::Rect rect = (*ptexBox)[i].textbox;
		rect.x += m_cutRect.x1;
		rect.y += m_cutRect.y1;

		if ((rect.width < 3) || (rect.height < 3)) continue;

		// Need to Check!!!!!!!!!!!!!!!
		m_Extractor.verifyCutSize(rect, src->width, src->height);
		//==============================
		IplImage* pCut = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);

		cvSetImageROI(src, cvRect(rect.x, rect.y, rect.width, rect.height));		// posx, posy = left - top
		cvCopy(src, pCut);


		// Normalize //
		//	cv::Rect nRect = GetNomalizedSize(averArea, (*ptexBox)[i].textbox);
		cv::Rect nRect = GetNomalizedWordSize((*ptexBox)[i].textbox);


		(*ptexBox)[i].pcutImg = cvCreateImage(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H), pCut->depth, pCut->nChannels);
		cvSet((*ptexBox)[i].pcutImg, cvScalar(255, 255, 255));


		cvSetImageROI((*ptexBox)[i].pcutImg, nRect);
		cvResize(pCut, (*ptexBox)[i].pcutImg);
		cvReleaseImage(&pCut);

		//cvShowImage("Cut", ptexBox[i].pcutImg);
		//break;
	}


	//===========================================//



	if (m_pImg){
		std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();

		// Set by User !
		float fTh = 0.7f;

		for (int i = 0; i < (*ptexBox).size(); i++){
			float fD = 0.0f;

			COLORf color;
			color.r = (float)(rand() % 100)*0.01f;
			color.g = (float)(rand() % 100)*0.01f;
			color.b = (float)(rand() % 100)*0.01f;
			color.a = 0.5f;


			for (int j = 0; j < (*ptexBox).size(); j++){

				if ((*ptexBox)[j].IsMatched == true) continue;

				CUT_INFO cutInfo;
				cutInfo.fileid = getHashCode((CStringA)m_pSelectcImg->GetPath());
				cutInfo.posid = (int)(*ptexBox)[i].textbox.x * 10000 + (int)(*ptexBox)[i].textbox.y;
				CString strId;
				strId.Format(L"%u%u", cutInfo.fileid, cutInfo.posid);
				cutInfo.id = getHashCode((CStringA)strId);
			//	cutInfo.th = fTh;
				cutInfo.th = 0;


				//		if (i == j) continue;
				fD = MatchingCutImgs((*ptexBox)[i].pcutImg, (*ptexBox)[j].pcutImg);
				if (fD > fTh){

					cv::Rect rect = (*ptexBox)[j].textbox;
					rect.x += m_cutRect.x1;
					rect.y += m_cutRect.y1;

					_MATCHInfo mInfo;
					mInfo.pImg = (*ptexBox)[j].pcutImg;
					mInfo.pos.x = rect.x + rect.width*0.5f;
					mInfo.pos.y = rect.y + rect.height*0.5f;
					mInfo.pos.z = 0;
					
					
				//	mInfo.accuracy = fD;
					mInfo.accuracy = (*ptexBox)[j].fConfi;
					mInfo.strCode = (*ptexBox)[j].strCode;
					//mInfo.strAccracy.Format(L"%d", (int)(fD * 100));

					mInfo.strAccracy.Format(L"%d", i);


					mInfo.rect.set(rect.x, rect.x + rect.width, rect.y, rect.y + rect.height);
					mInfo.searchId = i;
					mInfo.cInfo = cutInfo;

					//	mInfo.color = SINGLETON_TMat::GetInstance()->GetColor((fD)*1.1f);
					//	m_resColor.a = ((fD)*m_colorAccScale)*0.5f;
					mInfo.color = color;
					mInfo.IsAdded = false;
					m_pSelectcImg->AddMatchedPoint(mInfo, 0);

					//cvShowImage("Cut01", ptexBox[i].pcutImg);
					//cvShowImage("Cut02", ptexBox[j].pcutImg);
					//break;

					(*ptexBox)[j].IsMatched = true;
				}
			}
		}
	}
	pView->AddMatchResultList();
}

void CExtractView::DoFineExtractionText(_TEXT_ORDER order)
{
	std::vector<cv::Rect> textbox;
	cv::Mat img2;
	m_MatImg.copyTo(img2);

	//m_Extractor.ShrinkCharacter(img2);
	//ContractImage(img2);

	if (order == V_ORDER){
		int searchLength = m_guideLine[2].GetStartPnt().x - m_guideLine[3].GetStartPnt().x;
		if (searchLength < 0)
			searchLength *= -1;
		m_Extractor.ProcFineExtraction(img2, order, searchLength, searchLength);
	}
	else{  // H_ORDER
		int searchLength = m_guideLine[0].GetStartPnt().y - m_guideLine[1].GetStartPnt().y;
		if (searchLength < 0)
			searchLength *= -1;
		m_Extractor.ProcFineExtraction(img2, order, searchLength, searchLength);

	}
	img2.release();


	Render();
	return;


	IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 0);
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	for (int i = 0; i < ptexBox->size(); i++){

		if ((*ptexBox)[i].pcutImg != NULL){
			cvReleaseImage(&(*ptexBox)[i].pcutImg);
			(*ptexBox)[i].pcutImg = NULL;
		}

		cv::Rect rect = (*ptexBox)[i].textbox;
		rect.x += m_cutRect.x1;
		rect.y += m_cutRect.y1;


// Need to Check!!!!!!!!!!!!!!!
		m_Extractor.verifyCutSize(rect, src->width, src->height);
//==============================
		IplImage* pCut = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);		

		cvSetImageROI(src, cvRect(rect.x, rect.y, rect.width, rect.height));		// posx, posy = left - top
		cvCopy(src, pCut);

		(*ptexBox)[i].pcutImg = cvCreateImage(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H), pCut->depth, pCut->nChannels);
		cvResize(pCut, (*ptexBox)[i].pcutImg);
		cvReleaseImage(&pCut);

		//cvShowImage("Cut", ptexBox[i].pcutImg);
		//break;
	}















	//	CutNSearchExtractions();



	// 1. Set cutimage from extracted box for Matching between extracted chars //
	//if (m_pImg){
	//	IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 0);
	//	std::vector<_EXTRACT_BOX> ptexBox = m_Extractor.GetTextBoxes();
	//	for (int i = 0; i < ptexBox.size(); i++){

	//		if (ptexBox[i].pcutImg != NULL){
	//			cvReleaseImage(&ptexBox[i].pcutImg);
	//			ptexBox[i].pcutImg = NULL;
	//		}

	//		cv::Rect rect = ptexBox[i].textbox;
	//		rect.x += m_cutRect.x1;
	//		rect.y += m_cutRect.y1;

	//		IplImage* pCut = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);
	//		cvSetImageROI(src, cvRect(rect.x, rect.y, rect.width, rect.height));		// posx, posy = left - top
	//		cvCopy(src, pCut);

	//		ptexBox[i].pcutImg = cvCreateImage(cvSize(_NORMALIZE_SIZE, _NORMALIZE_SIZE), pCut->depth, pCut->nChannels);
	//		cvResize(pCut, ptexBox[i].pcutImg);
	//		cvReleaseImage(&pCut);

	//		//cvShowImage("Cut", ptexBox[i].pcutImg);
	//		//break;
	//	}
	//}

	Render();
}
void CExtractView::DoExtractionWord(_TEXT_ORDER order)
{
	std::vector<cv::Rect> textbox;
	cv::Mat img2;
	m_MatImg.copyTo(img2);



	//m_Extractor.ShrinkCharacter(img2);
	//ContractImage(img2);

	if (order == V_ORDER){
		int searchLength = m_guideLine[2].GetStartPnt().x - m_guideLine[3].GetStartPnt().x;
		if (searchLength < 0)
			searchLength *= -1;
		m_Extractor.ProcExtractionText(img2, order, searchLength, searchLength);
	}
	else{  // H_ORDER
		int searchLength = m_guideLine[0].GetStartPnt().y - m_guideLine[1].GetStartPnt().y;
		if (searchLength < 0)
			searchLength *= -1;
		m_Extractor.ProcExtractionText(img2, order, searchLength, searchLength);

	}
	img2.release();

	

	//IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 0);
	//std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	//for (int i = 0; i < ptexBox->size(); i++){

	//	if ((*ptexBox)[i].pcutImg != NULL){
	//		cvReleaseImage(&(*ptexBox)[i].pcutImg);
	//		(*ptexBox)[i].pcutImg = NULL;
	//	}

	//	cv::Rect rect = (*ptexBox)[i].textbox;
	//	rect.x += m_cutRect.x1;
	//	rect.y += m_cutRect.y1;


	//	// Need to Check!!!!!!!!!!!!!!!
	//	m_Extractor.verifyCutSize(rect, src->width, src->height);
	//	//==============================
	//	IplImage* pCut = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);

	//	cvSetImageROI(src, cvRect(rect.x, rect.y, rect.width, rect.height));		// posx, posy = left - top
	//	cvCopy(src, pCut);



	//	//m_pCropImg = cvCreateImage(cvSize(CANADA_SIZEW, CANADA_SIZEH), m_pSrcImg->depth, m_pSrcImg->nChannels);
	//	//cvSetImageROI(m_pSrcImg, cvRect(m_rectCrop.x1, m_rectCrop.y1, m_rectCrop.width, m_rectCrop.height));		// posx, posy = left - top
	//	////	cvCopy(m_pSrcImg, m_pCropImg);
	//	//cvResize(m_pSrcImg, m_pCropImg);
	//	//cvResetImageROI(m_pSrcImg);

	//	float fRatio = (float)(*ptexBox)[i].textbox.width / (float)(*ptexBox)[i].textbox.height;
	//	if (fRatio > 1.0f)
	//		fRatio = 1.0f;

	//	(*ptexBox)[i].pcutImg = cvCreateImage(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H), pCut->depth, pCut->nChannels);
	//	cvSetImageROI((*ptexBox)[i].pcutImg, cvRect(0, 0, _NORMALIZE_SIZE_H*fRatio, _NORMALIZE_SIZE_H));
	//	cvResize(pCut, (*ptexBox)[i].pcutImg);
	//	cvReleaseImage(&pCut);

	//	//cvShowImage("Cut", ptexBox[i].pcutImg);
	//	//break;
	//}

	

	// 1. Set cutimage from extracted box for Matching between extracted chars //
	//if (m_pImg){
	//	IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 0);
	//	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	//	for (int i = 0; i < ptexBox.size(); i++){

	//		if (ptexBox[i].pcutImg != NULL){
	//			cvReleaseImage(&ptexBox[i].pcutImg);
	//			ptexBox[i].pcutImg = NULL;
	//		}

	//		cv::Rect rect = ptexBox[i].textbox;
	//		rect.x += m_cutRect.x1;
	//		rect.y += m_cutRect.y1;

	//		IplImage* pCut = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);
	//		cvSetImageROI(src, cvRect(rect.x, rect.y, rect.width, rect.height));		// posx, posy = left - top
	//		cvCopy(src, pCut);

	//		ptexBox[i].pcutImg = cvCreateImage(cvSize(_NORMALIZE_SIZE, _NORMALIZE_SIZE), pCut->depth, pCut->nChannels);
	//		cvResize(pCut, ptexBox[i].pcutImg);
	//		cvReleaseImage(&pCut);

	//		//cvShowImage("Cut", ptexBox[i].pcutImg);
	//		//break;
	//	}
	//}

	Render();
}


void CExtractView::DoExtractionLine(_TEXT_ORDER order)
{
	std::vector<cv::Rect> textbox;
	cv::Mat img2;
	m_MatImg.copyTo(img2);

	//m_Extractor.ShrinkCharacter(img2);
	//ContractImage(img2);

	if (order == V_ORDER){
		int searchLength = m_guideLine[2].GetStartPnt().x - m_guideLine[3].GetStartPnt().x;
		if (searchLength < 0)
			searchLength *= -1;
		m_Extractor.ProcExtractionLine(img2, order, searchLength, 0);
	}
	else{  // H_ORDER
		int searchLength = m_guideLine[0].GetStartPnt().y - m_guideLine[1].GetStartPnt().y;
		if (searchLength < 0)
			searchLength *= -1;
		m_Extractor.ProcExtractionLine(img2, order, 0, searchLength);

	}		
	img2.release();
	Render();		
}

void CExtractView::IDragMap(int x, int y, short sFlag)
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

void CExtractView::OnMouseMove(UINT nFlags, CPoint point)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	// TODO: Add your message handler code here and/or call default
	if (m_bUserEditMode != __EDIT_NONE){
		if (GetCapture()){
			m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectEnd);
			Render();
		}
	}
	else{
		if (GetCapture()){
			if ((point.x > 0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight)){

				if (m_mouseMode == _CAM_MOVE){		// MOVE
					int xDelta = point.x - m_stratPnt.x;
					int yDelta = point.y - m_stratPnt.y;

					if (xDelta*xDelta > yDelta*yDelta){
						m_moveVec.x = 1.0f;			m_moveVec.y = 0;
					}
					else{
						m_moveVec.y = 1.0f;			m_moveVec.x = 0;
					}
					if ((point.x > 0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight)){
						IDragMap(point.x, point.y, 1);
					}
				}
				else if (m_mouseMode == _ARROW_MOVE){
					if (m_pImg){
						POINT3D currArrowPos = m_cameraPri.ScreenToWorld(point.x, point.y);
						float xDelta = currArrowPos.x - m_startArrowPos.x;
						float yDelta = currArrowPos.y - m_startArrowPos.y;

						if ((point.x > 0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight)){
							switch (m_selGuideLineId)
							{
							case 0:
							case 1:
								m_guideLine[m_selGuideLineId].SetIncrement(0.0f, yDelta / m_pImg->GetfYScale());
								break;
							case 2:
							case 3:
								m_guideLine[m_selGuideLineId].SetIncrement(xDelta / m_pImg->GetfXScale(), 0.0f);
								break;
							default:
								break;
							}
						}

						m_startArrowPos = currArrowPos;
					}
				}
				Render();
			}
		}
	}

	COGLWnd::OnMouseMove(nFlags, point);
}


void CExtractView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	m_CNSRectStart = m_CNSRectEnd;
	if (m_bUserEditMode != __EDIT_NONE){		
		m_stratPnt = point;
		m_cameraPri.InsetsectRayToPlane(m_PN, m_PO, point.x, point.y, m_CNSRectStart);
		m_CNSRectEnd = m_CNSRectStart;
		SetCapture();
	}
	else{
		if (SelectObject3D(point.x, point.y, 1, 1, 0) == 0){
			m_stratPnt = point;
			m_mouseMode = _CAM_MOVE;
		}
		else{
			m_startArrowPos = m_cameraPri.ScreenToWorld(point.x, point.y);
			m_mouseMode = _ARROW_MOVE;
		}
		IDragMap(point.x, point.y, 0);
		SetCapture();
	}	
	

	Render();

	COGLWnd::OnLButtonDown(nFlags, point);
}


void CExtractView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	

	if (m_bUserEditMode != __EDIT_NONE){
		int x, y, width, height;
				
		width = point.x - m_stratPnt.x;
		x = m_stratPnt.x + width*0.5f;
		if (m_stratPnt.x > point.x){			
			width = m_stratPnt.x - point.x;
			x = point.x + width*0.5f;
		}
		
		height = point.y - m_stratPnt.y;
		y = m_stratPnt.y + height*0.5f;
		if (m_stratPnt.y > point.y){
			height = m_stratPnt.y - point.y;
			y = point.y + height*0.5f;
		}		

		if (width == 0)	width = 1;
		if (height == 0)	height = 1;

		if (m_bUserEditMode == __EDIT_SELECTION){
			m_Extractor.InitSelectionOfExtractBoxs();
			m_Extractor.InitSelectionOfExtractLines();
			SelectObject3D(x, y, width, height, 1);
		}

		else if (m_bUserEditMode == __EDIT_ADDLINE){
			RECT2D selRect = m_pImg->ConvertVec3DtoImgateCoord(m_CNSRectStart, m_CNSRectEnd);

			cv::Rect rect;
			rect.x = selRect.x1;
			rect.y = selRect.y1;
			rect.width = selRect.width;
			rect.height = selRect.height;

			m_Extractor.AddExtLine(rect);
		}

		else if (m_bUserEditMode == __EDIT_ADDCHAR){
			RECT2D selRect = m_pImg->ConvertVec3DtoImgateCoord(m_CNSRectStart, m_CNSRectEnd);

			cv::Rect rect;
			rect.x = selRect.x1;
			rect.y = selRect.y1;
			rect.width = selRect.width;
			rect.height = selRect.height;

			m_Extractor.AddExtBox (rect);
		}


		
	}
	else{
		m_mouseMode = _CAM_MOVE;
		IDragMap(point.x, point.y, 2);
		
	}


	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
	m_CNSRectStart = m_CNSRectEnd;

	ReleaseCapture();
	Render();

	COGLWnd::OnLButtonUp(nFlags, point);
}


void CExtractView::InsertExtrationIntoDB()
{
	//CString strFile;

	if (m_pImg){
		IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 1);
		cv::Mat img = cv::cvarrToMat(src);

		std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
		int addcnt = 0;
		int x1, x2, y1, y2;
		for (int i = 0; i < ptexBox->size(); i++){

			for (int j = 0; j < ptexBox->size(); j++){

				if (i == j) continue;

				if (MatchingCutImgs((*ptexBox)[i].pcutImg, (*ptexBox)[j].pcutImg) > 0.75f){
					break;
				}

			}



			//cv::rectangle(img, ptexBox[i].textbox, cv::Scalar(0, 255, 0), 1, 8, 0);
			//cv::Mat crop = img(ptexBox[i].textbox);

			//x1 = ptexBox[i].textbox.x + m_cutRect.x1;
			//x2 = ptexBox[i].textbox.x + ptexBox[i].textbox.width+m_cutRect.x1;
			//y1 = ptexBox[i].textbox.y + m_cutRect.y1;
			//y2 = ptexBox[i].textbox.y + ptexBox[i].textbox.height + m_cutRect.y1;

			//if (SINGLETON_TMat::GetInstance()->InsertIntoLogDB(crop, x1, x2, y1, y2, m_pImg->GetCode()) == false){
			//	addcnt++;
			//}

			//strFile.Format(L"C:/FGBD_project/Books/log/%d_%d.bmp", pPage->GetCode(), i);
			//char* sz = T2A(strFile);
			//cv::imwrite(sz, crop);
			//	imshow("before deskew img ", crop);
		}
		cv::imshow("extractionWithOCR", img);
	}

}



float CExtractView::MatchingCutImgs(IplImage* pCut, IplImage* dst)
{
	float fAccur = 0.0f;
	bool IsOk = true;

	//if ((pCut->width != _NORMALIZE_SIZE) || (pCut->height != _NORMALIZE_SIZE) || (dst->width != _NORMALIZE_SIZE) || (dst->height != _NORMALIZE_SIZE)){
	//	IsOk = false;
	//}

	
	//if (IsOk){
	//	short newWidth = dst->width + _NORMALIZE_SIZE_W;
	//	short newHeight = dst->height + _NORMALIZE_SIZE_H;
	//	IplImage* src = cvCreateImage(cvSize(newWidth, newHeight), dst->depth, dst->nChannels);
	//	cvSet(src, cvScalar(255));

	//	CvRect rect;
	//	rect.x = newWidth / 2 - dst->width / 2;
	//	rect.y = newHeight / 2 - dst->height / 2;
	//	rect.width = dst->width;
	//	rect.height = dst->height;

	//	cvSetImageROI(src, rect);
	//	cvResize(dst, src);
	//	cvResetImageROI(src);



	//	IplImage *result_img = 0;
	//	result_img = cvCreateImage(cvSize(src->width - pCut->width + 1, src->height - pCut->height + 1), IPL_DEPTH_32F, 1);
	////	result_img = cvCreateImage(cvSize(1,1), IPL_DEPTH_32F, 1);
	//	cvMatchTemplate(src, pCut, result_img, CV_TM_CCOEFF_NORMED);

	//	float sTh = 0.0f;
	//	float* d = (float*)result_img->imageData;
	//	for (int y = 0; y < result_img->height; y++){
	//		for (int x = 0; x < result_img->width; x++){
	//			float fD = *(d + y*result_img->width + x);
	//			if (fD > sTh)	{
	//				sTh = fD;
	//				fAccur = fD;
	//			}
	//		}
	//	}

	//	cvReleaseImage(&src);
	//	cvReleaseImage(&result_img);
	//}


	return 1.0f;
//	return fAccur;

}


int CExtractView::SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode)
{
	m_selGuideLineId = -1;

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
	gluPickMatrix(x, viewport[3] - y, rect_width, rect_height, viewport);
	gluPerspective(m_cameraPri.m_Cntfovy, (float)viewport[2] / (float)viewport[3], m_cameraPri.GetNearPlane(), m_cameraPri.GetFarPlane());

	//m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	glMatrixMode(GL_MODELVIEW);
	

	if (selmode == 0){
		glPushMatrix();
		glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
		glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);
		for (int i = 0; i < 4; i++){
			glPushName(i);
			m_guideLine[i].DrawButtions(1.0f, 1.0f, 1.0f);
			glPopName();
		}
		glPopMatrix();

		hits = glRenderMode(GL_RENDER);
		if (hits > 0)
		{
			m_selGuideLineId = selectBuff[3];
		}
	}
	else if (selmode == 1){		// Extracted Item selection //
		DrawExtractionsForPick();
		hits = glRenderMode(GL_RENDER);
		if (hits > 0)
		{
			int selid = 0;
			for (int i = 0; i < hits; i++){
				selid = (int)selectBuff[i * 4 + 3];

				if (selid >= 1000){		// Text				
					m_Extractor.AddSelectionOfExtractBoxs(selid - 1000);
				}

				if (selid >= 10000){		// Line
					m_Extractor.AddSelectionOfExtractLines(selid - 10000);
				}

			}
		}
	}


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return hits;
}

void CExtractView::SetLanguageType(_LANGUAGE_TYPE ltype)
{
	m_Extractor.SetLanguageType(ltype);
}

void CExtractView::SetVertiCharSpace(int _v)
{
	m_Extractor.SetVertiCharSpace(_v);
}
void CExtractView::SetHoriCharSpze(int _h)
{
	m_Extractor.SetHoriCharSpze(_h);
}

bool CExtractView::IsSelectedRect(int _id)
{
	return false;
}


void CExtractView::DrawCNSRect(float r, float g, float b, float a)
{
	glLineWidth(2);
	glDisable(GL_DEPTH_TEST);
	glColor4f(r, g, b, a);
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_CNSRectStart.x, m_CNSRectStart.y, m_CNSRectStart.z);
	glVertex3f(m_CNSRectStart.x, m_CNSRectEnd.y, m_CNSRectEnd.z);
	glVertex3f(m_CNSRectEnd.x, m_CNSRectEnd.y, m_CNSRectEnd.z);
	glVertex3f(m_CNSRectEnd.x, m_CNSRectStart.y, m_CNSRectStart.z);
	glVertex3f(m_CNSRectStart.x, m_CNSRectStart.y, m_CNSRectStart.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1);
}

void CExtractView::DrawExtractedLine()
{

	glLineWidth(2);
	glPushMatrix();
	glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
	glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

	std::vector<_EXTRACT_BOX>* pLine = m_Extractor.GetLineBoxes();
	if (pLine->size() > 0){
		// Draw detected position //
		for (int i = 0; i < pLine->size(); i++){
			RECT2D rect;
			rect.set(pLine->at(i).textbox.x, pLine->at(i).textbox.x + pLine->at(i).textbox.width, pLine->at(i).textbox.y, pLine->at(i).textbox.y + pLine->at(i).textbox.height);

			if ((*pLine)[i].IsSelected == false){
				glColor4f(1.0f, 0.5f, 0.0f, 0.2f);
				glBegin(GL_QUADS);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

				glColor4f(1.0f, 0.5f, 0.0f, 0.9f);
				glBegin(GL_LINE_STRIP);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();
			}
			else{
				glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
				glBegin(GL_QUADS);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();


				glColor4f(1.0f, 0.0f, 0.0f, 0.99f);
				glLineStipple(2, 0xAAAA);
				glEnable(GL_LINE_STIPPLE);

				glBegin(GL_LINE_STRIP);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

				glDisable(GL_LINE_STIPPLE);
			}
		}
	}
	glPopMatrix();

	glLineWidth(1);
}


void CExtractView::DrawExtractedLineForPick()
{


		glPushMatrix();
		glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
		glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

		std::vector<_EXTRACT_BOX>* pLine = m_Extractor.GetLineBoxes();
		if (pLine->size() > 0){
			// Draw detected position //
			for (int i = 0; i < pLine->size(); i++){
				RECT2D rect;
				rect.set(pLine->at(i).textbox.x, pLine->at(i).textbox.x + pLine->at(i).textbox.width, pLine->at(i).textbox.y, pLine->at(i).textbox.y + pLine->at(i).textbox.height);

				glPushName(i + 10000);
				glColor4f(0.0f, 0.0f, 1.0f, 0.2f);
				glBegin(GL_QUADS);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();
				glPopName();
			}
		}
		glPopMatrix();

}


void CExtractView::DeleteExtSelections()
{
	m_Extractor.DelSelectionOfExtractLines();
	m_Extractor.DelSelectionOfExtractBoxs();

	Render();
}

BOOL CExtractView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if ((m_bUserEditMode==__EDIT_NONE) || (m_bUserEditMode==__EDIT_SELECTION)){
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		//	mtSetPoint3D(&m_CNSRectEnd, 0.0f, 0.0f, MAX_CAM_HIGHTLEVEL * 3);
		//	m_CNSRectStart = m_CNSRectEnd;
		return TRUE;
	}
	else{		
		SetCursor(LoadCursor(NULL, IDC_CROSS));
		return TRUE;
	}
	return COGLWnd::OnSetCursor(pWnd, nHitTest, message);
}





void CExtractView::DoExtractionTextFromLines(_TEXT_ORDER order)
{
	BeginWaitCursor();

	cv::Mat img2(m_MatImg.size(), m_MatImg.type());
	//Apply thresholding
	cv::threshold(m_MatImg, img2, 200, 255, cv::THRESH_BINARY);

	cv::Mat img3;
	img2.copyTo(img3);
	cv::imshow("BeforeBinary1", img3);



	int averArea = 0;
	int areaCnt = 0;
	float fEng = 0, fChi = 0;
	
	std::vector<_OCR_RES> boundRect;
	std::vector<_EXTRACT_BOX>* vecLine = m_Extractor.GetLineBoxes();

	for (int j = 0; j < vecLine->size(); j++){
		

		cv::Rect lineRect = (*vecLine)[j].textbox;

		int nWidth = (*vecLine)[j].textbox.width;
		int nHeight = (*vecLine)[j].textbox.height * 3;

		cv::Rect imgRect;
		imgRect.x = 0;
		imgRect.y = (*vecLine)[j].textbox.height;
		imgRect.width = (*vecLine)[j].textbox.width;
		imgRect.height = (*vecLine)[j].textbox.height;


		cv::Mat imgLine(nHeight, nWidth, img2.type(), cvScalar(255));
		img2(lineRect).copyTo(imgLine(imgRect));


		//1.  English extraction //
		boundRect.clear();
		fEng = m_OCRMng.extractWithOCR(imgLine, boundRect, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL);
		for (int i = 0; i < boundRect.size(); i++){

			if (boundRect[i].fConfidence > 0.85){
				// remove extreacted characters //
				//m_Extractor.verifyCutSize(boundRect[i].rect, imgLine.cols, imgLine.rows);
				//imgLine(boundRect[i].rect).setTo(cv::Scalar(255));
				

				boundRect[i].rect.x += lineRect.x;
				boundRect[i].rect.y += (lineRect.y - lineRect.height);			
				boundRect[i].type = 0;
				m_Extractor.AddExtBox(boundRect[i]);
				averArea += (boundRect[i].rect.area());
				areaCnt++;

				m_Extractor.verifyCutSize(boundRect[i].rect, img3.cols, img3.rows);
				img3(boundRect[i].rect).setTo(cv::Scalar(255));
			}
		}

		img3(lineRect).copyTo(imgLine(imgRect));

		cv::imshow("AfterBinary2", img3);
		//2. Chinese Charactes ============//
		boundRect.clear();
		fChi = m_OCRMng.extractWithOCR(imgLine, boundRect, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);
		for (int i = 0; i < boundRect.size(); i++){
			if (boundRect[i].fConfidence > 0.80){

				//m_Extractor.verifyCutSize(boundRect[i].rect, imgLine.cols, imgLine.rows);
				//imgLine(boundRect[i].rect).setTo(cv::Scalar(255));

				boundRect[i].rect.x += lineRect.x;
				boundRect[i].rect.y += (lineRect.y - lineRect.height);

				boundRect[i].type = 1;
				m_Extractor.AddExtBox(boundRect[i]);
				averArea += (boundRect[i].rect.area());
				areaCnt++;

				m_Extractor.verifyCutSize(boundRect[i].rect, img3.cols, img3.rows);
				img3(boundRect[i].rect).setTo(cv::Scalar(255));
			}
		}

		cv::imshow("AfterBinary3", img3);

		//	cv::imshow("AfterBinary3", img2);
		//boundRect.clear();
		//fEng = m_OCRMng.extractWithOCR(imgLine, boundRect, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL);
		//for (int i = 0; i < boundRect.size(); i++){

		//	if (boundRect[i].fConfidence > 60){
		//		imgLine(boundRect[i].rect).setTo(cv::Scalar(255));

		//		boundRect[i].rect.x += lineRect.x;
		//		boundRect[i].rect.y += (lineRect.y - lineRect.height);
		//		m_Extractor.AddExtBox(boundRect[i]);
		//		averArea += (boundRect[i].rect.area());
		//		areaCnt++;
		//	}
		//}

		// Error tests==========================get word box================================//
		//boundRect.clear();
		//fEng = m_OCRMng.extractWithOCR(imgLine, boundRect, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL);
		//for (int i = 0; i < boundRect.size(); i++){
		//		boundRect[i].rect.x += lineRect.x;
		//		boundRect[i].rect.y += (lineRect.y - lineRect.height);

		//		boundRect[i].type = 0;
		//		m_Extractor.AddExtBox(boundRect[i]);
		//		averArea += (boundRect[i].rect.area());
		//		areaCnt++;
		//}
		//===============================================================================//


		imgLine.release();
	}
	


	/*
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	std::vector<_OCR_RES> ocrTextBox;
//	for (int i = 0; i < ptexBox->size(); i++){
//	for (int i = ptexBox->size()-1 ; i >=0; i--){

//		cv::Rect rect = (*ptexBox)[i].textbox;
		//rect.x += m_cutRect.x1;
		//rect.y += m_cutRect.y1;
		
		// Need to Check!!!!!!!!!!!!!!!
		//cv::Mat imgCut;

		//rect.x -= 2;
		//rect.width += 4;
		//rect.y -= 4;
		//rect.height += 4;


		//m_Extractor.verifyCutSize(rect, m_MatImg.cols, m_MatImg.rows);
		//m_MatImg(rect).copyTo(imgCut);

				
		float fEng = 0, fChi = 0;
		std::vector<_OCR_RES> boundRectEng;
		fEng = m_OCRMng.extractWithOCR(img2, boundRectEng, m_OCRMng.GetEngTess());
		//std::vector<_OCR_RES> boundRectChi;
		//fChi = m_OCRMng.extractWithOCR(imgCut, boundRectChi, m_OCRMng.GetChiTess());

		bool IsAdd = false;
		if (fEng > fChi){
			for (int i = 0; i < boundRectEng.size(); i++){

				//boundRectEng[i].rect.x += rect.x;
				//boundRectEng[i].rect.y += rect.y;

				ocrTextBox.push_back(boundRectEng[i]);
				IsAdd = true;
			}
		}

		else{
			//for (int i = 0; i < boundRectChi.size(); i++){

			//	boundRectChi[i].rect.x += rect.x;
			//	boundRectChi[i].rect.y += rect.y;
			//	ocrTextBox.push_back(boundRectChi[i]);

			//	IsAdd = true;
			//}
		}
	//	imgCut.release();


		//if (IsAdd == false){
		//	_OCR_RES tmp;
		//	tmp.rect = (*ptexBox)[i].textbox;
		//	tmp.fConfidence = 0.0f;
		//	tmp.strCode = "";
		//	ocrTextBox.push_back(tmp);
		//}
//	}
*/


//	m_Extractor.ClearExtractResult();
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	if (areaCnt>0){
		averArea /= areaCnt;
	}

	// Make Cut image for List Ctrl ===================================//

//	IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(m_pImg->GetPath(), 0);
////	ptexBox = m_Extractor.GetTextBoxes();
//	for (int i = 0; i < ptexBox->size(); i++){
//
//		if ((*ptexBox)[i].pcutImg != NULL){
//			cvReleaseImage(&(*ptexBox)[i].pcutImg);
//			(*ptexBox)[i].pcutImg = NULL;
//		}
//		
//		cv::Rect rect = (*ptexBox)[i].textbox;
//		rect.x += m_cutRect.x1;
//		rect.y += m_cutRect.y1;
//
//		if ((rect.width < 3 ) || (rect.height < 3)) continue;
//
//		// Need to Check!!!!!!!!!!!!!!!
//		m_Extractor.verifyCutSize(rect, src->width, src->height);
//		//==============================
//		IplImage* pCut = cvCreateImage(cvSize(rect.width, rect.height), src->depth, src->nChannels);
//
//		cvSetImageROI(src, cvRect(rect.x, rect.y, rect.width, rect.height));		// posx, posy = left - top
//		cvCopy(src, pCut);
//			
//
//		// Normalize //
//	//	cv::Rect nRect = GetNomalizedSize(averArea, (*ptexBox)[i].textbox);
//		cv::Rect nRect = GetNomalizedWordSize((*ptexBox)[i].textbox);
//
//		
//		(*ptexBox)[i].pcutImg = cvCreateImage(cvSize(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H), pCut->depth, pCut->nChannels);
//		cvSetImageROI((*ptexBox)[i].pcutImg, nRect);
//		cvResize(pCut, (*ptexBox)[i].pcutImg);
//		cvReleaseImage(&pCut);
//
//		//cvShowImage("Cut", ptexBox[i].pcutImg);
//		//break;
//	}

	// For Test //
	//FILE* fp = 0;
	//fopen_s(&fp, "D:/temp.txt", "w");
	//if (fp){
	//	for (int i = 0; i < ptexBox->size(); i++){

	//		wchar_t *wchar_str;
	//		char *char_str;
	//		int char_str_len;

	//		// 1. CString to wchar * conversion
	//		wchar_str = (*ptexBox)[i].strCode.GetBuffer((*ptexBox)[i].strCode.GetLength());
	//		char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	//		char_str = new char[char_str_len];

	//		// 2. wchar_t* to char* conversion
	//		WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0, 0);

	//		int len = strlen(char_str) + 1;
	//		fwrite(char_str, len, 1, fp);

	//		delete[] char_str;
	//	}
	//	fclose(fp);
	//}

	//::ShellExecute(NULL, L"open", L"notepad", L"D:/temp.txt", NULL, SW_SHOW);

	EndWaitCursor();
	Render();
}


cv::Rect CExtractView::GetNomalizedWordSize(cv::Rect rect)
{
	cv::Rect norRect;
	norRect.x = 0;
	norRect.y = 0;

	float fScale = (float)_NORMALIZE_SIZE_H / (float)rect.height;
	norRect.width = rect.width*fScale;
	norRect.height = rect.height*fScale;


	// Checkl Aspect Ratio //
	float aRatio = (float)rect.width / (float)rect.height;

	if (aRatio < 0.5f){ 
		norRect.width = _NORMALIZE_SIZE_H / 2;
		norRect.x = norRect.width / 2;

		norRect.height = _NORMALIZE_SIZE_H;
	}
	else if (aRatio > 1.5f){
		norRect.height = _NORMALIZE_SIZE_H / 2;
		norRect.y = norRect.height / 2;

		norRect.width = _NORMALIZE_SIZE_H;
	}


	else{
		int wcnt = (norRect.width / _NORMALIZE_SIZE_H);
		if (wcnt < 1)			wcnt = 1;
		if (wcnt > 10)			wcnt = 10;


		norRect.width = _NORMALIZE_SIZE_H*wcnt;

		if (rect.height > _NORMALIZE_SIZE_H){
			norRect.height = _NORMALIZE_SIZE_H;
		}
	}

	
	return norRect;


	//cv::Rect norRect;
	//norRect.x = 0;
	//norRect.y = 0;

	//float fScale = (float)_NORMALIZE_SIZE_H / (float)rect.height;
	//norRect.width = rect.width*fScale;
	//norRect.height = rect.height*fScale;

	//if (norRect.width < _NORMALIZE_SIZE_H){
	//	norRect.width = _NORMALIZE_SIZE_H;
	//}

	//if (rect.width > _NORMALIZE_SIZE_W){
	//	norRect.width = _NORMALIZE_SIZE_W;
	//}

	//if (rect.height > _NORMALIZE_SIZE_H){
	//	norRect.height = _NORMALIZE_SIZE_H;
	//}

	//return norRect;
}

cv::Rect CExtractView::GetNomalizedSize(int averArea, cv::Rect rect)
{
	cv::Rect norRect;

	float fRatio = (float)rect.width / (float)rect.height;
	float fArea = (float)rect.width * (float)rect.height;

	if (rect.width > _NORMALIZE_SIZE_W){
		norRect.width = _NORMALIZE_SIZE_W;
		norRect.x = 0;
	}
	else{
		if (fArea > averArea*0.2f){
			if (fRatio > 1.0f){
				norRect.width = _NORMALIZE_SIZE_W;
				norRect.x = 0;
			}
			else{
				norRect.width = (float)_NORMALIZE_SIZE_W*fRatio;
				norRect.x = (_NORMALIZE_SIZE_W - norRect.width) / 2;
			}
		}
		else{
			norRect.width = _NORMALIZE_SIZE_W/2;
			norRect.x = (_NORMALIZE_SIZE_W - norRect.width) / 2;
		}
	}


	if (rect.height > _NORMALIZE_SIZE_H){
		norRect.height = _NORMALIZE_SIZE_W;
		norRect.y = 0;
	}
	else{
		if (fArea > averArea*0.2f){
			if (fRatio < 1.0f){
				norRect.height = _NORMALIZE_SIZE_H;
				norRect.y = 0;
			}
			else{
				norRect.height = (float)_NORMALIZE_SIZE_H/fRatio;
				norRect.y = (_NORMALIZE_SIZE_H - norRect.width) / 2;
			}
		}
		else{
			norRect.height = _NORMALIZE_SIZE_H / 2;
			norRect.y = (_NORMALIZE_SIZE_H - norRect.height) / 2;
		}
	}

	return norRect;
}



void CExtractView::DoExtractionTextFromTexts(_TEXT_ORDER order)
{
	BeginWaitCursor();

	cv::Mat img2(m_MatImg.size(), m_MatImg.type());
	//Apply thresholding
	cv::threshold(m_MatImg, img2, 200, 255, cv::THRESH_BINARY);
	//	cv::imshow("BeforeBinary1", img2);


	int averArea = 0;
	int areaCnt = 0;
	float fEng = 0, fChi = 0;

	std::vector<_OCR_RES> boundRectEng;
	std::vector<_OCR_RES> boundRectChi;
//	std::vector<_EXTRACT_BOX> pTextBox = (*m_Extractor.GetTextBoxes());
//	(*m_Extractor.GetTextBoxes()) = std::vector<_EXTRACT_BOX>();
	std::vector<_EXTRACT_BOX> pTextBox = (*m_Extractor.GetLineBoxes());
	

	for (int j = 0; j < pTextBox.size(); j++){

		cv::Rect lineRect = pTextBox[j].textbox;

		//float fAratio = (float)(pTextBox)[j].textbox.width / (float)(pTextBox)[j].textbox.height;
		//float fScale = 32.0f / (float)(pTextBox)[j].textbox.height;



		int nWidth = (pTextBox)[j].textbox.width;
		int nHeight = (pTextBox)[j].textbox.height*3;

		cv::Rect imgRect;
		imgRect.x = 0;
		imgRect.y = (pTextBox)[j].textbox.height;
		imgRect.width = (pTextBox)[j].textbox.width;
		imgRect.height = (pTextBox)[j].textbox.height;


		cv::Mat imgLine(nHeight, nWidth, img2.type(), cvScalar(255));
		img2(lineRect).copyTo(imgLine(imgRect));

		//cv::imshow("Cut Text", imgLine);
		//break;

		//1.  English extraction //
		boundRectEng.clear();
		boundRectChi.clear();
		fEng = m_OCRMng.extractWithOCR(imgLine, boundRectEng, m_OCRMng.GetEngTess(), tesseract::RIL_SYMBOL);
		fChi = m_OCRMng.extractWithOCR(imgLine, boundRectChi, m_OCRMng.GetChiTess(), tesseract::RIL_SYMBOL);
		

		for (int i = 0; i < boundRectEng.size(); i++){
			if (boundRectEng[i].fConfidence > boundRectChi[i].fConfidence){
				boundRectEng[i].rect.x += lineRect.x;
				boundRectEng[i].rect.y += (lineRect.y - lineRect.height);
				boundRectEng[i].type = 0;
				m_Extractor.AddExtBox(boundRectEng[i]);
				averArea += (boundRectEng[i].rect.area());
				areaCnt++;
			}
			else{
				boundRectChi[i].rect.x += lineRect.x;
				boundRectChi[i].rect.y += (lineRect.y - lineRect.height);
				boundRectEng[i].type = 0;
				m_Extractor.AddExtBox(boundRectChi[i]);
				averArea += (boundRectChi[i].rect.area());
				areaCnt++;
			}
		}


		imgLine.release();
	}


	//	m_Extractor.ClearExtractResult();
	std::vector<_EXTRACT_BOX>* ptexBox = m_Extractor.GetTextBoxes();
	if (areaCnt>0){
		averArea /= areaCnt;
	}

	EndWaitCursor();
	Render();
}