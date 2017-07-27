#include "stdafx.h"
#include "Extractor.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ZDataManager.h"


#define _MAX_EXTRACT_ITERATION 20
#define _MIN_BOX_SIZE 2



class comparator{
public:
	bool operator()(std::vector<cv::Point> c1, std::vector<cv::Point>c2){
		return cv::boundingRect(cv::Mat(c1)).x<cv::boundingRect(cv::Mat(c2)).x;
	}
};

CExtractor::CExtractor()
{
	m_xExpand = 0;
	m_yExpand = 0;

	m_maxWidth = 0;
	m_maxHeight = 0;

	m_fineExtractCoff = 0;
//	m_languageType = _WESTERN_LANGUAGE;
	m_languageType = _ESTERN_LANGUAGE;
}


CExtractor::~CExtractor()
{
	for (int i = 0; i < m_exTextBox.size(); i++){
		if (m_exTextBox[i].pcutImg != NULL){
			cvReleaseImage(&m_exTextBox[i].pcutImg);
			m_exTextBox[i].pcutImg = NULL;
		}
	}
	m_exTextBox.clear();
}

void CExtractor::InitExtractor()
{
	m_xExpand = 0;
	m_yExpand = 0;

	m_maxWidth = 0;
	m_maxHeight = 0;

	m_exTextBox.clear();
	vecLines.clear();

}
void CExtractor::ProcDeskewing(cv::Mat img)
{
	
	// extract Contoures first //
	std::vector<cv::Rect> textbox;
//	cv::Mat resImg;
	//getContours(img, textbox, resImg);
	//cv::cvtColor(resImg, resImg, CV_BGR2GRAY);

	extractContours(img, textbox);
	//detectLetters(img, textbox);

//	cv::Mat resImg(cv::Mat(img.cols, img.rows, CV_8U));
//	cv::Mat resImg(img.rows, img.cols, CV_8UC1, cv::Scalar(255));
//	imshow("new img ", resImg);
	
	cv::Mat imgPanel(img.rows, img.cols, img.type(), cv::Scalar(255));

	for (int i = 0; i < textbox.size(); i++){
		cv::Mat crop = img(textbox[i]);
	//	imshow("before deskew img ", crop);
		DeSkew(crop);
	//	imshow("deskew img ", crop);
		crop.copyTo(imgPanel(textbox[i]));
	}

	imshow("res img ", imgPanel);


	cvtColor(img, img, CV_GRAY2BGR);
	for (int i = 0; i < textbox.size(); i++){
		cv::rectangle(img, textbox[i], cv::Scalar(0, 255, 0), 3, 8, 0);
	}
	//		cv::imwrite("imgOut1.jpg", img);

	cv::imshow("extraction", img);
}

void CExtractor::DeSkew(cv::Mat& srcimg)
{
	cv::Mat img;
	cv::threshold(srcimg, img, 100, 255, cv::THRESH_OTSU);

	cv::bitwise_not(img, img);

	std::vector<cv::Point> points;
	findNonZero(img, points);
	cv::RotatedRect box = cv::minAreaRect(points);

	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;

	cv::Point2f vertices[4];
	box.points(vertices);
	for (int i = 0; i < 4; ++i)
		cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0));


//	imshow("detact img ", img);

	// Rotate image //
	cv::Mat rotMat, rotatedFrame;
	cv::Point center = cv::Point(img.cols / 2, img.rows / 2);
	rotMat = getRotationMatrix2D(cv::Point2f(img.cols / 2, img.rows / 2), angle, 1);
	cv::warpAffine(img, rotatedFrame, rotMat, img.size(), cv::INTER_CUBIC);
//	imshow("img_rot", rotatedFrame);


	srcimg = rotatedFrame;
}

void CExtractor::extractContours(cv::Mat img, std::vector<cv::Rect>& boundRect)
{
	
	cv::Mat img_gray, img_sobel, img_threshold, element;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
	cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	element = getStructuringElement(cv::MORPH_RECT, cv::Size(10,3));
	cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
	std::vector< std::vector< cv::Point> > contours;
	cv::findContours(img_threshold, contours, 0, 1);
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++){
		if (contours[i].size()>100)
		{
			cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
			cv::Rect appRect(boundingRect(cv::Mat(contours_poly[i])));
			if (appRect.width > appRect.height)
				boundRect.push_back(appRect);
		}
	}

	// Merge bounding box //
}

void CExtractor::DeSkewImg()
{

	// Calculate Deskew //
	cv::Mat input = cv::imread("C:/Temp/deskew.bmp");
	cvtColor(input, input, CV_BGR2GRAY);
	cv::Mat img;
	cv::threshold(input, img, 100, 255, cv::THRESH_OTSU);

	cv::bitwise_not(img, img);
	imshow("img ", img);


	std::vector<cv::Point> points;
	findNonZero(img, points);
	cv::RotatedRect box = cv::minAreaRect(points);

	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;

	cv::Point2f vertices[4];
	box.points(vertices);
	for (int i = 0; i < 4; ++i)
		cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0));
	imshow("img ", img);
	
	// Rotate image //
	cv::Mat rotMat, rotatedFrame;
	cv::Point center = cv::Point(img.cols / 2, img.rows / 2);
	rotMat = getRotationMatrix2D(cv::Point2f(img.cols / 2, img.rows / 2), angle, 1);
	cv::warpAffine(img, rotatedFrame, rotMat, img.size(), cv::INTER_CUBIC);
	imshow("img_rot", rotatedFrame);
}


// TEST //
void CExtractor::detectLetters(cv::Mat& image, std::vector< std::vector<cv::Point> > contours_poly, std::vector<cv::Rect>& boundRect)
{



	//Sort contorus by x value going from left to right
	sort(contours_poly.begin(), contours_poly.end(), comparator());


	//Loop through all contours to extract
	for (int i = 0; i< contours_poly.size(); i++){

		cv::Rect r = cv::boundingRect(cv::Mat(contours_poly[i]));


		cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
		//Draw mask onto image
		drawContours(mask, contours_poly, i, cv::Scalar(255), CV_FILLED);

		//Check for equal sign (2 dashes on top of each other) and merge
		if (i + 1<contours_poly.size()){
			cv::Rect r2 = cv::boundingRect(cv::Mat(contours_poly[i + 1]));
			if (abs(r2.x - r.x)<20){
				//Draw mask onto image
				drawContours(mask, contours_poly, i + 1, cv::Scalar(255), CV_FILLED);
				i++;
				int minX = std::min(r.x, r2.x);
				int minY = std::min(r.y, r2.y);
				int maxX = std::max(r.x + r.width, r2.x + r2.width);
				int maxY = std::max(r.y + r.height, r2.y + r2.height);
				r = cv::Rect(minX, minY, maxX - minX, maxY - minY);

			}
		}
		//Copy
		cv::Mat extractPic;
		//Extract the character using the mask
		image.copyTo(extractPic, mask);
		cv::Mat resizedPic = extractPic(r);
		cv::Mat image = resizedPic.clone();

		boundRect.push_back(r);

		//Show image
	//	imwrite("Search Mask", resizedPic);
	//	imshow("Search Mask", resizedPic);

	}
}


