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

#include <BlendInt/Gui/VBlock.hpp>

namespace BlendInt {

	VBlock::VBlock ()
	: AbstractContainer()
	{
		set_size(80, 60);
		set_margin(0, 0, 0, 0);
	}

	VBlock::~VBlock ()
	{
	}

	void VBlock::PushBack (AbstractWidget* widget)
	{
		AbstractWidget* orig_last = last();

		if(PushBackSubWidget(widget)) {

			widget->SetEmboss(true);
			FillInVBlock(position(), size(), margin());

			if(orig_last) {
				orig_last->SetEmboss(false);
				orig_last->SetRoundCornerType(orig_last->round_type() & ~(RoundBottomLeft | RoundBottomRight));
				widget->SetRoundCornerType(RoundBottomLeft | RoundBottomRight);
			} else {
				widget->SetRoundCornerType(RoundAll);
			}

		}
	}

	bool VBlock::IsExpandX() const
	{
		bool expand = false;

		for(AbstractWidget* p = first(); p; p = p->next())
		{
			if(p->IsExpandX()) {
				expand = true;
				break;
			}
		}

		return expand;
	}

	bool VBlock::IsExpandY () const
	{
		bool expand = false;

		for(AbstractWidget* p = first(); p; p = p->next())
		{
			if(p->IsExpandY()) {
				expand = true;
				break;
			}
		}

		return expand;
	}

	Size VBlock::GetPreferredSize () const
	{
		Size preferred_size;

		if(first() == 0) {

			preferred_size.set_width(80);
			preferred_size.set_height(60);

		} else {

			Size tmp;
			int max_width = 0;
			int max_height = 0;
			int sum = 0;

			for(AbstractWidget* p = first(); p; p = p->next())
			{
				if(p->visiable()) {
					sum++;
					tmp = p->GetPreferredSize();

					max_width = std::max(max_width, tmp.width());
					max_height = std::max(max_height, tmp.height());
				}
			}
			preferred_size.set_width(max_width);
			preferred_size.set_height(sum * (max_height - 1));

			preferred_size.add_width(margin().hsum());
			preferred_size.add_height(margin().vsum());
		}

		return preferred_size;
	}

	void VBlock::PerformMarginUpdate(const Margin& request)
	{
		FillInVBlock(position(), size(), request);
	}

	bool VBlock::SizeUpdateTest (const SizeUpdateRequest& request)
	{
		// Do not allow sub widget changing its size
		if(request.source()->container() == this) {
			return false;
		}

		return true;
	}

	bool VBlock::PositionUpdateTest (const PositionUpdateRequest& request)
	{
		// Do not allow sub widget changing its position
		if(request.source()->container() == this) {
			return false;
		}

		return true;
	}

	void VBlock::PerformSizeUpdate (const SizeUpdateRequest& request)
	{
		if(request.target() == this) {
			set_size(*request.size());
			FillInVBlock(position(), *request.size(), margin());
		}

		ReportSizeUpdate(request);
	}

	void VBlock::PerformPositionUpdate (
	        const PositionUpdateRequest& request)
	{
		if (request.target() == this) {
			int x = request.position()->x() - position().x();
			int y = request.position()->y() - position().y();

			set_position(*request.position());
			MoveSubWidgets(x, y);
		}

		ReportPositionUpdate(request);
	}

	ResponseType VBlock::Draw (const RedrawEvent& event)
	{
		return Ignore;
	}

	ResponseType VBlock::CursorEnterEvent (bool entered)
	{
		return Ignore;
	}

	ResponseType VBlock::KeyPressEvent (const KeyEvent& event)
	{
		return Ignore;
	}

	ResponseType VBlock::ContextMenuPressEvent (const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType VBlock::ContextMenuReleaseEvent (const ContextMenuEvent& event)
	{
		return Ignore;
	}

	ResponseType VBlock::MousePressEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType VBlock::MouseReleaseEvent (const MouseEvent& event)
	{
		return Ignore;
	}

	ResponseType VBlock::MouseMoveEvent (const MouseEvent& event)
	{
		return Ignore;
	}
	
	void VBlock::FillInVBlock (const Point& out_pos, const Size& out_size,
					const Margin& margin)
	{
		int x = out_pos.x() + margin.left();
		int y = out_pos.y() + margin.bottom();
		int w = out_size.width() - margin.hsum();
		int h = out_size.height() - margin.vsum();

		FillInVBlock(x, y, w, h);
	}

	void VBlock::FillInVBlock (int x, int y, int w, int h)
	{
		int sum = GetSubWidgetSize();
		if(sum == 0) return;
		int average_height = h / sum + 1;

		y = y + h;
		for(AbstractWidget* p = first(); p; p = p->next())
		{
			ResizeSubWidget(p, w, average_height);
			y = y - average_height + 1;
			SetSubWidgetPosition(p, x, y);
		}
	}

}
