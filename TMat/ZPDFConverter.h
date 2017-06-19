#pragma once
#include "mupdf/pdf.h"

#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"



class CZPDFConverter
{
public:
	CZPDFConverter();
	~CZPDFConverter();

	IplImage* LoadPDF(CString strpath, unsigned short nChannel);

	
	

private:
	 

	
};

