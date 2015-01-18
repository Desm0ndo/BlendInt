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

#ifdef __UNIX__
#ifdef __APPLE__
#include <gl3.h>
#include <gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif  // __UNIX__
 
#include <vector>
#include <pthread.h>

#include <Cpp/Events.hpp>

#include <BlendInt/Core/Types.hpp>
#include <BlendInt/Core/Object.hpp>
#include <BlendInt/Core/Point.hpp>
#include <BlendInt/Core/Size.hpp>
#include <BlendInt/Core/Margin.hpp>

namespace BlendInt {

	class Context;
	class AbstractView;

	/**
	 * @brief Set/reset the managed flag of a View
	 *
	 * A managed View will be deleted when the superview is destroyed.
	 */
	template<typename T>
	T* Manage (T* obj, bool val = true)
	{
		obj->set_manage(val);
		return obj;
	}

	/**
	 * @brief Check if a view is contained in a container
	 */
	extern bool IsContained (AbstractView* container, AbstractView* view);

	class GeometryUpdateRequest
	{
	public:

		GeometryUpdateRequest (AbstractView* source, AbstractView* target)
		: m_source(source),
		  m_target(target)
		{

		}

		~GeometryUpdateRequest ()
		{

		}

		AbstractView* source () const
		{
			return m_source;
		}

		AbstractView* target () const
		{
			return m_target;
		}

	private:

		/**
		 * Disabled
		 */
		GeometryUpdateRequest();

		AbstractView* m_source;
		AbstractView* m_target;
	};

	class SizeUpdateRequest: public GeometryUpdateRequest
	{
	public:

		SizeUpdateRequest (AbstractView* source, AbstractView* target)
		: GeometryUpdateRequest(source, target), m_size(0)
		{
		}

		SizeUpdateRequest (AbstractView* source, AbstractView* target, const Size* size)
		: GeometryUpdateRequest(source, target), m_size(size)
		{

		}

		const Size* size() const
		{
			return m_size;
		}

		void set_size (const Size* size)
		{
			m_size = size;
		}

	private:

		const Size* m_size;
	};

	class PositionUpdateRequest: public GeometryUpdateRequest
	{
	public:

		PositionUpdateRequest (AbstractView* source, AbstractView* target)
		: GeometryUpdateRequest(source, target), m_position(0)
		{
		}

		PositionUpdateRequest (AbstractView* source, AbstractView* target, const Point* pos)
		: GeometryUpdateRequest(source, target), m_position(pos)
		{

		}

		const Point* position() const
		{
			return m_position;
		}

		void set_position (const Point* pos)
		{
			m_position = pos;
		}

	private:

		const Point* m_position;
	};

	class VisibilityUpdateRequest: public GeometryUpdateRequest
	{
	public:

		VisibilityUpdateRequest (AbstractView* source, AbstractView* target)
		: GeometryUpdateRequest(source, target), m_visibility(0)
		{

		}

		VisibilityUpdateRequest (AbstractView* source, AbstractView* target, const bool* visibility)
		: GeometryUpdateRequest(source, target), m_visibility (visibility)
		{

		}

		const bool* visibility () const
		{
			return m_visibility;
		}

		void set_visibility (const bool* visibility)
		{
			m_visibility = visibility;
		}

	private:

		const bool* m_visibility;
	};

	// ----------------------------------------------------

	/**
	 * @brief Abstract class for all views
	 *
	 * AbstractView is the base class of all user interface objects in
	 * BlendInt, it receives mouse, keyboard and other events from the
	 * window system, and draw a representation of itself with OpenGL
	 * APIs in the OpenGL Context.
	 *
	 * There're 2 main different groups of the sub classes of
	 * AbstractView:
	 *
	 *	- frames
	 *	- widgets
	 *
	 * A Frame inherit from AbstractFrame and works like a window in
	 * the desktop, it's used to contains other widgets or other
	 * frames, dispatch render or input events, and usually manages
	 * its own projection/view/model matrix to display its sub views.
	 *
	 * The other group is called widget. All widgets inherit from
	 * AbstractWidget. A widget must be contained in a frame for
	 * display and interact with the user.
	 *
	 * @see AbstractFrame
	 * @see AbstractWidget
	 * @see Context
	 *
	 * @ingroup gui
	 */
	class AbstractView: public Object
	{
		DISALLOW_COPY_AND_ASSIGN(AbstractView);

	public:

		/**
		 * @brief The default constructor
		 */
		AbstractView ();

		AbstractView (int width, int height);

		/**
		 * @brief Destructor
		 */
		virtual ~AbstractView ();

		Point GetGlobalPosition () const;

		virtual Size GetPreferredSize () const;

		/**
		 * @brief Resize the view
		 * @param[in] width The new width of the view
		 * @param[in] height The new height of the view
		 *
		 * Call Update() to check the parameters, if valid, resize the
		 * view.
		 */
		void Resize (int width, int height);

