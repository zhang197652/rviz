/*!
 * \copyright (c) Nokia Corporation and/or its subsidiary(-ies) (qt-info@nokia.com) and/or contributors
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 *
 * \license{This source file is part of QmlOgre abd subject to the BSD license that is bundled
 * with this source code in the file LICENSE.}
 */
#include <QOpenGLFunctions>

#include <QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QDebug>
#include <QApplication>
#include <QCursor>

#include "ogrecamera.h"
#include "ogreengine.h"
#include "ogreitem.h"

#include "rviz/display.h"
#include "rviz/display_factory.h"
#include "rviz/display_group.h"
#include "rviz/debug.h"

namespace rviz
{

OgreEngine::OgreEngine(QQuickWindow *window)
    : m_resources_cfg(Ogre::StringUtil::BLANK)
	, m_root(0)
	, m_sceneManager(0)
{
	qmlRegisterType<rviz::OgreCamera>("OgreCamera", 1, 0, "OgreCamera");
    qmlRegisterType<rviz::OgreItem>("OgreItem", 1, 0, "OgreItem");
	
	m_rootDisplayGroup = new DisplayGroup();
	m_rootDisplayGroup->setName( "root" );
	m_displayFactory = new DisplayFactory();
	
	setQuickWindow(window);
}

OgreEngine::~OgreEngine()
{
    delete m_ogreContext;
	delete m_displayFactory;
	if (m_sceneManager) {
        m_root->destroySceneManager(m_sceneManager);
    }
}

Ogre::Root* OgreEngine::startEngine()
{
    qDebug() << "************** OgreEngine::startEngine ******************";
    m_resources_cfg = "resources.cfg";

    activateOgreContext();

    Ogre::Root *ogreRoot = new Ogre::Root;
    //bool rtn = ogreRoot->showConfigDialog();
#if 1
	Ogre::RenderSystem *renderSystem;
	const Ogre::RenderSystemList *rsList;

	// Get the list of available renderers.
#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR == 6
	rsList = ogreRoot->getAvailableRenderers();
#else
	rsList = &(ogreRoot->getAvailableRenderers());
#endif

	// Look for the OpenGL one, which we require.
	renderSystem = NULL;
	for( unsigned int i = 0; i < rsList->size(); i++ )
	{
		renderSystem = rsList->at( i );
		if( renderSystem->getName().compare("OpenGL Rendering Subsystem")== 0 )
		{
			break;
		}
	}

	if( renderSystem == NULL )
	{
		throw std::runtime_error( "Could not find the opengl rendering subsystem!\n" );
	}

	// We operate in windowed mode
	renderSystem->setConfigOption("Full Screen","No");

	/// We used to allow the user to set the RTT mode to PBuffer, FBO, or Copy.
	///   Copy is slow, and there doesn't seem to be a good reason to use it
	///   PBuffer limits the size of the renderable area of the RTT to the
	///           size of the first window created.
	///   FBO seem to be the only good option
	//  renderSys->setConfigOption("RTT Preferred Mode", "FBO");

	// Set the Full Screen Anti-Aliasing factor.
	//if (use_anti_aliasing_) {
	//	renderSys->setConfigOption("FSAA", "4");
	//}
#else	
    Ogre::RenderSystem *renderSystem = ogreRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
#endif
	ogreRoot->setRenderSystem(renderSystem);
    ogreRoot->initialise(false);

    Ogre::NameValuePairList params;

    params["externalGLControl"] = "true";
    params["currentGLContext"] = "true";

    //Finally create our window.
    m_ogreWindow = ogreRoot->createRenderWindow("OgreWindow", 1, 1, false, &params);
    m_ogreWindow->setVisible(false);
    m_ogreWindow->update(false);

    doneOgreContext();
	
	this->setupResources();
	
	m_sceneManager = ogreRoot->createSceneManager(Ogre::ST_GENERIC, "mySceneManager");
    m_sceneManager->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
    m_sceneManager->createLight("myLight")->setPosition(20, 80, 50);

	rviz::RvizDbg::print("OgreEngine DisplayGroup Initialize1");
	m_rootDisplayGroup->initialize( this ); // only initialize() a Display after its sub-properties are created.
	m_rootDisplayGroup->setEnabled( true );
	rviz::RvizDbg::print("OgreEngine DisplayGroup Initialize1");
	m_root = ogreRoot;
	
    return ogreRoot;
}

void OgreEngine::stopEngine(Ogre::Root *ogreRoot)
{
    if (ogreRoot) {
//        m_root->detachRenderTarget(m_renderTexture);
        // TODO tell node(s) to detach

    }

    delete ogreRoot;
}

void OgreEngine::setQuickWindow(QQuickWindow *window)
{    
    Q_ASSERT(window);

    m_quickWindow = window;
    m_qtContext = window->openglContext();//QOpenGLContext::currentContext();

    // create a new shared OpenGL context to be used exclusively by Ogre
    m_ogreContext = new QOpenGLContext();
    m_ogreContext->setFormat(m_quickWindow->requestedFormat());
    m_ogreContext->setShareContext(m_qtContext);
    m_ogreContext->create();
}

void OgreEngine::activateOgreContext()
{
    glPopAttrib();
    glPopClientAttrib();

    m_qtContext->functions()->glUseProgram(0);
    m_qtContext->doneCurrent();

    m_ogreContext->makeCurrent(m_quickWindow);
}

void OgreEngine::doneOgreContext()
{
    m_ogreContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_ogreContext->functions()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    m_ogreContext->functions()->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

    // unbind all possible remaining buffers; just to be on safe side
    m_ogreContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_COPY_READ_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
//    m_ogreContext->functions()->glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//    m_ogreContext->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_TEXTURE_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_ogreContext->doneCurrent();

    m_qtContext->makeCurrent(m_quickWindow);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
}

QOpenGLContext* OgreEngine::ogreContext() const
{
    return m_ogreContext;
}

QSGTexture* OgreEngine::createTextureFromId(uint id, const QSize &size, QQuickWindow::CreateTextureOptions options) const
{
    return m_quickWindow->createTextureFromId(id, size, options);
}

void OgreEngine::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(m_resources_cfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

Display* OgreEngine::createDisplay( const QString& class_lookup_name,
		const QString& name,
		bool enabled )
{
	rviz::RvizDbg::print("OgreEngine createDisplay1");
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	Display* new_display = m_rootDisplayGroup->createDisplay( class_lookup_name );
	addDisplay( new_display, enabled );
	new_display->setName( name );
	QApplication::restoreOverrideCursor();
	
	rviz::RvizDbg::print("OgreEngine createDisplay2");
	
	return new_display;
}

void OgreEngine::addDisplay( Display* display, bool enabled )
{
	m_rootDisplayGroup->addDisplay( display );
	rviz::RvizDbg::print("OgreEngine Display Initialize1");
	display->initialize( this );
	rviz::RvizDbg::print("OgreEngine Display Initialize2");
	display->setEnabled( enabled );
}

}
