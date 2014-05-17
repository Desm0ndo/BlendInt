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
#include <glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <BlendInt/Gui/Expander.hpp>

#include <BlendInt/Gui/ToggleButton.hpp>
#include <BlendInt/Gui/Frame.hpp>

#include <BlendInt/Service/Theme.hpp>
#include <BlendInt/Service/ShaderManager.hpp>

namespace BlendInt {

	ExpandButton::ExpandButton()
	: AbstractRoundButton(), m_vao(0)
	{
		InitializeExpandButton();
	}

	ExpandButton::ExpandButton (const String& text)
	: AbstractRoundButton(), m_vao(0)
	{
		InitializeExpandButton(text);
	}

	ExpandButton::~ExpandButton ()
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	void ExpandButton::Update (const UpdateRequest& request)
	{
		if(request.source() == Predefined) {

			switch (request.type()) {

				case FormSize: {
					const Size* size_p = static_cast<const Size*>(request.data());
					UpdateTextPosition(*size_p, round_type(), radius(), text(), font());
					glBindVertexArray(m_vao);
					GenerateFormBuffer(
									*size_p,
									round_type(),
									radius(),
									m_inner_buffer.get(),
									m_outer_buffer.get(),
									0);
					glBindVertexArray(0);
					Refresh();
					break;
				}

				case FormRoundType: {
					const int* type_p = static_cast<const int*>(request.data());
					UpdateTextPosition(size(), *type_p, radius(), text(), font());
					glBindVertexArray(m_vao);
					GenerateFormBuffer(
									size(),
									*type_p,
									radius(),
									m_inner_buffer.get(),
									m_outer_buffer.get(),
									0);
					glBindVertexArray(0);
					Refresh();
					break;
				}

				case FormRoundRadius: {
					const float* radius_p = static_cast<const float*>(request.data());
					UpdateTextPosition(size(), round_type(), *radius_p, text(), font());
					glBindVertexArray(m_vao);
					GenerateFormBuffer(
									size(),
									round_type(),
									*radius_p,
									m_inner_buffer.get(),
									m_outer_buffer.get(),
									0);
					glBindVertexArray(0);
					Refresh();
					break;
				}

				default:
					break;
			}

		}
	}

