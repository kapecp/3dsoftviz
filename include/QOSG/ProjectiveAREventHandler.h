#include <osgGA/GUIEventHandler>
#include <QOSG/ProjectiveARWindow.h>
#include <osgGA/Export>

namespace QOSG {

class ProjectiveAREventHandler : public osgGA::GUIEventHandler
{
public:
	explicit ProjectiveAREventHandler( ProjectiveARWindow* window )
	{
		this->window = window;
	}
	virtual bool handle( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& );
private:
	ProjectiveARWindow* window;

};

bool ProjectiveAREventHandler::handle( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	switch ( ea.getEventType() ) {
		case ( osgGA::GUIEventAdapter::KEYDOWN ): {
			switch ( ea.getKey() ) {
				// FULLSCREEN
				case 'l':
					if ( window->isFullScreen() ) {
						window->showNormal();
					}
					else {
						window->showFullScreen();
					}
					return false;
					break;

				case 'L':
					if ( window->isFullScreen() ) {
						window->showNormal();
					}
					else {
						window->showFullScreen();
					}
					return false;
					break;

				// TOOLBARS
				case 'k':
					window->toggleToolBars();
					return false;
					break;

				case 'K':
					window->toggleToolBars();
					return false;
					break;

				default:
					return false;
			}
		}
		default:
			return false;
	}
}

}
