#include "stdafx.h"
#include "ZDataManager.h"
#include "DragDropTreeCtrl.h"
#include <thread>

#include "Extractor.h"


UINT Threadproc(LPVOID param);

UINT Threadproc(LPVOID param)
{
	THREADINFO *temp = (THREADINFO*)param;
	unsigned int addCnt = 0;

	for (; addCnt < temp->img.size(); addCnt++){
		CZPageObject* img = temp->img[addCnt];
//		img->LoadPageImage(temp->hDC, temp->hRC, 128);
	}



	//static int addcnt = 0;
	//while (1){
	//	TRACE(L"Thread running\n");
	//	CZPageObject* img = temp->img[
	//	Sleep(100);
	//	addcnt++;
	//}
	return 0;
}

CZDataManager::CZDataManager()
{	
	m_pPDF = NULL;
	memset(m_bSlot, 0x00, sizeof(m_bSlot));
	m_xOffset = DEFAULT_X_OFFSET;
	m_yOffset = DEFAULT_Y_OFFSET;

	float scale = 1.0f / 4.147f; // sum of sin vlaue between 0 ~ 90
	float fstepRad = (3.141529f*0.5f) / (float)ANI_FRAME_CNT;
	for (int i = 0; i < ANI_FRAME_CNT; i++){
		m_fAniAcceration[i] = (1.0f - sin(fstepRad*i))*scale;
	}

	mtSetPoint3D(&m_AccColor[9], 1.0f, 0.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[8], 1.0f, 0.2f, 0.0f);
	mtSetPoint3D(&m_AccColor[7], 1.0f, 0.4f, 0.0f);
	mtSetPoint3D(&m_AccColor[6], 1.0f, 0.6f, 0.0f);
	mtSetPoint3D(&m_AccColor[5], 1.0f, 0.8f, 0.0f);
	mtSetPoint3D(&m_AccColor[4], 1.0f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[3], 0.8f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[2], 0.6f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[1], 0.4f, 1.0f, 0.0f);
	mtSetPoint3D(&m_AccColor[0], 0.2f, 1.0f, 0.0f);
}

float CZDataManager::GetAniAcceration(int idx)
{
	if (idx < ANI_FRAME_CNT)
		return m_fAniAcceration[idx];
	else
		return 0.0f;
}


CZDataManager::~CZDataManager()
{
	if (m_pPDF != NULL){
		delete m_pPDF;
	}

	_vecPageObj::iterator iter = GetVecImageBegin();
	for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++){
		
		GLuint tex = (*iter)->GetThTex();
		GLuint texfull = (*iter)->GetTexId();
		if (tex > 0){
			glDeleteTextures(1, &tex);
		}
		if (texfull > 0){
			glDeleteTextures(1, &texfull);
		}
		delete (*iter);
	}

	m_vecImageData.img.clear();
	m_mapImageData.clear();
	m_mapGrupImg.clear();
}

void CZDataManager::InitData()
{
	if (m_pPDF == NULL){
		m_pPDF = new CZPDFConverter;		
	}
}



void CZDataManager::LoadImageTexture(CString strpath, GLuint &_texid)
{
	USES_CONVERSION;
	char* sz = T2A(strpath);

	IplImage *pimg = cvLoadImage(sz);
	if (pimg){
		//	cvShowImage(sz, pimg);
		cvCvtColor(pimg, pimg, CV_BGR2RGB);

		// glupload Image - Thumnail image=======================================================//
		glGenTextures(1, &_texid);
		glBindTexture(GL_TEXTURE_2D, _texid);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg->width, pimg->height, GL_RGB, GL_UNSIGNED_BYTE, pimg->imageData);
		//======================================================================================//

	}
	cvReleaseImage(&pimg);
}

void CZDataManager::PushImageDataSet(unsigned long _code, unsigned long _pcode, CZPageObject* pimg)
{
	std::map<unsigned long, PAGEGROUP>::iterator iter_gr;
	std::map<unsigned long, CZPageObject*>::iterator iter;

	iter = m_mapImageData.find(_code);
	if (iter == m_mapImageData.end()){

		// Store Data Information ================================//!!!!
		m_mapImageData[_code] = pimg;		// for duplication checking

		// push image data sequecily ..
		m_vecImageData.img.push_back(std::move(pimg));

		// Make Group =========================//
		iter_gr = m_mapGrupImg.find(_pcode);
		if (iter_gr == m_mapGrupImg.end()){		// New Group
			_vecPageObj vecImg;
			vecImg.push_back(std::move(pimg));
			m_mapGrupImg[_pcode].imgVec = vecImg;
			m_mapGrupImg[_pcode].nSlot = -1;
		}
		else{
			m_mapGrupImg[_pcode].imgVec.push_back(std::move(pimg));
		}
		//======================================//

		//	pM->ResetOutputWnd();
		//pM->AddOutputString(pimg->GetPath(), true);
	}
}

void CZDataManager::TestThread()
{
//	std::vector<int> buff;
//	CWinThread *pThread = AfxBeginThread(Threadproc, &m_thtest);
}

void CZDataManager::Th_GenerateThumnail()
{
//	CWinThread *pThread = AfxBeginThread(Threadproc, &m_vecImageData);
}

