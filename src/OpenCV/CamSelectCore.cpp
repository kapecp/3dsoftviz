#include "OpenCV/CamSelectCore.h"
#include <map>

using namespace OpenCVCam;

OpenCVCam::CamSelectCore * OpenCVCam::CamSelectCore::mCamSelectCore;
std::map<int, cv::VideoCapture> mapcam;
std::vector<cv::VideoCapture> cam;

//typedef Camera cv::VideoCapture;

OpenCVCam::CamSelectCore::CamSelectCore( QApplication* app)
{
	mCamSelectCore = this;
	this->app=app;
	mapcam.clear();
	int max=this->countCameras();
	for (int i=1;i<max;i++){
		std::cout << "kamera c.: " << i << std::endl;
		static QMutex mutex;
		mutex.lock();
		mapcam.insert(std::pair<int,cv::VideoCapture>(i,cv::VideoCapture(i)));
		cv::VideoCapture cap(max+1);
		cam.push_back(cap);//NULL
		mutex.unlock();
	}
	std::cout << "skuska1" << std::endl;
}

cv::VideoCapture OpenCVCam::CamSelectCore::selectCamera()
{
	// implementacia kamery
	QString data;
	for(int i=0; i < cam.size(); i++){
		data.append(QString::number(i+1));
		data.append(",");
		if (cam[i].isOpened()){
			data.append("yes");data.append(",");
			data.append(QString::number((int)cam[i].get(CV_CAP_PROP_FRAME_WIDTH)));
			data.append(",");
			data.append(QString::number((int)cam[i].get(CV_CAP_PROP_FRAME_HEIGHT)));
		} else{
			data.append("no");data.append(",");
			data.append("0");
			data.append(",");
			data.append("0");
		}
		if (i<cam.size()-1) data.append(";");
	}
//	static QMutex mutex;
//	mutex.lock();
//	for(std::map<int, cv::VideoCapture>::iterator itr = mapcam.begin(); itr != mapcam.end(); ++itr) {
//		data.append(QString::number(itr->first));
//		data.append(",");
//		if (itr->second.isOpened()){
//			data.append("yes");data.append(",");
//			//data.append("640");
//			data.append(QString::number((int)itr->second.get(CV_CAP_PROP_FRAME_WIDTH)));
//			data.append(",");
//			//data.append("480");
//			data.append(QString::number((int)itr->second.get(CV_CAP_PROP_FRAME_HEIGHT)));
//			if (itr!= mapcam.end()) data.append(";");
//		} else{
//			data.append("no");data.append(",");
//			data.append("0");
//			data.append(",");
//			data.append("0");
//			if (itr!= mapcam.end()) data.append(";");
//		}
//	}
//	mutex.unlock();
	std::cout << data.toUtf8().constData() << std::endl;
	//data.append("1,yes,640,480;2,no,0,0;3,yes,200,200");
	QOpenCVCam::CamSelectWindow *csw = new QOpenCVCam::CamSelectWindow(AppCore::Core::getInstance(this->app)->getCoreWindow(),this->app,data);

	csw->show();

	return NULL;
}

OpenCVCam::CamSelectCore * OpenCVCam::CamSelectCore::getInstance(QApplication *app)
{
	if(mCamSelectCore == NULL)
	{
		mCamSelectCore = new OpenCVCam::CamSelectCore(app);
	}
	return mCamSelectCore;
}

int OpenCVCam::CamSelectCore::countCameras()
{
   //cv::VideoCapture temp_camera;
   int maxTested = 5;
   for (int i = 1; i < maxTested; i++){
	 cv::VideoCapture temp_camera(i);
	 bool res = (!temp_camera.isOpened());
	 temp_camera.release();
	 temp_camera.~VideoCapture();
	 if (res)
	 {
	   return i;
	 }
   }
   return maxTested;
}

void OpenCVCam::CamSelectCore::setCam(int dev_id, int width, int height){
	cv::VideoCapture cap(dev_id-1);
	cap.set(CV_CAP_PROP_FRAME_WIDTH,(double)width);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,(double)height);
	cam[dev_id-1]=cap;
}