	ResponseType ExpandButton::Draw (const RedrawEvent& event)
	{
		glBindVertexArray(m_vao);

		RefPtr<GLSLProgram> program = ShaderManager::instance->default_triangle_program();
		program->Use();

		glm::vec3 pos((float)position().x(), (float)position().y(), (float)z());
		glm::mat4 mvp = glm::translate(event.projection_matrix() * event.view_matrix(), pos);

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));

		Theme* tm = Theme::instance;

		glm::vec4 color;

		if (hover()) {
			if(checked()) {
				color.r = tm->regular().inner_sel.highlight_red() / 255.f;
				color.g = tm->regular().inner_sel.highlight_green() / 255.f;
				color.b = tm->regular().inner_sel.highlight_blue() / 255.f;
				color.a = tm->regular().inner_sel.a() / 255.f;
			} else {
				color.r = tm->regular().inner.highlight_red() / 255.f;
				color.g = tm->regular().inner.highlight_green() / 255.f;
				color.b = tm->regular().inner.highlight_blue() / 255.f;
				color.a = tm->regular().inner.a() / 255.f;
			}
		} else {
			if (checked()) {
				color.r = tm->regular().inner_sel.r() / 255.f;
				color.g = tm->regular().inner_sel.g() / 255.f;
				color.b = tm->regular().inner_sel.b() / 255.f;
				color.a = tm->regular().inner_sel.a() / 255.f;
			} else {
				color.r = tm->regular().inner.r() / 255.f;
				color.g = tm->regular().inner.g() / 255.f;
				color.b = tm->regular().inner.b() / 255.f;
				color.a = tm->regular().inner.a() / 255.f;
			}
		}

		program->SetVertexAttrib4fv("Color", glm::value_ptr(color));
		program->SetUniform1i("AA", 0);

		glEnableVertexAttribArray(0);

		DrawTriangleFan(0, m_inner_buffer.get());

		color.r = tm->regular().outline.r() / 255.f;
		color.g = tm->regular().outline.g() / 255.f;
		color.b = tm->regular().outline.b() / 255.f;
		color.a = tm->regular().outline.a() / 255.f;

		program->SetVertexAttrib4fv("Color", glm::value_ptr(color));
		program->SetUniform1i("AA", 1);

		DrawTriangleStrip(0, m_outer_buffer.get());

		glDisableVertexAttribArray(0);

		program->Reset();

		glBindVertexArray(0);

		if(text().size()) {
			font().Print(mvp, text(), text_length(), 0);
		}

		return Accept;
	}

	void ExpandButton::InitializeExpandButton ()
	{
		set_round_type(RoundAll);
		set_expand_x(true);
		set_checkable(true);

		unsigned int h = font().GetHeight();

		set_size(h + radius() * 2 + DefaultButtonPadding().left() + DefaultButtonPadding().right(),
						h + DefaultButtonPadding().top() + DefaultButtonPadding().bottom());

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		m_inner_buffer.reset(new GLArrayBuffer);
		m_outer_buffer.reset(new GLArrayBuffer);
		GenerateFormBuffer(
						size(),
						round_type(),
						radius(),
						m_inner_buffer.get(),
						m_outer_buffer.get(),
						0);
		glBindVertexArray(0);
	}

	void ExpandButton::InitializeExpandButton (const String& text)
	{
		set_round_type(RoundAll);
		set_expand_x(true);
		set_checkable(true);
		set_text(text);

		unsigned int h = font().GetHeight();

		if(text.empty()) {
			set_size(h + radius() * 2 + DefaultButtonPadding().left() + DefaultButtonPadding().right(),
							h + DefaultButtonPadding().top() + DefaultButtonPadding().bottom());
		} else {
			set_text_length(text.length());
			Rect text_outline = font().GetTextOutline(text);

			unsigned int width = text_outline.width() + radius() * 2 + DefaultButtonPadding().left() + DefaultButtonPadding().right();
			unsigned int height = h + DefaultButtonPadding().top() + DefaultButtonPadding().bottom();

			set_size(width, height);

			set_pen((width - text_outline.width()) / 2,
							(height - font().GetHeight()) / 2
											+ std::abs(font().GetDescender()));
		}

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		m_inner_buffer.reset(new GLArrayBuffer);
		m_outer_buffer.reset(new GLArrayBuffer);

		GenerateFormBuffer(
						size(),
						round_type(),
						radius(),
						m_inner_buffer.get(),
						m_outer_buffer.get(),
						0);

		glBindVertexArray(0);
	}

	// ----------------------

	Expander::Expander ()
	: m_title_button(0), m_frame(0), m_space (4), m_frame_height(0)
	{
		m_title_button = Manage(new ExpandButton);
		m_frame = Manage(new Frame);
		m_frame->SetExpand(true);

		PushBackSubWidget(m_title_button);
		PushBackSubWidget(m_frame);

		int x = position().x();
		int y = position().y();
		unsigned int width = 0;
		unsigned int height = 0;

		Size sub_prefer = m_title_button->GetPreferredSize();
		width = std::max(width, sub_prefer.width());
		height += sub_prefer.height();

		ResizeSubWidget(m_title_button, width, sub_prefer.height());
		y -= sub_prefer.height();
		SetSubWidgetPosition(m_title_button, x, y);

		height += m_space;
		y -= m_space;

		sub_prefer = m_frame->GetPreferredSize();
		width = std::max(width, sub_prefer.width());
		height += sub_prefer.height();

		ResizeSubWidget(m_title_button, width, sub_prefer.height());
		y -= sub_prefer.height();
		SetSubWidgetPosition(m_title_button, x, y);

		width = width + margin().left() + margin().right();
		height = height + margin().top() + margin().bottom();

		set_size(width, height);

		m_frame_height = m_frame->size().height();

		events()->connect(m_title_button->toggled(), this, &Expander::OnToggled);
	}

	Expander::Expander (const String& title)
	: m_title_button(0), m_frame(0), m_space (4), m_frame_height(0)
	{
		m_title_button = Manage(new ExpandButton(title));
		m_frame = Manage(new Frame);
		m_frame->SetExpand(true);

		PushBackSubWidget(m_title_button);
		PushBackSubWidget(m_frame);

		int x = position().x();
		int y = position().y();
		unsigned int width = 0;
		unsigned int height = 0;

		Size sub_prefer = m_title_button->GetPreferredSize();
		width = std::max(width, sub_prefer.width());
		height += sub_prefer.height();

		ResizeSubWidget(m_title_button, width, sub_prefer.height());
		y -= sub_prefer.height();
		SetSubWidgetPosition(m_title_button, x, y);

		height += m_space;
		y -= m_space;

		sub_prefer = m_frame->GetPreferredSize();
		width = std::max(width, sub_prefer.width());
		height += sub_prefer.height();

		ResizeSubWidget(m_title_button, width, sub_prefer.height());
		y -= sub_prefer.height();
		SetSubWidgetPosition(m_title_button, x, y);

		width = width + margin().left() + margin().right();
		height = height + margin().top() + margin().bottom();

		set_size(width, height);

		m_frame_height = m_frame->size().height();

		events()->connect(m_title_button->toggled(), this, &Expander::OnToggled);
	}

	Expander::~Expander ()
	{
	}

	bool Expander::Setup (AbstractWidget* widget)
	{
		bool ret = false;

		if(m_frame->Setup(widget)) {
			ret = true;
		}

		return ret;
	}

	Size Expander::GetPreferredSize() const
	{
		Size prefer;

		Size sub_prefer = m_title_button->GetPreferredSize();
		prefer.set_width(std::max(prefer.width(), sub_prefer.width()));
		prefer.add_height(sub_prefer.height());

		prefer.add_height(m_space);

		sub_prefer = m_frame->GetPreferredSize();
		prefer.set_width(std::max(prefer.width(), sub_prefer.width()));
		prefer.add_height(sub_prefer.height());

		prefer.add_width(margin().left() + margin().right());
		prefer.add_height(margin().top() + margin().bottom());

		return prefer;
	}

	bool Expander::UpdateTest (const UpdateRequest& request)
	{
		if(request.source() == Predefined) {

			switch (request.type()) {

				case SubWidgetSize:
					return false;	// DO not allow sub widget geometry reset outside

				case SubWidgetPosition:
					return false;

				default:
					return AbstractDequeContainer::UpdateTest(request);

			}

		} else {
			return false;
		}
	}

	void Expander::Update (const UpdateRequest& request)
	{
		if(request.source() == Predefined) {
			switch (request.type()) {

				case FormSize: {
					if (sub_widget_size()) {
						const Size* size_p = static_cast<const Size*>(request.data());
						FillWithPreferredHeight(position(), *size_p, margin(), m_space);
					}
					break;
				}

				case FormPosition: {
					if (sub_widget_size()) {
						const Point* pos_p = static_cast<const Point*>(request.data());

						int x = pos_p->x() - position().x();
						int y = pos_p->y() - position().y();

						MoveSubWidgets(x, y);
					}
					break;
				}

				case ContainerMargin: {

					const Margin* margin_p = static_cast<const Margin*>(request.data());
					set_margin(*margin_p);
					FillWithPreferredHeight(position(), size(), *margin_p, m_space);

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

	ResponseType Expander::Draw (const RedrawEvent& event)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		RefPtr<GLSLProgram> program = ShaderManager::instance->default_triangle_program();
		program->Use();

		glm::vec3 pos((float)position().x(), (float)position().y(), (float)z());
		glm::mat4 mvp = glm::translate(event.projection_matrix() * event.view_matrix(), pos);

		program->SetUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));

		std::vector<GLfloat> vertices(12);

		GenerateFlatRectVertices(size(), 0.f, &vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		program->SetVertexAttrib4f("Color", 0.85f, 0.85f, 0.85f, 0.75f);
		program->SetUniform1i("AA", 0);

		glEnableVertexAttribArray(0);	// 0 is the locaiton in shader

		glVertexAttribPointer(
						0, // attribute
						2,		// number of elements per vertex, here (x,y)
						GL_FLOAT,	// the type of each element
						GL_FALSE,	// take our values as-is
						0,		// no extra data between each position
						BUFFER_OFFSET(0)	// the first element
		);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

		glDisableVertexAttribArray(0);

		program->Reset();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);

		return IgnoreAndContinue;
	}

	ResponseType Expander::CursorEnterEvent (bool entered)
	{
		return Ignore;
	}

	ResponseType Expander::KeyPressEvent (const KeyEvent& event)
	{
		return Ignore;
	}

	ResponseType Expander::ContextMenuPressEvent (
	        const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType Expander::ContextMenuReleaseEvent (
	        const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType Expander::MousePressEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType Expander::MouseReleaseEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType Expander::MouseMoveEvent (const MouseEvent& event)
	{
		return Ignore;
	}
	
	void Expander::FillWithPreferredHeight (const Point& out_pos,
					const Size& out_size, const Margin& margin, int space)
	{
		int x = out_pos.x() + margin.left();
		int y = out_pos.y() + margin.bottom();
		int width = out_size.width() - margin.left() - margin.right();
		int height = out_size.height() - margin.top() - margin.bottom();

		if(width >= 0 && height >= space)
			FillWithPreferredHeight(x, y, width, height, space);
	}
	
	void Expander::FillWithPreferredHeight (int x, int y, unsigned int width,
					unsigned int height, int space)
	{
		y = y + height;

		Size prefer;

		unsigned int button_preferred_height = 0;
		unsigned int sum = 0;

		prefer = m_title_button->GetPreferredSize();
		button_preferred_height = prefer.height();
		sum += prefer.height();

		if (m_frame->visiable()) {
			prefer = m_frame->GetPreferredSize();
			sum += prefer.height();

			if (button_preferred_height < (height - space)) {
				y = y - button_preferred_height;
				ResizeSubWidget(m_title_button, width,
								button_preferred_height);
				SetSubWidgetPosition(m_title_button, x, y);

				y -= space;

				ResizeSubWidget(m_frame, width,
								height - button_preferred_height - space);
				y = y - (height - button_preferred_height - space);
				SetSubWidgetPosition(m_frame, x, y);
			} else {
				y = y - (height - space);
				ResizeSubWidget(m_title_button, width, height - space);
				SetSubWidgetPosition(m_title_button, x, y);

				y -= space;
				ResizeSubWidget(m_frame, width, 0);
				SetSubWidgetPosition(m_frame, x, y);
			}
		} else {
			y = y - height;
			ResizeSubWidget(m_title_button, width, height);
			SetSubWidgetPosition(m_title_button, x, y);
		}
	}
	
	void Expander::SetTitle (const String& text)
	{
		m_title_button->SetText(text);
	}

	const std::wstring* Expander::GetTitle () const
	{
		if(m_title_button) {
			return &m_title_button->text();
		} else {
			return 0;
		}
	}

	void Expander::OnToggled (bool toggle)
	{
		if(toggle) {
			int x = position().x();
			int y = position().y() + size().height();
			m_frame->SetVisible(false);
			m_frame_height = m_frame->size().height();
			Resize(size().width(), m_title_button->size().height() + margin().top() + margin().bottom());
			y = y - size().height();
			SetPosition(x, y);
		} else {
			int x = position().x();
			int y = position().y() + size().height();

			m_frame->SetVisible(true);

			Resize(size().width(),
							m_title_button->size().height() + m_space
											+ m_frame_height + margin().top()
											+ margin().bottom());
			y = y - size().height();
			SetPosition(x, y);
		}

		Refresh();
	}

}
