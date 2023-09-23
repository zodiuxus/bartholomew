#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
 
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
 
/*
 Before using the OV5640 camera, it need to be set (see command.txt)
 For instance: $ sudo media-ctl --device /dev/media1 --set-v4l2 '"ov56402-003c":0[fmt:YUYV8_2X8/640x480]' do not set large resolutions, due to
 the limited memory (500 MB) Use cap(1) because the camera lives at
 /dev/video1
*/
 
using namespace cv;
using namespace std;
 
struct termios tty;
 
int main(int argc, const char ** argv)
{
  /*
    The part of the code responsible for serial communication
  */
 
  int serial_port = open("/dev/ttyUSB0", O_RDWR);
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
  }
 
  tty.c_iflag &= ~ISIG;
  tty.c_iflag &= ~( IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
  tty.c_oflag &= ~OPOST;
  tty.c_oflag &= ~ONLCR;
 
  cfsetspeed(&tty, B9600);
 
  if(tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }
  unsigned char seenFace[] = {'Y', '\r'};
 
  /*
    The part of the code responsible for facial recognition and sending the signal
  */

  CascadeClassifier face_cascade;
  face_cascade.load(samples::findFile("haarcascade_frontalface_default.xml"));
 
  VideoCapture cap(1);
  if (!cap.isOpened()) {
    cerr << "ERROR: Unable to open the camera" << endl;
    return 0;
  }
 
  Mat frame;
  Mat frame_gray;
 
  cout << "Start grabbing, press a key on Live window to terminate" << endl;
  while(1) {
    cap >> frame;
    if (frame.empty()) {
        cerr << "ERROR: Unable to grab from the camera" << endl;
        break;
    }
 
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    std::vector<Rect> faces;
    face_cascade.detectMultiScale( frame_gray, faces );
    if (faces.size()) {
       write(serial_port, seenFace, sizeof(seenFace));
    }
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        rectangle(frame, faces[i], Scalar(0, 255, 0));
        Mat faceROI = frame_gray( faces[i] );
 
    }
    //-- Show what you got
    imshow( "Capture - Face detection", frame );
    int key = waitKey(20);
    key = (key==255) ? -1 : key;
    if (key>=0)
      break;
  }
 
  cout << "Closing the camera" << endl;
  cap.release();
  destroyAllWindows();
  close(serial_port);
  cout << "bye!" <<endl;
  return 0;
}
