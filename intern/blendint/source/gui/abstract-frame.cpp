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

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <core/rect.hpp>

#include <opengl/opengl.hpp>
#include <opengl/gl-framebuffer.hpp>

#include <gui/abstract-frame.hpp>
#include <gui/abstract-window.hpp>

namespace BlendInt {

	glm::mat4 AbstractFrame::default_view_matrix = glm::lookAt(
		glm::vec3(0.f, 0.f, 1.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f));

	AbstractFrame::AbstractFrame()
	: AbstractView()
	{
		destroyed_.reset(new Cpp::Event<AbstractFrame*>);
	}

	AbstractFrame::AbstractFrame(int width, int height)
	: AbstractView(width, height)
	{
		destroyed_.reset(new Cpp::Event<AbstractFrame*>);
	}

	AbstractFrame::~AbstractFrame()
	{
		destroyed_->fire(this);
	}

	Point AbstractFrame::GetAbsolutePosition (const AbstractWidget* widget)
	{
#ifdef DEBUG
		assert(widget);
#endif

		Point pos = widget->position();

		AbstractView* p = widget->superview();
		while(p && (p != this)) {
			pos = pos + p->position() + p->GetOffset();
			p = p->superview();
		}

		pos = pos + position() + GetOffset();
		return pos;
	}

	Point AbstractFrame::GetRelativePosition(const AbstractWidget* widget)
	{
#ifdef DEBUG
		assert(widget);
#endif

		Point pos = widget->position();

		AbstractView* p = widget->superview();
		while(p && (p != this)) {
			pos = pos + p->position() + p->GetOffset();
			p = p->superview();
		}

		return pos;
	}

	AbstractFrame* AbstractFrame::GetFrame(AbstractView* widget)
	{
		AbstractView* container = widget->superview ();
		AbstractFrame* frame = 0;

		if(container == 0) {
			return dynamic_cast<AbstractFrame*>(widget);
		} else {
			while(container) {
				frame = dynamic_cast<AbstractFrame*>(container);
				if(frame) break;
				container = container->superview ();
			}
		}

		return frame;
	}

	Response AbstractFrame::PerformContextMenuPress (AbstractWindow* context)
	{
		return subs_count() ? Ignore : Finish;
	}

	Response AbstractFrame::PerformContextMenuRelease (AbstractWindow* context)
	{
		return subs_count() ? Ignore : Finish;
	}

	Response AbstractFrame::DispatchKeyEvent(AbstractView* subview, AbstractWindow* context)
	{
		if(subview == this) {
			return Ignore;
		} else {

			Response response = Ignore;

			if(subview->superview ()) {
				response = DispatchKeyEvent(subview->superview(), context);
				if(response == Finish) {
					return response;
				} else {
					return subview->PerformKeyPress(context);
				}
			} else {
				return subview->PerformKeyPress(context);
			}

		}
	}

	AbstractView* AbstractFrame::DispatchMousePressEvent(
			AbstractView* subview, AbstractWindow* context)
	{
		if(subview == this) {
			return 0;
		} else {

			Response response = Ignore;
			AbstractView* ret_val = 0;

			if(subview->superview ()) {

				ret_val = DispatchMousePressEvent(subview->superview(), context);

				if(ret_val == 0) {

					response = subview->PerformMousePress(context);

					return response == Finish ? subview : 0;

				} else {
					return ret_val;
				}

			} else {
				response = subview->PerformMousePress(context);
				return response == Finish ? subview : 0;
			}

		}
	}

	Response AbstractFrame::DispatchMouseMoveEvent(AbstractView* subview, AbstractWindow* context)
	{
		if(subview == this) {
			return Ignore;
		} else {

			if(subview->superview ()) {
				if(DispatchMouseMoveEvent(subview->superview (), context) == Ignore) {
					return subview->PerformMouseMove(context);
				} else {
					return Finish;
				}

			} else {
				return subview->PerformMouseMove(context);
			}

		}
	}