void CExtractor::contractCharacters(cv::Mat img, std::vector<std::vector<cv::Point> >& contours)
{
	cv::Size size(3, 3);
	cv::GaussianBlur(img, img, size, 0);
	adaptiveThreshold(img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	cv::bitwise_not(img, img);
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
	//	cv::findContours(cropped, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0));
	cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

}

void CExtractor::getContours(cv::Mat& img, cv::Mat& oImg, std::vector<cv::Rect>& boundRect)
{
	if (0){
		cv::Mat tmpImg;
		tmpImg = img.clone();

		adaptiveThreshold(tmpImg, tmpImg, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
		cv::bitwise_not(tmpImg, tmpImg);

		std::vector<std::vector<cv::Point> > contours2;
		std::vector<cv::Vec4i> hierarchy2;
		/// Find contours
		//	cv::findContours(cropped, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0));
		cv::findContours(tmpImg, contours2, hierarchy2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

		/// Approximate contours to polygons + get bounding rects and circles
		std::vector<std::vector<cv::Point> > contours_poly2(contours2.size());
		for (int i = 0; i < contours2.size(); i++)
		{
			approxPolyDP(cv::Mat(contours2[i]), contours_poly2[i], 1, true);
		}

		cv::imshow("contour_before", img);
		cv::Scalar color = cv::Scalar(255);
		for (int i = 0; i < contours_poly2.size(); i++)
		{
			//	if (boundRect[i].area()<100)continue;
			drawContours(img, contours_poly2, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

			//	rectangle(cropped2, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		}
		cv::imshow("contour_after", img);
	}



//	cv::Mat img = cv::imread(filename, 0);

//	cv::imshow("original", img);

	//Apply blur to smooth edges and use adapative thresholding
	//cv::Size size(1, 1);
	//cv::GaussianBlur(img, img, size, 0);
	adaptiveThreshold(img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	cv::bitwise_not(img, img);


	//cv::Mat img2 = img.clone();


	//std::vector<cv::Point> points;
	//cv::Mat_<uchar>::iterator it = img.begin<uchar>();
	//cv::Mat_<uchar>::iterator end = img.end<uchar>();
	//for (; it != end; ++it)
	//	if (*it)
	//		points.push_back(it.pos());

	//cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	//double angle = box.angle;
	//if (angle < -45.)
	//	angle += 90.;

	//cv::Point2f vertices[4];
	//box.points(vertices);
	//for (int i = 0; i < 4; ++i)
	//	cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);



	//cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);

	//cv::Mat rotated;
	//cv::warpAffine(img2, rotated, rot_mat, img.size(), cv::INTER_CUBIC);

	//cv::imshow("Cropped", rotated);


	//cv::Size box_size = box.size;
	//if (box.angle < -45.)
	//	std::swap(box_size.width, box_size.height);
	//cv::Mat cropped;

	//cv::getRectSubPix(rotated, box_size, box.center, cropped);
//	cv::imshow("rotated", cropped);
//	imwrite("example5.jpg", cropped);






	
//
////	cv::Mat cropped3 = cropped.clone();
//	cv::Mat cropped3 = img.clone();
//	cvtColor(cropped3, cropped3, CV_GRAY2RGB);
//




	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
//	cv::findContours(cropped, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0));
	cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));


	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
//	std::vector<cv::Rect> boundRect(contours.size());
//	std::vector<cv::Point2f>center(contours.size());
//	std::vector<float>radius(contours.size());


	//Get poly contours
	
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
	}


	//Get only important contours, merge contours that are within another	
	//std::vector<std::vector<cv::Point> > validContours;



	ProcExtractTextBox(contours_poly, 0, 0);



	//cv::Scalar color = cv::Scalar(255);
	//for (int i = 0; i < contours_poly.size(); i++)
	//{
	//	//	if (boundRect[i].area()<100)continue;
	//	drawContours(img, contours_poly, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

	//	//	rectangle(cropped2, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	//}
	//cv::imshow("contour_polygon", img);




//	cv::Scalar color = cv::Scalar(255, 255, 255);
//	ProcExtractTextBox(contours_poly, img.cols, 0);
//	ProcExtractTextBox(contours_poly, m_averTextSize.width, m_averTextSize.height);





//Get bounding rects
//for (int i = 0; i < m_exTextBox.size(); i++){
//	//	boundRect[i] = cv::boundingRect(cv::Mat(validContours[i]));
//	boundRect.push_back(m_exTextBox[i].textbox);
//}


//Display
//cv::Scalar color = cv::Scalar(255);
//for (int i = 0; i< contours_poly.size(); i++)
//{
//	drawContours(img, contours_poly, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
//}
//cv::imshow("contgour", img);


//imwrite("example6.jpg",cropped2);
//	imshow("Contours", cropped2);

//	detectLetters(cropped3, validContours, boundRect);



}

void CExtractor::ProcExtractTextBox(std::vector<std::vector<cv::Point> >& contour, int maxWidth, int maxHeight)
{
	m_exTextBox.clear();
	// Vaildation contour =======================================//
	//	std::vector<cv::Rect> boundRect;
	int minSize = 16;
	//int maxWidth = 0, maxHeight = 0;

	m_maxWidth = 0;
	m_maxHeight = 0;
	

	float aRatio = 0.0f;
	for (int i = 0; i < contour.size(); i++){

		cv::Rect r = cv::boundingRect(cv::Mat(contour[i]));
		if (r.area() < minSize)continue;

		bool inside = false;
		for (int j = 0; j < contour.size(); j++){
			if (j == i)continue;

			cv::Rect r2 = cv::boundingRect(cv::Mat(contour[j]));
			if (r2.area() < minSize || r2.area() < r.area())continue;
			if (r.x > r2.x&&r.x + r.width<r2.x + r2.width&&
				r.y>r2.y&&r.y + r.height < r2.y + r2.height){
				inside = true;
			}
		}
		if (inside)continue;

		_EXTRACT_BOX textBox;
		textBox.init();
		textBox.textbox = cv::boundingRect(cv::Mat(contour[i]));
		textBox.textboxForCheck = textBox.textbox;


		// Find the max size of page, expand some along to x-axis ( In case of left to right, it should be y-axis expend //

		
		aRatio = (float)textBox.textbox.width / (float)textBox.textbox.height;
		if ((aRatio > 0.5f) && (aRatio < 2.0f)){
			if (textBox.textbox.width > m_maxWidth){
				m_maxWidth = textBox.textbox.width;
			}

			if (textBox.textbox.height > m_maxHeight){
				m_maxHeight = textBox.textbox.height;
			}
		}


		//if (maxWidth < maxHeight){
		//	maxHeight = maxWidth;
		//}

		

		// !!X-Axix expension by 3 pixel//
		float fExpand = 1;
		textBox.textboxForCheck.x -= fExpand;
		textBox.textboxForCheck.width += fExpand * 2;

		fExpand = 1;
		textBox.textboxForCheck.y -= fExpand;
		textBox.textboxForCheck.height += fExpand * 2;
		//=====================================//

		m_exTextBox.push_back(textBox);
	}


	
	m_maxWidth *= 0.9f;
	m_maxHeight *= 0.9f;

	m_maxWidth += m_xExpand;
	m_maxHeight += m_yExpand;

	if (m_maxWidth < 0)	
		m_maxWidth = 0;
	if (m_maxHeight < 0)	
		m_maxHeight = 0;

	if (maxWidth > 0){
		m_maxWidth = maxWidth;
	}

	if (maxHeight > 0){
		m_maxHeight = maxHeight;
	}


// !! sorting ======================//
	//std::vector<_EXTRACT_BOX> tmp = m_exTextBox;
	//m_exTextBox = std::vector<_EXTRACT_BOX>();

	//_EXTRACT_BOX tmp;
	//for (int i = 0; i < m_exTextBox.size(); i++){
	//	for (int j = 0; j < m_exTextBox.size(); j++){

	//		if (j == i) continue;
	//		if (m_exTextBox[i].textbox.y < m_exTextBox[j].textbox.y){
	//			tmp = m_exTextBox[i];
	//			m_exTextBox[i] = m_exTextBox[j];
	//			m_exTextBox[j] = tmp;
	//		}
	//	}
	//}


	//addcnt++;
	//if ((m_maxHeight == 0) && (m_maxWidth == 0))
	//{
	//	m_maxWidth = maxWidth;
	//	m_maxHeight = maxHeight;
	//}
	

	// Initial extraction //
	int depth = 0;
	RcvMergeTextBox(m_maxWidth, m_maxHeight, aRatio, depth);

	// check ambigous ..
//	float area = m_averTextSize.width*m_averTextSize.height;
	float area = m_maxWidth*m_maxHeight;
	aRatio = (float)m_maxWidth / (float)m_maxHeight;

	for (int i = 0; i < m_exTextBox.size(); i++){
		float r = (float)m_exTextBox[i].textbox.width / (float)m_exTextBox[i].textbox.height;

		if (m_exTextBox[i].IsAmbig == false){


			if ((m_exTextBox[i].textbox.area() < area*0.1f) ||
				(m_exTextBox[i].textbox.area() > area*1.25f))
			{
				m_exTextBox[i].IsAmbig = true;
			}
			else{
				for (int j = 0; j < m_exTextBox.size(); j++){
					if (j != i){
						if (IsBoxToBoxIntersect(m_exTextBox[i].textbox, m_exTextBox[j].textbox)==true){
							m_exTextBox[i].IsAmbig = true;
							m_exTextBox[j].IsAmbig = true;
							break;
						}
					}
				}
			}
		}
	}
}

RECT2D CExtractor::GetSearchingRect()
{
	RECT2D rect;
	rect.set(0, m_maxWidth, 0, m_maxHeight);
	return rect;
}

bool CExtractor::RcvMergeTextBox(int maxwidth, int maxheight, float aRatio, int& depth)
{	
	std::vector<_EXTRACT_BOX> tmp = m_exTextBox;
	m_exTextBox = std::vector<_EXTRACT_BOX>();

	m_averTextSize.width = 0;
	m_averTextSize.height = 0;
	int addcnt = 0;

	int newWidth = 0, newHeight = 0;
	bool IsMerged = false;
	for (int i = 0; i < tmp.size(); i++){
//	for (int i = tmp.size()-1; i >=0; i--){
		if (tmp[i].IsMerged) continue;

		for (int j = 0; j < tmp.size(); j++){
//		for (int j = tmp.size()-1; j >=0; j--){
			if (j != i){
				if (tmp[j].IsMerged) continue;

				if (tmp[i].textbox.y > tmp[j].textbox.y)	continue;

				//if ((abs(tmp[i].textboxForCheck.x - tmp[j].textboxForCheck.x) < maxwidth) &&
				//	(abs(tmp[i].textboxForCheck.y - tmp[j].textboxForCheck.y) < maxheight)){

				if (IsBoxToBoxIntersect(tmp[i].textboxForCheck, tmp[j].textboxForCheck) == true){
//				if (IsBoxToBoxIntersect(tmp[i].textbox, tmp[j].textbox) == true){

					// Merge Two box //
					int x, y, x2, y2, width, height;

					if (tmp[i].textbox.x < tmp[j].textbox.x)
						x = tmp[i].textbox.x;
					else
						x = tmp[j].textbox.x;
					if (tmp[i].textbox.x + tmp[i].textbox.width > tmp[j].textbox.x + tmp[j].textbox.width)
						x2 = tmp[i].textbox.x + tmp[i].textbox.width;
					else
						x2 = tmp[j].textbox.x + tmp[j].textbox.width;


					if (tmp[i].textbox.y < tmp[j].textbox.y)
						y = tmp[i].textbox.y;
					else
						y = tmp[j].textbox.y;
					if (tmp[i].textbox.y + tmp[i].textbox.height > tmp[j].textbox.y + tmp[j].textbox.height)
						y2 = tmp[i].textbox.y + tmp[i].textbox.height;
					else
						y2 = tmp[j].textbox.y + tmp[j].textbox.height;

					width = x2 - x;
					height = y2 - y;

					if ((width < (maxwidth*1.25f)) && (height < (maxheight*1.25f))){
						tmp[i].textbox.x = x;
						tmp[i].textbox.y = y;
						tmp[i].textbox.width = width;
						tmp[i].textbox.height = height;

						tmp[j].IsMerged = true;
						IsMerged = true;

						break;
					}
				}
			}
		}
	}



	for (int i = 0; i < tmp.size(); i++){
		if (tmp[i].IsMerged == false){
			_EXTRACT_BOX tbox;
			tbox.init();
			tbox.textbox = tmp[i].textbox;
			tbox.textboxForCheck = tmp[i].textbox;


			float fExpand = 4;
			tbox.textboxForCheck.x -= fExpand;
			tbox.textboxForCheck.width += fExpand * 2;

			fExpand = 2;
			tbox.textboxForCheck.y -= fExpand;
			tbox.textboxForCheck.height += fExpand * 2;

			// Filtering line//
			if ((tbox.textbox.height > m_maxHeight*2.0f) || (tbox.textbox.width > m_maxWidth*2.0f))
			{
				float ar = (float)tbox.textbox.width / (float)tbox.textbox.height;
				if ((ar<0.1f) || (ar>10.0f)){
					continue;
				}
			}
			m_exTextBox.push_back(tbox);
		}
		//m_exTextBox.push_back(tmp[i]);

		m_averTextSize.width += tmp[i].textbox.width;
		m_averTextSize.height += tmp[i].textbox.height;
		addcnt++;
	}
	tmp.clear();


	if (addcnt > 0){
		float aWidth = (float)m_averTextSize.width / (float)addcnt;
		float aHeight = (float)m_averTextSize.height / (float)addcnt;
		m_averTextSize.set(0, aWidth, 0, aHeight);
	}
	
	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)){
		//maxwidth = newWidth;
		//maxheight = newHeight;

		//maxwidth = (maxwidth + m_averTextSize.width)*0.5f;
		//maxheight = (maxheight + m_averTextSize.height)*0.5f;

		RcvMergeTextBox(maxwidth, maxheight, aRatio, depth);
	//	RcvMergeTextBox(m_averTextSize.width, m_averTextSize.height, aRatio, depth);
	}
	depth++;


	


	return true;
}

int CExtractor::IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2)
{
	cv::Rect a1, a2;
	a1 = b1;
	a2 = b2;

	if (b2.area() < b1.area()){
		a1 = b2;
		a2 = b1;
	}
	

	if (((a1.x + a1.width) <= a2.x) || (a1.x >= (a2.x + a2.width)) || ((a1.y + a1.height) <= a2.y) || (a1.y >= (a2.y + a2.height))){
//	if (((a1.x + a1.width) < a2.x) || (a1.x > (a2.x + a2.width)) || ((a1.y + a1.height) < a2.y) || (a1.y > (a2.y + a2.height))){
		return 0;
	}
	

	//int magOverlap = 0;
	//int diff[4] = { 0, };
	//diff[0] = (a1.x + a1.width) - a2.x;
	//diff[1] = (a2.x + a2.width) - a1.x;
	//diff[2] = (a1.y + a1.height) - a2.y;
	//diff[3] = (a2.y + a2.height) - a2.y;

	//for (int i = 0; i < 4; i++){
	//	if (magOverlap < diff[i])
	//		magOverlap = diff[i];
	//}

	return true;
}


