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

#include <GL/glew.h>

#include <BlendInt/Slider.hpp>

#include <BlendInt/Interface.hpp>
#include <BlendInt/Theme.hpp>

#include <iostream>

namespace BlendInt {

	SlideButton::SlideButton()
	:  Button()
	{
		set_padding (0, 0, 0, 0);
		set_emboss(false);
	}

	SlideButton::SlideButton(AbstractForm* parent)
	:  Button(parent)
	{
		set_padding (0, 0, 0, 0);
		set_emboss(false);
	}

	SlideButton::~SlideButton()
	{

	}

	void SlideButton::update (int property_type)
	{
		switch(property_type)
		{
			case FormPropertySize:
				update_shape(&m_size);
				break;
			case FormPropertyRoundCorner: {
				update_shape(&m_size);
				break;
			}
			case WidgetPropertyPadding: {
				// do not allow change padding
				m_padding.set_value(0, 0, 0, 0);
				break;
			}
			default:
				break;
		}
	}

	void SlideButton::render ()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslatef(position().x(),
					 position().y(),
					 z());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if(m_status_down) {
			m_buffer.set_index(0);
		} else {
			m_buffer.set_index(2);
		}

		m_buffer.bind();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(2, GL_FLOAT, sizeof(GLfloat) * 6, BUFFER_OFFSET(0));
		glColorPointer(4, GL_FLOAT, sizeof(GLfloat) * 6, BUFFER_OFFSET(2 * sizeof(GLfloat)));

		glDrawArrays(GL_POLYGON, 0, m_buffer.vertices());

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		m_buffer.unbind();

		// draw outline
		m_buffer.set_index(1);
		unsigned char tcol[4] = { themes()->scroll.outline.r(),
		        themes()->scroll.outline.g(),
		        themes()->scroll.outline.b(),
		        themes()->scroll.outline.a()};

		tcol[3] = tcol[3] / WIDGET_AA_JITTER;

		m_buffer.bind();

		/* outline */
		glEnableClientState(GL_VERTEX_ARRAY);
		glColor4ubv(tcol);
		for (int j = 0; j < WIDGET_AA_JITTER; j++) {
			glTranslatef(jit[j][0], jit[j][1], 0.0f);
			glVertexPointer(2, GL_FLOAT, 0, 0);
			glDrawArrays(GL_QUAD_STRIP, 0, m_buffer.vertices());
			glTranslatef(-jit[j][0], -jit[j][1], 0.0f);
		}
		glDisableClientState(GL_VERTEX_ARRAY);

		m_buffer.unbind();

		glDisable(GL_BLEND);

