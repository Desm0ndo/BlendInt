/*
 * This file is part of BlendInt (a Blender-like Interface Library in
 * OpenGL).
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is free software:
 * you can redistribute it and/or modify it under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BlendInt.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#include <BlendInt/Gui/ListView.hpp>

namespace BlendInt {

	ListView::ListView ()
	: AbstractScrollable()
	{
		set_size(400, 300);

	}

	ListView::~ListView ()
	{
	}

	ResponseType ListView::Draw (const RedrawEvent& event)
	{
		return Ignore;
	}

	ResponseType ListView::FocusEvent (bool focus)
	{
		return Ignore;
	}

	ResponseType ListView::CursorEnterEvent (bool entered)
	{
		return Ignore;
	}

	ResponseType ListView::KeyPressEvent (const KeyEvent& event)
	{
		return Ignore;
	}

	ResponseType ListView::ContextMenuPressEvent (
	        const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType ListView::ContextMenuReleaseEvent (
	        const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType ListView::MousePressEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType ListView::MouseReleaseEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType ListView::MouseMoveEvent (const MouseEvent& event)
	{
		return Ignore;
	}

}