bool CExtractor::IsSphereToSphereIntersect(_MOONCircle c1, _MOONCircle c2)
{
	float fDist = (c1.center.x - c2.center.x)*(c1.center.x - c2.center.x) + (c1.center.y - c2.center.y)*(c1.center.y - c2.center.y);
	fDist = sqrt(fDist);
	if (fDist > (c1.fRadious + c2.fRadious)){
		return false;
	}
	return true;


	//cv::Rect a1, a2;
	//a1 = b1;
	//a2 = b2;

	//if (b2.area() < b1.area()){
	//	a1 = b2;
	//	a2 = b1;
	//}

	//if (((a1.x + a1.width) <= a2.x) || (a1.x >= (a2.x + a2.width)) || ((a1.y + a1.height) <= a2.y) || (a1.y >= (a2.y + a2.height))){
	//	//	if (((a1.x + a1.width) < a2.x) || (a1.x > (a2.x + a2.width)) || ((a1.y + a1.height) < a2.y) || (a1.y > (a2.y + a2.height))){
	//	return false;
	//}

	return true;
}

void CExtractor::extractWithOCR(cv::Mat image, std::vector<cv::Rect>& boundRect)
{
	/*
//	const char CurDir[MAX_PATH];
//	GetCurrentDirectory(MAX_PATH, CurDir);
	
	tesseract::TessBaseAPI tess;
	tess.Init("./", "eng");


	//==================//
	//tess.SetPageSegMode(tesseract::PSM_AUTO_OSD);
	//======================//
	tess.SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());
	tess.Recognize(0);


	tesseract::Orientation orientation;
	tesseract::WritingDirection direction;
	tesseract::TextlineOrder order;
	float deskew_angle;
	//tesseract::PageIterator* it = tess.AnalyseLayout();
	//it->Orientation(&orientation, &direction, &order, &deskew_angle);
	//printf("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
		//	"Deskew angle: %.4f\n",
	//	orientation, direction, order, deskew_angle);


	const char* out = tess.GetUTF8Text();

	//
	tesseract::PageIteratorLevel level;
	level = tesseract::RIL_WORD;
	
	tesseract::ResultIterator* ri = tess.GetIterator();
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			//	ri->Orientation(&orientation, &direction, &order, &deskew_angle);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			//printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
			//	word, conf, x1, y1, x2, y2);
			//	delete[] word;

			//rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 2);

			boundRect.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
		} while (ri->Next(level));
	}
//	imshow("Display window", image); // Show our image inside it.

*/
}