		/**
		 * @brief Resize the view
		 * @param[in] size The new size of the view
		 *
		 * Call Update() to check the parameters, if valid, resize the
		 * view.
		 */
		void Resize (const Size& size);

		void MoveTo (int x, int y);

		void MoveTo (const Point& pos);

		void SetVisible (bool visible);

		void RequestRedraw ();

		AbstractView* operator [] (int i) const;

		AbstractView* GetWidgetAt (int i) const;

		const Point& position () const
		{
			return position_;
		}

		virtual bool IsExpandX () const
		{
			return false;
		}

		virtual bool IsExpandY () const
		{
			return false;
		}

		virtual bool Contain (const Point& point) const;

		// always return (0, 0) except AbstractScrollable
		virtual Point GetOffset () const
		{
			return Point(0, 0);
		}

		const Size& size () const
		{
			return size_;
		}

		inline bool focus () const
		{
			return view_flag_ & ViewFocused;
		}

		inline bool hover () const
		{
			return view_flag_ & ViewHover;
		}

		inline bool visiable () const
		{
			return view_flag_ & ViewVisible;
		}

		inline bool managed () const
		{
			return view_flag_ & ViewManaged;
		}

		inline bool refresh () const
		{
			return view_flag_ & ViewRefresh;
		}

		inline bool pressed_ext () const
		{
			return view_flag_ & ViewPressed;
		}

		inline int subs_count () const
		{
			return subs_count_;
		}

		inline AbstractView* superview() const
		{
			return superview_;
		}

		inline AbstractView* previous_view () const
		{
			return previous_view_;
		}

		inline AbstractView* next_view () const
		{
			return next_view_;
		}

		inline AbstractView* first_subview () const
		{
			return first_subview_;
		}

		inline AbstractView* last_subview () const
		{
			return last_subview_;
		}

		static void MoveToFirst (AbstractView* view);

		static void MoveToLast (AbstractView* view);

		static void MoveForward (AbstractView* view);

		static void MoveBackward (AbstractView* view);

		static void SetDefaultBorderWidth (float border);

		static inline float default_border_width ()
		{
			return kBorderWidth;
		}

#ifdef DEBUG

		inline void set_name (const char* name)
		{
			name_ = name;
		}

		inline void set_name (const std::string& name)
		{
			name_ = name;
		}

		const std::string& name () const {return name_;}

#endif

	protected:

		/**
		 * @brief preset the position of this view
		 * @param x
		 * @param y
		 *
		 * @note this function should be called only in the constructor of subclass
		 * to set the position without through Update() for performance.
		 */
		void set_position (int x, int y)
		{
			position_.reset(x, y);
		}

		/**
		 * @brief preset the position of this view
		 * @param pos
		 *
		 * @note this function should be called only in the constructor of subclass
		 * to set the position without through Update() for performance.
		 */
		void set_position (const Point& pos)
		{
			position_ = pos;
		}

		/**
		 * @brief preset the size of the form
		 * @param width
		 * @param height
		 *
		 * @note this function should be called only in the constructor of subclass to set
		 * the size without through Update() for performance.
		 */
		inline void set_size (int width, int height)
		{
			size_.reset(width, height);
		}

		/**
		 * @brief preset the size of the form
		 * @param size
		 *
		 * @note this function should be called only in the constructor of subclass to set
		 * the size without through Update() for performance.
		 */
		inline void set_size (const Size& size)
		{
			size_ = size;
		}

		inline void set_focusable (bool focusable)
		{
			if(focusable) {
				SETBIT(view_flag_, ViewFocusable);
			} else {
				CLRBIT(view_flag_, ViewFocusable);
			}
		}

		inline void set_focus (bool focus)
		{
			if(focus) {
				SETBIT(view_flag_, ViewFocused);
			} else {
				CLRBIT(view_flag_, ViewFocused);
			}
		}

		inline void set_hover (bool hover)
		{
			if(hover) {
				SETBIT(view_flag_, ViewHover);
			} else {
				CLRBIT(view_flag_, ViewHover);
			}
		}

		inline void set_visible (bool visiable)
		{
			if(visiable) {
				SETBIT(view_flag_, ViewVisible);
			} else {
				CLRBIT(view_flag_, ViewVisible);
			}
		}

		inline void set_pressed (bool pressed)
		{
			if(pressed) {
				SETBIT(view_flag_, ViewPressed);
			} else {
				CLRBIT(view_flag_, ViewPressed);
			}
		}

		inline void set_refresh (bool refresh)
		{
			if(refresh) {
				SETBIT(view_flag_, ViewRefresh);
			} else {
				CLRBIT(view_flag_, ViewRefresh);
			}
		}

		virtual bool PreDraw (const Context* context) = 0;

		virtual ResponseType Draw (const Context* context) = 0;

		virtual void PostDraw (const Context* context) = 0;

