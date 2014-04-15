/*
 * This file is part of BlendInt (a Blender-like Interface Library in
 * OpenGL).
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is free
 * software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BlendInt.	 If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#ifdef __UNIX__
#ifdef __APPLE__
#include <gl3.h>
#include <gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <glm/glm.hpp>

#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

#include <BlendInt/Interface.hpp>

#include <BlendInt/Core/Size.hpp>

#ifdef USE_FONTCONFIG
#include <BlendInt/Core/FontConfig.hpp>
#endif

#include <BlendInt/OpenGL/GLTexture2D.hpp>
#include <BlendInt/OpenGL/GLFramebuffer.hpp>
#include <BlendInt/OpenGL/GLRenderbuffer.hpp>

#include <BlendInt/Gui/FontCache.hpp>
#include <BlendInt/Gui/AbstractWidget.hpp>
#include <BlendInt/Gui/AbstractContainer.hpp>

#include <BlendInt/Window/KeyEvent.hpp>
#include <BlendInt/Window/MouseEvent.hpp>
#include <BlendInt/Window/ContextMenuEvent.hpp>

#include <BlendInt/Service/Theme.hpp>
#include <BlendInt/Service/ShaderManager.hpp>
#include <BlendInt/Service/ContextManager.hpp>
#include <BlendInt/Service/StockItems.hpp>

#include "Intern/ScreenBuffer.hpp"

namespace BlendInt {

	Interface* Interface::instance = 0;

	bool Interface::Initialize ()
	{
		/*
		 glewExperimental = true;	// Needed in core profile
		 if (glewInit() != GLEW_OK) {
		 std::cerr << "Fail to initialize GLEW" << endl;
		 return false;
		 }

		 if (!GLEW_VERSION_2_0) {
		 std::cerr << "No support for OpenGL 2.0 found" << std::endl;
		 return false;
		 }
		 */

		bool success = true;

#ifdef DEBUG
		int major, minor;
		GetGLVersion(&major, &minor);
		DBG_PRINT_MSG("OpenGL version: %d.%d", major, minor);
		GetGLSLVersion(&major, &minor);
		DBG_PRINT_MSG("OpenGL shading language version: %d.%d", major, minor);
#endif

		if (!instance) {
			instance = new Interface();
		}

		if (!instance) {
			success = false;
		}

#ifdef USE_FONTCONFIG
		if (success && FontConfig::initialize()) {

			/*
			 FontConfig* ftconfig = FontConfig::instance();
			 if (!ftconfig->loadDefaultFontToMem()) {
			 cerr << "Cannot load default font into memory" << endl;
			 success = false;
			 }
			 */

		} else {

			DBG_PRINT_MSG("%s", "Cannot initialize FontConfig");
			success = false;

		}
#endif

		if (success && ThemeManager::initialize()) {
			// do nothing
		} else {
			DBG_PRINT_MSG("%s", "Cannot initialize Themes");
			success = false;
		}

		if (success && ShaderManager::Initialize()) {
			// do nothing
		} else {
			DBG_PRINT_MSG("%s", "The Shader Manager is not initialized successfully!");
			success = false;
		}

		if (success && StockItems::Initialize()) {
			// do nothing
		} else {
			DBG_PRINT_MSG("%s", "Cannot initialize Stock Icons");
			success = false;
		}

		if (success && ContextManager::Initialize()) {

		} else {
			DBG_PRINT_MSG("%s", "Cannot initialize Context Manager");
			success = false;
		}

#ifdef USE_FONTCONFIG

#ifdef __APPLE__

		// Create a default font
		FontCache::Create("Sans-Serif", 9, 96, false, false);

#endif

#ifdef __LINUX__

		// Create a default font
		FontCache::Create("Sans", 9, 96, false, false);

#endif

