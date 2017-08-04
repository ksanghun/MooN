#include "stdafx.h"
#include "ZDataManager.h"
#include "DragDropTreeCtrl.h"
#include <thread>

#include "Extractor.h"


// For convert image file to base64 //
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
//===================================//
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}


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



	m_IsLogUpdate = false;
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
	for (; iter != GetVecImageEnd(); iter++){
		
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



	// Write Log Data //
	if (m_IsLogUpdate){
		FILE* fp;
		CString strFile;
		strFile.Format(L"%s/log.info", m_strLogPath);
		fopen_s(&fp, (CStringA)strFile, "wb");
		if (fp){
			fwrite(&m_wordId, sizeof(int), 1, fp);
		}
		fclose(fp);

		// Write Word index info //
		strFile.Format(L"%s/log.db", m_strLogPath);
		fopen_s(&fp, (CStringA)strFile, "wb");
		if (fp){
			std::map<unsigned long, WORD_RECORD>::iterator iter = m_mapLogWord.begin();
			for (; iter != m_mapLogWord.end(); iter++){
				for (int i = 0; i < iter->second.posList.size(); i++){
					WORD_POS wItem = iter->second.posList[i];
					fwrite(&wItem, sizeof(WORD_POS), 1, fp);
				}
			}
		}
		fclose(fp);
	}

	ResetMatchingResult();
}

void CZDataManager::InitData()
{
	if (m_pPDF == NULL){
		m_pPDF = new CZPDFConverter;		
	}

	
}