		virtual void PerformFocusOn (const Context* context) = 0;

		virtual void PerformFocusOff (const Context* context) = 0;

		virtual void PerformHoverIn (const Context* context) = 0;

		virtual void PerformHoverOut (const Context* context) = 0;

		virtual ResponseType PerformKeyPress (const Context* context) = 0;

		virtual ResponseType PerformContextMenuPress (const Context* context) = 0;

		virtual ResponseType PerformContextMenuRelease (const Context* context) = 0;

		virtual ResponseType PerformMousePress (const Context* context) = 0;

		virtual ResponseType PerformMouseRelease (const Context* context) = 0;

		virtual ResponseType PerformMouseMove (const Context* context) = 0;

		virtual bool SizeUpdateTest (const SizeUpdateRequest& request);

		virtual bool PositionUpdateTest (const PositionUpdateRequest& request);

		virtual bool VisibilityUpdateTest (const VisibilityUpdateRequest& request);

		virtual void PerformSizeUpdate (const SizeUpdateRequest& request);

		virtual void PerformPositionUpdate (const PositionUpdateRequest& request);

		virtual void PerformVisibilityUpdate (const VisibilityUpdateRequest& request);

		void ReportSizeUpdate (const SizeUpdateRequest& request);

		void ReportPositionUpdate (const PositionUpdateRequest& request);

		void ReportVisibilityRequest (const VisibilityUpdateRequest& request);

		bool PushFrontSubView (AbstractView* view);

		bool InsertSubView (int index, AbstractView* view);

		bool PushBackSubView (AbstractView* view);

		virtual bool RemoveSubView (AbstractView* view);

		void ClearSubViews ();

		void ResizeSubView (AbstractView* sub, int width, int height);

		void ResizeSubView (AbstractView* sub, const Size& size);

		void MoveSubViewTo (AbstractView* sub, int x, int y);

		void MoveSubViewTo (AbstractView* sub, const Point& pos);

		void SetSubViewVisibility (AbstractView* sub, bool visible);

		void DrawSubViewsOnce (const Context* context);

		static void GenerateVertices (
				const Size& size,
				float border,
				int round_type,
				float radius,
				std::vector<GLfloat>* inner,
				std::vector<GLfloat>* outer);

		static void GenerateVertices (
				const Size& size,
				float border,
				int round_type,
				float radius,
				Orientation shadedir,
				short shadetop,
				short shadedown,
				std::vector<GLfloat>* inner,
				std::vector<GLfloat>* outer);

		static int GetOutlineVertices (int round_type);

		/**
		 * @brief Used to get emboss vertices
		 * @return
		 */
		static int GetHalfOutlineVertices (int round_type);

	private:

		friend class Context;
		friend class AbstractFrame;
		friend class AbstractWidget;
		friend class AbstractAdjustment;

		template <typename T> friend T* Manage (T* obj, bool val);

		enum ViewFlagIndex {

			ViewManaged = (1 << 0),

			ViewVisible = (1 << 1),

			// only valid when use off-screen render in container
			ViewRefresh = (1 << 2),

			ViewFocused = (1 << 3),

			/** If this view is in cursor hover list in Context */
			ViewHover = (1 << 4),

			// set this flag when the view or frame is pressed
			ViewPressed = (1 << 5),

			ViewFocusable = (1 << 6)

		};

		/**
		 * @brief Dispatch draw
		 * @param[in] view
		 * @param[in] profile
		 * @param[in] use_parent_status
		 * 	- true: use superview refresh() status to set view's refresh flag
		 * 	- false: set view's flag to false after Draw()
		 */
		static void DispatchDrawEvent (AbstractView* view, const Context* context);

		static void GenerateTriangleStripVertices (
						const std::vector<GLfloat>* inner,
						const std::vector<GLfloat>* edge,
						unsigned int num,
						std::vector<GLfloat>* strip);

		static inline float make_shaded_offset (short shadetop, short shadedown, float fact);

		void DistributeHorizontally (int x, int width, int space);

		void DistributeVertically (int y, int height, int space);

		void AlignHorizontally (int y, int height, int alignment);

		void AlignVertically (int x, int width, int alignment);

		void set_manage (bool val)
		{
			if(val) {
				SETBIT(view_flag_, ViewManaged);
			} else {
				CLRBIT(view_flag_, ViewManaged);
			}
		}

		Point position_;

		Size size_;

		uint32_t view_flag_;

		int subs_count_;	// count of sub widgets

		AbstractView* superview_;

		AbstractView* previous_view_;

		AbstractView* next_view_;

		/**
		 * @brief The first sub view
		 */
		AbstractView* first_subview_;

		AbstractView* last_subview_;

#ifdef DEBUG
		std::string name_;
#endif

		static float kBorderWidth;

		static const float cornervec[WIDGET_CURVE_RESOLU][2];

	};

} /* namespace BlendInt */
