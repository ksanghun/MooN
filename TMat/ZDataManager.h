#pragma once
#include "ZSingtonTMat.h"
#include "ZPDFConverter.h"
#include "ZPageObject.h"



typedef std::vector<CZPageObject*> _vecPageObj;

struct THREADINFO
{
	_vecPageObj img;
};
THREADINFO;


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

	_vecPageObj::iterator GetVecImageBegin() { return m_vecImageData.img.begin(); }
	_vecPageObj::iterator GetVecImageEnd() { return m_vecImageData.img.end(); }
	_vecPageObj& GetImgVec() { return m_vecImageData.img; }

private:
	CZPDFConverter* m_pPDF;
	THREADINFO m_vecImageData;

	// Image Data Set //
	std::map<unsigned long, CZPageObject*> m_mapImageData;
	std::map<unsigned long, _vecPageObj> m_mapGrupImg;
//	_vecPageObj m_vecImageData;

};

typedef CZSingtonTMat<CZDataManager> SINGLETON_TMat;
