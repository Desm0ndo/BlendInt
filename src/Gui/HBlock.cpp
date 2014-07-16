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

#include <BlendInt/Gui/HBlock.hpp>

namespace BlendInt {

	HBlock::HBlock ()
	: AbstractDequeContainer()
	{
		set_size(100, 20);
		set_margin(0, 0, 0, 0);
	}
	
	HBlock::~HBlock ()
	{
	}
	
	void HBlock::PushBack (AbstractWidget* widget)
	{
		AbstractWidget* last = 0;
		if(sub_widget_size()) {
			last = sub_widgets()->back();
		}

		if(PushBackSubWidget(widget)) {

			widget->SetEmboss(true);
			FillInHBlock(position(), size(), margin());

			if(last) {
				last->SetRoundCornerType(last->round_type() & ~(RoundTopRight | RoundBottomRight));
				widget->SetRoundCornerType(RoundTopRight | RoundBottomRight);
			} else {
				widget->SetRoundCornerType(RoundAll);
			}

		}
	}
	
	bool HBlock::IsExpandX () const
	{
		bool expand = false;

		for(AbstractWidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if((*it)->IsExpandX()) {
				expand = true;
				break;
			}
		}

		return expand;
	}

	bool HBlock::IsExpandY () const
	{
		bool expand = false;

		for(AbstractWidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			if((*it)->IsExpandY()) {
				expand = true;
				break;
			}
		}

		return expand;
	}
	
	Size HBlock::GetPreferredSize () const
	{
		Size preferred_size;

		if(sub_widget_size() == 0) {

			preferred_size.set_width(100);
			preferred_size.set_height(20);

		} else {

			AbstractWidget* widget = 0;
			Size tmp;
			int max_width = 0;
			int max_height = 0;

			for(AbstractWidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
			{
				widget = *it;

				if(widget->visiable()) {
					tmp = widget->GetPreferredSize();

					max_width = std::max(max_width, tmp.width());
					max_height = std::max(max_height, tmp.height());
				}
			}
			preferred_size.set_width(sub_widget_size() * (max_width - 1));
			preferred_size.set_height(max_height);

			preferred_size.add_width(margin().hsum());
			preferred_size.add_height(margin().vsum());
		}

		return preferred_size;
	}
	
	void HBlock::UpdateContainer (const ContainerUpdateRequest& request)
	{
		if(request.target() == this) {

			switch(request.type()) {

				case ContainerMargin: {

					const Margin* margin_p = static_cast<const Margin*>(request.data());
					FillInHBlock(position(), size(), *margin_p);

					break;
				}

				default: {
					ReportContainerUpdate(request);
					break;
				}
			}
		}
	}
	
	void HBlock::UpdateGeometry (const GeometryUpdateRequest& request)
	{
		if(request.target() == this) {

			switch(request.type()) {

				case WidgetPosition: {
					const Point* pos_p = static_cast<const Point*>(request.data());

					int x = pos_p->x() - position().x();
					int y = pos_p->y() - position().y();

					set_position(*pos_p);

					MoveSubWidgets(x, y);

					break;
				}

				case WidgetSize: {

					const Size* size_p = static_cast<const Size*>(request.data());

					set_size(*size_p);
					FillInHBlock(position(), *size_p, margin());

					break;
				}

				default: {
					break;
				}
			}

		}

		ReportGeometryUpdate(request);
	}

	void HBlock::ProcessSizeUpdate(const SizeUpdateRequest& request)
	{
		if(request.target() == this) {
			set_size(*request.size());
			FillInHBlock(position(), *request.size(), margin());
		}

		ReportSizeUpdate(request);
	}
	
	void HBlock::ProcessPositionUpdate(const PositionUpdateRequest& request)
	{
		if (request.target() == this) {
			int x = request.position()->x() - position().x();
			int y = request.position()->y() - position().y();

			set_position(*request.position());

			MoveSubWidgets(x, y);
		}

		ReportPositionUpdate(request);
	}

	ResponseType HBlock::Draw (const RedrawEvent& event)
	{
		return Ignore;
	}
	
	ResponseType HBlock::CursorEnterEvent (bool entered)
	{
		return Ignore;
	}
	
	ResponseType HBlock::KeyPressEvent (const KeyEvent& event)
	{
		return Ignore;
	}
	
	ResponseType HBlock::ContextMenuPressEvent (const ContextMenuEvent& event)
	{
		return Ignore;
	}
	
	ResponseType HBlock::ContextMenuReleaseEvent (const ContextMenuEvent& event)
	{
		return Ignore;
	}
	
	ResponseType HBlock::MousePressEvent (const MouseEvent& event)
	{
		return Ignore;
	}
	
	ResponseType HBlock::MouseReleaseEvent (const MouseEvent& event)
	{
		return Ignore;
	}
	
	ResponseType HBlock::MouseMoveEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	void HBlock::FillInHBlock (const Point& out_pos, const Size& out_size,
					const Margin& margin)
	{
		int x = out_pos.x() + margin.left();
		int y = out_pos.y() + margin.bottom();
		int w = out_size.width() - margin.hsum();
		int h = out_size.height() - margin.vsum();

		FillInHBlock(x, y, w, h);
	}

	bool HBlock::UpdateGeometryTest (const GeometryUpdateRequest& request)
	{
		if(request.source() == this) {
			return true;
		} else if (request.source() == container()) {
			return true;
		} else {	// called by sub widget
			return false;
		}
	}

	void HBlock::FillInHBlock (int x, int y, int w, int h)
	{
		if(sub_widget_size() == 0) return;
		int average_width = w / sub_widget_size() + 1;

		AbstractWidget* widget = 0;

		for(AbstractWidgetDeque::iterator it = sub_widgets()->begin(); it != sub_widgets()->end(); it++)
		{
			widget = *it;

			ResizeSubWidget(widget, average_width, h);
			SetSubWidgetPosition(widget, x, y);
			x = x + average_width - 1;
		}
	}

}

