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
}


CZDataManager::~CZDataManager()
{
	if (m_pPDF != NULL){
		delete m_pPDF;
	}

	_vecPageObj::iterator iter = GetVecImageBegin();
	for (; iter != SINGLETON_TMat::GetInstance()->GetVecImageEnd(); iter++){
		
		GLuint tex = (*iter)->GetThTex();
		glDeleteTextures(1, &tex);
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
	std::map<unsigned long, _vecPageObj>::iterator iter_gr;
	std::map<unsigned long, CZPageObject*>::iterator iter;

	iter = m_mapImageData.find(_code);
	if (iter == m_mapImageData.end()){

		// Store Data Information ================================//!!!!
		m_mapImageData[_code] = pimg;		// for duplication checking

		// push image data sequecily ..
		m_vecImageData.img.push_back(pimg);

		// Make Group =========================//
		iter_gr = m_mapGrupImg.find(_pcode);
		if (iter_gr == m_mapGrupImg.end()){		// New Group
			_vecPageObj vecImg;
			vecImg.push_back(std::move(pimg));
			m_mapGrupImg[_pcode] = std::move(vecImg);
		}
		else{
			m_mapGrupImg[_pcode].push_back(std::move(pimg));
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