void CExtractor::TestFunc()
{
//	std::vector<cv::Rect> textbox;
//	cv::Mat image = cv::imread("C:/Temp/deskew.bmp");
//	extractWithOCR(image);
//	ProcDeskewing();
//	DeSkewImg();
//	getContours("C:/Temp/deskew.bmp");
}


void CExtractor::ChangeXExpand(int _d)
{
	m_xExpand += _d; 
	m_maxWidth += _d;
}
void CExtractor::ChangeYExpand(int _d)
{
	m_yExpand += _d; 
	m_maxHeight += _d;
}

cv::Rect CExtractor::GetBoundingBox(cv::Rect r1, cv::Rect r2)
{
	cv::Rect rect;

	int x1, x2, y1, y2;
	if (r1.x < r2.x)		x1 = r1.x;
	else					x1 = r2.x;

	if (r1.y < r2.y)		y1 = r1.y;
	else					y1 = r2.y;


	if ((r1.x + r1.width) >(r2.x + r2.width))		x2 = r1.x + r1.width;
	else											x2 = r2.x + r2.width;

	if ((r1.y + r1.height) >(r2.y + r2.height))		y2 = r1.y + r1.height;
	else											y2 = r2.y + r2.height;

	rect.x = x1-1;
	rect.y = y1-1;
	rect.width = x2 - x1 +2;
	rect.height = y2 - y1 +2;

	return rect;
}

// Extraction Functions ===================//
void CExtractor::ProcFineExtraction(cv::Mat& img, _TEXT_ORDER _torder, int _w, int _h)
{
//	ContractImage(img);

	CheckAmbiguous();
	// Detect Contours========================================================================//
	//adaptiveThreshold(img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	//cv::bitwise_not(img, img);
	
	std::vector<_EXTRACT_BOX> tmp;
	//= m_exTextBox;
//	m_exTextBox = std::vector<_EXTRACT_BOX>();
//	int area = _w*_h;

	for (int i = 0; i < m_exTextBox.size(); i++){
		if (m_exTextBox[i].IsMerged == true) continue;
		
		int width = m_exTextBox[i].detectWidth;
		int height = m_exTextBox[i].detectHeight;

		if ((m_exTextBox[i].IsBig)){
			m_exTextBox[i].IsAmbig = true;
		//	int area = m_exTextBox[i].detectArea;
		//	int detectSize = sqrt(area);
			

			std::vector<_EXTRACT_BOX> vecBox;

		//	if (m_exTextBox[i].IsBig){				
				FineExtractTexts(img, m_exTextBox[i].textbox, vecBox, _torder, width, height, 0, true);
				for (int j = 0; j < vecBox.size(); j++){
					_EXTRACT_BOX tmpbox = vecBox[j];
					tmpbox.textbox.x += m_exTextBox[i].textbox.x;		// Translate position //
					tmpbox.textbox.y += m_exTextBox[i].textbox.y;
				//	tmpbox.detectArea = detectSize*detectSize;
					tmpbox.detectWidth = width;
					tmpbox.detectHeight = height;

					// recover contracted size to original size==========//
					//tmpbox.textbox.x--;
					//tmpbox.textbox.y--;
					//tmpbox.textbox.width += 2;
					//tmpbox.textbox.height += 2;
					//======================================================//

					tmp.push_back(tmpbox);
				}
		//	}
			//else{
			//	FineExtractTexts(img, m_exTextBox[i].textbox, vecBox, _torder, detectSize, detectSize, 0, false);
			//	for (int j = 0; j < vecBox.size(); j++){
			//		_EXTRACT_BOX tmpbox = vecBox[j];
			//		tmpbox.textbox.x += m_exTextBox[i].textbox.x;		// Translate position //
			//		tmpbox.textbox.y += m_exTextBox[i].textbox.y;
			//		tmpbox.detectArea = detectSize*detectSize;
			//		tmp.push_back(tmpbox);
			//	}
			//}			
			vecBox.clear();
		}

		if (m_exTextBox[i].pNextBox != NULL){
			_EXTRACT_BOX* pBox = m_exTextBox[i].pNextBox;
			m_exTextBox[i].IsMerged = true;
			m_exTextBox[i].IsAmbig = true;
			pBox->IsMerged = true;
			pBox->IsAmbig = true;
			
			cv::Rect rect = GetBoundingBox(m_exTextBox[i].textbox, m_exTextBox[i].pNextBox->textbox);

			while (pBox->pNextBox != NULL){						
				pBox = pBox->pNextBox;
				rect = GetBoundingBox(rect, pBox->textbox);

				pBox->IsMerged = true;
				pBox->IsAmbig = true;
			}

			int area = width*height;
			std::vector<_EXTRACT_BOX> vecBox;
		//	int detectSize = sqrt(area);  // littel smaller //

			if ((rect.width > 0) || (rect.height > 0)){

				if (rect.area() > area*1.25f){
					FineExtractTexts(img, rect, vecBox, _torder, width, height, m_fineExtractCoff, false);
				}
				else{
					FineExtractTexts(img, rect, vecBox, _torder, width, height, -m_fineExtractCoff, false);
				}

				for (int j = 0; j < vecBox.size(); j++){
					_EXTRACT_BOX tmpbox = vecBox[j];
					tmpbox.textbox.x += rect.x;		// Translate position //
					tmpbox.textbox.y += rect.y;
				//	tmpbox.detectArea = detectSize*detectSize;
				//	tmpbox.detectArea = area;
					tmpbox.detectWidth = width;
					tmpbox.detectHeight = height;
					tmp.push_back(tmpbox);
				}
			}
			vecBox.clear();
		}

		if (m_exTextBox[i].IsAmbig == false){
			m_exTextBox[i].pNextBox = NULL;
			tmp.push_back(m_exTextBox[i]);
		//	m_exTextBox.push_back(tmp[i]);
		}
	}

	m_exTextBox = std::vector<_EXTRACT_BOX>();
	m_exTextBox = tmp;
	tmp.clear();















	/*

	//	aRatio = (float)m_maxWidth / (float)m_maxHeight;
	for (int i = 0; i < tmp.size(); i++){
		tmp[i].IsAmbig = false;
		tmp[i].IsMerged = false;
	}
	for (int i = 0; i < tmp.size(); i++){

		if (tmp[i].IsMerged == true) continue;

		float r = (float)tmp[i].textbox.width / (float)tmp[i].textbox.height;

		if (tmp[i].IsAmbig == false){
			int area = tmp[i].detectArea;
			if ((tmp[i].textbox.area() > area*1.5f)){
				tmp[i].IsAmbig = true;

				std::vector<_EXTRACT_BOX> vecBox;
				int detectSize = sqrt(area);
				FineExtractTexts(img, tmp[i].textbox, vecBox, _torder, detectSize, detectSize);

				for (int j = 0; j < vecBox.size(); j++){
					_EXTRACT_BOX tmpbox = vecBox[j];
					tmpbox.textbox.x += tmp[i].textbox.x;		// Translate position //
					tmpbox.textbox.y += tmp[i].textbox.y;
					tmpbox.detectArea = detectSize*detectSize;
					m_exTextBox.push_back(tmpbox);
				}
				vecBox.clear();
			}
			else{
				for (int j = 0; j < tmp.size(); j++){

					if (tmp[j].IsMerged == true) continue;

					if (j != i){
						if (tmp[i].lineId == tmp[j].lineId){		// In case of Same line //
							if (IsBoxToBoxIntersect(tmp[i].textbox, tmp[j].textbox) != 0){
								// Fine Extraction //
								tmp[i].IsAmbig = true;
								tmp[j].IsAmbig = true;

								tmp[i].IsMerged = true;
								tmp[j].IsMerged = true;

								std::vector<_EXTRACT_BOX> vecBox;
								int detectSize = sqrt(area);  // littel smaller //
								cv::Rect rect = GetBoundingBox(tmp[i].textbox, tmp[j].textbox);
								
								FineExtractTexts(img, rect, vecBox, _torder, detectSize, detectSize);

								for (int j = 0; j < vecBox.size(); j++){
									_EXTRACT_BOX tmpbox = vecBox[j];
									tmpbox.textbox.x += rect.x;		// Translate position //
									tmpbox.textbox.y += rect.y;
									tmpbox.detectArea = detectSize*detectSize;
									m_exTextBox.push_back(tmpbox);
								}
								vecBox.clear();

								break;
							}
						}
					}
				}
			}
		}
		if (tmp[i].IsAmbig == false){
			m_exTextBox.push_back(tmp[i]);
		}
	}
	tmp.clear();
	
	CheckAmbiguous();
*/

	CheckAmbiguous();

	m_fineExtractCoff++;
	if (m_fineExtractCoff > 5)
		m_fineExtractCoff = 1;
}


