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

#ifndef _BLENDINT_GUI_COLORDISK_HPP_
#define _BLENDINT_GUI_COLORDISK_HPP_

#include <BlendInt/OpenGL/GLArrayBuffer.hpp>
#include <BlendInt/Gui/Widget.hpp>

namespace BlendInt {

	class ColorDisk: public Widget
	{
		DISALLOW_COPY_AND_ASSIGN(ColorDisk);

	public:

		ColorDisk ();

		virtual ~ColorDisk ();

	private:

		RefPtr<GLArrayBuffer> m_outline;
		RefPtr<GLArrayBuffer> m_inner;

	};

}

#endif /* _BLENDINT_GUI_COLORDISK_HPP_ */