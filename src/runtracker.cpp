#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"

//#include <dirent.h>

using namespace std;
using namespace cv;

int rect[2][2];
int num;

void onMouse(int event,int x,int y,int flags,void* param )
{
	switch (event)
	{
		case CV_EVENT_LBUTTONDOWN:
			if(num<2)
			{
				rect[num][0] = x;
				rect[num][1] = y;
				num++;
			}
			
			break;
	default:
		break;
	}
	
}

void runkcf(char* videoname){

	//if (argc > 5) return -1;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = false;
	bool LAB = false;
	FILE* fp;

	//for(int i = 0; i < argc; i++){
	//	if ( strcmp (argv[i], "hog") == 0 )
	//		HOG = false;
	//	if ( strcmp (argv[i], "fixed_window") == 0 )
	//		FIXEDWINDOW = true;
	//	if ( strcmp (argv[i], "singlescale") == 0 )
	//		MULTISCALE = false;
	//	if ( strcmp (argv[i], "show") == 0 )
	//		SILENT = false;
	//	if ( strcmp (argv[i], "lab") == 0 ){
	//		LAB = true;
	//		HOG = true;
	//	}
	//	if ( strcmp (argv[i], "gray") == 0 )
	//		HOG = false;
	//}
	
	// Create KCFTracker object
 	//KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
	KCFTracker tracker(1, 0, 1, 0);

	// Frame readed
	Mat frame;

	// Tracker results
	Rect result;

	// Path to list.txt
	ifstream listFile;
	//fp = fopen("image.txt","r");
	string fileName = "E:\\Track\\MilTrack_matlab_v1.0\\image.txt";
  	listFile.open(fileName);

  	// Read groundtruth for the 1st frame
  	ifstream groundtruthFile;
	string groundtruth = "E:\\code\\KCFcpp-master\\kcf-test\\region.txt";
  	groundtruthFile.open(groundtruth);
  	string firstLine;
  	getline(groundtruthFile, firstLine);
	groundtruthFile.close();
  	
  	istringstream ss(firstLine);

  	// Read groundtruth like a dumb
  	float x1, y1, x2, y2, x3, y3, x4, y4;
  	char ch;
	ss >> x1; 
	ss >> ch;
	ss >> y1;
	ss >> ch;
	ss >> x2;
	ss >> ch;
	ss >> y2;
	ss >> ch;
	ss >> x3;
	ss >> ch;
	ss >> y3;
	ss >> ch;
	ss >> x4;
	ss >> ch;
	ss >> y4; 

	//char videoname[256] = "E:\\OPA_repos\\testvideo\\C0006.MP4";
	//char videoname[256] = "E:\\OPA_repos\\testvideo\\test2.avi";
	//char videoname[256] = "E:\\OPA_repos\\testvideo\\SelectVideo\\test1_0009.avi";
	
	//char videoname[256] = "E:\\PEA\\VIDEO\\2or3_outdoorday.avi";
	
	//char videoname[256] = "E:\\Track\\video\\Panasonic Auto Tracking Demo.mp4";
	char videonameI[256] ;
	
	//sprintf(videonameI,"%s.avi",videoname);
	CvCapture* pcap = cvCreateFileCapture(videoname);
	IplImage* src_img,*pResizeImg;
	// Using min and max of X and Y for groundtruth rectangle
	//float xMin =  min(x1, min(x2, min(x3, x4)));
	//float yMin =  min(y1, min(y2, min(y3, y4)));
	//float width = max(x1, max(x2, max(x3, x4))) - xMin;
	//float height = max(y1, max(y2, max(y3, y4))) - yMin;
	src_img = cvQueryFrame(pcap);
	int w = src_img->width;
	int h = src_img->height;
	float scale = 0.5f;//640/(w+0.001);
	pResizeImg = cvCreateImage(cvSize(w*scale,h*scale),8,3);
	cvResize(src_img,pResizeImg,1);
	//cvShowImage("src",pResizeImg);
	//num=0;
	//cvSetMouseCallback("src",onMouse,0);
	//cvWaitKey(0);
	//float xMin =  144;
	//float yMin =  42;
	//float width = 36;
	//float height = 36;
	float xMin =  rect[0][0];
	float yMin =  rect[0][1];
	float width = rect[1][0] - rect[0][0];
	float height = rect[1][1] - rect[0][1];
	
	// Read Images
	//ifstream listFramesFile;
	//string listFrames = "E:\\Track\\MilTrack_matlab_v1.0\\image.txt";
	//listFramesFile.open(listFrames);
	//string frameName;


	// Write Results
	ofstream resultsFile;
	char resultsPath[256];//
	sprintf(resultsPath,"%s.txt",videoname);
	//= "E:\\code\\KCFcpp-master\\kcf-test\\output.txt";
	resultsFile.open(resultsPath);

	// Frame counter
	int nFrames = 0;
	int index=0;
	uchar key=0;
	while ( 1 ){
		//frameName = frameName;

		//// Read each frame from the list
		//frame = imread(frameName, CV_LOAD_IMAGE_COLOR);
		printf("*************%d**********\n",index);
		src_img = cvQueryFrame(pcap);
		if(src_img == NULL) break;
		cvResize(src_img,pResizeImg,1);
		if(/*key!='s'*/index==0) 
		{
			index++;
			cvShowImage("13",pResizeImg);
			key = cvWaitKey(1);
			continue;
		}
			
		// First frame, give the groundtruth to the tracker
		if (nFrames == 0) {
			cvShowImage("src",pResizeImg);
			num=0;
			cvSetMouseCallback("src",onMouse,0);
			cvWaitKey(0);
			float xMin =  rect[0][0];
			float yMin =  rect[0][1];
			float width = rect[1][0] - rect[0][0];
			float height = rect[1][1] - rect[0][1];
			tracker.init( Rect(xMin, yMin, width, height), pResizeImg );
			cvRectangle( pResizeImg, Point( xMin, yMin ), Point( xMin+width, yMin+height), Scalar( 0, 255, 255 ), 1, 8 );
			resultsFile << xMin << "," << yMin << "," << width << "," << height << endl;
			cvDestroyWindow("src");
		}
		// Update
		else{
			double t1;
			double t2;
			t1 = clock();
			result = tracker.update(pResizeImg);
			t2 = clock();
			printf("process time = %3.5f\n",t2-t1);
			cvRectangle( pResizeImg, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
			resultsFile << result.x << "," << result.y << "," << result.width << "," << result.height << endl;
		}

		nFrames++;

		if (!SILENT){
			cvShowImage("Image", pResizeImg) ;
			waitKey(1);
		}
	}
	resultsFile.close();

	listFile.close();
	cvReleaseImage(&pResizeImg);
	cvReleaseCapture(&pcap);
	cvDestroyAllWindows;
}


int main()
{
	FILE* fp;
	char videolist[256] = "E:\\OPA_repos\\testvideo\\CW\\list.txt"; 
	char videoname[256] ;//= "E:\\OPA_repos\\testvideo\\SelectVideo\\test1_0009";
	fp = fopen(videolist,"r+");
	while (!feof(fp))
	{
		printf("enter:\n");
		fscanf(fp,"%s",videoname);
		runkcf(videoname);
		//cvDestroyAllWindows;
	}
	fclose(fp);
	
	//runkcf(videoname);
}