		glPopMatrix();
	}

	void SlideButton::move_mouse(MouseEvent* event)
	{
		// if no parent slider, don't react to mouse move
		if(m_parent.type != ParentForm) return;

		AbstractSlider* parent = dynamic_cast<AbstractSlider*>(m_parent.object.form);
		if(!parent) return;

		if(m_status_down) {
			m_status_hover = false;

			if(parent->orientation()) {	// Vertical

				position_ref().set_y(m_position_origin.y() + event->position().y() - m_move_start.y());
				if(position().y() < (parent->position().y() + parent->padding().bottom())) {
					position_ref().set_y(parent->position().y() + parent->padding().bottom());
				}
				if(position().y() > (int)(parent->position().y() + parent->size().height() - parent->padding().top() - m_size.height())) {
					position_ref().set_y(parent->position().y() + parent->size().height() - parent->padding().top() - m_size.height());
				}

			} else {

				position_ref().set_x(m_position_origin.x() + event->position().x() - m_move_start.x());
				if(position().x() < (parent->position().x() + parent->padding().left())) {
					position_ref().set_x(parent->position().x() + parent->padding().left());
				}
				if(position().x() >
						(int)(parent->position().x() + parent->size().width() - parent->padding().right() - m_size.width())) {
					position_ref().set_x(parent->position().x() + parent->size().width() - parent->padding().right() - m_size.width());
				}

			}
			event->accept(this);
			return;

		} else {

			if(contain(event->position())) {
				if (m_status_down) {
					m_status_hover = false;
				} else {
					m_status_hover = true;
				}
				event->accept(this);
			} else {
				m_status_hover = false;
			}
		}
	}

	void SlideButton::press_mouse(MouseEvent* event)
	{
		if(contain(event->position())) {
			if (event->button() == MouseButtonLeft) {
				m_status_down = true;
				m_move_start.set_x(event->position().x());
				m_move_start.set_y(event->position().y());
				m_position_origin = position();
				event->accept(this);
			}
		}
	}

	void SlideButton::release_mouse(MouseEvent* event)
	{
		if (event->button() == MouseButtonLeft) {

		}
		m_status_down = false;

	}

	void SlideButton::update_shape(const Size* size)
	{
		float outer_v[WIDGET_SIZE_MAX][2];	// vertices for drawing outline
		float inner_v[WIDGET_SIZE_MAX][6];	// vertices for drawing inner

		if(m_buffer.size() != 3)
			m_buffer.generate(3);

		VerticesSum vert_sum;

		Orientation shadedir;

		if(m_parent.type != ParentForm) {
			shadedir = size->width() < size->height() ? Horizontal : Vertical;
		} else {
			AbstractSlider* parent = dynamic_cast<AbstractSlider*>(m_parent.object.form);
			if(parent) {
				shadedir = parent->orientation() ? Horizontal : Vertical;
			}
		}
		Color color = themes()->scroll.item;

		if(shadedir)
			m_round_radius = 0.5f * size->height();
		else
			m_round_radius = 0.5f * size->width();

		short shadetop = themes()->scroll.shadetop;
		short shadedown = themes()->scroll.shadedown;

		if (shadetop > shadedown)
				shadetop += 20;   /* XXX violates themes... */
		else shadedown += 20;

		if(shadedir) {
			vert_sum = generate_vertices(size,
					color,
					shadetop,
					shadedown,
					shadedir,
					inner_v, outer_v);
		} else {	// swap shadetop and shadedown
			vert_sum = generate_vertices(size,
					color,
					shadedown,
					shadetop,
					shadedir,
					inner_v, outer_v);
		}

		m_buffer.set_index(0);
		m_buffer.set_property(vert_sum.total, sizeof(inner_v[0]), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		m_buffer.bind();
		m_buffer.upload(inner_v);
		m_buffer.unbind();

		float quad_strip[WIDGET_SIZE_MAX * 2 + 2][2]; /* + 2 because the last pair is wrapped */

		verts_to_quad_strip (inner_v, outer_v, vert_sum.total, quad_strip);

		m_buffer.set_index(1);
		m_buffer.set_property(vert_sum.total * 2 + 2, sizeof(quad_strip[0]), GL_ARRAY_BUFFER, GL_STATIC_DRAW);

		m_buffer.bind();
		m_buffer.upload(quad_strip);
		m_buffer.unbind();

		color.highlight(color, 5);

		if(shadedir) {
			vert_sum = generate_vertices(size,
					color,
					shadetop,
					shadedown,
					shadedir,
					inner_v, outer_v);
		} else {	// swap shadetop and shadedown
			vert_sum = generate_vertices(size,
					color,
					shadedown,
					shadetop,
					shadedir,
					inner_v, outer_v);
		}

		m_buffer.set_index(2);
		m_buffer.set_property(vert_sum.total, sizeof(inner_v[0]), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		m_buffer.bind();
		m_buffer.upload(inner_v);
		m_buffer.unbind();

	}

	// -------------------- Slider ---------------------------

	Slider::Slider(Orientation orientation)
	: AbstractSlider(orientation), m_slide_button(0)
	{
		m_slide_button = new SlideButton(this);

		// set default size
		if (orientation) {
			resize (25, 200);
			set_expand_y(true);
		} else {
			resize (200, 25);
			set_expand_x(true);
		}

		size_t button_size = std::min (size().width() - padding().left() - padding().right(),
				size().height() - padding().top() - padding().bottom());

		m_slide_button->resize(button_size, button_size);
		//m_slide_button->set_position (position().x() + padding().left(), position().y() + padding().bottom());
		update(SliderPropertyValue);
	}

	Slider::Slider(Orientation orientation, AbstractForm* parent)
	: AbstractSlider(orientation, parent), m_slide_button(0)
	{
		m_slide_button = new SlideButton(this);

		// set default size
		if (orientation) {
			resize (25, 200);
			set_expand_y(true);
		} else {
			resize (200, 25);
			set_expand_x(true);
		}

		size_t button_size = std::min (size().width() - padding().left() - padding().right(),
				size().height() - padding().top() - padding().bottom());

		m_slide_button->resize(button_size, button_size);

		//m_slide_button->set_position (position().x() + padding().left(), position().y() + padding().bottom());
		update(SliderPropertyValue);
	}

	Slider::~Slider()
	{

	}

	void Slider::set_control_size (size_t size)
	{
		if(orientation()) {	// Vertical
			m_slide_button->resize(m_slide_button->size().width(), size);
		} else {
			m_slide_button->resize(size, m_slide_button->size().height());
		}
	}

	void Slider::set_control_widget(Button* widget)
	{
		if(widget) {
			if(m_slide_button)
				delete m_slide_button;

			bind(widget);
			m_slide_button = widget;
		}
	}

	void Slider::update (int property_type)
	{
		switch (property_type) {
			case FormPropertyPosition: {
				m_slide_button->set_position (position().x() + padding().left(), position().y() + padding().bottom());
				return;
			}

			case FormPropertySize: {

				size_t button_size = std::min (size().width() - padding().left() - padding().right(),
						size().height() - padding().top() - padding().bottom());

				m_slide_button->resize(button_size, button_size);

				if(orientation()) {
					m_slide_button->set_position (m_slide_button->position().x(),
							position().y() + m_padding.bottom() + value() * get_space() / (float)(maximum() - minimum()));
				} else {
					m_slide_button->set_position (position().x() + m_padding.left() + value() * get_space() / (float)(maximum() - minimum()),
							m_slide_button->position().y());
				}

				return;
			}

			case SliderPropertyValue: {

				if(orientation()) {
					m_slide_button->set_position (m_slide_button->position().x(),
							position().y() + m_padding.bottom() + value() * get_space() / (float)(maximum() - minimum()));
				} else {
					m_slide_button->set_position (position().x() + m_padding.left() + value() * get_space() / (float)(maximum() - minimum()),
							m_slide_button->position().y());
				}

				return;
			}

			default:
				break;
		}
	}

	void Slider::render ()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslatef(position().x(),
					 position().y(),
					 z());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();

		glTranslatef(m_padding.left(),
					 m_padding.bottom(), 0);

		glColor4ub(themes()->scroll.outline.r(),
				themes()->scroll.outline.g(),
				themes()->scroll.outline.b(),
				themes()->scroll.outline.a());

		int space = 0;

		if(orientation()) {
			space = m_size.height() - m_padding.top() - m_padding.bottom();
			glTranslatef(m_slide_button->size().width() / 2.0, 0, 0);
			glBegin(GL_LINES);
				glVertex2i(0, 0);
				glVertex2i(0, space);
			glEnd();
		} else {
			space = m_size.width() - m_padding.left() - m_padding.right();
			glTranslatef(0, m_slide_button->size().height() / 2.0 - 0.5, 0);
			glBegin(GL_LINES);
				glVertex2i(0, 0);
				glVertex2i(space, 0);
			glEnd();
		}

		glPopMatrix();

#ifdef DEBUG
		glLineWidth(1);
		glEnable(GL_LINE_STIPPLE);

		glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
		glLineStipple(1, 0xAAAA);
		glBegin(GL_LINE_LOOP);
			glVertex2i(0, 0);
			glVertex2i(m_size.width(), 0);
			glVertex2i(m_size.width(), m_size.height());
			glVertex2i(0, m_size.height());
		glEnd();

		glDisable(GL_LINE_STIPPLE);
#endif

		glDisable(GL_BLEND);

		glPopMatrix();

		Interface::instance()->dispatch_render_event(m_slide_button);
	}

	void Slider::move_mouse (MouseEvent* event)
	{
		if(m_slide_button->down()) {
			Interface::instance()->dispatch_mouse_move_event(m_slide_button, event);

			int value = 0;

			if (orientation()) {
				int ymin = position().y() + m_padding.bottom() + m_slide_button->size().height() / 2;
				int ymax = position().y() + m_size.height() - m_padding.top() - m_slide_button->size().height() / 2;
				if(event->position().y() < ymin ||	event->position().y() > ymax)
					return;	// if the mouse move too far, don't count the value repeatedly

				value = (m_slide_button->position().y() - position().y()
				        - m_padding.bottom()) / (float) get_space()
				        * (maximum() - minimum());

			} else {
				int xmin = position().x() + m_padding.left() + m_slide_button->size().width() / 2;
				int xmax = position().x() + m_size.width() - m_padding.right() - m_slide_button->size().width() / 2;
				if(event->position().x() < xmin ||	event->position().x() > xmax)
					return;	// if the mouse move too far, don't count the value repeatedly

				value = (m_slide_button->position().x() - position().x()
				        - m_padding.left()) / (float) get_space()
				        * (maximum() - minimum());
			}

			set_value(value);
			m_slider_moved.fire(value);

			return;
		}

		if(contain(event->position())) {
			Interface::instance()->dispatch_mouse_move_event(m_slide_button, event);
		}
	}

	void Slider::press_mouse (MouseEvent* event)
	{
		if(m_slide_button->down()) {
			interface()->dispatch_mouse_press_event(m_slide_button, event);
			return;
		}

		if(contain_no_padding(event->position())) {
			interface()->dispatch_mouse_press_event(m_slide_button, event);
			if(event->accepted()) return;

			// Move to where mouse click
//			Coord2d inner_pos;
//			inner_pos.set_x(static_cast<double>(event->position().x() - m_pos.x() - m_padding.left() - m_slider_control->size().width() / 2));
//			inner_pos.set_y(static_cast<double>(event->position().y() - m_pos.y() - m_padding.bottom() - m_slider_control->size().height() / 2));
//			int space = get_space();
//			int value;
//
//			if (orientation()) {
//				value = (maximum() - minimum()) * inner_pos.y() / (double) space;
//			} else {
//				value = (maximum() - minimum()) * inner_pos.x() / (double) space;
//			}
//			if(value < minimum()) value = minimum();
//			if(value > maximum()) value = maximum();

//			int space = get_space();

			int val;

			if (orientation()) {
				if(event->position().y() < m_slide_button->position().y())
					val = value() - step();
				else
					val = value() + step();
			} else {
				if(event->position().x() < m_slide_button->position().x())
					val = value() - step();
				else
					val = value() + step();
			}
			if(val < minimum()) val = minimum();
			if(val > maximum()) val = maximum();

			set_value(val);
			m_slider_moved.fire(val);
		}
	}

	void Slider::release_mouse (MouseEvent* event)
	{
		if(m_slide_button->down()) {
			Interface::instance()->dispatch_mouse_release_event(m_slide_button, event);
			return;
		}
		if(contain(event->position())) {
			if (event->button() == MouseButtonLeft) {

			}
			Interface::instance()->dispatch_mouse_release_event(m_slide_button, event);
		}
	}

	int Slider::get_space ()
	{
		int space = 0;
		if(orientation())	// Vertical
			space = m_size.height() - m_padding.top() - m_padding.bottom() - m_slide_button->size().height();
		else
			space = m_size.width() - m_padding.left() - m_padding.right() - m_slide_button->size().width();

		return space;
	}

}
