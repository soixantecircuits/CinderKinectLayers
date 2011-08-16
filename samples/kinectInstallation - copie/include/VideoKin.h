#pragma once

#include "Kinect.h"

#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"

class VideoKin {
    public:
        VideoKin();
        void setup();
        void update();
        void draw();
    
        ci::Kinect              mKinect;
        ci::gl::Texture::Format mColorFormat;
        ci::gl::Texture         mColorTexture;
        ci::Rectf               mCenteredRect;
};