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
#endif	// __UNIX__

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Gui/ToolBar.hpp>
#include <BlendInt/Service/ShaderManager.hpp>
#include <BlendInt/Service/Theme.hpp>

namespace BlendInt {

	ToolBar::ToolBar ()
	: AbstractDequeContainer(),
	  m_vao(0),
	  m_space(4),
	  m_move_status(false),
	  m_last_x(0),
	  m_start_x(0)
	{
		//set_preferred_size(200, 32);
		set_size(200, 32);

		set_margin(4, 4, 4, 4);	// the same as MenuBar

		set_expand_x(true);
		set_expand_y(false);
		set_scissor_test(true);

		InitializeToolBar();
	}

	ToolBar::~ToolBar ()
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	void ToolBar::Add (AbstractWidget* widget)
	{
		int x = GetLastPosition();
		int y = position().y() + margin().bottom();
		unsigned int h = size().height() - margin().top() - margin().bottom();

		if(PushBackSubWidget(widget)) {
			SetSubWidgetPosition(widget, x, y);
			if(widget->expand_y()) {
				ResizeSubWidget(widget, widget->size().width(), h);
			} else {

				if(widget->size().height() > h) {
					ResizeSubWidget(widget, widget->size().width(), h);
				} else {
					SetSubWidgetPosition(widget, x,
									y + (h - widget->size().height()) / 2);
				}

			}
		}
	}

	void ToolBar::AddButton (const RefPtr<ActionItem>& action)
	{
		ToolButton* button = Manage(new ToolButton);
		int x = GetLastPosition();
		int y = position().y() + margin().bottom();
		unsigned int h = size().height() - margin().top() - margin().bottom();

		if(PushBackSubWidget(button)) {
			SetSubWidgetPosition(button, x, y);
			if(button->expand_y()) {
				ResizeSubWidget(button, button->size().width(), h);
			} else {

				if(button->size().height() > h) {
					ResizeSubWidget(button, button->size().width(), h);
				} else {
					SetSubWidgetPosition(button, x,
									y + (h - button->size().height()) / 2);
				}

			}
		}
	}

	Size ToolBar::GetPreferredSize () const
	{
		Size preferred_size;

		if(sub_widget_size() == 0) {

			Font font;	// Get default font height
			preferred_size.set_width(200);

			int max_font_height = font.GetHeight();

			preferred_size.set_height(
			        max_font_height
			                + margin().top()
			                + margin().bottom());	// top padding: 2, bottom padding: 2

		} else {

			AbstractWidget* widget = 0;
			Size tmp_size;

			preferred_size.set_width(-m_space);
			for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
			{
				widget = *it;

				if(widget->visiable()) {
					tmp_size = widget->GetPreferredSize();

					preferred_size.add_width(tmp_size.width() + m_space);
					preferred_size.set_height(std::max(preferred_size.height(), tmp_size.height()));
				}
			}

			preferred_size.add_width(margin().left() + margin().right());
			preferred_size.add_height(margin().top() + margin().bottom());
		}

		return preferred_size;
	}

	void ToolBar::Update (const UpdateRequest& request)
	{
		if(request.source() == Predefined) {

			switch (request.type()) {

				case FormPosition: {

					const Point* pos_p = static_cast<const Point*>(request.data());

					int x = pos_p->x() - position().x();
					int y = pos_p->y() - position().y();

					MoveSubWidgets(x, y);

					break;
				}

				case FormSize: {
					const Size* size_p = static_cast<const Size*>(request.data());

					glBindVertexArray(m_vao);

					std::vector<GLfloat> vertices(12);

					GenerateFlatRectVertices(*size_p, 0.f, &vertices);

					m_inner->Bind();
					m_inner->SetData(sizeof(GLfloat) * vertices.size(), &vertices[0]);
					m_inner->Reset();

					glBindVertexArray(0);

					int x = position().x() + margin().left();
					if(sub_widget_size()) {
						x = sub_widgets()->front()->position().x();
					}

					int y = position().y() + margin().bottom();
					unsigned int w = size_p->width() - margin().left() - margin().right();
					unsigned int h = size_p->height() - margin().top() - margin().bottom();

					FillSubWidgets(x, y, w, h, m_space);

					break;
				}

				case ContainerMargin: {
					const Margin* margin_p = static_cast<const Margin*>(request.data());

					int x = position().x() + margin_p->left();
					if(sub_widget_size()) {
						x = sub_widgets()->front()->position().x();
					}

					int y = position().y() + margin_p->bottom();
					unsigned int w = size().width() - margin_p->left() - margin_p->right();
					unsigned int h = size().height() - margin_p->top() - margin_p->bottom();

					FillSubWidgets(x, y, w, h, m_space);

					break;
				}

				case WidgetRefresh: {
					Refresh();
					break;
				}

				default:
					break;
			}

		}
	}

