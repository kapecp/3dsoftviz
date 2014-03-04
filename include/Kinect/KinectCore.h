#ifndef KinectCORE_H
#define KinectCORE_H

#include "Core/Core.h"

namespace Kinect
{
/**
    *@brief Class KinectCore
    *@author Autor: Patrik Hlavac
    *@date 4.3.2014
    */
class KinectCore
{

public:
    KinectCore( QApplication* app);
    ~KinectCore(void);
    /**
         * @author Autor: Patrik Hlavac
         * @brief getInstance Return instance of KinectCore class
         * @param app QApplication
         * @return KinectCore instance
         */
    static KinectCore *getInstance( QApplication* app);

    static KinectCore *mKinectCore;
private:
    QApplication *app;
};
}

#endif // KinectCORE_H