void CExtractor::ProcExtractionLine(cv::Mat& img, _TEXT_ORDER _torder, int _w, int _h)
{
	vecLines.clear();
// Detect Contours========================================================================//
	adaptiveThreshold(img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	cv::bitwise_not(img, img);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
	cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());

	//Get poly contours
	float aRatio = 0;
	int minSize = 16;
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);

	}

	int maxwidth = _w;
	int maxheight = _h;

	if (maxwidth < _MIN_BOX_SIZE)	maxwidth = _MIN_BOX_SIZE;
	if (maxheight < _MIN_BOX_SIZE)  maxheight = _MIN_BOX_SIZE;

// Detect Lines===========================================================================//	
	if (_torder == V_ORDER){		// Vertical
	//	maxheight = img.rows;
		DetectLines(contours_poly, vecLines, maxwidth, img.rows, 0, maxwidth);		// 1.2f for rotation //
	}
	else{
	//	maxwidth = img.cols;
		DetectLines(contours_poly, vecLines, img.cols, maxheight, maxheight, 0);
	}
//=========================================================================================//
	

}



void CExtractor::ProcExtractionText(cv::Mat& img, _TEXT_ORDER _torder, int _w, int _h)
{
	// ExtractTexts ===========================================================//
	m_fineExtractCoff = 0;

	adaptiveThreshold(img, img, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	cv::bitwise_not(img, img);

	for (int i = 0; i < m_exTextBox.size(); i++){
		if (m_exTextBox[i].pcutImg != NULL){
			cvReleaseImage(&m_exTextBox[i].pcutImg);
		}
	}
	m_exTextBox.clear();

	std::vector<_EXTRACT_BOX> vecBox;
	for (int i = 0; i < vecLines.size(); i++){
		vecBox.clear();
		ExtractTexts(img, vecLines[i].textbox, vecBox, _torder);

		for (int j = 0; j < vecBox.size(); j++){
			_EXTRACT_BOX tmp = vecBox[j];
			tmp.textbox.x += vecLines[i].textbox.x;		// Translate position //
			tmp.textbox.y += vecLines[i].textbox.y;
			tmp.textSphere.setbyRect(tmp.textbox);
			tmp.lineId = i;
			m_exTextBox.push_back(tmp);
		}
	}
	//	oImg.release();
}

void CExtractor::ExtractTexts(cv::Mat& img, cv::Rect lineBox, std::vector<_EXTRACT_BOX>& vecBox, _TEXT_ORDER _torder)
{
	cv::Mat imgCut;
	img(lineBox).copyTo(imgCut);

// Find Contour //
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
	cv::findContours(imgCut, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());

	float aRatio = 0;
	int minSize = 25;
	int maxwidth = 0, maxheight = 0;
	int cnt = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
		cv::Rect r = cv::boundingRect(cv::Mat(contours[i]));
		if (r.area() < minSize) continue;

		aRatio = (float)r.width / (float)r.height;
		if ((aRatio > 0.1f) && (aRatio < 10.0f)){
			if (r.width > maxwidth){
				maxwidth = r.width;
			}
			if (r.height > maxheight){
				maxheight = r.height;
			}
		}
	}


	if (maxwidth < _MIN_BOX_SIZE)	maxwidth = _MIN_BOX_SIZE;
	if (maxheight < _MIN_BOX_SIZE)  maxheight = _MIN_BOX_SIZE;

	if (_torder == V_ORDER){		// Vertical
		DetectChars(contours_poly, vecBox, lineBox.width, maxheight, 3,2);
	}
	else{  // hori
		if (m_languageType == _WESTERN_LANGUAGE){
			maxwidth = lineBox.width;
			DetectChars(contours_poly, vecBox, maxwidth, lineBox.height, 0, 2);
		}
		else{
			DetectChars(contours_poly, vecBox, maxwidth, lineBox.height, 0, 2);
		}
	}


	//std::vector<_EXTRACT_BOX> tmp = vecBox;
	//vecBox = std::vector<_EXTRACT_BOX>();


	// Checkl ambigous..................===================================//
	float area = 0.0f;

	if (m_languageType == _WESTERN_LANGUAGE){
		area = maxwidth*maxheight;
	}
	else{
		if (maxwidth < maxheight){
			area = maxwidth*maxwidth;
		}
		else{
			area = maxheight*maxheight;
		}
	}


	
	//	aRatio = (float)m_maxWidth / (float)m_maxHeight;
	for (int i = 0; i < vecBox.size(); i++){
		float r = (float)vecBox[i].textbox.width / (float)vecBox[i].textbox.height;
//		vecBox[i].detectArea = area;
		vecBox[i].detectWidth = maxwidth;
		vecBox[i].detectHeight = maxheight;

		if (vecBox[i].IsAmbig == false){
			if ((vecBox[i].textbox.area() > area*1.5f)){
			//	vecBox[i].IsAmbig = true;
				vecBox[i].IsBig = true;
				
			}
			else{
				for (int j = 0; j < vecBox.size(); j++){
					if (j != i){
						if (IsBoxToBoxIntersect(vecBox[i].textbox, vecBox[j].textbox) != 0){
							// Fine Extraction //
							vecBox[i].IsAmbig = true;
						//	vecBox[j].IsAmbig = true;
							if (vecBox[i].pNextBox == NULL){								
								if (vecBox[j].pNextBox != &vecBox[i]){
									vecBox[i].pNextBox = &vecBox[j];
								}
								else{
									TRACE("Dup");
								}
							}

							

							break;
						}
					}
				}
			}
		}

		//if (vecBox[i].IsAmbig == false){
		//	vecBox.push_back(tmp[i]);
		//}

	}

//	vecBox.clear();


//=================================================================//
//	cv::imshow("cut", imgCut);
	imgCut.release();
}