#endif

		return success;
	}

	void Interface::Release ()
	{
		ContextManager::Release();
		StockItems::Release();
		ShaderManager::Release();
		ThemeManager::release();
		FontCache::releaseAll();

#ifdef USE_FONTCONFIG
		FontConfig::release();
#endif

		if (instance) {
			delete instance;
			instance = 0;
		}
	}

	Interface::Interface ()
	: m_focus_style(FocusOnClick), m_current_context(0)
	{
		m_events.reset(new Cpp::ConnectionScope);
	}

	Interface::~Interface ()
	{
		if(m_current_context) {

			if(m_current_context->managed() &&
							(m_current_context->count() == 0)) {
				m_current_context->destroyed().disconnectOne(this, &Interface::OnContextDestroyed);
				delete m_current_context;
			}
		}
	}

	const Size& Interface::size () const
	{
		return ContextManager::instance->size();
	}

	void Interface::Resize (const Size& size)
	{
		//ContextManager::instance->ResizeFromInterface(size);
		if(m_current_context) {
			m_current_context->Resize(size);
		}
	}

	void Interface::ResizeContext(Context* context, const Size& size)
	{
		context->Resize(size);
	}

	void Interface::Resize (unsigned int width, unsigned int height)
	{
		//ContextManager::instance->ResizeFromInterface(width, height);
		if(m_current_context) {
			m_current_context->Resize(width, height);
		}
	}

	void Interface::ResizeContext(Context* context, unsigned int width, unsigned int height)
	{
		context->Resize(width, height);
	}

	void Interface::Draw ()
	{
//		ContextManager::instance->DrawFromInterface();

		if(m_current_context) {
			m_current_context->Draw();
		}
	}

	void Interface::Draw (const RedrawEvent& event)
	{
		if(m_current_context) {
			m_current_context->Draw(event);
		}
	}

	void Interface::DrawContext (Context* context, const RedrawEvent& event)
	{
		ResponseType response = context->Draw(event);

		if(response == Accept) {
			// DO nothing;
		}
	}

	void Interface::DispatchKeyEvent (Context* context, const KeyEvent& event)
	{
		if(AbstractWidget::focused_widget) {
			switch (event.action()) {

				case KeyPress: {
#ifdef DEBUG
					if(event.key() == Key_F6 && event.text().empty()) {
						//DrawToOffScreen();
						//RenderToImage();
					}
#endif
					AbstractWidget::focused_widget->KeyPressEvent(event);
					break;
				}

				case KeyRelease: {
					// item->KeyReleaseEvent(dynamic_cast<BlendInt::KeyEvent*>(event));
					//cm->m_focus->KeyReleaseEvent(event);
					break;
				}

				case KeyRepeat: {
					// item->KeyRepeatEvent(&event);
					break;
				}

				default:
				break;
			}
		}
	}

	void Interface::DispatchKeyEvent (const KeyEvent& event)
	{
		/*
		if(AbstractWidget::focused_widget) {
			switch (event.action()) {

				case KeyPress: {
#ifdef DEBUG
					if(event.key() == Key_F6 && event.text().empty()) {
						//DrawToOffScreen();
						//RenderToImage();
					}
#endif
					AbstractWidget::focused_widget->KeyPressEvent(event);
					break;
				}

				case KeyRelease: {
					// item->KeyReleaseEvent(dynamic_cast<BlendInt::KeyEvent*>(event));
					//cm->m_focus->KeyReleaseEvent(event);
					break;
				}

				case KeyRepeat: {
					// item->KeyRepeatEvent(&event);
					break;
				}

				default:
				break;
			}
		}
		*/
		if(m_current_context) {
			DispatchKeyEvent(m_current_context, event);
		}
	}

	void Interface::DispatchMouseEvent (Context* context, const MouseEvent& event)
	{
		switch (event.action()) {

			case MouseMove: {
				DispatchCursorMoveEvent(event);
				return;
			}

			case MousePress: {
				DispatchMousePressEvent(event);
				return;
			}

			case MouseRelease: {
				DispatchMouseReleaseEvent(event);
				return;
			}

			default:
			break;
		}
	}


	void Interface::DispatchMouseEvent (const MouseEvent& event)
	{
		/*
		switch (event.action()) {

			case MouseMove: {
				DispatchCursorMoveEvent(event);
				return;
			}

			case MousePress: {
				DispatchMousePressEvent(event);
				return;
			}

			case MouseRelease: {
				DispatchMouseReleaseEvent(event);
				return;
			}

			default:
			break;
		}
		*/

		if(m_current_context) {
			DispatchMouseEvent(m_current_context, event);
		}
	}

	void Interface::RenderToImage()
	{
		GLsizei width = ContextManager::instance->size().width();
		GLsizei height = ContextManager::instance->size().height();

		// Create and set texture to render to.
		GLTexture2D* tex = new GLTexture2D;
		tex->Generate();
		tex->Bind();
		tex->SetWrapMode(GL_REPEAT, GL_REPEAT);
		tex->SetMinFilter(GL_NEAREST);
		tex->SetMagFilter(GL_NEAREST);
		tex->SetImage(0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		GLFramebuffer* fb = new GLFramebuffer;
		fb->Generate();
		fb->Bind();

		// Set "renderedTexture" as our colour attachement #0
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, tex->id(), 0);
		//fb->Attach(*tex, GL_COLOR_ATTACHMENT0);

		GLuint rb = 0;
		glGenRenderbuffers(1, &rb);

		glBindRenderbuffer(GL_RENDERBUFFER, rb);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
				width, height);

		//-------------------------

