#include "Video.h"
#include "Resources.h"

#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;


Video::Video(float alpha) : mAlpha(alpha)
{
}

void Video::setup()
{   
    try {
        mMovie = qtime::MovieGl( loadResource( "loop_bubble.mov" ) );
        mMovie.setLoop();
        mMovie.play();
        console() << "The movie is loaded and playing." << std::endl;
    } 
    catch ( ... ) {
        console() << "Unable to load or play the movie!" << std::endl;
    }

    mFrameFormat = gl::Texture::Format();
    mFrameFormat.setTargetRect();
    mFrameTexture = gl::Texture( loadImage( loadResource( RES_CINDER_LOGO ) ), mFrameFormat ); 
    
    mCenteredRect = Rectf( mFrameTexture.getBounds() ).getCenteredFit( getWindowBounds(), true );
}

void Video::update()
{
    if( mMovie.checkPlayable() )
		mFrameTexture = gl::Texture( mMovie.getTexture() );
    
    mCenteredRect = Rectf( mFrameTexture.getBounds() ).getCenteredFit( getWindowBounds(), true );
}

void Video::draw()
{ 
    gl::enableAlphaBlending();//the order is important for the velocity 
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA );
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha ) );
    gl::draw(mFrameTexture, mCenteredRect);
    gl::disableAlphaBlending();
    glDisable( GL_BLEND );
}