void CExtractor::DetectLines(std::vector<std::vector<cv::Point> >& contour, std::vector<_EXTRACT_BOX>& veclineBox, int maxWidth, int maxHeight, int extX, int extY)
{
//	float aRatio = 0.0f;
	int minSize = 25;
	for (int i = 0; i < contour.size(); i++){
		cv::Rect r = cv::boundingRect(cv::Mat(contour[i]));

		_EXTRACT_BOX textBox;
		textBox.init();
		textBox.textbox = r;
		textBox.setExtendBox(extX, extY);
		textBox.textSphere.setbyRect(textBox.textbox);

		if (textBox.textbox.area() < minSize) continue;

		//bool inside = false;
		//for (int j = 0; j < contour.size(); j++){
		//	if (j == i)continue;
		//	cv::Rect r2 = cv::boundingRect(cv::Mat(contour[j]));

		//	if (r2.area() < minSize || r2.area() < r.area())continue;
		//	if (r.x > r2.x&&r.x + r.width<r2.x + r2.width&&
		//		r.y>r2.y&&r.y + r.height < r2.y + r2.height){
		//		inside = true;
		//	}
		//}
		//if (inside)continue;



		if ((textBox.textbox.width < maxWidth*2.0f) && (textBox.textbox.height < maxHeight*2.0f)){
			veclineBox.push_back(textBox);
		}
	}

	// TEST  : Sort by Size min to max ============/
	//_EXTRACT_BOX tmp;
	//for (int i = 0; i < veclineBox.size(); i++){
	//	for (int j = 0; j < veclineBox.size(); j++){
	//		if (i != j){
	//			if (veclineBox[j].textbox.area() > veclineBox[i].textbox.area()){
	//				tmp = veclineBox[i];
	//				veclineBox[i] = veclineBox[j];
	//				veclineBox[j] = tmp;
	//			}
	//		}
	//	}
	//}
	//==================================//

	// Initial extraction //
	int depth = 0;
	RcvMeargingBoundingBox(maxWidth, maxHeight, veclineBox, depth, extX, extY, _MERGE_LINE);

}

void CExtractor::DetectChars(std::vector<std::vector<cv::Point> >& contour, std::vector<_EXTRACT_BOX>& veclineBox, int _width, int _height, int extX, int extY)
{
	//!! Next step: Convex Hull Check !!//
	//Input: a list P of points in the plane.

	//Sort the points of P by x - coordinate(in case of a tie, sort by y - coordinate).

	//Initialize U and L as empty lists.
	//The lists will hold the vertices of upper and lower hulls respectively.

	//for i = 1, 2, ..., n:
	//   while L contains at least two points and the sequence of last two points
	//	   of L and the point P[i] does not make a counter - clockwise turn :
	//   remove the last point from L
	//	   append P[i] to L

	//	   for i = n, n - 1, ..., 1 :
	//		   while U contains at least two points and the sequence of last two points
	//			   of U and the point P[i] does not make a counter - clockwise turn :
	//   remove the last point from U
	//	   append P[i] to U

	//Remove the last point of each list(it's the same as the first point of the other list).
	//Concatenate L and U to obtain the convex hull of P.
	//Points in the result will be listed in counter - clockwise order.
	//===========================================================================================================================//


	//	float aRatio = 0.0f;
	int minSize = 4;
	for (int i = 0; i < contour.size(); i++){
		cv::Rect r = cv::boundingRect(cv::Mat(contour[i]));


		_EXTRACT_BOX textBox;
		textBox.init();
		textBox.textbox = r;

		float aRatio = 1.0f;
		if (textBox.textbox.height > 0){
			aRatio = (float)textBox.textbox.width / (float)textBox.textbox.height;
		}

		textBox.setExtendBox(extX, extY);
		textBox.textSphere.setbyRect(textBox.textbox);

		if (textBox.textbox.area() < minSize) continue;

		bool inside = false;
		for (int j = 0; j < contour.size(); j++){
			if (j == i)continue;
			cv::Rect r2 = cv::boundingRect(cv::Mat(contour[j]));

			if (r2.area() < minSize || r2.area() < r.area())continue;
			if (r.x > r2.x&&r.x + r.width<r2.x + r2.width&&
				r.y>r2.y&&r.y + r.height < r2.y + r2.height){
				inside = true;
			}
		}
		if (inside)continue;

	//	if ((textBox.textbox.area() <= maxSize*maxSize)){
		if ((textBox.textbox.width < _width*2.0f) && (textBox.textbox.height < _height*2.0f)){
			veclineBox.push_back(textBox);
		}
	}




	// TEST  : Sort by Size min to max ============/
	//_EXTRACT_BOX tmp;
	//for (int i = 0; i < veclineBox.size(); i++){
	//	for (int j = 0; j < veclineBox.size(); j++){
	//		if (i != j){
	//			if (veclineBox[j].textbox.area() < veclineBox[i].textbox.area()){
	//				tmp = veclineBox[i];
	//				veclineBox[i] = veclineBox[j];
	//				veclineBox[j] = tmp;
	//			}
	//		}
	//	}
	//}
	//==================================//
	
	// Initial extraction //
	int depth = 0;
//	RcvMeargingBoundingCircle(minsize, maxSize, veclineBox, depth, extX, extY);

	
	
	RcvMeargingBoundingBox(_width, _height, veclineBox, depth, extX, extY, _MERGE_TEXT);
}


int CExtractor::FindOptimalCircle(std::vector<_EXTRACT_BOX>& tmp, int i, int minSize, int maxSize, _EXTRACT_BOX& resBox)
{
	int x, y, x2, y2;
	float width, height;
	//	float areaRatio = 10000.0f;
	//	float mergeArea = 1000000.0f;
	float minRadious = 100000.0f;
	int selectdId = -1;
	for (int j = 0; j < tmp.size(); j++){
		//	for (int j = tmp.size()-1; j >=0; j--){
		if (j != i){
			if (tmp[j].IsMerged) continue;
		//	if (tmp[i].textbox.y < tmp[j].textbox.y)	continue;

			if (IsSphereToSphereIntersect(tmp[i].textSphere, tmp[j].textSphere) == true){
				// Merge Two box //


				if (tmp[i].textbox.x < tmp[j].textbox.x)					x = tmp[i].textbox.x;
				else														x = tmp[j].textbox.x;

				if (tmp[i].textbox.x + tmp[i].textbox.width > tmp[j].textbox.x + tmp[j].textbox.width)					x2 = tmp[i].textbox.x + tmp[i].textbox.width;
				else																									x2 = tmp[j].textbox.x + tmp[j].textbox.width;

				if (tmp[i].textbox.y < tmp[j].textbox.y)					y = tmp[i].textbox.y;
				else														y = tmp[j].textbox.y;

				if (tmp[i].textbox.y + tmp[i].textbox.height > tmp[j].textbox.y + tmp[j].textbox.height)				y2 = tmp[i].textbox.y + tmp[i].textbox.height;
				else																									y2 = tmp[j].textbox.y + tmp[j].textbox.height;

				width = x2 - x;
				height = y2 - y;


				if ((width < (minSize*1.25f)) && (height < (maxSize*1.25f))){

					// case of V_ORDER //
					//if (minHeight >  height){
					selectdId = j;
					//	minHeight = height;

					resBox.textbox.x = x;
					resBox.textbox.y = y;
					resBox.textbox.width = width;
					resBox.textbox.height = height;
					break;
					//}
				}

				//cv::Rect mRect(x, y, width, height);
				//_MOONCircle mCircle;
				//mCircle.setbyRect(mRect);

				//if (mCircle.fRadious * 2 < maxSize){
				//	if (mCircle.fRadious * 2 < minRadious){
				//		selectdId = j;
				//		minRadious = mCircle.fRadious * 2;

				//		resBox.textbox.x = x;
				//		resBox.textbox.y = y;
				//		resBox.textbox.width = width;
				//		resBox.textbox.height = height;
				//	}
				//}
			}
		}
	}

	return selectdId;
}

