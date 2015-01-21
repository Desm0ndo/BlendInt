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

#include <BlendInt/Stock/Icons.hpp>
#include <BlendInt/Gui/VLayout.hpp>

#include <BlendInt/Gui/Context.hpp>

namespace BlendInt {

	ExpandButton::ExpandButton()
	: AbstractButton()
	{
		InitializeExpandButton();
	}

	ExpandButton::ExpandButton (const String& text)
	: AbstractButton()
	{
		InitializeExpandButton(text);
	}

	ExpandButton::~ExpandButton ()
	{
	}

	bool ExpandButton::IsExpandX () const
	{
		return true;
	}

	Size ExpandButton::GetPreferredSize () const
	{
		Font font;
		int h = font.height();

		Size prefer(h + round_radius() * 2 + kPadding.hsum() + 100,
						h + kPadding.vsum());

		return prefer;
	}

	void ExpandButton::PerformSizeUpdate (const SizeUpdateRequest& request)
	{
		if(request.target() == this) {
			set_size(*request.size());
			RequestRedraw();
		}

		if(request.source() == this) {
			ReportSizeUpdate(request);
		}
	}

	void ExpandButton::PerformRoundTypeUpdate (int round_type)
	{
		set_round_type(round_type);

		RequestRedraw();
	}

	void ExpandButton::PerformRoundRadiusUpdate (float radius)
	{
		set_round_radius(radius);
		RequestRedraw();
	}

	ResponseType ExpandButton::Draw (const Context* context)
	{
		if(text()) {
			// font().Print(0.f, 0.f, text(), text_length(), 0);
		}

		float rotate = 0.f;
		if(is_checked()) {
			rotate = 0.f;
		} else {
			rotate = -90.f;
		}

		float x = Context::icons->num()->size().width()/2.f;
		float y = size().height()/2.f;

		Context::icons->num()->Draw(x, y, rotate, 1.5f, Color(0x0F0F0FFF));

		return Finish;
	}

	void ExpandButton::InitializeExpandButton ()
	{
		set_checkable(true);

		Font font;
		int h = font.height();
		set_size(h + round_radius() * 2 + kPadding.hsum(),
						h + kPadding.vsum());
	}

	void ExpandButton::InitializeExpandButton (const String& text)
	{
		set_checkable(true);

		RefPtr<Text> t(new Text(text));
		set_text(t);

		int h = t->font().height();

		if(text.empty()) {
			set_size(h + round_radius() * 2 + kPadding.hsum(),
							h + kPadding.vsum());
		} else {
			Rect text_outline;	// = font().GetTextOutline(text);

			int width = text_outline.width() + round_radius() * 2 + kPadding.hsum();
			int height = h + kPadding.vsum();

			set_size(width, height);

//			set_pen((width - text_outline.width()) / 2,
//							(height - font().height()) / 2
//											+ std::abs(font().descender()));
		}
	}

	// ----------------------

	Expander::Expander ()
	: Widget(), frame_height_(0)
	{
		ExpandButton* title_button = Manage(new ExpandButton);
		VLayout* frame = Manage(new VLayout);

		PushBackSubView(title_button);	// 0
		PushBackSubView(frame);	// 1

		int width = 0;
		int height = 0;

		Size tmp = title_button->GetPreferredSize();
		width = std::max(width, tmp.width());
		height += tmp.height();

		tmp = frame->GetPreferredSize();
		width = std::max(width, tmp.width());
		height += tmp.height();

		set_size(width, height);

		FillInExpander(size());
		frame_height_ = frame->size().height();

		events()->connect(title_button->toggled(), this, &Expander::OnToggled);
	}

	Expander::Expander (const String& title)
	: Widget(), frame_height_(0)
	{
		ExpandButton* title_button = Manage(new ExpandButton(title));
		VLayout* frame = Manage(new VLayout);

		PushBackSubView(title_button);	// 0
		PushBackSubView(frame);	// 1

		int width = 0;
		int height = 0;

		Size tmp = title_button->GetPreferredSize();
		width = std::max(width, tmp.width());
		height += tmp.height();

		tmp = frame->GetPreferredSize();
		width = std::max(width, tmp.width());
		height += tmp.height();

		set_size(width, height);

		FillInExpander(size());
		frame_height_ = frame->size().height();

		events()->connect(title_button->toggled(), this, &Expander::OnToggled);
	}

