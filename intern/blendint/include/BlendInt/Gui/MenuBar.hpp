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

#ifndef _BLENDINT_GUI_MENUBAR_HPP_
#define _BLENDINT_GUI_MENUBAR_HPP_

#include <BlendInt/Gui/Layout.hpp>
#include <BlendInt/Gui/MenuButton.hpp>

namespace BlendInt {

	class MenuBar: public Layout
	{
		DISALLOW_COPY_AND_ASSIGN(MenuBar);

	public:

		MenuBar ();

		virtual ~MenuBar ();

		MenuButton* Append (const RefPtr<Menu>& menu);

		MenuButton* Append (const String& text, const RefPtr<Menu>& menu);

		MenuButton* Append (MenuButton* button, const RefPtr<Menu>& menu);

		MenuButton* Append (MenuButton* button);

		void SetMenu (size_t index, const RefPtr<Menu>& menu);

		void SetMenu (MenuButton* button, const RefPtr<Menu>& menu);

		virtual Size GetPreferredSize () const;

		virtual bool IsExpandX () const;

		MenuButton* GetMenuButton (int index);

		int space () const
		{
			return m_space;
		}

	protected:

		virtual void PerformMarginUpdate (const Margin& request);

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual ResponseType Draw (Profile& profile);

	private:

		void OnMenuButtonClicked ();

		void OnMenuItemTriggered (Action* item);

		void OnMenuHide (AbstractWidget* widget, int type);

		void InitializeMenuBar ();

		int GetLastPosition ();

		GLuint m_vao;

		int m_space;

		MenuButton* m_active_button;

		RefPtr<GLArrayBuffer> inner_;

	};

}

#endif /* _BLENDINT_GUI_MENUBAR_HPP_ */