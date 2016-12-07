#include "camLib.hpp" 

using namespace std;

VideoCapture cap(0);

//const string source = "camLib";

// The Constructor...
Camera::Camera(void) {
	//Check if opening the camera worked...
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
    }
	cout << " Camera warming up..." << endl;
	
	// Determine camera output size automatically...
	dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    fps     = cap.get(CV_CAP_PROP_FPS);	// get frames-per-second of the video device
	// Print values out...
    cout << "Frame size : " << dWidth << " x " << dHeight << " --- fps: " << fps << endl;
    
	// Prepare the output window...
    //namedWindow("Motion Indicator Window",CV_WINDOW_AUTOSIZE); 
    
    // Read three images first...
	cap >> prev_frame;
	cvtColor(prev_frame, prev_frame, CV_RGB2GRAY);
	cap >> current_frame;
	cvtColor(current_frame, current_frame, CV_RGB2GRAY);
	cap >> next_frame;
	cvtColor(next_frame, next_frame, CV_RGB2GRAY);
	
	cap >> frame;
	bool isColor = (frame.type() == CV_8UC3);
	// Initialize Video Writer...
	int codec = CV_FOURCC('H', '2', '6', '4'); 	//('M', 'J', 'P', 'G');	//select desired codec (must be available at runtime...
	string filename = "./output.avi";
	writer.open(filename, codec, fps, frame.size(), isColor);
	//Check if succeeded
	if (!writer.isOpened())	{
		cerr << "Could not open the output video file for write\n";
	}
	
	// Parameter:
	// number_of_changes is the amount of changes in the resut matrix...
	// color is the color of the rectangle...
	number_of_changes = 0;
	
	// Detect motion in window...
	x_start = 10, x_stop= dWidth-10;
	y_start = 10, y_stop = dHeight-10;
	
	// Maximum deviation in the image, the  higher the value, the more motion is allowed...
	max_deviation = 20;
	
	// Erode kernel...
	kernel_ero = getStructuringElement(MORPH_RECT, Size(2,2));
	
	
}

//----------------------------------------------------------------------

// The Destructor...
Camera::~Camera(void)  {
	cout << "Shutting down camera and closing files..." << endl;
	writer.release();	// Release VideoWriter()
	
}
 
// The camera access function... 
Mat Camera::captureVideo(void)	{ 
	// Read in a new frame...
	cap >> frame;
	result = frame;
	cvtColor(frame, frame, CV_RGB2GRAY);
	// Image manipulations come here...
	// This line displays the image resulting from calculating the difference between
	// consecutive images...
	diffImg(prev_frame, current_frame, next_frame);
	imshow("Motion Indicator", result);
	
	// Put images in the right order...
	prev_frame		= current_frame;
	current_frame	= next_frame;
	next_frame		= frame;
	
    return result;
}

int Camera::diffImg(Mat prev, Mat current, Mat next)	{
	absdiff(prev, next, diff1);
	absdiff(next, current, diff2);
	bitwise_and(diff1, diff2, motion);
	threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
	erode(motion, motion, kernel_ero);
	
	return 0;
}


int Camera::detectMotion(void)  {
	// Calculate the standard deviation...							
	meanStdDev(motion, mean, stddev);	
	
	// if not too many changes then the motion is real...
	if(stddev[0] < max_deviation)
	{
		number_of_changes = 0;
		min_x = motion.cols, max_x=0;
		min_y = motion.rows, max_y=0;
		// loop over the image and detect changes...
		for(int j=y_start; j < y_stop; j+=2)  {		// height...
			for(int i=x_start; i < x_stop; i+=2)  {	// width...
				// Check if the pixel at (j, i) intensity is equal to 255
				// this means that the pixel is different in the sequence
				// of images (prev_frame, current_frame, next_frame)
				if(static_cast<int>(motion.at<uchar>(j,i)) == 255)
				{
					number_of_changes++;
					if(min_x>i) min_x = i;
					if(max_x<i) max_x = i;
					if(min_y>j) min_y = j;
					if(max_y<j) max_y = j;
				}
			}
		}
		if(number_of_changes) {
			//check if out of bounds...
			if(min_x-10 > 0) min_x -= 10;
			if(min_y-10 > 0) min_y -= 10;
			if(max_x+10 < frame.cols-1) max_x += 10;
			if(max_y+10 < frame.rows-1) max_y += 10;
			// draw rectangle around the changed pixel...
			Point x(min_x,min_y);
			Point y(max_x,max_y);
			Rect rect(x,y);
			//cropped = result(rect);
			//cropped.copyTo(result_cropped);
			rectangle(result,rect,Scalar(0,255,255),1,4);
			imshow("Motion Indicator", result);
		}
		return number_of_changes;	
	}		
	 			
	return 0;
}

//int Camera::closeVideo(void)  {
//	writer.release();
//	
//	return 0;
//}

int Camera::saveVideo(Mat *frameStack) {
	//cap >> result;
	//writer.write(*frameStack);
		
	return 0;
}