	Expander::~Expander ()
	{
	}

	bool Expander::Setup (AbstractWidget* widget)
	{
		VLayout* frame = dynamic_cast<VLayout*>(GetWidgetAt(1));
		frame->AddWidget(widget);
		return true;
	}

	Size Expander::GetPreferredSize() const
	{
		Size prefer;

		Size tmp;
		for(AbstractView* p = first_subview(); p; p = p->next_view())
		{
			tmp = p->GetPreferredSize();
			prefer.set_width(std::max(prefer.width(), tmp.width()));
			prefer.add_height(tmp.height());
		}

		return prefer;
	}

	bool Expander::IsExpandX () const
	{
		bool expand = false;

		for(AbstractView* p = first_subview(); p; p = p->next_view())
		{
			if(p->IsExpandX()) {
				expand = true;
				break;
			}
		}

		return expand;
	}

	bool Expander::IsExpandY () const
	{
		bool expand = false;

		for(AbstractView* p = first_subview(); p; p = p->next_view())
		{
			if(p->IsExpandY()) {
				expand = true;
				break;
			}
		}

		return expand;
	}

	void Expander::PerformSizeUpdate (const SizeUpdateRequest& request)
	{
		if(request.target() == this) {
			FillInExpander(*request.size());

			set_size(*request.size());
			RequestRedraw();
		}

		ReportSizeUpdate(request);
	}

	ResponseType Expander::Draw (const Context* context)
	{
		return subs_count() ? Ignore : Finish;
	}

	void Expander::FillInExpander (const Size& out_size)
	{
		int x = 0;
		int y = 0;
		int w = out_size.width();
		int h = out_size.height();

		if(w >= 0 && h >= 0)
			FillInExpander(x, y, w, h);
	}
	
	void Expander::FillInExpander (int x, int y, int width,
					int height)
	{
		int button_preferred_height = 0;
		ExpandButton* button = dynamic_cast<ExpandButton*>(GetWidgetAt(0));
		VLayout* frame = dynamic_cast<VLayout*>(GetWidgetAt(1));

		button_preferred_height = button->GetPreferredSize().height();

		if(frame->visiable()) {

			if(button_preferred_height < height) {

				y = y + height;
				ResizeSubView(button, width, button_preferred_height);
				y -= button_preferred_height;
				MoveSubViewTo(button, x, y);

				ResizeSubView(frame, width, height - button_preferred_height);
				y -= frame->size().height();

				MoveSubViewTo(frame, x, y);

			} else {

				ResizeSubView(button, width, height);
				MoveSubViewTo(button, x, y);
				ResizeSubView(frame, width, 0);
				MoveSubViewTo(frame, x, y);
			}

		} else {
			ResizeSubView(button, width, height);
			MoveSubViewTo(button, x, y);
		}
	}
	
	void Expander::SetTitle (const String& text)
	{
		dynamic_cast<ExpandButton*>(GetWidgetAt(0))->SetText(text);
	}

	const String& Expander::GetTitle () const
	{
		ExpandButton* button = dynamic_cast<ExpandButton*>(GetWidgetAt(0));

		return button->text()->text();
	}
	
	void Expander::OnToggled (AbstractButton* sender, bool toggle)
	{
		ExpandButton* button = dynamic_cast<ExpandButton*>(GetWidgetAt(0));
		VLayout* frame = dynamic_cast<VLayout*>(GetWidgetAt(1));

		if(toggle) {
			int x = position().x();
			int y = position().y() + size().height();
			frame->SetVisible(false);
			frame_height_ = frame->size().height();
			Resize(size().width(), button->size().height());
			y = y - size().height();
			MoveTo(x, y);
		} else {
			int x = position().x();
			int y = position().y() + size().height();

			frame->SetVisible(true);

			Resize(size().width(),
							button->size().height() + frame_height_);
			y = y - size().height();
			MoveTo(x, y);
		}

		RequestRedraw();
	}

}