	Response AbstractFrame::DispatchMouseReleaseEvent(
			AbstractView* subview, AbstractWindow* context)
	{
		if(subview == this) {
			return Ignore;
		} else {

			if(subview->superview ()) {
				if(DispatchMouseReleaseEvent(subview->superview (), context) == Ignore) {
					return subview->PerformMouseRelease(context);
				} else {
					return Finish;
				}

			} else {
				DBG_PRINT_MSG("mouse press in %s", subview->name().c_str());
				return subview->PerformMouseRelease(context);
			}

		}
	}

	AbstractWidget* AbstractFrame::DispatchHoverEventsInWidgets(AbstractWidget* orig,
			AbstractWindow* context)
	{
		AbstractWidget* hovered_widget = orig;
		AbstractWidget* tmp = 0;

		context->register_active_frame(this);
		Point local;	// the relative local position of the cursor in a widget
		Point offset;

		// find the new top hovered widget
		if (hovered_widget != nullptr) {

			AbstractView* superview = hovered_widget->superview();
			Rect rect;

			AbstractWidget* widget = dynamic_cast<AbstractWidget*>(superview);
			if(widget) {
				rect.set_position(GetAbsolutePosition(widget));
				rect.set_size(widget->size());
			} else {
				assert(superview == this);
				rect.set_position(position());
				rect.set_size(size());
			}

			bool hovered = rect.contains(context->GetCursorPosition());

			if(hovered) {

				offset = superview->GetOffset();
				local.reset(
						context->GetCursorPosition().x() - rect.x() - offset.x(),
						context->GetCursorPosition().y() - rect.y() - offset.y());

				if(hovered_widget->Contain(local)) {
					hovered_widget = DispatchHoverEventDeeper(
							hovered_widget, context, local);
				} else {

					delegate_mouse_hover_out_event(hovered_widget, context);

					// find which contianer contains cursor position
					while (superview) {

						if (superview == this) {	// FIXME: the widget may be mvoed to another context
							superview = 0;
							break;
						}

						offset = superview->GetOffset();
						local.reset(
								local.x() + superview->position().x() + offset.x(),
								local.y() + superview->position().y() + offset.y());

						if (superview->Contain(local)) break;

						superview = superview->superview();
					}

					hovered_widget = dynamic_cast<AbstractWidget*>(superview);

					if(hovered_widget) {
						hovered_widget = DispatchHoverEventDeeper(hovered_widget, context, local);
					}

				}

			} else {

				delegate_mouse_hover_out_event(hovered_widget, context);

				// find which contianer contains cursor position
				superview = superview->superview();
				while (superview != nullptr) {

					if (superview == this) {	// FIXME: the widget may be mvoed to another context
						superview = nullptr;
						break;
					}

					widget = dynamic_cast<AbstractWidget*>(superview);
					if(widget) {
						rect.set_position(GetAbsolutePosition(widget));
						rect.set_size(widget->size());
					} else {
						assert(superview == this);
						rect.set_position(position());
						rect.set_size(size());
					}

					offset = superview->GetOffset();
					local.reset(
							context->GetCursorPosition().x() - rect.x() - offset.x(),
							context->GetCursorPosition().y() - rect.y() - offset.y());

					if(rect.contains(context->GetCursorPosition())) break;

					superview = superview->superview();
				}

				hovered_widget = dynamic_cast<AbstractWidget*>(superview);
				if(hovered_widget) {
					for (AbstractView* p = widget->last_subview (); p;
							p = p->previous_view ()) {

						tmp = dynamic_cast<AbstractWidget*>(p);

						if(tmp) {
							if (p->visiable () && p->Contain (local)) {
								hovered_widget = tmp;

								delegate_mouse_hover_in_event (hovered_widget, context);
								hovered_widget = DispatchHoverEventDeeper(hovered_widget, context, local);

								break;
							}
						} else {
							break;
						}
					}
				}

			}

		} else {

			offset = GetOffset();
			local.reset(
					context->GetCursorPosition().x() - position().x() - offset.x(),
					context->GetCursorPosition().y() - position().y() - offset.y());

			for(AbstractView* p = last_subview(); p; p = p->previous_view())
			{
				tmp = dynamic_cast<AbstractWidget*>(p);

				if(tmp) {
					if (p->visiable() && p->Contain(local)) {
						hovered_widget = tmp;
						delegate_mouse_hover_in_event(hovered_widget, context);
						break;
					}
				} else {
					break;
				}
			}

			if(hovered_widget) {
				hovered_widget = DispatchHoverEventDeeper(
						hovered_widget, context, local);
			}

		}

		return hovered_widget;
	}

