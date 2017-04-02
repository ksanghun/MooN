#include "stdafx.h"
#include "ZPDFConverter.h"


CZPDFConverter::CZPDFConverter()
{

}


CZPDFConverter::~CZPDFConverter()
{
	
}



IplImage* CZPDFConverter::LoadPDF(CString strpath, unsigned short nChannel)
{
	//fz_annot *annot;
	USES_CONVERSION;
	char* sz = T2A(strpath);

	IplImage* pGray = NULL;
	IplImage* pRgbImg = NULL;

	fz_pixmap *pix=NULL;
	pdf_document *pdf = NULL;
	fz_page *page = NULL;
	fz_document *doc = NULL;
	fz_context *ctx = fz_new_context(NULL, NULL, 0);

	int currentpage = 0;


	fz_register_document_handlers(ctx);
	doc = fz_open_document(ctx, sz);
	pdf = pdf_specifics(ctx, doc);
	fz_matrix page_ctm;
	fz_scale(&page_ctm, 1.337f, 1.337f);
	//fz_scale(&page_ctm, 2.5, 2.5);
	//fz_scale(&page_ctm, 2, 2);
	page = fz_load_page(ctx, doc, currentpage);
	pix = fz_new_pixmap_from_page_contents(ctx, page, &page_ctm, fz_device_rgb(ctx));

	//int idx = 0;

	pRgbImg = cvCreateImage(cvSize(pix->w, pix->h), 8, 4);
//	pRgbImg->imageData = (char*)pix->samples;
	memcpy(pRgbImg->imageData, pix->samples, pix->w*pix->h * 4);

	if (nChannel != 1){
		fz_drop_page(ctx, page);
		fz_drop_document(ctx, doc);
		fz_drop_pixmap(ctx, pix);
		fz_drop_context(ctx);
		return pRgbImg;
	}
	else{  // Channel is 1
		pGray = cvCreateImage(cvSize(pRgbImg->width, pRgbImg->height), 8, 1);
		cvCvtColor(pRgbImg, pGray, CV_RGB2GRAY);//Change from RGB to GrayScale
		
		fz_drop_page(ctx, page);
		fz_drop_document(ctx, doc);
		fz_drop_pixmap(ctx, pix);
		fz_drop_context(ctx);

		cvReleaseImage(&pRgbImg);

		return pGray;
	}
	return NULL;
}



/*
IplImage* CZPDFConverter::LoadPDF(CString strpath, unsigned short nChannel)
{
//fz_annot *annot;
USES_CONVERSION;
char* sz = T2A(strpath);

IplImage* pGray=NULL;
IplImage* pRgbImg = NULL;
fz_pixmap *pix;
pdf_document *pdf = NULL;
fz_page *page = NULL;
fz_document *doc = NULL;
fz_context *m_ctx = fz_new_context(NULL, NULL, 0);

int currentpage = 0;


fz_register_document_handlers(m_ctx);
doc = fz_open_document(m_ctx, sz);
pdf = pdf_specifics(m_ctx, doc);
fz_matrix page_ctm;
//	fz_scale(&page_ctm, 1.337f, 1.337f);
fz_scale(&page_ctm, 1, 1);
page = fz_load_page(m_ctx, doc, currentpage);
pix = fz_new_pixmap_from_page_contents(m_ctx, page, &page_ctm, fz_device_rgb(m_ctx));

//annot_count = 0;
//for (annot = fz_first_annot(ctx, page); annot; annot = fz_next_annot(ctx, annot))
//{
//	pix = fz_new_pixmap_from_annot(ctx, annot, &page_ctm, fz_device_rgb(ctx));
//	texture_from_pixmap(&annot_tex[annot_count++], pix);
//	fz_drop_pixmap(ctx, pix);
//}

int idx = 0;

pRgbImg = cvCreateImage(cvSize(pix->w, pix->h), 8, 4);
pRgbImg->imageData = (char*)pix->samples;
//	memcpy(pRgbImg->imageData, pix->samples, pix->w*pix->h * 4);

if (nChannel != 1){
fz_drop_page(m_ctx, page);
fz_drop_document(m_ctx, doc);
fz_drop_context(m_ctx);
return pRgbImg;
}
else{  // Channel is 1
pGray = cvCreateImage(cvSize(pRgbImg->width, pRgbImg->height), 8, 1);
cvCvtColor(pRgbImg, pGray, CV_RGB2GRAY);//Change from RGB to GrayScale

fz_drop_page(m_ctx, page);
fz_drop_document(m_ctx, doc);
fz_drop_context(m_ctx);
return pGray;
}


//for (int x = 1; x < pix->w-1; x++){
//	for (int y = 1; y < pix->h-1; y++){
//		idx = y*pix->w + x;

//		testimg->imageData[idx] = pix->samples[idx * 4];
//		//testimg->imageData[idx*4+1] = pix->samples[idx * 4+1];
//		//testimg->imageData[idx*4+2] = pix->samples[idx * 4+2];
//		//testimg->imageData[idx*4+3] = pix->samples[idx * 4+3];
//	}
//}
//
//IplImage *gray = cvCreateImage(cvSize(testimg->width, testimg->height), 8, 1);
//cvCvtColor(testimg, gray, CV_RGB2GRAY);//Change from RGB to GrayScale

//cvShowImage("Do it!", gray);




}

*/