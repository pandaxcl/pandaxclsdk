#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(int argc, const char*argv[])
{
	IplImage *img = cvLoadImage(argv[1]);
	const char*szWindowName = "Example1";
	cvNamedWindow(szWindowName, CV_WINDOW_AUTOSIZE);
	cvShowImage(szWindowName, img);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow(szWindowName);
	return 0;
}
