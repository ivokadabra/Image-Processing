#include <opencv2/opencv.hpp>

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


cv:: Mat Segmentierung(cv::Mat &img) {
	//int histSize = 256;
	//int hist_w = 256; int hist_h = 400;
	Mat hist;
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	double minv, maxv;
	minMaxLoc(img, &minv, &maxv);

	Histogramm_zeichnen(img, hist, histImage, hist_w, 400, Scalar(255, 255, 255));
	imshow("Histogramm segment", histImage);

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



	imshow("Scale", histImage);

	//imshow("Segment", scalier);

   return histImage;
}




cv::Mat filtern(cv::Mat &src, cv::Mat &des, int filesize, int blur_choise) {


	switch (blur_choise) {

	case 1: {




		cv::medianBlur(src, des, filesize);
		cv::imshow("Image111", des);

		break;
	}
	case 2:
		cv::GaussianBlur(src, des, Size(5, 5), filesize);
		cv::imshow("Image112", des);
		break;

	case 3:
		cv::bilateralFilter(src, des, filesize, 50, 1.5);
		cv::imshow("Image113", des);
		break;
	default:
		cout << "Error" << endl;

	}
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
	cv::Mat destinationImage;

	for (int i = 0; i < size; i++)
	{

		for (int j = 0; j < size; j++)
		{

			//pixel = i + res * j;
			img->at < ushort >(Point(i, j)) = buffer[i + (j*size)];

		}
	}
	double minv, maxv;
	minMaxLoc(*img, &minv, &maxv);
	cout << "kleinster Wert:" << minv << endl;
	cout << "groesster Wert:" << maxv << endl;


	convertScaleAbs(*img, destinationImage, 255 / (maxv - minv),
		(minv * 255) / (maxv - minv));



	//cout << "kleinster Wert nach Skalierung:" << min << endl;
	//cout << "groesster Wert nach Skalierung:" << max << endl;


	cv::imshow("Image", destinationImage);

	delete[]buffer;
	file.close();

	return destinationImage;



}



int
main(int argc, char **argv)
{


   for(int k=0;k<=116;k++){

	cv::Mat destinationImage(512, 512, CV_16UC1);
	cv::Mat destinationImage_1;
	cv::Mat des;
        cv::Mat seg;
        cv::Mat probe;
        string path="/home/ivokadabra/DisplayImage";


        
	string datei = path + "/" + std::to_string(k) + ".dcm";
        //probe = cv::imread(datei, 0);
        ifstream file;
        file.open(datei, ios::in | ios::binary);
	file.seekg(0, file.end);
	int length = file.tellg();
       if (length==-1){
       cout<<"Bild fehlt"<<endl;
      }

    else{
	destinationImage = convert(datei, &destinationImage);
       
        cout<<"k:"<<k<<endl;
        for(int i=1;i<=3;i++){

	destinationImage_1 = filtern(destinationImage, des, 9, i);
        cout<<"k in :"<<k<<endl;
	//string s = path + std::to_string(k) + ".png";
     

	//imwrite(s, destinationImage_1);



	//cv::Mat img = cv::imread("20.png", 0);

	//imshow("Histogramm Original", img);

	/*if (!img.data)
	{
		cout << "Bild fehlt" << endl;
		return -1;
	}*/
	//Erzeugen eines Fensters und Anzeige des

        // imshow("png", img);


	//int hist_w = 256; int hist_h = 400;
	//Mat hist;
	//Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	//Histogramm_zeichnen(destinationImage, hist, histImage, hist_w, hist_h, Scalar(255,255,255)); // Weißes Histogramm
	//imshow("Histogramm Original", histImage );
	seg=Segmentierung(destinationImage_1);

       

        string str =path + "/segment_" + std::to_string(i) + ".png";


	imwrite(str, seg);


	waitKey(0);
    
    }
}
waitKey(0);
}
	return 0;
}
