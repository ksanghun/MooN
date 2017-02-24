#pragma once
#include "ZSingtonTMat.h"
#include "ZPDFConverter.h"
#include "ZPageObject.h"


#define ANI_FRAME_CNT 10

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
	short SelectPages(unsigned long cCode);
	void UpdatePageStatus(POINT3D camPos);

	_vecPageObj::iterator GetVecImageBegin() { return m_vecImageData.img.begin(); }
	_vecPageObj::iterator GetVecImageEnd() { return m_vecImageData.img.end(); }
	_vecPageObj& GetImgVec() { return m_vecImageData.img; }

	int  GetEmptySlot();
	void ReturnSlot(int idx);
	float GetAniAcceration(int idx);
	void LoadImageTexture(CString strpath, GLuint &_texid);
	IplImage* LoadPDFImage(CString strpath, unsigned short nChannel);

	CZPageObject* GetPageByOrderID(int idx);

	POINT3D GetColor(float fvalue);
	void SetPdfTexId(GLuint _texid) { m_pdfTexid = _texid; }
	GLuint GetPdfTexId() { return m_pdfTexid; }
	IplImage* LoadIplImagePDF(CString strpath, unsigned short ch);
private:
	CZPDFConverter* m_pPDF;
	THREADINFO m_vecImageData;
	bool m_bSlot[MAX_SLOT_SIZE];
	float m_xOffset, m_yOffset;

	// Image Data Set //
	std::map<unsigned long, CZPageObject*> m_mapImageData;
	std::map<unsigned long, PAGEGROUP> m_mapGrupImg;
//	_vecPageObj m_vecImageData;

	float m_fAniAcceration[ANI_FRAME_CNT];

	POINT3D m_AccColor[10];
	GLuint m_pdfTexid;
};

typedef CZSingtonTMat<CZDataManager> SINGLETON_TMat;
