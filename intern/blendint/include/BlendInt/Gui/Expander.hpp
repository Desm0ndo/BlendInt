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

#pragma once

#include <BlendInt/Gui/AbstractButton.hpp>

namespace BlendInt {

	class ToggleButton;

	class ExpandButton: public AbstractButton
	{
		DISALLOW_COPY_AND_ASSIGN(ExpandButton);

	public:

		ExpandButton ();

		ExpandButton (const String& text);

		virtual ~ExpandButton ();

		virtual bool IsExpandX () const;

	protected:

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual void PerformRoundTypeUpdate (int round_type);

		virtual void PerformRoundRadiusUpdate (float radius);

		virtual ResponseType Draw (AbstractWindow* context);

	};


	// --------------------------

	/**
	 * @brief Expander
	 */
	class Expander: public Widget
	{
		DISALLOW_COPY_AND_ASSIGN(Expander);

	public:

		Expander ();

		Expander (const String& title);

		virtual ~Expander ();

		bool Setup (AbstractWidget* widget);

		void SetTitle (const String& text);

		const String& GetTitle () const;

		virtual bool IsExpandX () const;

		virtual bool IsExpandY () const;

		virtual Size GetPreferredSize () const;

	protected:

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual ResponseType Draw (AbstractWindow* context);

	protected:

		void FillInExpander (const Size& out_size);

		void FillInExpander (int x, int y, int width, int height);

		void OnToggled (AbstractButton* sender, bool toggle);

		int frame_height_;

	};

}
