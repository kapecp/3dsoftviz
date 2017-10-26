#include "Layout/LayoutThread.h"

#include "Layout/FRAlgorithm.h"
#include "Util/ApplicationConfig.h"

namespace Layout {

LayoutThread::LayoutThread( Layout::FRAlgorithm* alg )
{
	this->alg = alg;
	appConf = Util::ApplicationConfig::get();
}

LayoutThread::~LayoutThread( void )
{
}

void LayoutThread::run()
{
	this->sleep( static_cast<unsigned long>( appConf->getValue( "Layout.Thread.StartSleepTime" ).toLong() ) );
	alg->Run();
}
void LayoutThread::pause()
{
	alg->PauseAlg();
}
void LayoutThread::play()
{
	alg->RunAlg();
}
void LayoutThread::wakeUp()
{
	alg->WakeUpAlg();
}
void LayoutThread::setAlphaValue( float val )
{
	alg->SetAlphaValue( val );
}
bool LayoutThread::isRunning()
{
	return alg->IsRunning();
}
void LayoutThread::requestEnd()
{
	alg->RequestEnd();
}
Layout::FRAlgorithm* LayoutThread::getAlg()
{
	return alg;
}
void LayoutThread::playEdgeBundling()
{
	alg->RunAlgEdgeBundling();
}
void LayoutThread::stopEdgeBundling()
{
	alg->StopAlgEdgeBundling();
}
void LayoutThread::setAlphaEdgeBundlingValue( float val )
{
	alg->SetAlphaEdgeBundlingValue( val );
}
void LayoutThread::setProjectiveForceEnabled( bool enable )
{
	alg->setProjectiveForceEnabled( enable );
}
void LayoutThread::pauseAllAlg()
{
	//order is important
	alg->StopAlgEdgeBundling();
	alg->PauseAlg();
}

} // namespace Layout
