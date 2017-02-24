#pragma once
#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "mupdf/pdf.h"

class CZPDFConverter
{
public:
	CZPDFConverter();
	~CZPDFConverter();

	IplImage* LoadPDF();

	

private:
	IplImage* m_pLoadImg;
	IplImage* m_pGrayImg;
	fz_context *m_ctx;
};

