#include <opencv/highgui.h>

int main(int argc, const char*argv[])
{
	const char*szWindowName = "Example1";
	cvNamedWindow(szWindowName, CV_WINDOW_AUTOSIZE);

	CvCapture*capture = cvCreateCameraCapture(-1);
	while(true)
	{
		IplImage*frame = cvQueryFrame(capture);
		if (!frame) break;
		cvShowImage(szWindowName, frame);
		char c = cvWaitKey(1000/60);
		if (27 == c) break;
	}
	cvReleaseCapture(&capture);

	cvDestroyWindow(szWindowName);
	return 0;
}