	void AbstractFrame::ClearHoverWidgets(AbstractView* hovered_widget)
	{
#ifdef DEBUG
		assert(hovered_widget);
#endif

		while (hovered_widget && (hovered_widget != this)) {
			hovered_widget->set_hover(false);
			hovered_widget = hovered_widget->superview();
		}
	}

	void AbstractFrame::ClearHoverWidgets(AbstractView* hovered_widget, AbstractWindow* context)
	{
#ifdef DEBUG
		assert(hovered_widget);
#endif

		while (hovered_widget && (hovered_widget != this)) {
			hovered_widget->set_hover(false);
			hovered_widget->PerformHoverOut(context);
			hovered_widget = hovered_widget->superview();
		}
	}

	bool AbstractFrame::RenderSubFramesToTexture (
		AbstractFrame* frame,
		AbstractWindow* context,
		const glm::mat4& projection,
		const glm::mat3& model,
		GLTexture2D* texture)
	{
		bool retval = false;

		assert(texture != nullptr);

        GLTexture2D* tex = texture;
        if(!tex->id())
            tex->generate();

        tex->bind();
        tex->SetWrapMode(GL_REPEAT, GL_REPEAT);
        tex->SetMinFilter(GL_NEAREST);
        tex->SetMagFilter(GL_NEAREST);
        tex->SetImage(0, GL_RGBA, frame->size().width(), frame->size().height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        GLFramebuffer* fb = new GLFramebuffer;
        fb->generate();
        fb->bind();

        // Set "renderedTexture" as our colour attachement #0
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, tex->id(), 0);
        //fb->Attach(*tex, GL_COLOR_ATTACHMENT0);

        // Critical: Create a Depth_STENCIL renderbuffer for this off-screen rendering
        GLuint rb;
        glGenRenderbuffers(1, &rb);

        glBindRenderbuffer(GL_RENDERBUFFER, rb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL,
                              frame->size().width(), frame->size().height());
        //Attach depth buffer to FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, rb);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rb);

        if(GLFramebuffer::CheckStatus()) {

            AbstractWindow::shaders->SetWidgetProjectionMatrix(projection);
            AbstractWindow::shaders->SetWidgetModelMatrix(model);

            // in this off-screen framebuffer, a new stencil buffer was created, reset the stencil count to 0 and restore later
            GLuint original_stencil_count = context->stencil_count_;
			context->stencil_count_ = 0;

            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClearDepth(1.0);
            glClearStencil(0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            //glEnable(GL_BLEND);

            glViewport(0, 0, frame->size().width(), frame->size().height());

            // Draw context:
            frame->DrawSubViewsOnce(context);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glViewport(0, 0, context->size().width(), context->size().height());
			#ifdef DEBUG
			assert(context->stencil_count_ == 0);
			#endif
			context->stencil_count_ = original_stencil_count;

			retval = true;
        }

        fb->reset();
        tex->reset();

        //delete tex; tex = 0;

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glDeleteRenderbuffers(1, &rb);

        delete fb; fb = nullptr;

        return retval;
	}

	AbstractWidget* AbstractFrame::DispatchHoverEventDeeper(AbstractWidget* widget, AbstractWindow* context,
			Point& local)
	{
		AbstractWidget* retval = widget;
		AbstractWidget* tmp = 0;

		Point offset = widget->GetOffset();
		local.reset(
				local.x() - widget->position().x() - offset.x(),
		        local.y() - widget->position().y() - offset.y());

		for (AbstractView* p = widget->last_subview (); p;
				p = p->previous_view ()) {

			tmp = dynamic_cast<AbstractWidget*>(p);

			if(tmp) {
				if (p->visiable () && p->Contain (local)) {
					retval = tmp;
					delegate_mouse_hover_in_event (retval, context);
					retval = DispatchHoverEventDeeper(retval, context, local);

					break;
				}
			} else {
				break;
			}
	
		}

		return retval;
	}

}