#include "stdafx.h"
#include "ZPDFConverter.h"


CZPDFConverter::CZPDFConverter()
{
	m_ctx = fz_new_context(NULL, NULL, 0);
	m_pLoadImg = NULL;
}


CZPDFConverter::~CZPDFConverter()
{
	cvReleaseImage(&m_pLoadImg);
	cvReleaseImage(&m_pGrayImg);
	fz_drop_context(m_ctx);
}

IplImage* CZPDFConverter::LoadPDF()
{
	//fz_annot *annot;
	if (m_pLoadImg){
		cvReleaseImage(&m_pLoadImg);
	}
	if (m_pGrayImg){
		cvReleaseImage(&m_pGrayImg);
	}


	fz_pixmap *pix;

	pdf_document *pdf = NULL;
	fz_page *page = NULL;
	fz_document *doc = NULL;

	int currentpage = 0;

	
	fz_register_document_handlers(m_ctx);
	doc = fz_open_document(m_ctx, "C:/Temp/test.pdf");
	pdf = pdf_specifics(m_ctx, doc);
	fz_matrix page_ctm;
	fz_scale(&page_ctm, 1.337f, 1.337f);
//	fz_scale(&page_ctm, 2, 2);
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

	m_pLoadImg = cvCreateImage(cvSize(pix->w, pix->h), 8, 4);
	memcpy(m_pLoadImg->imageData, pix->samples, pix->w*pix->h * 4);
	m_pGrayImg = cvCreateImage(cvSize(m_pLoadImg->width, m_pLoadImg->height), 8, 1);
	cvCvtColor(m_pLoadImg, m_pGrayImg, CV_RGB2GRAY);//Change from RGB to GrayScale

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

	fz_drop_page(m_ctx, page);
	fz_drop_document(m_ctx, doc);
		
	return m_pGrayImg;
}