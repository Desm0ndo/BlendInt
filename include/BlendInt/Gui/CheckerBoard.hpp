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

#ifndef _BLENDINT_CHECKERBOARD_HPP_
#define _BLENDINT_CHECKERBOARD_HPP_

#include <BlendInt/OpenGL/GLArrayBuffer.hpp>
#include <BlendInt/OpenGL/GLElementArrayBuffer.hpp>

#include <BlendInt/Gui/AbstractForm.hpp>

namespace BlendInt {

	/**
	 * @brief CheckerBoard form
	 *
	 * A form show a gray checkerboard: light: #999999, dark: #666666
	 */
	class CheckerBoard: public AbstractForm
	{
	public:

		explicit CheckerBoard (size_t cell_size);

		virtual ~CheckerBoard ();

		void SetCellSize (size_t size);

		size_t cell_size () const {return m_cell_size;}

		virtual void Draw (const glm::mat4& mvp, short gamma = 0);

	protected:

		virtual void UpdateGeometry (const UpdateRequest& request);

		static void GenerateCheckerVertices (const Size& size,
						size_t cell_size,
						std::vector<GLfloat>* vertices,
						std::vector<GLuint>* light_indices,
						std::vector<GLuint>* dark_indices);


		static void GenerateCheckerVertices (size_t num,
						size_t cell_size,
						std::vector<GLfloat>* vertex,
						std::vector<GLuint>* index);

	private:

		void InitOnce ();

		GLuint m_vao;

		RefPtr<GLArrayBuffer> m_vbo;
		RefPtr<GLElementArrayBuffer> m_light_ibo;
		RefPtr<GLElementArrayBuffer> m_dark_ibo;


		size_t m_cell_size;
	};

}

#endif /* _BLENDINT_CHECKERBOARD_HPP_ */
