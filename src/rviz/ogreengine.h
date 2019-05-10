/*!
 * \copyright (c) Nokia Corporation and/or its subsidiary(-ies) (qt-info@nokia.com) and/or contributors
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 *
 * \license{This source file is part of QmlOgre abd subject to the BSD license that is bundled
 * with this source code in the file LICENSE.}
 */

#ifndef OGREENGINEITEM_H
#define OGREENGINEITEM_H

#include <QObject>
#include <QQuickWindow>
#include <QOpenGLContext>
#include <Ogre.h>

#include "rviz/display_context.h"

namespace Ogre {
class Root;
class RenderTexture;
class Viewport;
class RenderTarget;
}

namespace rviz
{
class ColorProperty;
class Display;
class DisplayFactory;
class DisplayGroup;
class FrameManager;
class Property;
class BoolProperty;
class IntProperty;
class PropertyTreeModel;
class RenderPanel;
class SelectionManager;
class StatusList;
class TfFrameProperty;
class ViewportMouseEvent;
class WindowManagerInterface;
class Tool;
class OgreRenderQueueClearer;

class VisualizationManagerPrivate;
	
/**
 * @brief The OgreEngineItem class
 * Must only be constructed from within Qt QML rendering thread.
 */
class OgreEngine : public DisplayContext
{
    Q_OBJECT

public:
    OgreEngine(QQuickWindow *quickWindow = 0);
    ~OgreEngine();
	
	Ogre::SceneManager* getSceneManager() const { return m_sceneManager; }
	
    Ogre::Root *startEngine();
    void stopEngine(Ogre::Root *ogreRoot);

    void activateOgreContext();
    void doneOgreContext();

    QOpenGLContext* ogreContext() const;

	QSGTexture* createTextureFromId(uint id, const QSize &size, QQuickWindow::CreateTextureOptions options = QQuickWindow::CreateTextureOption(0)) const;
	
    void setupResources(void);
	
	Display* createDisplay( const QString& class_lookup_name, const QString& name, bool enabled );
	void addDisplay( Display* display, bool enabled );
	
	QString getFixedFrame() const {return "fixedFrame";}
	void setFixedFrame( const QString& frame ) {}
	
	tf::TransformListener* getTFClient() const {return NULL;}
	SelectionManager* getSelectionManager() const { return NULL; }
	virtual ToolManager* getToolManager() const { return NULL; }
	virtual ViewManager* getViewManager() const { return NULL; }
	WindowManagerInterface* getWindowManager() const { return NULL; }
	FrameManager* getFrameManager() const { return NULL; }
	uint64_t getFrameCount() const { return 0; }
    ros::CallbackQueueInterface* getUpdateQueue() { return NULL; }
	ros::CallbackQueueInterface* getThreadedQueue() { return NULL; }
	void handleChar( QKeyEvent* event, RenderPanel* panel ) {}
	void handleMouseEvent( const ViewportMouseEvent& event ) {}
	void queueRender() {}
	virtual uint32_t getDefaultVisibilityBit() const { return 0; }
	virtual BitAllocator* visibilityBits() { return NULL; }
	virtual void setStatus( const QString & message ) {}

	virtual DisplayGroup* getRootDisplayGroup() const { return m_rootDisplayGroup; }
	virtual DisplayFactory* getDisplayFactory() const { return m_displayFactory; }

private:
    Ogre::String m_resources_cfg;
    Ogre::RenderWindow *m_ogreWindow;
	
	Ogre::Root *m_root;
	Ogre::SceneManager *m_sceneManager;
	
    QQuickWindow *m_quickWindow;

    /** Pointer to QOpenGLContext to be used by Ogre. */
    QOpenGLContext* m_ogreContext;
    /** Pointer to QOpenGLContext to be restored after Ogre context. */
    QOpenGLContext* m_qtContext;
	
	DisplayGroup *m_rootDisplayGroup;
	DisplayFactory *m_displayFactory;

protected:
    void setQuickWindow(QQuickWindow *window);
};

}

#endif // OGREENGINEITEM_H