int CExtractor::FindOptimalBox(std::vector<_EXTRACT_BOX>& tmp, int i, int maxwidth, int maxheight, _EXTRACT_BOX& resBox)
{
	int x, y, x2, y2;
	float width, height;
//	float areaRatio = 10000.0f;
//	float mergeArea = 1000000.0f;
	float maxOveralp = 0;
	int selectdId = -1;
	for (int j = 0; j < tmp.size(); j++){
//	for (int j = tmp.size()-1; j >=0; j--){
		if (j != i){
			if (tmp[j].IsMerged) continue;
	//		if (tmp[i].textbox.y > tmp[j].textbox.y)	continue;
	//		int overlap = IsBoxToBoxIntersect(tmp[i].textboxForCheck, tmp[j].textboxForCheck);
			if (IsBoxToBoxIntersect(tmp[i].textboxForCheck, tmp[j].textboxForCheck) !=0){
				// Merge Two box //				

				if (tmp[i].textbox.x < tmp[j].textbox.x)					x = tmp[i].textbox.x;
				else														x = tmp[j].textbox.x;

				if (tmp[i].textbox.x + tmp[i].textbox.width > tmp[j].textbox.x + tmp[j].textbox.width)					x2 = tmp[i].textbox.x + tmp[i].textbox.width;
				else																									x2 = tmp[j].textbox.x + tmp[j].textbox.width;
				
				if (tmp[i].textbox.y < tmp[j].textbox.y)					y = tmp[i].textbox.y;
				else														y = tmp[j].textbox.y;

				if (tmp[i].textbox.y + tmp[i].textbox.height > tmp[j].textbox.y + tmp[j].textbox.height)				y2 = tmp[i].textbox.y + tmp[i].textbox.height;
				else																									y2 = tmp[j].textbox.y + tmp[j].textbox.height;

				width = x2 - x;
				height = y2 - y;
			
				if ((width < (maxwidth*1.25f)) && (height < (maxheight*1.25f))){
			//	if ((width < (maxwidth*1.05f)) && (height < (maxheight*1.05f))){
					// case of V_ORDER //
				//	if (maxOveralp <  overlap){
						selectdId = j;
				//		maxOveralp = height;

						resBox.textbox.x = x;
						resBox.textbox.y = y;
						resBox.textbox.width = width;
						resBox.textbox.height = height;
						break;
				//	}
				}				
			}
		}
	}
	return selectdId;
}

bool CExtractor::RcvMeargingBoundingCircle(int minSize, int maxSize, std::vector<_EXTRACT_BOX>& veclineBox, int& depth, int extX, int extY)
{
	std::vector<_EXTRACT_BOX> tmp = veclineBox;
	veclineBox = std::vector<_EXTRACT_BOX>();

	m_averTextSize.width = 0;
	m_averTextSize.height = 0;
	int addcnt = 0;
	int minArea = minSize*maxSize*0.25f;


	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_EXTRACT_BOX resBox;
	for (int i = 0; i < tmp.size(); i++){
		//	for (int i = tmp.size()-1; i >=0; i--){
		if (tmp[i].IsMerged) continue;
		if (tmp[i].textbox.area() < minArea) continue;



		resBox.init();
		int sid = FindOptimalCircle(tmp, i, minSize, maxSize, resBox);
		if (sid >= 0){
			tmp[i].textbox.x = resBox.textbox.x;
			tmp[i].textbox.y = resBox.textbox.y;
			tmp[i].textbox.width = resBox.textbox.width;
			tmp[i].textbox.height = resBox.textbox.height;

			tmp[sid].IsMerged = true;
			IsMerged = true;
		}
	}



	for (int i = 0; i < tmp.size(); i++){
		if (tmp[i].IsMerged == false){
			_EXTRACT_BOX tbox;
			tbox.init();
			tbox.textbox = tmp[i].textbox;
			tbox.setExtendBox(extX, extY);
			tbox.textSphere.setbyRect(tbox.textbox);


			if ((tbox.textbox.area() > minSize*maxSize)){
				float ar = (float)tbox.textbox.width / (float)tbox.textbox.height;
				if ((ar<0.1f) || (ar>10.0f)){
					continue;
				}
			}
			veclineBox.push_back(tbox);

			m_averTextSize.width += tmp[i].textbox.width;
			m_averTextSize.height += tmp[i].textbox.height;
			addcnt++;
		}
	}
	tmp.clear();

	if (addcnt > 0){
		float aWidth = (float)m_averTextSize.width / (float)addcnt;
		float aHeight = (float)m_averTextSize.height / (float)addcnt;
		m_averTextSize.set(0, aWidth, 0, aHeight);
	}

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)){
		RcvMeargingBoundingCircle(minSize, maxSize, veclineBox, depth, extX, extY);
	}
	depth++;

	return true;
}

bool CExtractor::RcvMeargingBoundingBox(int maxwidth, int maxheight, std::vector<_EXTRACT_BOX>& veclineBox, int& depth, int extX, int extY, _MERGE_TYPE mergeType)
{


	int minArea = maxwidth*maxwidth;
	if (maxheight < maxwidth)
		minArea = maxheight*maxheight;


	std::vector<_EXTRACT_BOX> tmp = veclineBox;
	veclineBox = std::vector<_EXTRACT_BOX>();

	m_averTextSize.width = 0;
	m_averTextSize.height = 0;
	int addcnt = 0;


	int nWidth = 0, nHeight = 0;
	bool IsMerged = false;
	_EXTRACT_BOX resBox;
	for (int i = 0; i < tmp.size(); i++){
		//	for (int i = tmp.size()-1; i >=0; i--){
		if (tmp[i].IsMerged) continue;
		
		
		
		resBox.init();
		int sid = FindOptimalBox(tmp, i, maxwidth, maxheight, resBox);
		if (sid >= 0){
			tmp[i].textbox.x = resBox.textbox.x;
			tmp[i].textbox.y = resBox.textbox.y;
			tmp[i].textbox.width = resBox.textbox.width;
			tmp[i].textbox.height = resBox.textbox.height;

			tmp[sid].IsMerged = true;
			IsMerged = true;
		}
	}



	for (int i = 0; i < tmp.size(); i++){
		if (tmp[i].IsMerged == false){
			_EXTRACT_BOX tbox;
			tbox.init();
			tbox.textbox = tmp[i].textbox;
			tbox.setExtendBox(extX, extY);
			tbox.textSphere.setbyRect(tbox.textbox);

			// Filtering line//
			float arw = (float)tbox.textbox.width / (float)tbox.textbox.height;
			float arh = (float)tbox.textbox.height / (float)tbox.textbox.width;
			if ((tbox.textbox.height > maxheight*2.0f) || (tbox.textbox.width > maxwidth*2.0f))
			{
				
				if ((arw<0.1f) || (arh<0.1f)){
					continue;
				}
			}

// Adjust Size=====================================//  In case of Chinese, Korean
			if ((mergeType == _MERGE_TEXT) && (m_languageType==_ESTERN_LANGUAGE)){		// character detection
				if (arw > 3){  // " --- "
					if (extY < 4){
						tbox.textbox.y -= 2;
						if (tbox.textbox.y < 0)	tbox.textbox.y = 0;
						tbox.textbox.height += 4;

						int yDiff = maxheight - tbox.textbox.height;
						if (yDiff < 0){
							tbox.textbox.height += yDiff;
						}

						tbox.setExtendBox(extX, extY);
					}
				}
				if (arh > 3){ // " | " 
					if (extX < 4){
						tbox.textbox.x -= 2;
						if (tbox.textbox.x < 0)	tbox.textbox.x = 0;
						tbox.textbox.width += 4;
						

						int xDiff = maxwidth - tbox.textbox.width;
						if (xDiff < 0){
							tbox.textbox.height += xDiff;
						}

						tbox.setExtendBox(extX, extY);
					}
				}

				if (tbox.textbox.area() < minArea*0.5f){
					if (extY < 4){
						tbox.textbox.y -= 1;
						if (tbox.textbox.y < 0)	tbox.textbox.y = 0;
						tbox.textbox.height += 2;

						int yDiff = maxheight - tbox.textbox.height;
						if (yDiff < 0){
							tbox.textbox.height += yDiff;
						}


						tbox.setExtendBox(extX, extY);
					}
					if (extX < 4){
						tbox.textbox.x -= 1;
						if (tbox.textbox.x < 0)	tbox.textbox.x = 0;
						tbox.textbox.width += 2;

						int xDiff = maxwidth - tbox.textbox.width;
						if (xDiff < 0){
							tbox.textbox.height += xDiff;
						}


						tbox.setExtendBox(extX, extY);
					}
				}
			}
//=====================================================================//



			veclineBox.push_back(tbox);

			m_averTextSize.width += tmp[i].textbox.width;
			m_averTextSize.height += tmp[i].textbox.height;
			addcnt++;		
		}
	}
	tmp.clear();

	if (addcnt > 0){
		float aWidth = (float)m_averTextSize.width / (float)addcnt;
		float aHeight = (float)m_averTextSize.height / (float)addcnt;
		m_averTextSize.set(0, aWidth, 0, aHeight);
	}

	if ((depth < _MAX_EXTRACT_ITERATION) && (IsMerged)){
		depth++;


		for (int i = 0; i < veclineBox.size(); i++){
			if (veclineBox[i].IsMerged == true){
				int a = 0;
			}
		}

		RcvMeargingBoundingBox(maxwidth, maxheight, veclineBox, depth, extX, extY, mergeType);
		TRACE("Recursive: %d\n", depth);
	}

	//if ((depth < 5)){
	//	depth++;
	//	RcvMeargingBoundingBox(maxwidth, maxheight, veclineBox, depth, extX, extY);
	//	TRACE("Recursive: %d\n", depth);		
	//}


	

	return true;
}

