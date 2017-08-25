#pragma once
#include "ZSingtonTMat.h"
#include "ZPDFConverter.h"
#include "ZPageObject.h"
#include <string>




//#define _NORMALIZE_SIZE 32


#define _NORMALIZE_SIZE_H 32
#define _NORMALIZE_SIZE_W 32*10

#define ANI_FRAME_CNT 10

typedef std::vector<CZPageObject*> _vecPageObj;


struct PAGEGROUP{
	int nSlot;
	_vecPageObj imgVec;
};



struct WORD_RECORD{
	std::vector<WORD_POS> posList;
};




struct THREADINFO
{
	_vecPageObj img;
};


typedef struct _MATCHResults
{
	IplImage* pImgCut;
	unsigned short searchId;
	unsigned long cutId;
	unsigned long fileId;
	unsigned long posId;
	unsigned long matchId;
	unsigned long matchFile;
	unsigned long matchPos;

	RECT2D rect;


	float fTh;
	float accuracy;

	CString strBase64;
	CString strCode;

}_MATCHResults;

typedef std::vector<_MATCHResults> _vecMATCHResult;

struct MATCHGROUP{
	int searchId;
	_vecMATCHResult matche;
};



class CDragDropTreeCtrl;
class CZDataManager
{
public:
	CZDataManager();
	~CZDataManager();

	void InitData();	
	void PushImageDataSet(unsigned long _code, unsigned long _pcode, CZPageObject* pimg);
	void PopImageDataSet(unsigned long _code);

	void TestThread();
	void Th_GenerateThumnail();
	short SelectPages(unsigned long cCode);
	void UpdatePageStatus(POINT3D camPos);
	void ResetResult();

	_vecPageObj::iterator GetVecImageBegin() { return m_vecImageData.img.begin(); }
	_vecPageObj::iterator GetVecImageEnd() { return m_vecImageData.img.end(); }
	_vecPageObj& GetImgVec() { return m_vecImageData.img; }
	std::map<unsigned long, MATCHGROUP>& GetMatchResults() { return m_matchResGroup; }

	int  GetEmptySlot();
	void ReturnSlot(int idx);
	float GetAniAcceration(int idx);
	void LoadImageTexture(CString strpath, GLuint &_texid);
	IplImage* LoadPDFImage(CString strpath, unsigned short nChannel);

	CZPageObject* GetPageByOrderID(int idx);

	POINT3D GetColor(float fvalue);
	void SetPdfTexId(GLuint _texid) { m_pdfTexid = _texid; }
	GLuint GetPdfTexId() { return m_pdfTexid; }
	IplImage* LoadIplImage(CString strpath, unsigned short ch);
	short DeSelectPages();

	void SetResColor(COLORf _color) { m_resColor = _color; }
	COLORf GetResColor() { return m_resColor; }

	bool InsertIntoLogDB(cv::Mat cutImg, int x1, int x2, int y1, int y2, unsigned int pageCode);
	void SetLogPath(CString str); 
	CString GetLogPath() { return m_strLogPath; }
	void GetWordBoundaryByPageCode(unsigned long pcode, std::vector<WORD_POS>& vecBoundary);
	void GetWordBoundaryByWordId(unsigned long wid, std::vector<WORD_POS>& vecBoundary);

//	void AddMatchingResult(_MATCHInfo* match);
	void ResetMatchingResult();
	void SetMatchingResults(IplImage* pCut);
	void SetMatchingResultsExtraction();
	void SortMatchingResults();
	void GenerateMatchingResultsImg();



	//=Base64 Conversion===============================================================//
	CString base64_encode(unsigned char const*, unsigned int len);
	std::string base64_decode(std::string const& s);
	//=================================================================//

	CBitmap* GetLogCBitmap(IplImage* pImg);
	cv::Rect GetNomalizedWordSize(RECT2D rect);

private:

	float SearchInLogFile(IplImage& pCut, IplImage& dst);

//	std::vector<_MATCHResults> m_matchResults;
//	std::vector<_MATCHResults> m_matchResults;
	std::map<unsigned long, MATCHGROUP> m_matchResGroup;

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
	COLORf m_resColor;


	CString m_strLogPath;
	std::vector<unsigned long> m_vecPageList;
	unsigned long m_wordId;

	std::map<unsigned long, WORD_RECORD> m_mapLogWord;
	std::map<unsigned long, WORD_RECORD> m_mapLogPage;


	bool m_IsLogUpdate;


};

typedef CZSingtonTMat<CZDataManager> SINGLETON_TMat;
