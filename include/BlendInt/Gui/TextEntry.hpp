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

#ifndef _BLENDINT_TEXTENTRY_HPP_
#define _BLENDINT_TEXTENTRY_HPP_

#include <BlendInt/Core/Rect.hpp>
#include <BlendInt/Core/Timer.hpp>
#include <BlendInt/Core/String.hpp>
#include <BlendInt/Core/Point.hpp>
#include <BlendInt/Core/Margin.hpp>

#include <BlendInt/OpenGL/GLArrayBuffer.hpp>

#include <BlendInt/Gui/RoundWidget.hpp>
#include <BlendInt/Gui/Font.hpp>

namespace BlendInt {

	class TextEntry: public RoundWidget
	{
		DISALLOW_COPY_AND_ASSIGN(TextEntry);

	public:

		TextEntry();

		virtual ~TextEntry();

		void SetText (const String& text);

		void SetFont (const Font& font);

		const String& text () const {return m_text;}

	protected:

		virtual bool Update (const UpdateRequest& request);

		virtual ResponseType Draw (const RedrawEvent& event);

		virtual ResponseType KeyPressEvent (const KeyEvent& event);

		virtual ResponseType MousePressEvent (const MouseEvent& event);

	private:

		void InitOnce ();

		void OnReverseCursor ();

		size_t GetValidTextSize ();

		int GetCursorPosition (const MouseEvent& event);

		/**
		 * @brief Get the index and length of the text to show
		 * @param[out] start The index in the text to print
		 * @param[out] length The length of the text to print
		 */
		void GetVisibleTextPlace (size_t* start, size_t* length);

		void AdjustVisibleTextLength ();

		GLuint m_vao;

		Font m_font;

		String m_text;

		size_t m_start;	// where start print the text

		size_t m_length;

		int m_cursor_position;

		/**
		 * @brief The position to print text
		 */
		Point m_origin;

		/**
		 * @brief Box in which hold the text
		 */
		Rect m_text_outline;

		RefPtr<GLArrayBuffer> m_inner_buffer;
		RefPtr<GLArrayBuffer> m_outer_buffer;
		RefPtr<GLArrayBuffer> m_cursor_buffer;

		static Margin DefaultTextEntryPadding;
	};

}

#endif /* _BLENDINT_TEXTENTRY_HPP_ */
