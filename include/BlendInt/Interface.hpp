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
 * License along with BlendInt.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

/**
 * Doxygen documentation
 */

// the top level groups

/** @defgroup time Time input */
/** @defgroup interface The main interface class entry */
/** @defgroup exceptions Exceptions */
/** @defgroup opengl Wrapper classes for OpenGL API */
/** @defgroup gui GUI classes */
/** @defgroup animation Animation */

/**
 * @defgroup functions_throw_exception Functions throw exception
 * @ingroup exceptions
 */

#ifndef _BLENDINT_INTERFACE_HPP_
#define _BLENDINT_INTERFACE_HPP_

#include <BlendInt/Types.hpp>
#include <BlendInt/Size.hpp>

#include <Cpp/Events.hpp>

#include <boost/smart_ptr.hpp>

#define BLENDINT_EVENTS_INIT_ONCE_IN_MAIN Cpp::Events::ProcessInit processInit

namespace BlendInt {

	class AbstractWidget;
	class KeyEvent;
	class MouseEvent;

	/**
	 * @brief The main entry for this library
	 *
	 * @ingroup interface
	 */
	class Interface
	{
	public:

		static Interface* instance ();

		static bool initialize ();

		static void release ();

		bool bind (AbstractWidget* object);

		bool unbind (AbstractWidget* object);

		void Render ();

		void GLFWKeyEvent (int key, int scancode, int action, int mods);

		void GLFWMouseButtonEvent (int button, int action, int mods);

		void GLFWCursorPosEvent (double xpos, double ypos);

		void DispatchKeyEvent (KeyEvent* event);

		void DispatchMousePressEvent (MouseEvent* event);

		void DispatchMouseReleaseEvent (MouseEvent* event);

		void DispatchMouseMoveEvent (MouseEvent* event);

		const Size& size () const;

		void Resize (const Size& size);

		void Resize (unsigned int width, unsigned int height);

		Cpp::EventRef<unsigned int, unsigned int> resized() {return m_resized;}

		// Cpp::ConnectionScope& events () {return m_events;}
		boost::scoped_ptr<Cpp::ConnectionScope>& events() {return m_events;}

	private:

		void dispatch_key_press_event (AbstractWidget* obj, KeyEvent* event);

		void dispatch_mouse_press_event (AbstractWidget* obj, MouseEvent* event);

		void dispatch_mouse_release_event (AbstractWidget* obj, MouseEvent* event);

		void dispatch_mouse_move_event (AbstractWidget* obj, MouseEvent* event);

		Interface ();

		~Interface ();

		double cursor_pos_x_;	/** cursor x position */
		double cursor_pos_y_;	/** cursor y position */

		Size m_size;

		boost::scoped_ptr<Cpp::ConnectionScope> m_events;

		Cpp::Event<unsigned int, unsigned int> m_resized;

		static Interface* interface;

#ifdef DEBUG
		void draw_grid (int width, int height);
#endif

	};

	inline Interface* interface()
	{
		return Interface::instance();
	}


}

#endif /* _BIL_INTERFACE_HPP_ */

