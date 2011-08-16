#include "VideoKin.h"
#include "libfreenect.h"
#include "Resources.h"

#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;

VideoKin::VideoKin() 
{
}

void VideoKin::setup() 
{
    try {
        console() << "There is " << Kinect::getNumDevices() << " Kinect connected." << std::endl;
        mKinect = Kinect( Kinect::Device() );
        console() << "Kinect is initilized." << std::endl; 
    } 
    catch ( ... ) {
        console() << "Unable to initialize the Kinect!" << std::endl;
    } 
    
    mColorFormat = gl::Texture::Format();
    mColorFormat.setTargetRect();
    mColorTexture = gl::Texture( loadImage( loadResource( RES_CINDER_LOGO ) ), mColorFormat );
    mCenteredRect = Rectf( mColorTexture.getBounds() ).getCenteredFit( getWindowBounds(), true );
}

void VideoKin::update()
{
    if( mKinect.checkNewVideoFrame() )
            mColorTexture = gl::Texture( mKinect.getVideoImage() );
   
    mCenteredRect = Rectf( mColorTexture.getBounds() ).getCenteredFit( getWindowBounds(), true );
}

void VideoKin::draw()
{
    if( mColorTexture )
    {
        gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
        gl::draw(mColorTexture, mCenteredRect);
    }

}