//		//Attach depth buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, rb);

		//-------------------------
		//Does the GPU support current FBO configuration?
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status) {
			case GL_FRAMEBUFFER_COMPLETE:
			DBG_PRINT_MSG("%s", "good");
			break;
			default:
			DBG_PRINT_MSG("%s", "Fail to check framebuffer status");
			break;
		}

		//-------------------------
		//and now render to GL_TEXTURE_2D
		fb->Bind();

		Draw();

		//Bind 0, which means render to back buffer
		fb->Reset();

		tex->WriteToFile("output.png");
		tex->Reset();

		//Delete resources
		delete tex; tex = 0;

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glDeleteRenderbuffers(1, &rb);

		//Bind 0, which means render to back buffer, as a result, fb is unbound
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		fb->Reset();
		delete fb; fb = 0;

		//Draw();
	}

	void Interface::DispatchCursorMoveEvent (const MouseEvent& event)
	{
		AbstractWidget* widget = 0;
		ContextManager* cm = ContextManager::instance;
		ResponseType response;

		// search which widget in stack contains the cursor
		while (cm->m_hover_deque->size()) {

			if (cm->m_hover_deque->back()->visiable() &&
							cm->m_hover_deque->back()->Contain(event.position())) {
				widget = cm->m_hover_deque->back();
				break;
			} else {
				cm->m_hover_deque->back()->CursorEnterEvent(false);
				cm->m_hover_deque->back()->m_flag.reset(AbstractWidget::WidgetFlagContextHoverList);
			}

			cm->m_hover_deque->pop_back();
		}

		BuildWidgetListAtCursorPoint(event.position(), widget);

		/*
		for (std::deque<AbstractWidget*>::iterator it =
				cm->m_hover_deque->begin(); it != cm->m_hover_deque->end();
				it++)
		{
			DBG_PRINT_MSG("cursor on: %s", (*it)->name().c_str());
		}
		*/

		// tell the focused widget first
		if(AbstractWidget::focused_widget) {
			response = AbstractWidget::focused_widget->MouseMoveEvent(event);

			if(response == AcceptAndBreak)
				return;
			// check the event status
		}

		for (std::deque<AbstractWidget*>::reverse_iterator it =
				cm->m_hover_deque->rbegin(); it != cm->m_hover_deque->rend();
				it++)
		{
			response = (*it)->MouseMoveEvent(event);
			// check the event status
			if (response == Accept) {
				// TODO: do sth
			}
		}

	}

	void Interface::DispatchMousePressEvent(const MouseEvent& event)
	{
		ContextManager* cm = ContextManager::instance;
		ResponseType response;
		bool focus_set = false;

		for(std::deque<AbstractWidget*>::reverse_iterator it = cm->m_hover_deque->rbegin(); it != cm->m_hover_deque->rend(); it++)
		{
			response = (*it)->MousePressEvent(event);
			//DBG_PRINT_MSG("mouse press: %s", (*it)->name().c_str());

			if((!focus_set) && (response == Accept)) {
				cm->SetFocusedWidget(*it);
				focus_set = true;
			}

			if(response == AcceptAndBreak) {
				break;
			}
		}
	}

	void Interface::DispatchMouseReleaseEvent(const MouseEvent& event)
	{
		ContextManager* cm = ContextManager::instance;
		ResponseType response;

		// tell the focused widget first
		if(AbstractWidget::focused_widget) {
			response = AbstractWidget::focused_widget->MouseReleaseEvent(event);

			// Check the event status
		}

		for(std::deque<AbstractWidget*>::reverse_iterator it = cm->m_hover_deque->rbegin(); it != cm->m_hover_deque->rend(); it++)
		{
			response = (*it)->MouseReleaseEvent(event);

			if(response == AcceptAndBreak) break;
		}
	}

	void Interface::BuildWidgetListAtCursorPoint (const Point& cursor,
			AbstractWidget* parent)
	{
		/*
		if (parent) {
			parent->m_flag.set(AbstractWidget::WidgetFlagContextHoverList);

			AbstractContainer* p = dynamic_cast<AbstractContainer*>(parent);
			if(p) {
				for (WidgetDeque::iterator it =
						p->m_sub_widgets->begin(); it != p->m_sub_widgets->end();
						it++) {
					if ((*it)->visiable() && (*it)->Contain(cursor)) {
						ContextManager::instance->m_hover_deque->push_back(*it);
						ContextManager::instance->m_hover_deque->back()->CursorEnterEvent(true);
						BuildWidgetListAtCursorPoint(cursor, *it);
						break;	// if break or continue the loop?
					}
				}
			} else {
			}
		} else {
			ContextManager::instance->m_hover_deque->clear();

			map<int, ContextLayer>::reverse_iterator map_it;
			set<AbstractWidget*>::iterator set_it;
			set<AbstractWidget*>* set_p = 0;

			bool stop = false;

			for (map_it = ContextManager::instance->m_layers.rbegin();
							map_it != ContextManager::instance->m_layers.rend();
							map_it++) {
				set_p = map_it->second.widgets;
				for (set_it = set_p->begin(); set_it != set_p->end();
						set_it++) {
					if ((*set_it)->visiable() && (*set_it)->Contain(cursor)) {
						ContextManager::instance->m_hover_deque->push_back(*set_it);
						ContextManager::instance->m_hover_deque->back()->CursorEnterEvent(true);
						BuildWidgetListAtCursorPoint(cursor, *set_it);
						stop = true;
					}

					if (stop)
					break;
				}
				if (stop)
				break;
			}
		}
		*/
	}

	void Interface::GetGLVersion (int *major, int *minor)
	{
		const char* verstr = (const char*) glGetString(GL_VERSION);
		if((verstr == NULL) || (sscanf(verstr, "%d.%d", major, minor) != 2)) {
			*major = *minor = 0;
			fprintf(stderr, "Invalid GL_VERSION format!!!\n");
		}
	}

	void Interface::DispatchCursorMoveEvent(Context* context, const MouseEvent& event)
	{
		AbstractWidget* widget = 0;
		ResponseType response;

		// search which widget in stack contains the cursor
		while (context->m_hover_deque->size()) {

			if (context->m_hover_deque->back()->visiable() &&
							context->m_hover_deque->back()->Contain(event.position())) {
				widget = context->m_hover_deque->back();
				break;
			} else {
				context->m_hover_deque->back()->CursorEnterEvent(false);
				context->m_hover_deque->back()->m_flag.reset(AbstractWidget::WidgetFlagContextHoverList);
			}

			context->m_hover_deque->pop_back();
		}

		context->BuildCursorHoverList(event, widget);

		/*
		for (std::deque<AbstractWidget*>::iterator it =
				cm->m_hover_deque->begin(); it != cm->m_hover_deque->end();
				it++)
		{
			DBG_PRINT_MSG("cursor on: %s", (*it)->name().c_str());
		}
		*/

		// tell the focused widget first
		if(AbstractWidget::focused_widget) {
			response = AbstractWidget::focused_widget->MouseMoveEvent(event);

			if(response == AcceptAndBreak)
				return;
			// check the event status
		}

		for (std::deque<AbstractWidget*>::reverse_iterator it =
				context->m_hover_deque->rbegin(); it != context->m_hover_deque->rend();
				it++)
		{
			response = (*it)->MouseMoveEvent(event);
			// check the event status
			if (response == Accept) {
				// TODO: do sth
			}
		}
	}

	void Interface::DispatchMousePressEvent(Context* context, const MouseEvent& event)
	{
		ResponseType response;
		bool focus_set = false;

		for(std::deque<AbstractWidget*>::reverse_iterator it = context->m_hover_deque->rbegin(); it != context->m_hover_deque->rend(); it++)
		{
			response = (*it)->MousePressEvent(event);
			//DBG_PRINT_MSG("mouse press: %s", (*it)->name().c_str());

			if((!focus_set) && (response == Accept)) {
				context->SetFocusedWidget(*it);
				focus_set = true;
			}

			if(response == AcceptAndBreak) {
				break;
			}
		}
	}

	void Interface::DispatchMouseReleaseEvent(Context* context, const MouseEvent& event)
	{
		ResponseType response;

		// tell the focused widget first
		if(AbstractWidget::focused_widget) {
			response = AbstractWidget::focused_widget->MouseReleaseEvent(event);

			// Check the event status
		}

		for(std::deque<AbstractWidget*>::reverse_iterator it = context->m_hover_deque->rbegin(); it != context->m_hover_deque->rend(); it++)
		{
			response = (*it)->MouseReleaseEvent(event);

			if(response == AcceptAndBreak) break;
		}
	}

	void Interface::SetCurrentContext(Context* context)
	{
		if(m_current_context) {
			m_current_context->destroyed().disconnectOne(this, &Interface::OnContextDestroyed);
		}

		m_current_context = context;
		m_events->connect(m_current_context->destroyed(), this, &Interface::OnContextDestroyed);
	}

	void Interface::OnContextDestroyed(AbstractWidget* context)
	{
		if(context == m_current_context) {
			m_current_context->destroyed().disconnectOne(this, &Interface::OnContextDestroyed);
			m_current_context = 0;
		}
	}

	unsigned int Interface::GetCurrentContextWidth() const
	{
		unsigned int w = 0;

		if(m_current_context) {
			w = m_current_context->size().width();
		}

		return w;
	}

	unsigned int Interface::GetCurrentContextHeight() const
	{
		unsigned int h = 0;

		if(m_current_context) {
			h = m_current_context->size().height();
		}

		return h;
	}

	void Interface::GetGLSLVersion (int *major, int *minor)
	{
		int gl_major, gl_minor;
		GetGLVersion(&gl_major, &gl_minor);

		*major = *minor = 0;
		if(gl_major == 1) {
			/* GL v1.x can provide GLSL v1.00 only as an extension */
			const char* extstr = (const char*)glGetString(GL_EXTENSIONS);
			if((extstr != NULL) && (strstr(extstr, "GL_ARB_shading_language_100") != NULL))
			{
				*major = 1;
				*minor = 0;
			}
		} else if (gl_major >= 2) {
			/* GL v2.0 and greater must parse the version string */
			const char* verstr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

			if((verstr == NULL) || (sscanf(verstr, "%d.%d", major, minor) != 2))
			{
				*major = *minor = 0;
				fprintf(stderr, "Invalid GL_SHADING_LANGUAGE_VERSION format!!!\n");
			}
		}
	}

}
