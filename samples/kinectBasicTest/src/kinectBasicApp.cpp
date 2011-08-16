#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include "libfreenect.h"
#include "CinderOpenCV.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 


#include "Kinect.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class kinectBasicApp : public AppBasic {
  public:
	void prepareSettings( Settings* settings );
	void setup();
	void mouseUp( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
    void chooseFeatures( cv::Mat currentFrame );
	void trackFeatures( cv::Mat currentFrame );
	
	gl::Texture         mColorTexture, mDepthTexture;
    Kinect              mKinect;
	Surface             mColorSurface, mDepthSurface;
    int                 i;
    cv::Mat             mPrevFrame;
    vector<cv::Point2f> mPrevFeatures, mFeatures;
    vector<uint8_t>     mFeatureStatuses;
    bool				mDrawPointsO, mDrawPointsN, mDrawV, mDrawColor, mDrawDepth;
    
    static const int MAX_FEATURES = 300;
};

void kinectBasicApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1280, 480 );//1280,480
    settings->setFullScreen( false );
    settings->setFrameRate( 25.0f );
}

void kinectBasicApp::setup()
{   
    mDrawPointsO = true;
    mDrawPointsN = true;
    mDrawV = false;
    mDrawColor = false;
    mDrawDepth = false;
    
	console() << "There are " << Kinect::getNumDevices() << " Kinects connected." << std::endl;

	mKinect = Kinect( Kinect::Device() ); // the default Device implies the first Kinect connected 
    
    i=1; 
}

void kinectBasicApp::mouseUp( MouseEvent event )
{
	writeImage( getHomeDirectory() + "kinect_video.png", mKinect.getVideoImage() );
	writeImage( getHomeDirectory() + "kinect_depth.png", mKinect.getDepthImage() );
	
	// set tilt to random angle
	mKinect.setTilt( Rand::randFloat() * 62 - 31 );
    
    //set led color each click
    Kinect::LedColor tabLed[7];
    tabLed[0]=Kinect::LED_OFF;
    tabLed[1]=Kinect::LED_GREEN;
    tabLed[2]=Kinect::LED_RED;
    tabLed[3]=Kinect::LED_YELLOW;
    tabLed[4]=Kinect::LED_BLINK_YELLOW;
    tabLed[5]=Kinect::LED_BLINK_GREEN;
    tabLed[6]=Kinect::LED_BLINK_RED_YELLOW;
	mKinect.setLedColor( tabLed[i] );
    i++;
    if (i==6)
    {
        i=0;
    }
    //Size of the surface
    console() << "Windows size is: " << getWindowSize() << endl; 
	//Size of the texture
    console() << "Texture's size is: " << mColorTexture.getSize() << endl;
    // Accelerometer Data
    console() << "Accelerometer data: " << mKinect.getAccel() << std::endl;
}

void kinectBasicApp::keyDown( KeyEvent event )
{
    if(event.getChar() == 'i' )
    {
        // toggle infrared video
        mKinect.setVideoInfrared( ! mKinect.isVideoInfrared() ); 
    }
    if( event.getChar() == 'o' ) 
    {
		mDrawPointsO = ! mDrawPointsO;
	}
    else if( event.getChar() == 'n' )
    {
        mDrawPointsN = ! mDrawPointsN;
    }
    else if( event.getChar() == 'v')
    {
        mDrawV = ! mDrawV;
    }
    else if( event.getChar() == 'c' )
    {
        mDrawColor = ! mDrawColor;
    }
    else if( event.getChar() == 'd' )
    {
        mDrawDepth = ! mDrawDepth;
    }
    else if( event.getChar() == 'a')
    {
        console() << mKinect.getAccel() << endl;
    }
    else if( event.getChar() == 'f' || event.getChar() == 'F' )
    {
		setFullScreen( ! isFullScreen() );
	}
}

void kinectBasicApp::chooseFeatures( cv::Mat currentFrame )
{
	cv::goodFeaturesToTrack( currentFrame, mFeatures, MAX_FEATURES, 0.005, 3.0 );
}

void kinectBasicApp::trackFeatures( cv::Mat currentFrame )
{
	vector<float> errors;
	mPrevFeatures = mFeatures;
	cv::calcOpticalFlowPyrLK( mPrevFrame, currentFrame, mPrevFeatures, mFeatures, mFeatureStatuses, errors );
}

void kinectBasicApp::update()
{	    
	if( mKinect.checkNewVideoFrame() )
    {
        ImageSourceRef colorImage = mKinect.getVideoImage();
        mColorSurface = colorImage;
        mColorTexture = colorImage;
        ImageSourceRef depthImage = mKinect.getDepthImage();
        mDepthSurface = depthImage;
        mDepthTexture = depthImage;
        cv::Mat currentFrame( toOcv( Channel8u( mDepthSurface ) ) );
        if( ! mPrevFrame.data ) 
        {
            
		}
		else 
        {
			chooseFeatures( mPrevFrame );
			trackFeatures( currentFrame );
		}
		mPrevFrame = currentFrame;        
    }   
}

void kinectBasicApp::draw()
{   
    if( ( ! mColorTexture ) || mPrevFeatures.empty() )
		return;
    
    gl::clear( Color( 0, 0, 0 ), true );
    gl::enableAlphaBlending();	
	gl::setMatricesWindow( getWindowSize() );
	glColor3f( 1, 1, 1 );
    if( mColorTexture && mDrawColor )
        gl::draw( mColorTexture, Vec2i( 640, 0 ) );
    if( mDepthTexture && mDrawDepth )
        gl::draw( mDepthTexture, Vec2i( 0, 0));
	glDisable( GL_TEXTURE_2D );
    
    // draw all the old points
	if( mDrawPointsO ) 
    {
        glColor4f( 1, 0, 0, 0.5f );
		for( vector<cv::Point2f>::const_iterator featureIt = mPrevFeatures.begin(); featureIt != mPrevFeatures.end(); ++featureIt )
			gl::drawSolidCircle( fromOcv( *featureIt ), 3);
    }
    
    // draw all the new points
    if( mDrawPointsN )
    {
        glColor4f( 0, 0, 1, 0.5f );
		for( vector<cv::Point2f>::const_iterator featureIt = mFeatures.begin(); featureIt != mFeatures.end(); ++featureIt )
            gl::drawSolidCircle( fromOcv( *featureIt ), 4, 3 );
    }
    
    // draw vector    
    if( mDrawV )
    {    
        glColor4f( 0, 1, 0, 0.5f );
        int cmp=0;
        for( vector<cv::Point2f>::const_iterator featureIt = mPrevFeatures.begin(); featureIt != mPrevFeatures.end(); ++featureIt )
        {               
            Vec2f coord2Do = fromOcv( *featureIt );
            Vec2f coord2Dn = fromOcv(mFeatures.at(cmp));
            Vec3f coord3Do;
            Vec3f coord3Dn;
            coord3Do.x = coord2Do.x;
            coord3Do.y = coord2Do.y;
            coord3Do.z = 0;
            coord3Dn.x = coord2Dn.x;
            coord3Dn.y = coord2Dn.y;
            coord3Dn.z = 0;
            gl::drawVector( coord3Do, coord3Dn);
            cmp++;
        }
	}
}


CINDER_APP_BASIC( kinectBasicApp, RendererGl )
