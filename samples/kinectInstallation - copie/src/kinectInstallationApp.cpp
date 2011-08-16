#include "VideoKin.h"
#include "Video.h"

#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;

class kinectInstallationApp : public AppBasic {
  public:
	void prepareSettings( Settings* settings );
	void setup();
	void mouseUp( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
    // KINECT AND MOVIE
    VideoKin            mVideoKin;
    Video               mVideo;
    
};

void kinectInstallationApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 640, 480 );
    settings->setFrameRate( 30.0f );
}

void kinectInstallationApp::setup()
{   
    mVideoKin = VideoKin();
    mVideoKin.setup();
    mVideo = Video(0.2);//By default, alpha = 0.5 
    mVideo.setup();
}

void kinectInstallationApp::mouseUp( MouseEvent event )
{	
    //Size of the window
    console() << "Window's size is: " << getWindowSize() << std::endl;
}

void kinectInstallationApp::keyDown( KeyEvent event )
{
    if( event.getCode() == KeyEvent::KEY_ESCAPE )
	{
		this->quit();
		this->shutdown();
	}
    else if( event.getChar() == 'f' || event.getChar() == 'F' )
    {
		setFullScreen( ! isFullScreen() );
	}
}

void kinectInstallationApp::update()
{	    
    mVideoKin.update();
    
    mVideo.update();
}

void kinectInstallationApp::draw()
{
    gl::clear();
    

    mVideoKin.draw();
    
    mVideo.draw();
}

CINDER_APP_BASIC( kinectInstallationApp, RendererGl )
