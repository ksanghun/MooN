#include "stdafx.h"
#include "ZDataManager.h"
#include "DragDropTreeCtrl.h"
#include <thread>




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

bool CZDataManager::SelectPages(unsigned long cCode)
{
	//std::map<unsigned long, CZPageObject*>::iterator iter;
	//iter = m_mapImageData.find(cCode);
	//if (iter != m_mapImageData.end()){
	//	return iter->second->SetSelection(1, offset);
	//}
	//else{
	//	return 0.0f;
	//}
	bool res = false;
	std::map<unsigned long, PAGEGROUP>::iterator iter_gr;
	iter_gr = m_mapGrupImg.find(cCode);
	if (iter_gr != m_mapGrupImg.end()){
	//	int nSlot = iter_gr->second.nSlot;

		if (iter_gr->second.nSlot == -1){		// Selection
			iter_gr->second.nSlot = GetEmptySlot();
			float xoffset = DEFAULT_X_OFFSET;
			for (int i = 0; i < iter_gr->second.imgVec.size(); i++){
				if (i%MAX_DESP_COLS == 0){
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset -= DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelection(iter_gr->second.nSlot, xoffset, m_yOffset);
			}
			res = true;
		}
		else{		// Deselection //
			ReturnSlot(iter_gr->second.nSlot);
			iter_gr->second.nSlot = -1;
			float xoffset = DEFAULT_X_OFFSET;
			for (int i = 0; i < iter_gr->second.imgVec.size(); i++){
				if (i % MAX_DESP_COLS == 0){
					xoffset = DEFAULT_X_OFFSET;
					m_yOffset += DEFAULT_PAGE_SIZE;
				}
				xoffset += iter_gr->second.imgVec[i]->SetSelection(-1, xoffset, m_yOffset);
			}
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


void CZDataManager::UpdatePageStatus(POINT3D camPos)
{
	// Distance between cam and pages//
	// if the distance is less than TH, load high-resolution image for page//
	for (int i = 0; i < m_vecImageData.img.size(); i++){
		if (m_vecImageData.img[i]->IsCandidate()){
			float fDist = mtDistance(camPos, m_vecImageData.img[i]->GetPos());
			if (fDist < UPDATE_DISTANCE){
				m_vecImageData.img[i]->LoadFullImage();
			}
		}
	}
}