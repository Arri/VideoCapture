#include "./include/camLib.hpp"



int main(int argc, char* argv[])
{
	const string DIR = "/home/pi/camClass/VideoCapture_version/pics/"; //Target directory to save images
	
	
	// Create a camera instance...
	Camera cam1;
	int number_of_changes = 0;
	int number_of_sequence = 0;
	const int min_sequence = 1000;	// Minimum sequence of detected motion befor accepting real motion...
	const int DELAY = 500;			// delay in microseconds...
	const int frameBuffer = 10;	// Frame buffer around motion ...
	const int lengthThreshold = 2;	// How many frames in a sequence need to show motion...
	
	// if more than 'there_is_motion' pixels are changed, we say there is motion...
	// and store the image...
	int there_is_motion = 5;
	int frameCounter = 0;		// Counter for frames...
	int frameSequence = 0;		// Count sequence of frames...
	int framesTotal = 0;		// Total number fa frames containing motion -- for saving...
	int saveVid = 0;			// Flag to save keep video if changes found...
	vector<Mat>*frameStack;		// Buffer to store frames before saving to SD card...
	Mat frame;					// Captured single frames...
	int flocation;				// Location of frame in vector stack
	int framesize;				// size of a frame...
	Mat *oneFrame;				// One of the stacked frames...
	
	frameStack = new vector<Mat> [10*frameBuffer*sizeof(cam1.captureVideo())];		// Allocate memory and start the camera...
	if(frameStack == NULL) {
		cerr << " Could not allocate enough memory..." << endl;
		return 0;
	}
	
	while(1){
       // Display the resulting frames...
       frame = cam1.captureVideo();			// Live stream of video on screen...
       framesize = sizeof(frame);
		     
       if(frameCounter < frameBuffer)	{
			frameCounter++;		// Count frames...
			frameStack->push_back(frame);	// Put new frame on stack...
			framesTotal++;
			cout << " ..Frame conter= " << frameCounter << endl;
			// Are there anough changes in the frame?...
			number_of_changes = cam1.detectMotion();
			if(number_of_changes >= there_is_motion)
			{
				cout << "...Motion Detected... number_of_changes= " << number_of_changes << "frameSequence= " << frameSequence << "... FramesTotal = " << framesTotal << " .... Size = " << framesize << endl;
				frameSequence++;	// Need a minimum amount of frames in a sequence showing motion...
				
				// Was motion detected over multiple frames...?
				if(frameSequence > lengthThreshold)  {
					saveVid = 1;		// Set flag to keep this video, as motion was detected
					frameCounter = 0;	// Reset the frame-counter...
				}
				
			} else{
				cout << "::: Set back :::" << endl;
				frameSequence = 0;
			}
		   
		}
		else if(saveVid == 1)  {
			saveVid = 0;
			frameCounter = 0;
			frameSequence = 0;
			cout <<"......Save Video .....\n" << endl;	
			for(int i=0; i<framesTotal; i++)  {
				flocation = i*framesize;
				memcpy(oneFrame, frameStack+flocation, flocation+framesize);
				cam1.saveVideo(oneFrame);	// Capture a frame...
				imshow("saving", *oneFrame);
			}
			frameStack->clear();
			framesTotal = 0;
		}
		else  {
			frameCounter = 0;
			frameSequence = 0;
			cout <<"----- Nothing to save -------" << endl;
			frameStack->clear();
		}
		
		
		if (waitKey(30) == 27) 	//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			frameStack->clear();
            break; 
		}
    }

    return 0;
}
