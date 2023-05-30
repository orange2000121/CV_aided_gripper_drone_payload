#ifndef BOTTOM_CAMERA_H
#define BOTTOM_CAMERA_H

class BottomCamera
{
public:
    void getLocationFromCplusplus(float *location);

private:
};

extern "C" {
    BottomCamera* BottomCamera_new() { return new BottomCamera(); }
    void getLocationFromCplusplus(float *location);
}

#endif // BOTTOM_CAMERA_H