#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>		// std::cout
#include <fstream>		// std::ifstream
#include <math.h>
#include <string>

using namespace cv;
using namespace std;



int hist_w = 0, hist_h = 0;


void Histogramm_zeichnen(Mat &img, Mat &hist, Mat &histImage, int hist_w, int hist_h, Scalar color)
{
	int histSize = 256;
	// Setzen des Bereichs für das Histogramm
	float range[] = { 0, 256 };
	const float* histRange = { range };
	// Berechnen des Histogramms
	calcHist(&img, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	//Größe des Histogrammbildes festlegen
	int bin_w = (int)((double)hist_w / histSize);
	// Normalisieren des Ergebnisses, dass es in Bild passt [ 0, histImage.rows ]
	 Size size_t(7,4);
         blur(hist,hist,size_t);
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	//Zeichnen des Histogramms in Blau
	for (int i = 0; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
			Point(bin_w*(i), hist_h),
			color);
	}
}


void Segmentierung(cv::Mat &img,cv::Mat &Binaer_bild) {
	//int histSize = 256;
	//int hist_w = 256; int hist_h = 400;
	Mat hist;
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	double minv, maxv;
	minMaxLoc(img, &minv, &maxv);

	Histogramm_zeichnen(img, hist, histImage, hist_w, 400, Scalar(255, 255, 255));
	
	int R = 25;


	cv::Mat scale(hist_w, 1, CV_32F, Scalar(0, 0, 0));
	cv::Mat scalier(1, hist_w, CV_32F, Scalar(0, 0, 0));



	// cout << "kleinster Wert:" << minv << endl;
	// cout << "groesster Wert:" << maxv << endl;



	for (int i = minv + R; i < maxv - R; i++) {
		for (int k = 1; k <= R; k++) {
			scale.at<float>(i) += (hist.at<float>(i + k) - hist.at<float>(i - k)) / (2.0*i);
		}

		scale.at<float>(i) = (scale.at<float>(i)*(1.0 / R));
	}

	cout << "scale finisch" << endl;
	for (int i = minv + R; i < maxv - R; i++) {
		for (int k = 1; k <= R; k++) {
			scalier.at<float>(i) += abs(scale.at<float>(i + k) - scale.at<float>(i - k));
		}

		scalier.at<float>(i) = (scalier.at<float>(i)*(1.0 / R));
	}


	cout << "scalier finisch" << endl;
	normalize(scalier, scalier, 1, histImage.rows, NORM_MINMAX, -1, Mat());

	//int bin_w = (int)( (double) hist_w/histSize );
	for (int i = minv + R; i < maxv - R; i++)
	{
		line(histImage, Point(i, 400 - cvRound(scalier.at<float>(i))),
			Point(i, 400),
			Scalar(0, 255, 250), 1);
	}
   

 imshow("Histogramm segment", histImage);
   int Schwellwert = 0;

  


      for (int i = maxv - R; i > minv + R; i--) {


		if (scalier.at<float>(i) > scalier.at<float>(i - 1)) {

			if (scalier.at<float>(i) > scalier.at<float>(i - 2)) {

				if (scalier.at<float>(i) > scalier.at<float>(i - 3)) {

					Schwellwert = i;

					break;

				}

			}

		}



	}
        
        cout<<"Schwellweet is :"<<Schwellwert<<endl;
        cv::threshold(img, Binaer_bild, Schwellwert, 255, cv::THRESH_BINARY);
		cv::threshold(img, img, 95, 255, cv::THRESH_BINARY);

      
}



cv::Mat filtern(cv::Mat &src, cv::Mat &des, int filesize) {


	cv::medianBlur(src, des, filesize);
	//cv::imshow("Image111", des);

	
	return des;

}



