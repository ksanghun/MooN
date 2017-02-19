#include "stdafx.h"
#include "ZPageObject.h"
#include "ZDataManager.h"
//#define SPACE_SIZE 7000

#define Z_TRANS 70000


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

	SetSize(DEFAULT_PAGE_SIZE, DEFAULT_PAGE_SIZE, DEFAULT_PAGE_SIZE);
	m_bCandidate = false;
	m_bIsSelected = false;
	m_bAniPos = false;
}


CZPageObject::~CZPageObject()
{
	if (texId > 0){
		glDeleteTextures(1, &texId);
	}
	if (thTexId > 0){
		glDeleteTextures(1, &thTexId);
	}

	m_matched_pos.clear();
}

//===========================//
void CZPageObject::SetRendomPos()
{
	float fScale = 20.0f;
	m_pos.x = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
	m_pos.y = (rand() % MAX_CAM_HIGHTLEVEL) - MAX_CAM_HIGHTLEVEL*0.5f;
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
			w = _size*fARatio*0.4f;
			h = _size*0.4f;
		}
		else{
			w = _size*0.4f;
			h = (_size / fARatio)*0.4f;
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
}


void CZPageObject::DrawForPicking()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Background//	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
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
		return m_fRectWidth+2;
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

void CZPageObject::AnimatePos()
{
	float fDelta = SINGLETON_TMat::GetInstance()->GetAniAcceration(m_nAniCnt);
	m_pos.x = m_pos.x + m_MoveVec.x*fDelta;
	m_pos.y = m_pos.y + m_MoveVec.y*fDelta;
	m_pos.z = m_pos.z + m_MoveVec.z*fDelta;
	m_nAniCnt++;
	if (m_nAniCnt >= ANI_FRAME_CNT){
		m_bAniPos = false;
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


void CZPageObject::DrawThumbNail(float fAlpha)
{
	if (thTexId == 0){
		return;	
	}
	if (m_bCandidate){
		fAlpha = 0.95f;
	}
	if (m_bAniPos){
		AnimatePos();
	}

	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	// Background//
	//	glDisable(GL_TEXTURE_2D);
	//glColor4f(m_vBgColor.x, m_vBgColor.y, m_vBgColor.z, 0.0f);
	//glBegin(GL_QUADS);
	//glVertex3f(m_vertexBg[0].x, m_vertexBg[0].y, m_vertexBg[0].z);
	//glVertex3f(m_vertexBg[1].x, m_vertexBg[1].y, m_vertexBg[1].z);
	//glVertex3f(m_vertexBg[2].x, m_vertexBg[2].y, m_vertexBg[2].z);
	//glVertex3f(m_vertexBg[3].x, m_vertexBg[3].y, m_vertexBg[3].z);
	//glEnd();
	//==================//


	glEnable(GL_TEXTURE_2D);
	if (texId == 0){
		glBindTexture(GL_TEXTURE_2D, thTexId);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, texId);
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


	// Detected //
	if (m_matched_pos.size() > 0){
		glColor4f(1.0f, 0.2f, 0.1f, fAlpha);
		glBegin(GL_LINE_STRIP);
		glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
		glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
		glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
		glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
		glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
		glEnd();

		// Draw detected position //
		glColor4f(1.0f, 0.2f, 0.1f, 0.7f);
		glPushMatrix();
		glScalef(m_fXScale, m_fYScale, 1.0f);
		glTranslatef(-nImgWidth*0.5f, -nImgHeight*0.5f, 0.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < m_matched_pos.size(); i++){
			glColor4f(m_matched_pos[i].color.x, m_matched_pos[i].color.y, m_matched_pos[i].color.z, 0.7f);
			glVertex3f(m_matched_pos[i].pos.x, nImgHeight - m_matched_pos[i].pos.y, 0.0f);
		}
		glEnd();
		glPopMatrix();

	}


//	glColor4f(0.3f, 0.7f, 0.9f, 0.7f);
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
	glLineWidth(1);

	glPopMatrix();

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

	IplImage *pimg = cvLoadImage(sz);
	if (pimg){
		//	cvShowImage(sz, pimg);
		cvCvtColor(pimg, pimg, CV_BGR2RGB);

		// glupload Image - Thumnail image=======================================================//
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, m_texture->sizeX,m_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,m_texture->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pimg->width, pimg->height, GL_RGB, GL_UNSIGNED_BYTE, pimg->imageData);
		//======================================================================================//
		cvReleaseImage(&pimg);
	}
	return texId;
}

bool CZPageObject::LoadThumbImage(unsigned short resolution)
{
	if (thTexId != 0){
		return false;
	}				

	USES_CONVERSION;	char* sz = T2A(strPath);

//	IplImage *pimg = cvLoadImage(sz);
	cv::Mat src = cv::imread(sz);
	if (src.empty()){
		return false;
	}

	SetSize(src.cols, src.rows, DEFAULT_PAGE_SIZE);

	cv::resize(src, src, cv::Size(resolution, resolution), 0, 0, CV_INTER_LINEAR);		// Memory Leak!!!!!!

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
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, resolution, resolution, GL_RGB, GL_UNSIGNED_BYTE, src.data);
	//======================================================================================//

	SetThTex(tid);

	src.release();
	return true;

}