	ResponseType ToolBar::Draw (const RedrawEvent& event)
	{
		glm::vec3 pos((float) position().x(), (float) position().y(),
						(float) z());
		glm::mat4 mvp = glm::translate(event.projection_matrix() * event.view_matrix(), pos);

		RefPtr<GLSLProgram> program = ShaderManager::instance->default_triangle_program();

		glBindVertexArray(m_vao);

		program->Use();

		glm::vec4 color;
		color.r = Theme::instance->regular().inner.r() / 255.f;
		color.g = Theme::instance->regular().inner.g() / 255.f;
		color.b = Theme::instance->regular().inner.b() / 255.f;
		color.a = Theme::instance->regular().inner.a() / 255.f;

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
		program->SetUniform1i("AA", 0);
		program->SetVertexAttrib4fv("Color", glm::value_ptr(color));
		program->SetUniform1i("Gamma", 0);

		glEnableVertexAttribArray(0);

		m_inner->Bind();

		glVertexAttribPointer(0, // attribute
							  2,			// number of elements per vertex, here (x,y)
							  GL_FLOAT,			 // the type of each element
							  GL_FALSE,			 // take our values as-is
							  0,				 // no extra data between each position
							  0					 // offset of first element
							  );

		glDrawArrays(GL_TRIANGLE_FAN, 0,
							m_inner->GetBufferSize()
											/ (2 * sizeof(GLfloat)));

		m_inner->Reset();

		program->SetUniform1i("AA", 1);
		program->SetUniform1i("Gamma", 0);
		program->SetVertexAttrib4f("Color", 0.f, 0.f, 0.f, 1.f);

		glDisableVertexAttribArray(0);

		program->Reset();

		glBindVertexArray(0);
		return Accept;
	}

	ResponseType ToolBar::CursorEnterEvent (bool entered)
	{
		return Accept;
	}

	ResponseType ToolBar::KeyPressEvent (const KeyEvent& event)
	{
		return Accept;
	}

	ResponseType ToolBar::ContextMenuPressEvent (
					const ContextMenuEvent& event)
	{
		return Accept;
	}

	ResponseType ToolBar::ContextMenuReleaseEvent (
					const ContextMenuEvent& event)
	{
		return Accept;
	}

	ResponseType ToolBar::MousePressEvent (const MouseEvent& event)
	{
		if(event.button() == MouseButtonMiddle) {
			if(sub_widget_size()) {
				m_move_status = true;
				m_start_x = event.position().x();
				m_last_x = sub_widgets()->front()->position().x();
			}
		}

		return Accept;
	}

	ResponseType ToolBar::MouseReleaseEvent (const MouseEvent& event)
	{
		if(m_move_status) {
			m_move_status = false;
			Refresh();
		}
		return Accept;
	}

