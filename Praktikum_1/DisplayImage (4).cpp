
#include <opencv2/opencv.hpp>

#include <iostream>		// std::cout
#include <fstream>		// std::ifstream
#include <math.h>
#include <string>
using namespace cv;
using namespace std;


cv::Mat convert (string s)
{

  std::ifstream is (s, ios::binary | ios::ate);

  int length = is.tellg ();
  cout << "length " << length << endl;
  int res;

  if (length < 512 * 512 * 2)
    {
      res = 256;
    }
  else
    res = 512;

  int Size = res * res * 2;
  cout << "bodySize: " << Size << endl;
  int Begin = length - Size;

  cout << "bodyBegin: " << Begin << endl;
  cout << "bodySize: " << Size << endl;

  is.seekg (Begin);

  ushort *buffer = new ushort[Size];

  is.read (reinterpret_cast < char *>(buffer), Size);



  is.close ();



  int pixel = 0;
  cv::Mat img (res, res, CV_16U);
  cv::Mat destinationImage;

  for (int i = 0; i < res; i++)
    {

      for (int j = 0; j < res; j++)
	{

	  pixel = i + res * j;
	  img.at < ushort > (j, i) = buffer[pixel];

	}
    }
  double minv, maxv;
  minMaxLoc (img, &minv, &maxv);
  cout << "kleinster Wert:" << minv << endl;
  cout << "groesster Wert:" << maxv << endl;


  convertScaleAbs (img, destinationImage, 255 / (maxv - minv),
		   (minv * -1) * 255 / (maxv - minv));

  double min, max;
  minMaxLoc (destinationImage, &min, &max);

  cout << "kleinster Wert nach Skalierung:" << min << endl;
  cout << "groesster Wert nach Skalierung:" << max << endl;


  cv::imshow ("Image", destinationImage);
  delete[]buffer;
  cv::waitKey ();

  return destinationImage;



}



int
main (int argc, char **argv)
{

  string path = argv[1];

  cv::Mat destinationImage;


  for (int i = 0; i <= 2; i++)
    {
      string datei = path + "/" + std::to_string (i) + ".dcm";

      destinationImage = convert (datei);


      string s = path + "/"+ std::to_string (i) + ".png";


      imwrite (s, destinationImage);



    }





  return 0;
}

