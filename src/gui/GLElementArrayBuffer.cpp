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

#ifdef __UNIX__
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <BlendInt/GLElementArrayBuffer.hpp>

namespace BlendInt {

	GLElementArrayBuffer::GLElementArrayBuffer()
	: Object(), m_id(0), m_vertices(0)
	{

	}

	GLElementArrayBuffer::~GLElementArrayBuffer ()
	{
		glDeleteBuffers(1, &m_id);
	}

	void GLElementArrayBuffer::Generate()
	{
		if(!m_id)
			Clear();

		glGenBuffers(1, &m_id);
	}

	void GLElementArrayBuffer::Clear()
	{
		glDeleteBuffers(1, &m_id);
		m_id = 0;
	}

	bool GLElementArrayBuffer::IsBbuffer ()
	{
		return glIsBuffer(m_id);
	}

	void GLElementArrayBuffer::Reset()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	GLenum GLElementArrayBuffer::GetUsage ()
	{
		GLint usage = 0;

		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);

		return usage;
	}

	GLint GLElementArrayBuffer::GetBufferSize ()
	{
		GLint buffer_size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);

		return buffer_size;
	}

}