	ResponseType ToolBar::MouseMoveEvent (const MouseEvent& event)
	{
		if(m_move_status && sub_widget_size()) {
			int xmin = sub_widgets()->front()->position().x();
			int direction = event.position().x() - m_start_x;
			int offset = m_last_x + event.position().x() - m_start_x;
			int width = size().width() - margin().left() - margin().right();
			int left = position().x() + margin().left();
			int right = position().x() + size().width() - margin().right();

			int xmax = xmin - m_space;
			for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
			{
				xmax += (*it)->size().width() + m_space;
			}

			DBG_PRINT_MSG("direction: %d, xmin: %d, xmax: %d, offset: %d, left: %d, right: %d", direction, xmin, xmax, offset, left, right);

			if((xmax - xmin) > width) {

				if(direction < 0) {	// left

					if(xmax > right) {
						int x = m_last_x;
						for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
						{
							SetSubWidgetPosition(*it, x + event.position().x() - m_start_x, (*it)->position().y());
							x += (*it)->size().width() + m_space;
						}
					}

				} else if (direction > 0) {

					if(xmin < left) {
						int x = m_last_x;
						for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
						{
							SetSubWidgetPosition(*it, x + event.position().x() - m_start_x, (*it)->position().y());
							x += (*it)->size().width() + m_space;
						}
					}

				}

			} else if((xmax - xmin) < width) {

				if(direction < 0) { // left
					if(xmin > left) {
						int x = m_last_x;
						for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
						{
							SetSubWidgetPosition(*it, x + event.position().x() - m_start_x, (*it)->position().y());
							x += (*it)->size().width() + m_space;
						}
					}
				} else if (direction > 0) {	// right
					if(xmax < right) {
						int x = m_last_x;
						for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
						{
							SetSubWidgetPosition(*it, x + event.position().x() - m_start_x, (*it)->position().y());
							x += (*it)->size().width() + m_space;
						}
					}
				}
			}

			Refresh();
		}
		return Accept;
	}

	void ToolBar::InitializeToolBar ()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		std::vector<GLfloat> vertices(12);

		GenerateFlatRectVertices(size(), 0.f, &vertices);

		m_inner.reset(new GLArrayBuffer);
		m_inner->Generate();

		m_inner->Bind();
		m_inner->SetData(sizeof(GLfloat) * vertices.size(), &vertices[0]);
		m_inner->Reset();

		glBindVertexArray(0);
	}
	
	void ToolBar::RealignSubWidgets (const Size& size, const Margin& margin,
					int space)
	{
		int x = position().x();
		int y = position().y();
		int h = size.height() - margin.top() - margin.bottom();

		if(h < 0) {
			DBG_PRINT_MSG("Error: the geometry for sub widget is not valid, height: %d", h);
		}

		x += margin.left();
		y += margin.bottom();
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			SetSubWidgetPosition(*it, x, y);
			ResizeSubWidget(*it, (*it)->size().width(), h);
			x += (*it)->size().width() + space;
		}
	}
	
	void ToolBar::FillSubWidgets (const Point& out_pos, const Size& out_size,
					const Margin& margin, int space)
	{
		int x = out_pos.x() + margin.left();
		int y = out_pos.y() + margin.bottom();
		unsigned int width = out_size.width() - margin.left() - margin.right();
		unsigned int height = out_size.height() - margin.top() - margin.bottom();

		FillSubWidgets (x, y, width, height, space);
	}
	
	void ToolBar::FillSubWidgets (int x, int y, unsigned int width,
					unsigned int height, int space)
	{
		DBG_PRINT_MSG("widget position: %d %d %u %u", x, y, width, height);

		AbstractWidget* widget = 0;
		for(WidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			widget = *it;
			SetSubWidgetPosition(widget, x, y);

			if(widget->expand_y()) {
				ResizeSubWidget(widget, widget->size().width(), height);
			} else {

				if(widget->size().height() > height) {
					ResizeSubWidget(widget, widget->size().width(), height);
				} else {
					SetSubWidgetPosition(widget, x,
									y + (height - widget->size().height()) / 2);
				}

			}

			x += widget->size().width() + space;
		}
	}

	int ToolBar::GetLastPosition () const
	{
		int x = position().x() + margin().left();

		if(sub_widget_size()) {
			x = sub_widgets()->back()->position().x();
			x += sub_widgets()->back()->size().width() + m_space;
		}

		return x;
	}

}