void CZDataManager::SetLogPath(CString str) 
{ 
	m_strLogPath = str; 
	// Load Log File //



		CString strFile;
		strFile.Format(L"%s/log.info", m_strLogPath);
		FILE* fp;
		fopen_s(&fp, (CStringA)strFile, "rb");
		if (fp){
			fread(&m_wordId, sizeof(int), 1, fp);
			fclose(fp);
		}

		strFile.Format(L"%s/log.db", m_strLogPath);
		fopen_s(&fp, (CStringA)strFile, "rb");
		if (fp){
			while (!feof(fp)){
				WORD_POS wItem;
				fread(&wItem, sizeof(WORD_POS), 1, fp);
				m_mapLogWord[wItem.wid].posList.push_back(wItem);
				m_mapLogPage[wItem.pagecode].posList.push_back(wItem);

			}
			fclose(fp);
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


void CZDataManager::PopImageDataSet(unsigned long _code)
{
	_vecPageObj::iterator iter = GetVecImageBegin();
	std::map<unsigned long, CZPageObject*>::iterator mapiter;
	std::map<unsigned long, PAGEGROUP>::iterator iter_gr;

	for (; iter != GetVecImageEnd();){
		if ((*iter)->GetPCode() == _code){
			GLuint tex = (*iter)->GetThTex();
			GLuint texfull = (*iter)->GetTexId();
			if (tex > 0){
				glDeleteTextures(1, &tex);
			}
			if (texfull > 0){
				glDeleteTextures(1, &texfull);
			}			

			// pop image from map
			mapiter = m_mapImageData.find((*iter)->GetCode());
			if (mapiter != m_mapImageData.end()){
				m_mapImageData.erase(mapiter);			

				iter_gr = m_mapGrupImg.find(_code);
				if (iter_gr != m_mapGrupImg.end()){		// New Group
				//	ReturnSlot(iter_gr->second.nSlot);

					if (iter_gr->second.nSlot != -1){	
						SelectPages(_code);
					}

					iter_gr->second.nSlot = -1;
					iter_gr->second.imgVec.clear();
					m_mapGrupImg.erase(iter_gr);
				}
			}	

			delete (*iter);			
			iter = GetImgVec().erase(iter);	
		}
		else{
			++iter;
		}
	}


//	std::remove(v.begin(), v.end(), 10);

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

	ResetMatchingResult();
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

	if ((str == L".pdf") || (str == L".jpg") || (str == L".JPG") || (str == L".jpeg")){
			pSrc = SINGLETON_TMat::GetInstance()->LoadPDFImage(strpath, ch);
	}
	else{
		if (ch == 1){
			pSrc = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
		}
		else{
			pSrc = cvLoadImage(sz);
		}
	}


	//if (ch == 1){
	//	if ((str == L".pdf") || (str == L".jpg")){
	//		pSrc = SINGLETON_TMat::GetInstance()->LoadPDFImage(strpath, ch);
	//	}
	//	else{
	//		pSrc = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
	//	}
	//}
	//else{
	//	if ((str == L".pdf") || (str == L".jpg")){
	//		pSrc = SINGLETON_TMat::GetInstance()->LoadPDFImage(strpath, ch);
	//	}
	//	else{
	//		pSrc = cvLoadImage(sz);
	//	}
	//}
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


bool CZDataManager::InsertIntoLogDB(cv::Mat cutImg, int x1, int x2, int y1, int y2, unsigned int pageCode)
{
	m_IsLogUpdate = true;

	IplImage src = cutImg;


	bool bmatched = false;
	unsigned long matchid = 0;
	WORD_POS wItem;
	wItem.x1 = x1;
	wItem.x2 = x2;
	wItem.y1 = y1;
	wItem.y2 = y2;
	wItem.pagecode = pageCode;

	USES_CONVERSION;
	CString strName;
	for (int i = 0; i < m_wordId; i++){
		// match with previous cut images //
		strName.Format(L"%s/%d.bmp", m_strLogPath, i);
		char* sz = T2A(strName);
		//=================================================================================================================
		IplImage *dst = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);

		float fAcc = SearchInLogFile(src, *dst);
		if (fAcc> 0.7f){
			matchid = i;
			bmatched = true;
			break;
		}
		cvReleaseImage(&dst);
	}

	if (bmatched){
		wItem.wid = matchid;
		m_mapLogWord[matchid].posList.push_back(wItem);
	}
	else{
		// save cut image under the name of "wordID"
		strName.Format(L"%s/%d.bmp", m_strLogPath, m_wordId);
		char* sz = T2A(strName);
		cv::imwrite(sz, cutImg);
		wItem.wid = m_wordId;
		m_mapLogWord[m_wordId].posList.push_back(wItem);
		m_wordId++;
	}
	return bmatched;
}

float CZDataManager::SearchInLogFile(IplImage& pCut, IplImage& dst)
{
	float fAccur = 0.0f;
	bool IsOk = true;
	if (pCut.width > dst.width*1.2)	IsOk = false;
	if (pCut.height > dst.height*1.5) IsOk = false;

	if (pCut.width < dst.width*0.8)	IsOk = false;
	if (pCut.height < dst.height*0.5) IsOk = false;


	if (IsOk){
		//cvShowImage("Resize", gray);

		short newWidth = dst.width * 2;
		short newHeight = dst.height * 2;
		IplImage* src = cvCreateImage(cvSize(newWidth, newHeight), dst.depth, dst.nChannels);
		cvSet(src, cvScalar(255, 255, 255));

		CvRect rect;
		rect.x = newWidth / 2 - dst.width / 2;
		rect.y = newHeight / 2 - dst.height / 2;
		rect.width = dst.width;
		rect.height = dst.height;

		cvSetImageROI(src, rect);
		cvResize(&dst, src);
		cvResetImageROI(src);

		IplImage *result_img = 0;
		result_img = cvCreateImage(cvSize(src->width - pCut.width + 1, src->height - pCut.height + 1), IPL_DEPTH_32F, 1);
		cvMatchTemplate(src, &pCut, result_img, CV_TM_CCOEFF_NORMED);

			
		float sTh = 0.0f;
		float* d = (float*)result_img->imageData;
		for (int y = 0; y < result_img->height; y++){
			for (int x = 0; x < result_img->width; x++){
				float fD = *(d + y*result_img->width + x);
				if (fD > sTh)	{
					sTh = fD;
					fAccur = fD;
				}
			}
		}

		cvReleaseImage(&src);
		cvReleaseImage(&result_img);
	}

	return fAccur;

}

void CZDataManager::GetWordBoundaryByPageCode(unsigned long pcode, std::vector<WORD_POS>& vecBoundary)
{
	std::map<unsigned long, WORD_RECORD>::iterator iter = m_mapLogPage.find(pcode);
	if (iter != m_mapLogPage.end()){
		for (int i = 0; i < iter->second.posList.size(); i++){
			vecBoundary.push_back(iter->second.posList[i]);
		}
	}
}

void CZDataManager::GetWordBoundaryByWordId(unsigned long wid, std::vector<WORD_POS>& vecBoundary)
{
	std::map<unsigned long, WORD_RECORD>::iterator iter = m_mapLogWord.find(wid);
	if (iter != m_mapLogPage.end()){
		for (int i = 0; i < iter->second.posList.size(); i++){
			vecBoundary.push_back(iter->second.posList[i]);
		}
	}
}


void CZDataManager::ResetMatchingResult()
{
	std::map<unsigned long, MATCHGROUP>::iterator iter_gr = m_matchResGroup.begin();

	for (; iter_gr != m_matchResGroup.end(); iter_gr++){
		//for (int j = 0; j < iter_gr->second.matche.size(); j++){
		//	if (iter_gr->second.matche[j].pImgCut != NULL){
		//		cvReleaseImage(&iter_gr->second.matche[j].pImgCut);
		//		iter_gr->second.matche[j].pImgCut = NULL;
		//	}
		//}
		iter_gr->second.matche.clear();
	}

	m_matchResGroup.clear();
}

void CZDataManager::SetMatchingResults(IplImage* pCut)
{
//	ResetMatchingResult();

	MATCHGROUP vecMatchRes;

	bool IsAdded = false;
	_vecPageObj pImgVec = GetImgVec();
	int cnt = 0;
	for (int i = 0; i < pImgVec.size(); i++){
		unsigned int matchFile = getHashCode((CStringA)pImgVec[i]->GetPath());
		
		CString strpath = pImgVec[i]->GetPath();
		USES_CONVERSION;
		char* sz = T2A(strpath);
		IplImage *pSrc = SINGLETON_TMat::GetInstance()->LoadIplImage(strpath, 1);

		std::vector<_MATCHInfo>& matches = pImgVec[i]->GetMatchResult();
		for (int j = 0; j < matches.size(); j++){

			if (matches[j].IsAdded == true)
				continue;


			unsigned int matchPos = (int)matches[j].rect.x1 * 10000 + (int)matches[j].rect.y1;

			CString strId;
			strId.Format(L"%u%u", matchFile, matchPos);
			unsigned int matchId = getHashCode((CStringA)strId);

			_MATCHResults matchRes;

			matchRes.searchId = matches[j].searchId;
			matchRes.cutId = matches[j].cInfo.id;
			matchRes.fileId = matches[j].cInfo.fileid;
			matchRes.posId = matches[j].cInfo.posid;
			matchRes.matchId = matchId;
			matchRes.matchFile = matchFile;
			matchRes.matchPos = matchPos;
			matchRes.accuracy = matches[j].accuracy;
			matchRes.fTh = matches[j].cInfo.th;


			if (pSrc != NULL){
			//	matchRes.pImgCut = cvCreateImage(cvSize(matches[j].rect.width, matches[j].rect.height), pSrc->depth, pSrc->nChannels);
				IplImage* pTmp = cvCreateImage(cvSize(matches[j].rect.width, matches[j].rect.height), pSrc->depth, pSrc->nChannels);
				cvSetImageROI(pSrc, cvRect(matches[j].rect.x1, matches[j].rect.y1, matches[j].rect.width, matches[j].rect.height));		// posx, posy = left - top
				cvCopy(pSrc, pTmp);
								


				int w = _NORMALIZE_SIZE_W, h = _NORMALIZE_SIZE_H;
				//if (matches[j].rect.width > pCut->width * 2.0f){
				//	w = 128;
				//}
				//if (matches[j].rect.height > pCut->height * 2.0f){
				//	h = 128;
				//}

				matchRes.pImgCut = cvCreateImage(cvSize(w, h), pTmp->depth, pTmp->nChannels);
				cvResize(pTmp, matchRes.pImgCut);
				cvReleaseImage(&pTmp);


				//Encode image file to base64 //
				cv::Mat m = cv::cvarrToMat(matchRes.pImgCut);
				std::vector<uchar> data_encode;
				imencode(".bmp", m, data_encode);
				matchRes.strBase64 = SINGLETON_TMat::GetInstance()->base64_encode((unsigned char*)&data_encode[0], data_encode.size());
				data_encode.clear();
				//===========================================//

				// Save Cut Image //
				CString strName;
				strName.Format(L"%s/%d_%u.bmp", m_strLogPath, (int)(matchRes.accuracy*100), matchId);
				cvSaveImage((CStringA)strName, matchRes.pImgCut);
			}

			m_matchResGroup[matchRes.searchId].matche.push_back(matchRes);
			m_matchResGroup[matchRes.searchId].searchId = matchRes.searchId;

			matches[j].IsAdded = true;
			IsAdded = true;

		}
	}
}

void CZDataManager::SetMatchingResultsExtraction()
{
	//	ResetMatchingResult();

	MATCHGROUP vecMatchRes;

	bool IsAdded = false;
	_vecPageObj pImgVec = GetImgVec();
	int cnt = 0;
	for (int i = 0; i < pImgVec.size(); i++){
		unsigned int matchFile = getHashCode((CStringA)pImgVec[i]->GetPath());


		std::vector<_MATCHInfo>& matches = pImgVec[i]->GetMatchResult();
		for (int j = 0; j < matches.size(); j++){

			if (matches[j].IsAdded == true)
				continue;


			unsigned int matchPos = (int)matches[j].rect.x1 * 10000 + (int)matches[j].rect.y1;

			CString strId;
			strId.Format(L"%u%u", matchFile, matchPos);
			unsigned int matchId = getHashCode((CStringA)strId);

			_MATCHResults matchRes;

			matchRes.searchId = matches[j].searchId;
			matchRes.cutId = matches[j].cInfo.id;
			matchRes.fileId = matches[j].cInfo.fileid;
			matchRes.posId = matches[j].cInfo.posid;
			matchRes.matchId = matchId;
			matchRes.matchFile = matchFile;
			matchRes.matchPos = matchPos;
			matchRes.accuracy = matches[j].accuracy;
			matchRes.fTh = matches[j].cInfo.th;
			matchRes.pImgCut = matches[j].pImg;
			matchRes.strCode = matches[j].strCode;


			m_matchResGroup[matchRes.searchId].matche.push_back(matchRes);
			m_matchResGroup[matchRes.searchId].searchId = matchRes.searchId;

			matches[j].IsAdded = true;
			IsAdded = true;

		}
	}
}

void CZDataManager::SortMatchingResults()
{
	// bubble sort //

	std::map<unsigned long, MATCHGROUP>::iterator iter_gr = m_matchResGroup.begin();

	for (; iter_gr != m_matchResGroup.end(); iter_gr++){
		int numItem = iter_gr->second.matche.size();
		if (numItem > 1){
			for (int i = 0; i < numItem - 1; i++)
			{
				for (int j = 0; j < numItem - i - 1; j++)
				{
					if (iter_gr->second.matche[j].accuracy < iter_gr->second.matche[j + 1].accuracy) /* For decreasing order use < */
					{
						_MATCHResults swap = iter_gr->second.matche[j];
						iter_gr->second.matche[j] = iter_gr->second.matche[j + 1];
						iter_gr->second.matche[j + 1] = swap;
					}
				}
			}
		}
	}
	
}

void CZDataManager::GenerateMatchingResultsImg()
{

	//IplImage* pImg = iter_gr->second.matche[i].pImgCut;
	//CString strBase64 = SINGLETON_TMat::GetInstance()->base64_encode((unsigned char*)pImg->imageData, pImg->imageSize);
	//SetItem(m_nRecordNum, 10, LVIF_TEXT, strBase64, 0, 0, 0, NULL);

	//cvSaveImage("d:/test.png", pImg);

}

CString CZDataManager::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	CString strEncode(ret.c_str());
	return strEncode;
}
std::string CZDataManager::base64_decode(std::string const& s)
{
	return "";
}

CBitmap* CZDataManager::GetLogCBitmap(IplImage* pImg)
{
	if (pImg){

		CDC dc;
		CDC memDC;

		CBitmap* bmp = new CBitmap;
		CBitmap* pOldBmp;

		if (!dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL))
			return NULL;

		if (!memDC.CreateCompatibleDC(&dc))
			return NULL;

		int w, h;
		int nWidth = pImg->width;
		int nHeight = pImg->height;
		BYTE* pSrcBits = (BYTE *)pImg->imageData;
		BYTE* pBmpBits = (BYTE *)malloc(sizeof(BYTE)*nWidth*nHeight * 4);


		// IplImage에 저장된 값을 직접 읽어서 
		// 비트맵 데이터를 만듬 
		for (h = 0; h < nHeight; ++h)
		{
			BYTE* pSrc = pSrcBits + pImg->widthStep * h;
			BYTE* pDst = pBmpBits + nWidth * 4 * h;
			for (w = 0; w < nWidth; ++w)
			{
				if (pImg->nChannels == 1){
					*(pDst++) = *(pSrc);
					*(pDst++) = *(pSrc);
					*(pDst++) = *(pSrc++);
					*(pDst++) = 0;
				}
				else if (pImg->nChannels==3){
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = 0;
				}
				else if (pImg->nChannels == 4){
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);;
				}
				
			}
		}
		//		memDC.CreateCompatibleDC(pDC);
		bmp->CreateCompatibleBitmap(&dc, nWidth, nHeight);
		// 위에서 만들어진 데이터를 가지고 
		// 비트맵을 만듬 
		bmp->SetBitmapBits(nWidth*nHeight * 4, pBmpBits);
		pOldBmp = memDC.SelectObject(bmp);


		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();
		dc.DeleteDC();

		return bmp;
	}
	else{
		return NULL;
	}
}