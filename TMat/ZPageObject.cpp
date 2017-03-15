#include "stdafx.h"
#include "ZPageObject.h"
#include "ZDataManager.h"
//#define SPACE_SIZE 7000

#define Z_TRANS 70000
//#define Z_TRANS 0

CZPageObject::CZPageObject()
{
	parentCode = 0;
	//nWidth = 0;
	//nHeight = 0;
	fARatio = 0.0f;

	thTexId = texId = 0;
	m_fRectWidth = 0.0f;

	mtSetPoint3D(&m_vBgColor, 0.3f, 0.7f, 0.9f);

	SetRendomPos();
	mtSetPoint3D(&m_targetPos, 0, 0, 0);

	m_fXScale = 1.0f;
	m_fYScale = 1.0f;

	m_fImgCols = 0.0f;
	m_fImgRows = 0.0f;

	SetSize(DEFAULT_PAGE_SIZE, DEFAULT_PAGE_SIZE, DEFAULT_PAGE_SIZE);
	m_bCandidate = false;
	m_bIsSelected = false;
	m_bAniPos = false;

	m_bIsNear = false;

}


CZPageObject::~CZPageObject()
{
	if (texId > 0){
		glDeleteTextures(1, &texId);
	}
	if (thTexId > 0){
		glDeleteTextures(1, &thTexId);
	}

	if (m_matched_pos.size() > 0){
		m_matched_pos.clear();
	}
}

//===========================//
void CZPageObject::SetRendomPos()
{
	float fScale = 20.0f;
	m_pos.x = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
	m_pos.y = (rand() % MAX_CAM_HIGHTLEVEL*0.5f) - MAX_CAM_HIGHTLEVEL*0.5f*0.5f;
	m_pos.z = -((rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f);

	m_pos.x = m_pos.x*fScale;
	m_pos.y = m_pos.y*fScale;
	m_pos.z = m_pos.z*fScale - Z_TRANS;
}

void CZPageObject::RotatePos(float fSpeed)
{
	if (!m_bCandidate){
		float fCos = cos(0.001);
		float fSin = sin(0.001);

		POINT3D tmpV = m_pos;
		tmpV.z += Z_TRANS;

		m_pos.x = tmpV.x*fCos - tmpV.z*fSin;
		m_pos.z = (tmpV.x*fSin + tmpV.z*fCos) - Z_TRANS;
	}
}

bool CZPageObject::AddMatchedPoint(_MATCHInfo info, int search_size)
{
	if (search_size > 0){
		if (IsDuplicate(info.pos, search_size) == false){
			m_matched_pos.push_back(info);
		}
	}
	else{
		m_matched_pos.push_back(info);
	}
	return true;
}

void CZPageObject::SetName(CString _strpath, CString _strpname, CString _strname, unsigned long _pcode, unsigned long _nCode)
{
	strPath = _strpath;
	strPName = _strpname;
	strName = _strname;
	parentCode = _pcode;
	nCode = _nCode;
//	mtSetPoint3D(&m_pos, 0.0f, 0.0f, 0.0f);
}
void CZPageObject::SetSize(unsigned short _w, unsigned short _h, float _size)
{
	nImgWidth = _w;
	nImgHeight = _h;
	fARatio = (float)_w / (float)_h;
	float w, h;

	if (_size > 0){
		if (fARatio <= 1.0f){
			w = _size*fARatio*0.45f;
			h = _size*0.45f;
		}
		else{
			w = _size*0.45f;
			h = (_size / fARatio)*0.45f;
		}
	}
	else{
		w = _w*0.5f;
		h = _h*0.5f;
	}

	m_fXScale = w*2.0f / (float)nImgWidth;
	m_fYScale = h*2.0f / (float)nImgHeight;

	mtSetPoint3D(&m_vertex[0], -w, -h, 0.0f);		mtSetPoint2D(&m_texcoord[0], 0.0f, 1.0f);
	mtSetPoint3D(&m_vertex[1], w, -h, 0.0f);		mtSetPoint2D(&m_texcoord[1], 1.0f, 1.0f);
	mtSetPoint3D(&m_vertex[2], w, h, 0.0f);			mtSetPoint2D(&m_texcoord[2], 1.0f, 0.0f);
	mtSetPoint3D(&m_vertex[3], -w, h, 0.0f);		mtSetPoint2D(&m_texcoord[3], 0.0f, 0.0f);


	mtSetPoint3D(&m_vertexBg[0], -_size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[1], _size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[2], _size*0.5f, _size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertexBg[3], -_size*0.5f, _size*0.5f, 0.0f);

	m_fRectWidth = _size*fARatio;

	m_RectImg.set(m_vertex[0].x, m_vertex[1].x, m_vertex[0].y, m_vertex[2].y);

}


void CZPageObject::DrawForPicking()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Background//	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	glEnd();
	//==================//
	glPopMatrix();

}

