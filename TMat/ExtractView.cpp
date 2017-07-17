#include "stdafx.h"
#include "ExtractView.h"
#include "ZDataManager.h"


CExtractView::CExtractView()
{
	m_pImg = NULL;
	m_eTexId = 0;
	m_MatImg = NULL;
}


CExtractView::~CExtractView()
{

}


void CExtractView::InitGLview(int _nWidth, int _nHeight)
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(5000 * 2);
	m_cameraPri.InitializeCamera(45.0f, 0, 0, m_lookAt, _nWidth, _nHeight);

	

	glInitNames();

//	SetTimer(200, 50, NULL);
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
void CExtractView::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.1f, 0.2f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (m_pImg){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_eTexId);
		m_pImg->DrawWithoutTexID(1.0f);
		glDisable(GL_TEXTURE_2D);

		
		glLineWidth(1);
		std::vector<_EXTRACT_BOX> pLine = m_Extractor.GetLineBoxes ();
		if (pLine.size() > 0){
			// Draw detected position //
			glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
			glPushMatrix();
			glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
			glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

			for (int i = 0; i < pLine.size(); i++){
				RECT2D rect;
				rect.set(pLine[i].textbox.x, pLine[i].textbox.x + pLine[i].textbox.width, pLine[i].textbox.y, pLine[i].textbox.y + pLine[i].textbox.height);

				glColor4f(0.0f, 0.0f, 1.0f, 0.3f);
				glBegin(GL_QUADS);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

				glColor4f(0.0f, 0.0f, 1.0f, 0.99f);
				glBegin(GL_LINE_STRIP);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();
			}
			glPopMatrix();
		}


		glLineWidth(2);
		std::vector<_EXTRACT_BOX> ptexBox = m_Extractor.GetTextBoxes();
		if (ptexBox.size() > 0){
			// Draw detected position //
			glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
			glPushMatrix();
			glScalef(m_pImg->GetfXScale(), m_pImg->GetfYScale(), 1.0f);
			glTranslatef(-m_pImg->GetImgWidth()*0.5f, -m_pImg->GetImgHeight()*0.5f, 0.0f);

			//if (m_bIsNear){		
			
			for (int i = 0; i < ptexBox.size(); i++){

				RECT2D rect;
				rect.set(ptexBox[i].textbox.x, ptexBox[i].textbox.x + ptexBox[i].textbox.width,
					ptexBox[i].textbox.y, ptexBox[i].textbox.y+ptexBox[i].textbox.height);
				
				if (ptexBox[i].IsAmbig)					glColor4f(1.0f, 0.0f, 0.0f, 0.99f);
				else									glColor4f(0.0f, 1.0f, 0.0f, 0.99f);

				glBegin(GL_LINE_STRIP);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y2, 0.0f);
				glVertex3f(rect.x2, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glVertex3f(rect.x1, m_pImg->GetImgHeight() - rect.y1, 0.0f);
				glEnd();

			}			
			glPopMatrix();
		}
	}

	DrawSearchRect();
	glLineWidth(1);


	SwapBuffers(m_CDCPtr->GetSafeHdc());

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

	m_posSearchBox = m_cameraPri.ScreenToWorld(0, m_rectHeight);
}


void CExtractView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 100){
		Render();
	}

	COGLWnd::OnTimer(nIDEvent);
}

void CExtractView::SetExtractImage(CZPageObject* pImg, RECT2D cutRect)
{ 
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

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
	m_pImg->SetName(L"", L"", L"", 0, 0);
	POINT3D pos;
	mtSetPoint3D(&pos, 0.0f, 0.0f, 0.0f);
	m_pImg->SetPosition(pos);





	IplImage* pCut = NULL;
	m_strImgPath = pImg->GetPath();
	

	USES_CONVERSION;
	char* sz = T2A(pImg->GetPath());
	IplImage *pimg = NULL;

	CString str = PathFindExtension(pImg->GetPath());
	if ((str == L".pdf") || (str == L".jpg") || (str == L".JPG") || (str == L".jpeg")){

		pimg = SINGLETON_TMat::GetInstance()->LoadPDFImage(pImg->GetPath(), 4);

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

	//m_pImg->m_fImgCols = pCut->width;
	//m_pImg->m_fImgRows = pCut->height;
	
	m_MatImg.release();

	
	m_MatImg = cv::cvarrToMat(pCut);
	cvtColor(m_MatImg, m_MatImg, CV_BGR2GRAY);
	

	cvReleaseImage(&pimg);
	cvReleaseImage(&pCut);


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
	}
		//}
//	cv::imshow("After-binaryMat", img);
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
	
	ContractImage(img2);

	//cv::imshow("ContractImage", img2);

	m_Extractor.getContours(img2, textbox);
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



void CExtractView::ExtractLines(_TEXT_ORDER order)
{
	std::vector<cv::Rect> textbox;
	cv::Mat img2;
	m_MatImg.copyTo(img2);

//	ContractImage(img2);

	m_Extractor.ProcExtraction(img2, order);
	img2.release();

	Render();
}