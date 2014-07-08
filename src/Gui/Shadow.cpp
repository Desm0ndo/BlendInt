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
#include <gl3.h>
#include <gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <math.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

#include <BlendInt/Gui/Shadow.hpp>
#include <BlendInt/Stock/Theme.hpp>
#include <BlendInt/Stock/Shaders.hpp>

namespace BlendInt {

	Shadow::Shadow()
	: AbstractRoundForm(),
	  m_vao(0),
	  m_depth(5)
	{
		set_size(400, 300);
		set_round_type(RoundAll);
		set_radius(5.0);

		InitializeShadow ();
	}

	Shadow::~Shadow()
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	void Shadow::UpdateGeometry (const UpdateRequest& request)
	{
		switch (request.type()) {

			case FormSize: {

				const Size* size_p = static_cast<const Size*>(request.data());

				std::vector<GLfloat> vertices;
				GenerateShadowVerticesExt(*size_p, RoundAll, 10.f, m_depth, vertices);
				m_buffer->Bind();
				m_buffer->SetData(sizeof(GLfloat) * vertices.size(), &vertices[0]);
				m_buffer->Reset();

				break;
			}

			/*
			case ShadowBlurRadius: {

				const float* blur_rad =
								static_cast<const float*>(request.data());

				glBindVertexArray(m_vao);
				GenerateShadowBuffers(size(), radius(), *blur_rad);
				glBindVertexArray(0);
				break;
			}
			*/

			default:
				break;
		}

	}

	void Shadow::Draw (const glm::mat4& mvp, short gamma)
	{
		using Stock::Shaders;

		RefPtr<GLSLProgram> program =
				Shaders::instance->default_triangle_program();
		program->Use();

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
		program->SetUniform1i("AA", 0);
		program->SetUniform1i("Gamma", gamma);
		program->SetVertexAttrib4f("Color", 1.f, 0.f, 0.f, 1.f);

		glBindVertexArray(m_vao);

		for(int i = 0; i < m_depth; i++)
		{
			glDrawArrays(GL_TRIANGLE_STRIP, sizeof(GLfloat) * (36 * 2 + 2) * i, 36 * 2 + 2);
		}

		glBindVertexArray(0);

		GLArrayBuffer::Reset();
		program->Reset();
	}

	void Shadow::DrawAt(const glm::mat4& mvp, int x, int y)
	{
		glm::mat4 transed_mvp = glm::translate(mvp, glm::vec3(x, y, 0.f));
		Draw(transed_mvp);
	}

	void Shadow::DrawAt(const glm::mat4& mvp, const Point& pos)
	{
		glm::mat4 transed_mvp = glm::translate(mvp, glm::vec3(pos.x(), pos.y(), 0.f));
		Draw(transed_mvp);
	}

	void Shadow::InitializeShadow ()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		std::vector<GLfloat> vertices;
		GenerateShadowVerticesExt(size(), RoundAll, 10.f, m_depth, vertices);

		DBG_PRINT_MSG("vertex size: %ld", vertices.size());

		m_buffer.reset(new GLArrayBuffer);
		m_buffer->Generate();
		m_buffer->Bind();
		m_buffer->SetData(sizeof(GLfloat) * vertices.size(), &vertices[0]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindVertexArray(0);
		GLArrayBuffer::Reset();
	}