void CExtractor::ShrinkCharacter(cv::Mat& img)
{
	cv::Mat tmpImg;
	tmpImg = img.clone();

	adaptiveThreshold(tmpImg, tmpImg, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	cv::bitwise_not(tmpImg, tmpImg);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
	cv::findContours(tmpImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<cv::Point> > contours_poly2(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly2[i], 1, true);
	}

//	cv::imshow("contour_before", img);
	cv::Scalar color = cv::Scalar(255);
	for (int i = 0; i < contours_poly2.size(); i++){	
		drawContours(img, contours_poly2, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());		
	}
//	cv::imshow("contour_after", img);
}


void CExtractor::FineExtractTexts(cv::Mat& img, cv::Rect lineBox, std::vector<_EXTRACT_BOX>& vecBox, _TEXT_ORDER _torder, int _w, int _h, int _incre, bool IsContract)
{
	cv::Mat imgCut;
	img(lineBox).copyTo(imgCut);

	//if (IsContract)
	//	ContractImage(imgCut);

	adaptiveThreshold(imgCut, imgCut, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 75, 10);
	cv::bitwise_not(imgCut, imgCut);

	// Find Contour //
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	/// Find contours
	cv::findContours(imgCut, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());

	float aRatio = 0;
	int minSize = 25;
	int maxwidth = 0, maxheight = 0;
	int cnt = 0;
	for (int i = contours.size()-1; i >=0; i--)
//	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
		cv::Rect r = cv::boundingRect(cv::Mat(contours[i]));
		aRatio = (float)r.width / (float)r.height;
		if ((aRatio > 0.1f) && (aRatio < 10.0f)){
			if (r.width > maxwidth){
				maxwidth = r.width;
			}
			if (r.height > maxheight){
				maxheight = r.height;
			}
		}
	}

	if (_torder == V_ORDER){
		maxwidth -= _incre;
		maxheight -= _incre;
	}
	else{
		maxheight -= _incre;
	}

	
	if (maxheight < maxwidth)
		maxheight = maxwidth;
	else
		maxwidth = maxheight;

	
	if (maxwidth < _MIN_BOX_SIZE)	maxwidth = _MIN_BOX_SIZE;
	if (maxheight < _MIN_BOX_SIZE)  maxheight = _MIN_BOX_SIZE;

	if (_torder == V_ORDER){		// Vertical
		DetectChars(contours_poly, vecBox, maxwidth, maxheight, 3, 1);
	}
	else{
		DetectChars(contours_poly, vecBox, maxwidth, maxheight, 1, 3);
	}


	// Checkl ambigous..................===================================//
	float area = maxwidth*maxheight;

	//	aRatio = (float)m_maxWidth / (float)m_maxHeight;
	//for (int i = 0; i < vecBox.size(); i++){
	//	float r = (float)vecBox[i].textbox.width / (float)vecBox[i].textbox.height;

	//	if (vecBox[i].IsAmbig == false){
	//		if ((vecBox[i].textbox.area() < area*0.2f) || (vecBox[i].textbox.area() > area*1.5f)){
	//			vecBox[i].IsAmbig = true;
	//		}
	//		else{
	//			for (int j = 0; j < vecBox.size(); j++){
	//				if (j != i){
	//					if (IsBoxToBoxIntersect(vecBox[i].textbox, vecBox[j].textbox) != 0){
	//						// Fine Extraction //
	//						vecBox[i].IsAmbig = true;
	//						vecBox[j].IsAmbig = true;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//	vecBox.clear();


	//=================================================================//
	//	cv::imshow("cut", imgCut);
	imgCut.release();
}


void CExtractor::ContractImage(cv::Mat& img)
{
	//	cv::imshow("Before-binaryMat", img);
	for (int y = 1; y < img.rows - 1; y++){
		for (int x = 1; x < img.cols - 1; x++){
			int pId = (y - 1)*img.step + x;
			int id = y*img.step + x;
			int nId = (y + 1)*img.step + x;

			if (img.data[id] < 128){		// black
				if (img.data[nId] > 128){
					img.data[id] = 255;
				}
			}
			//if (img.data[id] > 128){		// black
			//	if (img2.data[pId] < 128){
			//		img2.data[pId] = 255;
			//	}
			//}

			//pId = (y)*img.step + x - 1;
			//id = y*img.step + x;
			//nId = (y)*img.step + x + 1;

			//if (img.data[id] < 128){		// black
			//	if (img.data[nId] > 128){
			//		img.data[id] = 255;
			//	}
			//}

		}
	}	//}	
}

void CExtractor::CheckAmbiguous()
{
	for (int i = 0; i < m_exTextBox.size(); i++){
		m_exTextBox[i].IsAmbig = false;
		m_exTextBox[i].IsMerged = false;
		m_exTextBox[i].IsBig = false;
		m_exTextBox[i].IsSmall = false;
		m_exTextBox[i].pNextBox = NULL;
	}
	for (int i = 0; i < m_exTextBox.size(); i++){	
		int area = m_exTextBox[i].detectWidth*m_exTextBox[i].detectHeight;

		if (m_exTextBox[i].IsAmbig == false){
			//if ((m_exTextBox[i].textbox.area() < area*0.3f)){
			//	m_exTextBox[i].IsAmbig = true;
			//	m_exTextBox[i].IsSmall = true;
			//}
			if ((m_exTextBox[i].textbox.area() > area*1.3f)){
				m_exTextBox[i].IsAmbig = true;
				m_exTextBox[i].IsBig = true;
			}

			else{
				for (int j = 0; j < m_exTextBox.size(); j++){

					if (m_exTextBox[j].IsAmbig == true) continue;

					if ((j != i)&&(m_exTextBox[i].lineId == m_exTextBox[j].lineId)){
						if (IsBoxToBoxIntersect(m_exTextBox[i].textbox, m_exTextBox[j].textbox) != 0){
							// Fine Extraction //
							m_exTextBox[i].IsAmbig = true;
						//	m_exTextBox[j].IsAmbig = true;
							if (m_exTextBox[i].pNextBox == NULL){
								if (m_exTextBox[j].pNextBox != &m_exTextBox[i]){
									m_exTextBox[i].pNextBox = &m_exTextBox[j];
								}
								else{
									TRACE("Dup");
								}
							}
							break;
						}
					}
				}
			}
		}
	}
}