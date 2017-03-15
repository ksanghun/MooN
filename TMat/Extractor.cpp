#include "stdafx.h"
#include "Extractor.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ZDataManager.h"
class comparator{
public:
	bool operator()(std::vector<cv::Point> c1, std::vector<cv::Point>c2){
		return cv::boundingRect(cv::Mat(c1)).x<cv::boundingRect(cv::Mat(c2)).x;
	}
};

CExtractor::CExtractor()
{
}


CExtractor::~CExtractor()
{
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

void CExtractor::getContours(cv::Mat img, std::vector<cv::Rect>& boundRect, cv::Mat& cropped2)
{
//	cv::Mat img = cv::imread(filename, 0);

	cv::imshow("original", img);

	//Apply blur to smooth edges and use adapative thresholding
	cv::Size size(3, 3);
	cv::GaussianBlur(img, img, size, 0);
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

//	cv::Mat cropped2 = cropped.clone();
	cropped2 = img.clone();
//	cropped2 = cropped.clone();
	cropped2 = img.clone();
	cvtColor(cropped2, cropped2, CV_GRAY2RGB);

//	cv::Mat cropped3 = cropped.clone();
	cv::Mat cropped3 = img.clone();
	cvtColor(cropped3, cropped3, CV_GRAY2RGB);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	/// Find contours
//	cv::findContours(cropped, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0));
	cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0));



	/// Approximate contours to polygons + get bounding rects and circles
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
//	std::vector<cv::Rect> boundRect(contours.size());
	std::vector<cv::Point2f>center(contours.size());
	std::vector<float>radius(contours.size());


	//Get poly contours
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
	}


	//Get only important contours, merge contours that are within another
	int minSize = 10;
	std::vector<std::vector<cv::Point> > validContours;
	for (int i = 0; i<contours_poly.size(); i++){

		cv::Rect r = cv::boundingRect(cv::Mat(contours_poly[i]));
		if (r.area()<minSize)continue;

		bool inside = false;
		for (int j = 0; j<contours_poly.size(); j++){
			if (j == i)continue;

			cv::Rect r2 = cv::boundingRect(cv::Mat(contours_poly[j]));
			if (r2.area()<minSize || r2.area()<r.area())continue;
			if (r.x>r2.x&&r.x + r.width<r2.x + r2.width&&
				r.y>r2.y&&r.y + r.height<r2.y + r2.height){

				inside = true;
			}
		}
		if (inside)continue;
		validContours.push_back(contours_poly[i]);
	}


	//Get bounding rects
	for (int i = 0; i<validContours.size(); i++){
	//	boundRect[i] = cv::boundingRect(cv::Mat(validContours[i]));
		boundRect.push_back(cv::boundingRect(cv::Mat(validContours[i])));
	}


//Display
	//cv::Scalar color = cv::Scalar(0, 255, 0);
	//for (int i = 0; i< validContours.size(); i++)
	//{
	//	if (boundRect[i].area()<100)continue;
	//	drawContours(cropped2, validContours, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
	//	rectangle(cropped2, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	//}

	//imwrite("example6.jpg",cropped2);
//	imshow("Contours", cropped2);

//	detectLetters(cropped3, validContours, boundRect);
}

void CExtractor::extractWithOCR(cv::Mat image, std::vector<cv::Rect>& boundRect)
{
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