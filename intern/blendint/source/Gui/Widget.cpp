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

#include <BlendInt/Gui/Widget.hpp>
#include <BlendInt/Gui/AbstractWindow.hpp>

namespace BlendInt {

	Widget::Widget()
	: AbstractWidget(),
	  widget_flag_(0),
	  round_radius_(5.f)
	{
	}

	Widget::Widget(int width, int height)
	: AbstractWidget(width, height),
	  widget_flag_(0),
	  round_radius_(5.f)
	{
	}

	Widget::~Widget()
	{
	}

	void Widget::SetRoundRadius(float radius)
	{
		if(round_radius_ == radius) return;

		PerformRoundRadiusUpdate(radius);
	}

	void Widget::SetRoundType(int type)
	{
		if((widget_flag_ & 0x0F) == (type & 0x0F)) return;

		PerformRoundTypeUpdate(type & 0x0F);
	}

	void Widget::SetEmboss(bool emboss)
	{
		if(this->emboss() == emboss) return;

		PerformEmbossUpdate(emboss);
	}

	void Widget::PerformRoundTypeUpdate(int round)
	{
		set_round_type(round);
	}

	void Widget::PerformRoundRadiusUpdate(float radius)
	{
		round_radius_ = radius;
	}

	void Widget::PerformEmbossUpdate(bool emboss)
	{
		set_emboss(emboss);
	}

	void Widget::GenerateRoundedVertices(std::vector<GLfloat>* inner,
			std::vector<GLfloat>* outer)
	{
		GenerateVertices(size(),
				default_border_width() * AbstractWindow::theme->pixel(),
				round_type(),
				round_radius_,
				inner,
				outer);
	}

	void Widget::GenerateRoundedVertices(Orientation shadedir, short shadetop,
			short shadedown, std::vector<GLfloat>* inner,
			std::vector<GLfloat>* outer)
	{
		GenerateVertices(size(),
				default_border_width() * AbstractWindow::theme->pixel(),
				round_type(),
				round_radius_,
				shadedir,
				shadetop,
				shadedown,
				inner,
				outer);
	}

	ResponseType Widget::Draw(const AbstractWindow* context)
	{
		return subs_count() ? Ignore : Finish;
	}

}