	void Shadow::GenerateShadowVerticesExt (const Size& size, int round_type,
	        float radius, int depth, std::vector<GLfloat>& vertices)
	{
		float minx = 0.0f;
		float miny = 0.0f;
		float maxx = size.width();
		float maxy = size.height();

		if (2.0f * radius > size.height())
			radius = 0.5f * size.height();

		int count = 0;

		unsigned int corner = round_type & RoundAll;
		while (corner != 0) {
			count += corner & 0x1;
			corner = corner >> 1;
		}
		unsigned int outline_vertex_number = 4 - count + count * WIDGET_CURVE_RESOLU;

		unsigned int max_verts = (outline_vertex_number + 1) * 2 * 2 * depth;

		DBG_PRINT_MSG("max verts: %u", max_verts);

		if(vertices.size() != max_verts) {
			vertices.resize(max_verts);
		}

		float radi = radius;
		float rado = 0.f;
		count = 0;

		for(int i = 0; i < depth; i++) {

			radi = radius + i;
			rado = radi + 1.f;

			/* start with left-top, anti clockwise */
			if (round_type & RoundTopLeft) {
				for (int j = 0; j < WIDGET_CURVE_RESOLU; j++) {

					vertices[count + 0] = minx - i + radi - radi * cornervec[j][0];
					vertices[count + 1] = maxy + i - radi * cornervec[j][1];

					vertices[count + 2] = minx - (i + 1) + rado - rado * cornervec[j][0];
					vertices[count + 3] = maxy + (i + 1) - rado * cornervec[j][1];

					DBG_PRINT_MSG("top left vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

					count += 4;
				}
			} else {

				vertices[count + 0] = minx - i;
				vertices[count + 1] = maxy + i;

				vertices[count + 2] = minx - (i + 1);
				vertices[count + 3] = maxy + (i + 1);

				DBG_PRINT_MSG("top left vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

				count += 4;
			}

			if (round_type & RoundBottomLeft) {
				for (int j = 0; j < WIDGET_CURVE_RESOLU; j++) {

					vertices[count + 0] = minx - i + radi * cornervec[j][1];
					vertices[count + 1] = miny - i + radi - radi * cornervec[j][0];

					vertices[count + 2] = minx - (i + 1) + rado * cornervec[j][1];
					vertices[count + 3] = miny - (i + 1)+ rado - rado * cornervec[j][0];

					DBG_PRINT_MSG("bottom left vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

					count += 4;
				}
			} else {

				vertices[count + 0] = minx - i;
				vertices[count + 1] = miny - i;

				vertices[count + 2] = minx - (i + 1);
				vertices[count + 3] = miny - (i + 1);

				DBG_PRINT_MSG("bottom left vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

				count += 4;
			}

			if (round_type & RoundBottomRight) {
				for (int j = 0; j < WIDGET_CURVE_RESOLU; j++) {

					vertices[count + 0] = maxx + i - radi + radi * cornervec[j][0];
					vertices[count + 1] = miny - i + radi * cornervec[j][1];

					vertices[count + 2] = maxx + (i + 1) - rado + rado * cornervec[j][0];
					vertices[count + 3] = miny - (i + 1) + rado * cornervec[j][1];

					DBG_PRINT_MSG("bottom right vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

					count += 4;
				}
			} else {

				vertices[count + 0] = maxx + i;
				vertices[count + 1] = miny - i;

				vertices[count + 2] = maxx + (i + 1);
				vertices[count + 3] = miny - (i + 1);

				DBG_PRINT_MSG("bottom right vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

				count += 4;
			}

			if (round_type & RoundTopRight) {
				for (int j = 0; j < WIDGET_CURVE_RESOLU; j++) {

					vertices[count + 0] = maxx + i - radi * cornervec[j][1];
					vertices[count + 1] = maxy + i - radi + radi * cornervec[j][0];

					vertices[count + 2] = maxx + (i + 1) - rado * cornervec[j][1];
					vertices[count + 3] = maxy + (i + 1) - rado + rado * cornervec[j][0];

					DBG_PRINT_MSG("top right vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

					count += 4;
				}
			} else {

				vertices[count + 0] = maxx + i;
				vertices[count + 1] = maxy + i;

				vertices[count + 2] = maxx + (i + 1);
				vertices[count + 3] = maxy + (i + 1);

				DBG_PRINT_MSG("top right vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

				count += 4;
			}

			vertices[count + 0] = vertices[count - outline_vertex_number * 4];
			vertices[count + 1] = vertices[count - outline_vertex_number * 4 + 1];

			vertices[count + 2] = vertices[count - outline_vertex_number * 4 + 2];
			vertices[count + 3] = vertices[count - outline_vertex_number * 4 + 3];

			DBG_PRINT_MSG("top left vertices @ (%f, %f) (%f, %f) in i: %d", vertices[count + 0], vertices[count + 1], vertices[count + 2], vertices[count + 3], i);

			DBG_PRINT_MSG("count: %d", count);

			count += 4;

			//radi = radi + (float)i;
		}

	}

}