cv::Mat convert(string s, Mat* img)
{
	ifstream file;
	file.open(s, ios::in | ios::binary);
	file.seekg(0, file.end);
	int length = file.tellg();
	cout << "length " << length << endl;
	int size = 512;
	// int res;

	int header = length - (size * size * 2);	//L�nge ohne header
	hist_w = 512;
	hist_h = 512;


	if (header < 0) {
		size = 256;
		hist_w = 256;
		hist_h = 256;
		img->resize(256, 256);
		header = length - (size * size * 2);
	}
	file.seekg(header, file.beg);
	// int Size = res * res * 2;
	cout << "bodySize: " << size << endl;
	int buffergroesse = length - header;

	cout << "bodyBegin: " << buffergroesse << endl;
	cout << "bodySize: " << size << endl;

	

	ushort *buffer = new ushort[buffergroesse];

	file.read(reinterpret_cast <char *>(buffer), buffergroesse);

	//int pixel = 0;
	//cv::Mat img (res, res, CV_16U);
	

	for (int i = 0; i < size; i++)
	{

		for (int j = 0; j < size; j++)
		{

			//pixel = i + res * j;
			img->at < ushort >(Point(i, j)) = buffer[i + (j*size)];

		}
	}
	/*double minv, maxv;
	minMaxLoc(*img, &minv, &maxv);
	cout << "kleinster Wert:" << minv << endl;
	cout << "groesster Wert:" << maxv << endl;*/


	/*convertScaleAbs(*img, destinationImage, 255 / (maxv - minv),
		(minv * 255) / (maxv - minv));*/



	//cout << "kleinster Wert nach Skalierung:" << min << endl;
	//cout << "groesster Wert nach Skalierung:" << max << endl;


	//cv::imshow("Image", destinationImage);

	delete[]buffer;
	file.close();

	return *img;



}

Mat Scale(Mat pic) {

	Mat ausgabe;
	double minv, maxv;
	minMaxLoc(pic, &minv, &maxv);

	convertScaleAbs(pic, ausgabe, 255 / (maxv - minv),
			-(minv * 255) / (maxv - minv));

	return ausgabe;
}

int
main(int argc, char **argv)
{


   

	cv::Mat destinationImage(512, 512, CV_16UC1);
	cv::Mat destinationImage_1;
	cv::Mat destinationImage_0;
	cv::Mat des;
        cv::Mat seg;
        cv::Mat probe;
    cv::Mat Binaer_bild;
	cv::Mat Binaer_bild2;
    string path="/home/ivokadabra/DisplayImage/P2";
	cv::Mat max1, max2, max3;

	Mat x_1, x_2, x_3;
        

	for (int i = 16; i < 117; i++) {
	string datei = path + "/" + std::to_string(i) + ".dcm";
      
        ifstream file;
       
     
	destinationImage_0 = convert(datei, &destinationImage);
       
	probe = destinationImage_0.clone();
        

	destinationImage_1 = filtern(destinationImage_0, des, 5);

	Binaer_bild = destinationImage_1.clone();
	Binaer_bild2 = destinationImage_1.clone();
        
	Segmentierung(Binaer_bild, Binaer_bild2);
        
      if (i == 16) {

			max1 = probe;
			max2 = Binaer_bild2;
                       

			max3 = Binaer_bild;
		}
		else {
			max(max1, probe, max1);
			max(max2, Binaer_bild2, max2);
			max(max3, Binaer_bild, max3);
		}

	  x_1 = Scale(max1);
	  x_2 = Scale(max2);
	  x_3 = Scale(max3);

		cv::imshow("Max eingelesenen", x_1);
		cv::imshow("Max Binaerbild Krummungsberechnung", x_2);
		cv::imshow("Max Binaerbild feste Schwelle", x_3);
		cv::waitKey(20);
}

    cv::imwrite("max_images.png", x_1 );
            
    cv::imwrite("max_binaer.png",x_2 );
                
    cv::imwrite("max_kruemmung.png", x_3 );

	cv::waitKey();
	return 0;
}