void CZPageObject::SetSelection(bool _isSel)
{
	if (_isSel == true){
		mtSetPoint3D(&m_vBgColor, 0.99f, 0.0f, 0.0f);
	}
	else{
		mtSetPoint3D(&m_vBgColor, 0.3f, 0.7f, 0.9f);
	}
	m_bIsSelected = _isSel;
};

float CZPageObject::SetSelectionPosition(int nSlot, float xOffset, float yOffset, bool IsAni)
{
	m_bAniPos = IsAni;
	POINT3D targetPos;
	if (nSlot >= 0){
		targetPos.x = xOffset;
		targetPos.y = yOffset;
		targetPos.z = 0.0f;


		if (m_bAniPos){
			m_nAniCnt = 0;
			m_MoveVec = targetPos - m_pos;
		}
		else{
			m_pos = targetPos;
		}
		m_bCandidate = true;
		return DEFAULT_PAGE_SIZE+2;
	}
	else{
		float fScale = 20.0f;
		targetPos.x = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
		targetPos.y = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
		targetPos.z = -((rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f);

		targetPos.x = targetPos.x*fScale;
		targetPos.y = targetPos.y*fScale;
		targetPos.z = targetPos.z*fScale - Z_TRANS;



		if (IsAni){
			m_nAniCnt = 0;
			m_MoveVec = targetPos - m_pos;
		}
		else{
			m_pos = targetPos;
		}
		m_bCandidate = false;
		return 0.0f;
	}	
}

void CZPageObject::AnimatePos(bool IsZvalue)
{
	float fDelta = SINGLETON_TMat::GetInstance()->GetAniAcceration(m_nAniCnt);
	m_pos.x = m_pos.x + m_MoveVec.x*fDelta;
	m_pos.y = m_pos.y + m_MoveVec.y*fDelta;
	m_pos.z = m_pos.z + m_MoveVec.z*fDelta;
	m_nAniCnt++;
	if (m_nAniCnt >= ANI_FRAME_CNT){	
		m_bAniPos = false;
		if (IsZvalue) m_pos.z = 0.0f;
	}
}

void CZPageObject::SetTexId(GLuint _texid)
{
	if (_texid != texId){
		if (texId > 0){
			glDeleteTextures(1, &texId);
		}
		texId = _texid;
	}
}

void CZPageObject::DrawImage(float fAlpha)
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glBindTexture(GL_TEXTURE_2D, texId);
	glColor4f(1.0f, 1.0f, 1.0f, fAlpha);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);

	glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);

	glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);

	glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}


void CZPageObject::DrawBMPText()
{
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void CZPageObject::DrawWordBoundaryForPick()
{
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glLineWidth(3);
	if (m_wordBoundary.size() > 0){
		// Draw detected position //
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-nImgWidth*0.5f, -nImgHeight*0.5f, 0.0f);

		if (m_bIsNear){
			for (int i = 0; i < m_wordBoundary.size(); i++){
				//	glBegin(GL_LINE_STRIP);
				glPushName(m_wordBoundary[i].wid);
				glBegin(GL_QUADS);
				glVertex3f(m_wordBoundary[i].x1, nImgHeight - m_wordBoundary[i].y1, 0.0f);
				glVertex3f(m_wordBoundary[i].x1, nImgHeight - m_wordBoundary[i].y2, 0.0f);
				glVertex3f(m_wordBoundary[i].x2, nImgHeight - m_wordBoundary[i].y2, 0.0f);
				glVertex3f(m_wordBoundary[i].x2, nImgHeight - m_wordBoundary[i].y1, 0.0f);
				glEnd();
				glPopName();
			}
		}
		glPopMatrix();
	}
	glPopMatrix();

	glLineWidth(1);

	glEnable(GL_DEPTH_TEST);

}
void CZPageObject::DrawWordBoundary(int selid)
{
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glLineWidth(3);
	if (m_wordBoundary.size() > 0){
		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-nImgWidth*0.5f, -nImgHeight*0.5f, 0.0f);

		if (m_bIsNear){
			for (int i = 0; i < m_wordBoundary.size(); i++){

				glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
				if (m_wordBoundary[i].wid== selid)
					glColor4f(1.0f, 0.0f, 0.0f, 0.5f);

			//	glBegin(GL_LINE_STRIP);
				glBegin(GL_QUADS);				
				glVertex3f(m_wordBoundary[i].x1, nImgHeight - m_wordBoundary[i].y1, 0.0f);
				glVertex3f(m_wordBoundary[i].x1, nImgHeight - m_wordBoundary[i].y2, 0.0f);
				glVertex3f(m_wordBoundary[i].x2, nImgHeight - m_wordBoundary[i].y2, 0.0f);
				glVertex3f(m_wordBoundary[i].x2, nImgHeight - m_wordBoundary[i].y1, 0.0f);
				
				
			//	glVertex3f(m_wordBoundary[i].x1, nImgHeight - m_wordBoundary[i].y1, 0.0f);
				glEnd();
			}			
		}
		glPopMatrix();
	}
	glPopMatrix();

	glLineWidth(1);

	glEnable(GL_DEPTH_TEST);

}

