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

#pragma once

#include <deque>

#include <BlendInt/OpenGL/GLBuffer.hpp>

#include <BlendInt/Gui/AbstractShadow.hpp>

namespace BlendInt {

	class FrameShadow: public AbstractShadow
	{

	public:

		FrameShadow (const Size& size = Size(100, 100), int round_type = RoundNone, float round_radius = 5.f);

		virtual ~FrameShadow ();

		virtual void Draw () const;

		void Draw (float x, float y, short gamma = 0) const;

	protected:

		virtual void PerformSizeUpdate (const Size& size);

		virtual void PerformRoundTypeUpdate (int type);

		virtual void PerformRoundRadiusUpdate (float radius);

	private:

		void InitializeFrameShadowOnce ();

		GLuint vao_;

		GLBuffer<ARRAY_BUFFER> vertex_buffer_;

		GLBuffer<ELEMENT_ARRAY_BUFFER> element_buffer_;

	};

}