short CZDataManager::SelectPages(unsigned long cCode)
{
	//std::map<unsigned long, CZPageObject*>::iterator iter;
	//iter = m_mapImageData.find(cCode);
	//if (iter != m_mapImageData.end()){
	//	return iter->second->SetSelection(1, offset);
	//}
	//else{
	//	return 0.0f;
	//}
	short res = -1;
	std::map<unsigned long, PAGEGROUP>::iterator iter_gr;
	iter_gr = m_mapGrupImg.find(cCode);
	if (iter_gr != m_mapGrupImg.end()){
	//	int nSlot = iter_gr->second.nSlot;

		if (iter_gr->second.nSlot == -1){		// Selection
			iter_gr->second.nSlot = GetEmptySlot();
			float xoffset = DEFAULT_X_OFFSET;
			int i = 0;
			for (i = 0; i < iter_gr->second.imgVec.size(); i++){
				if (i%MAX_DESP_COLS == 0){
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset -= DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelectionPosition(iter_gr->second.nSlot, xoffset, m_yOffset, true);
			}
			if (i%MAX_DESP_COLS == 0){
				xoffset = DEFAULT_X_OFFSET;
				m_yOffset -= DEFAULT_PAGE_SIZE;
			}
			res = 1;
		}
		else{		// Deselection //
			ReturnSlot(iter_gr->second.nSlot);
			iter_gr->second.nSlot = -1;
			float xoffset = DEFAULT_X_OFFSET;
			int i = 0;
			for (i = 0; i < iter_gr->second.imgVec.size(); i++){
				if (i % MAX_DESP_COLS == 0){
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset += DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelectionPosition(-1, xoffset, m_yOffset, true);
			}
			if (i % MAX_DESP_COLS == 0){
				xoffset = DEFAULT_X_OFFSET;
				m_yOffset += DEFAULT_PAGE_SIZE;
			}
			res = 2;
		}
	}

	return res;
}

int CZDataManager::GetEmptySlot()
{
	for (int i = 0; i < MAX_SLOT_SIZE; i++){
		if (m_bSlot[i] == false){
			m_bSlot[i] = true;
			return i;
		}
	}
	return -1;
}

void CZDataManager::ReturnSlot(int idx)
{
	if (idx < MAX_SLOT_SIZE){
		m_bSlot[idx] = false;
	}
}

CZPageObject* CZDataManager::GetPageByOrderID(int idx)
{
	if (idx < m_vecImageData.img.size()){
	//	if (m_vecImageData.img[idx]->IsCandidate())
		
		
			return m_vecImageData.img[idx];
	//	else
	//		return NULL;
	}
	return NULL;
}

void CZDataManager::UpdatePageStatus(POINT3D camPos)
{
	// Distance between cam and pages//
	// if the distance is less than TH, load high-resolution image for page//
	for (int i = 0; i < m_vecImageData.img.size(); i++){
		if (m_vecImageData.img[i]->IsCandidate()){
			float fDist = mtDistance(camPos, m_vecImageData.img[i]->GetPos());
			if (fDist < DEFAULT_PAGE_SIZE*2.0f){
				m_vecImageData.img[i]->LoadFullImage();
				m_vecImageData.img[i]->SetIsNear(true);
			}
			else{
				m_vecImageData.img[i]->SetIsNear(false);
			}



		}
	}
}

void CZDataManager::ResetResult()
{
	for (int i = 0; i < m_vecImageData.img.size(); i++){
		
			m_vecImageData.img[i]->ClearMatchResult();

	}
}

POINT3D CZDataManager::GetColor(float fvalue)
{
	int idx = fvalue * 10;
	if (idx<0)
		idx = 0;
	if (idx>9)
		idx = 9;

	return m_AccColor[idx];
}

IplImage* CZDataManager::LoadPDFImage(CString strpath, unsigned short nChannel)
{
	if (m_pPDF){
		return m_pPDF->LoadPDF(strpath, nChannel);
	}
	return NULL;
}

  
IplImage* CZDataManager::LoadIplImage(CString strpath, unsigned short ch)
{
	USES_CONVERSION;
	char* sz = T2A(strpath);
	IplImage *pSrc = NULL;

	CString str = PathFindExtension(strpath);
	if (ch == 1){
		if ((str == L".pdf") || (str == L".jpg")){
			pSrc = SINGLETON_TMat::GetInstance()->LoadPDFImage(strpath, ch);
		}
		else{
			pSrc = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
		}
	}
	else{
		if ((str == L".pdf") || (str == L".jpg")){
			pSrc = SINGLETON_TMat::GetInstance()->LoadPDFImage(strpath, ch);
		}
		else{
			pSrc = cvLoadImage(sz);
		}
	}
	return pSrc;
}


short CZDataManager::DeSelectPages()
{

	short res = -1;
	std::map<unsigned long, PAGEGROUP>::iterator iter_gr = m_mapGrupImg.begin();

	for (; iter_gr != m_mapGrupImg.end(); iter_gr++){
		//	int nSlot = iter_gr->second.nSlot;

		if (iter_gr->second.nSlot != -1){		// Selection

			ReturnSlot(iter_gr->second.nSlot);
			iter_gr->second.nSlot = -1;
			float xoffset = DEFAULT_X_OFFSET;
			int i = 0;
			for (i = 0; i < iter_gr->second.imgVec.size(); i++){
				if (i % MAX_DESP_COLS == 0){
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset += DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelectionPosition(-1, xoffset, m_yOffset, true);
			}
			if (i % MAX_DESP_COLS == 0){
				xoffset = DEFAULT_X_OFFSET;
				m_yOffset += DEFAULT_PAGE_SIZE;
			}

		}
	}


	return res;
}