void CZPageObject::DrawThumbNail(float fAlpha)
{
	if (thTexId == 0){
		return;	
	}
	if (m_bCandidate){
		fAlpha = 0.95f;
	}
	if (m_bAniPos){
		AnimatePos(m_bCandidate);
	}		



	glEnable(GL_TEXTURE_2D);
	if (texId == 0){
		glBindTexture(GL_TEXTURE_2D, thTexId);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, texId);
	}
	

	glPushMatrix();
		glTranslatef(m_pos.x, m_pos.y, m_pos.z);


		// Background//
		if (m_matched_pos.size() > 0){
			glColor4f(1.0f, 0.1f, 0.0f, 0.9f);
			glBegin(GL_QUADS);
			glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
			glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
			glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
			glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
			glEnd();
		}


		glColor4f(1.0f, 1.0f, 1.0f, fAlpha);
		glBegin(GL_QUADS);
		glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
		glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
		glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
		glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
		glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
		glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
		glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
		glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		if (m_bIsSelected)
			glLineWidth(2);
		glColor4f(m_vBgColor.x, m_vBgColor.y, m_vBgColor.z, 0.7f);
		glBegin(GL_LINE_STRIP);
		glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
		glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
		glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
		glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
		glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
		glEnd();
		
		
		
		//==================//


		//if (m_matched_pos.size() > 0){
		////	glColor4f(1.0f, 0.2f, 0.1f, fAlpha);
		//	glColor4f(0.0f, 0.99f, 0.2f, 0.9f);
		//	glBegin(GL_LINE_STRIP);
		//	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
		//	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
		//	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
		//	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
		//	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
		//	glEnd();
		//}
		glLineWidth(1);
	
		// Show Result ==============================//
		glPointSize(8);
		if (m_matched_pos.size() > 0){
			// Draw detected position //
			glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
			glPushMatrix();
				glScalef(m_fXScale, m_fYScale, 1.0f);
				glTranslatef(-nImgWidth*0.5f, -nImgHeight*0.5f, 0.0f);	
				
				if (m_bIsNear){		
				//	glLineWidth(4);
					for (int i = 0; i < m_matched_pos.size(); i++){
						glBegin(GL_QUADS);
						glColor4f(m_matched_pos[i].color.r, m_matched_pos[i].color.g, m_matched_pos[i].color.b, m_matched_pos[i].color.a);
						glVertex3f(m_matched_pos[i].rect.x1, nImgHeight - m_matched_pos[i].rect.y1, 0.0f);
						glVertex3f(m_matched_pos[i].rect.x1, nImgHeight - m_matched_pos[i].rect.y2, 0.0f);
						glVertex3f(m_matched_pos[i].rect.x2, nImgHeight - m_matched_pos[i].rect.y2, 0.0f);
						glVertex3f(m_matched_pos[i].rect.x2, nImgHeight - m_matched_pos[i].rect.y1, 0.0f);
					//	glVertex3f(m_matched_pos[i].rect.x1, nImgHeight - m_matched_pos[i].rect.y1, 0.0f);
						glEnd();
					}
				//	glLineWidth(1);					
				}
				else{
					glBegin(GL_POINTS);
					for (int i = 0; i < m_matched_pos.size(); i++){
						glColor4f(m_matched_pos[i].color.r, m_matched_pos[i].color.g, m_matched_pos[i].color.b, m_matched_pos[i].color.a);
						glVertex3f(m_matched_pos[i].pos.x, nImgHeight - m_matched_pos[i].pos.y, 0.0f);
					}
					glEnd();
				}
			glPopMatrix();
		}

		glPointSize(1);
	glPopMatrix();
//	glColor4f(0.3f, 0.7f, 0.9f, 0.7f);	

}


void CZPageObject::SetThTex(GLuint _thtex)
{
	thTexId = _thtex;
}
void CZPageObject::SetTex(GLuint _tex)
{
	texId = _tex;
}

void CZPageObject::SetPosition(POINT3D pos)
{
	m_pos = pos;
}

void CZPageObject::ClearMatchResult()
{
	m_matched_pos.clear();
}

