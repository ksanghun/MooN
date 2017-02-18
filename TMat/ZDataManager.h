#pragma once
#include "ZSingtonTMat.h"
#include "ZPDFConverter.h"
#include "ZPageObject.h"

#define MAX_SLOT_SIZE 4096


typedef std::vector<CZPageObject*> _vecPageObj;


struct PAGEGROUP{
	int nSlot;
	_vecPageObj imgVec;
};




struct THREADINFO
{
	_vecPageObj img;
};



class CDragDropTreeCtrl;
class CZDataManager
{
public:
	CZDataManager();
	~CZDataManager();

	void InitData();	
	void PushImageDataSet(unsigned long _code, unsigned long _pcode, CZPageObject* pimg);

	void TestThread();
	void Th_GenerateThumnail();
	void SelectPages(unsigned long cCode);

	_vecPageObj::iterator GetVecImageBegin() { return m_vecImageData.img.begin(); }
	_vecPageObj::iterator GetVecImageEnd() { return m_vecImageData.img.end(); }
	_vecPageObj& GetImgVec() { return m_vecImageData.img; }

	int  GetEmptySlot();
	void ReturnSlot(int idx);

private:
	CZPDFConverter* m_pPDF;
	THREADINFO m_vecImageData;
	bool m_bSlot[MAX_SLOT_SIZE];


	// Image Data Set //
	std::map<unsigned long, CZPageObject*> m_mapImageData;
	std::map<unsigned long, PAGEGROUP> m_mapGrupImg;
//	_vecPageObj m_vecImageData;

};

typedef CZSingtonTMat<CZDataManager> SINGLETON_TMat;
