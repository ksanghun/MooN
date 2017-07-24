#pragma once
#include "math_tool.h"

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define DEFAULT_PAGE_SIZE 500
#define MAX_SLOT_SIZE 4096
#define MAX_DESP_COLS 25
#define UPDATE_DISTANCE 200

#define DEFAULT_X_OFFSET -5800;
#define DEFAULT_Y_OFFSET 2500;

#define MAX_CAM_HIGHTLEVEL 5000
#define MIN_CAM_HIGHTLEVEL 2
#define MAX_CAM_FOV 45.0f



typedef struct _MATCHInfo
{
	POINT3D pos;
	COLORf color;
	RECT2D rect;
	CString strAccracy;

	unsigned int searchId;
	float accuracy;
	bool IsAdded;

	CUT_INFO cInfo;
	IplImage* pImg;

}_MATCHInfo;





class CZPageObject
{
public:
	CZPageObject();
	~CZPageObject();

	void SetName(CString _strpath, CString _strpname, CString _strname, unsigned long _pcode, unsigned long _nCode);
	void SetSize(unsigned short _w, unsigned short _h, float _size);
	void SetThTex(GLuint _thtex);
	void SetTexId(GLuint _texid);
	void SetTex(GLuint _tex);
	void SetPosition(POINT3D pos);
	void SetRendomPos();
	//void SetSelecttion(bool _isSel);
	float SetSelectionPosition(int nSlot, float xOffset, float yOffset, bool IsAni);
//	void SetImgSize(unsigned short _w, unsigned short _h) { nImgWidth = _w; nImgHeight = _h; };

	void DrawThumbNail(float fAlpha);
	void DrawForPicking();
	void DrawImage(float fAlpha);
	void DrawBMPText();
	void DrawWordBoundary(int selid);
	void DrawWordBoundaryForPick();	
	void DrawMatchItem(int selId);
	void DrawMatchItemForPick();

	bool AddMatchedPoint(_MATCHInfo info, int search_size);

	GLuint GetTexId() { return texId; };
	GLuint GetThTex() { return thTexId; };
	CString GetPath() { return strPath; };
	CString GetPName() { return strPName; };
	CString GetName() { return strName; };
	//unsigned short GetWidth() { return nWidth; };
	//unsigned short GetHeight() { return nHeight; };
	POINT3D GetPos() { return m_pos; };
	unsigned short GetImgWidth() { return nImgWidth; };
	unsigned short GetImgHeight() { return nImgHeight; };
	unsigned long GetCode() { return nCode; }
	unsigned long GetPCode() { return parentCode; }

	bool IsDuplicate(_MATCHInfo& info, int search_size);

	void RotatePos(float fSpeed);
	bool LoadThumbImage(unsigned short resolution);
	GLuint LoadFullImage();

	void ClearMatchResult();
	std::vector<_MATCHInfo>& GetMatchResult() { return m_matched_pos; };
	int GetResultSize() { return m_matched_pos.size(); }
	bool IsCandidate() { return m_bCandidate; }
	void SetCandidate(bool isCan) { m_bCandidate = isCan; m_pos.z = 0.0f; }

	void SetSelection(bool IsSel);
	void AnimatePos(bool IsZvalue);

	RECT2D ConvertVec3DtoImgateCoord(POINT3D v1, POINT3D v2);
	void SetIsNear(bool isnear) { m_bIsNear = isnear; }

	bool UpdateTextBoundary();

	void DrawWithoutTexID(float fAlpha);
	RECT2D GetImgRect() { return m_ImgRectSize; }


	// For Extration =====================//
	//void GenTexIDForExtract(RECT2D cutRect);
	float GetImgCols() { return m_fImgCols; }
	float GetImgRows() { return m_fImgRows; }

	float GetfXScale() { return m_fXScale; }
	float GetfYScale() { return m_fYScale; }
	
private:
	CString strPath;
	CString strPName;
	CString strName;

	unsigned long parentCode;
	unsigned long nCode;

	//unsigned short nWidth;
	//unsigned short nHeight;
	float fARatio;

	GLuint thTexId, texId, m_texIdforExtract;

	float	m_fRectWidth;
	POINT3D m_pos;
	POINT3D m_targetPos;

	float m_fImgCols;
	float m_fImgRows;

	POINT3D m_vertex[4];
	POINT3D m_vertexBg[4];
	POINT2D m_texcoord[4];

	POINT3D m_vBgColor;
	bool	m_bIsSelected;
	
	unsigned short nImgWidth;
	unsigned short nImgHeight;
	
	

	
	float m_fXScale, m_fYScale;

	bool m_bAniPos;
	POINT3D m_MoveVec;
	short m_nAniCnt;

	bool m_bCandidate;
	bool m_bIsNear;

	RECT2D m_RectImg;
	RECT2D m_ImgRectSize;

	std::vector<WORD_POS> m_wordBoundary;
	std::vector<_MATCHInfo> m_matched_pos;
};