bool CZPageObject::IsDuplicate(POINT3D pos, int search_size)
{
	bool IsDup = false;
	for (int i = 0; i < m_matched_pos.size(); i++){
		float fDist = mtDistance(pos, m_matched_pos[i].pos);
		if (fDist < search_size){
			IsDup = true;
			break;
		}
	}
	return IsDup;
}


GLuint CZPageObject::LoadFullImage()
{
	if (texId != 0){
		return 0;
	}

	USES_CONVERSION;
	char* sz = T2A(strPath);
	IplImage *pimg = NULL;
	// glupload Image - Thumnail image=======================================================//
	// In case of PDF file//
	CString str = PathFindExtension(strPath);
	if ((str == L".pdf") || (str == L".jpg")){	

		pimg = SINGLETON_TMat::GetInstance()->LoadPDFImage(strPath, 4);
		if (pimg){
			SetSize(pimg->width, pimg->height, DEFAULT_PAGE_SIZE);
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg->width, pimg->height, GL_RGBA, GL_UNSIGNED_BYTE, pimg->imageData);
		}
	}
	else{
		pimg = cvLoadImage(sz);
		if (pimg){			
			cvCvtColor(pimg, pimg, CV_BGR2RGB);		
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg->width, pimg->height, GL_RGB, GL_UNSIGNED_BYTE, pimg->imageData);
		}
	}
	
	m_fImgCols = pimg->width;
	m_fImgRows = pimg->height;

	cvReleaseImage(&pimg);


	// Load word Boundary //
	SINGLETON_TMat::GetInstance()->GetWordBoundaryByPageCode(nCode, m_wordBoundary);


	return texId;
}

bool CZPageObject::LoadThumbImage(unsigned short resolution)
{
	if (thTexId != 0){
		return false;
	}

	// In case of PDF file//
	CString str = PathFindExtension(strPath);
	if ((str == L".pdf") || (str == L".jpg")){
		thTexId = SINGLETON_TMat::GetInstance()->GetPdfTexId();
		return true;
	}
	//=======================//


					

	USES_CONVERSION;	char* sz = T2A(strPath);

	//IplImage *src = cvLoadImage(sz);
	IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(strPath, 3);
	//cv::Mat src = cv::imread(sz);
	if (!src){
		return false;
	}

	SetSize(src->width, src->height, DEFAULT_PAGE_SIZE);

	IplImage*timg = cvCreateImage(cvSize(resolution, resolution), src->depth, src->nChannels);
	cvResize(src, timg);
//	cv::resize(src, src, cv::Size(resolution, resolution), 0, 0, CV_INTER_LINEAR);		// Memory Leak!!!!!!

	// glupload Image - Thumnail image=======================================================//
	GLuint tid = 0;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, resolution, resolution, GL_RGB, GL_UNSIGNED_BYTE, timg->imageData);
	//======================================================================================//

	SetThTex(tid);

//	src.release();
	cvReleaseImage(&src);
	cvReleaseImage(&timg);
	return true;

}

RECT2D CZPageObject::ConvertVec3DtoImgateCoord(POINT3D v1, POINT3D v2)
{
	v1 = v1 - m_pos;
	v2 = v2 - m_pos;
	
	v1.x -= m_RectImg.x1;
	v1.y -= m_RectImg.y1;
	v2.x -= m_RectImg.x1;
	v2.y -= m_RectImg.y1;

	v1.y = m_RectImg.height - v1.y;
	v2.y = m_RectImg.height - v2.y;
	
	RECT2D selRect;
	if (v1.x < v2.x){
		selRect.x1 = v1.x;		selRect.x2 = v2.x;
	}
	else{
		selRect.x1 = v2.x;		selRect.x2 = v1.x;
	}
	if (v1.y < v2.y){
		selRect.y1 = v1.y;		selRect.y2 = v2.y;
	}
	else{
		selRect.y1 = v2.y;		selRect.y2 = v1.y;
	}

	if (selRect.x1 < 0)	selRect.x1 = 0;
	if (selRect.x2 > m_RectImg.width - 1)	selRect.x2 = m_RectImg.width - 1;
	if (selRect.y1 < 0)	selRect.y1 = 0;
	if (selRect.y2 > m_RectImg.height - 1)	selRect.y2 = m_RectImg.height - 1;
	

	selRect.width = selRect.x2 - selRect.x1;
	selRect.height = selRect.y2 - selRect.y1;

	if ((selRect.width == 0) || (selRect.height == 0)){
		return RECT2D(0, 0, 0, 0);
	}

	float xScale = m_fImgCols / m_RectImg.width;
	float yScale = m_fImgRows / m_RectImg.height;

	selRect.x1 *= xScale;
	selRect.x2 *= xScale;
	selRect.y1 *= yScale;
	selRect.y2 *= yScale;

	selRect.width *= xScale;
	selRect.height *= yScale;

	return selRect;

}