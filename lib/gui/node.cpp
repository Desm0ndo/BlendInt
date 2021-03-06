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

#include <blendint/core/string.hpp>

#include <blendint/gui/flow-layout.hpp>
#include <blendint/gui/abstract-frame.hpp>
#include <blendint/gui/abstract-window.hpp>
#include <blendint/gui/label.hpp>
#include <blendint/gui/node.hpp>

namespace BlendInt {

Node::Node (const String& title)
    : AbstractNode(),
      main_layout_(0),
      layout_(0)
{
  set_round_type(RoundAll);
  set_round_radius(10.f);

  main_layout_ = new LinearLayout(Vertical);
  main_layout_->SetMargin(Margin(5, 5, 5, 5));

  layout_ = new LinearLayout(Vertical);

  Label* label = new Label(title);
  main_layout_->AddWidget(label);
  main_layout_->AddWidget(layout_);

  PushBackSubView(main_layout_);
  set_size(main_layout_->size());

  std::vector<GLfloat> inner_verts;
  std::vector<GLfloat> outer_verts;

  GenerateRoundedVertices(Vertical, AbstractWindow::theme()->node().shadetop,
                          AbstractWindow::theme()->node().shadedown,
                          &inner_verts, &outer_verts);

  glGenVertexArrays(2, vao_);
  vbo_.generate();

  glBindVertexArray(vao_[0]);

  vbo_.bind(0);
  vbo_.set_data(sizeof(GLfloat) * inner_verts.size(), &inner_verts[0]);
  glEnableVertexAttribArray(AttributeCoord);
  glVertexAttribPointer(AttributeCoord, 3,
                        GL_FLOAT,
                        GL_FALSE, 0, 0);

  glBindVertexArray(vao_[1]);
  vbo_.bind(1);
  vbo_.set_data(sizeof(GLfloat) * outer_verts.size(), &outer_verts[0]);
  glEnableVertexAttribArray(AttributeCoord);
  glVertexAttribPointer(AttributeCoord, 2,
                        GL_FLOAT,
                        GL_FALSE, 0, 0);

  glBindVertexArray(0);
  vbo_.reset();

  shadow_.reset(new WidgetShadow(size(), round_type(), round_radius()));
}

Node::~Node ()
{
  glDeleteVertexArrays(2, vao_);
}

bool Node::AddWidget (AbstractWidget* widget)
{
  return layout_->AddWidget(widget);
}

bool Node::InsertWidget (int index, AbstractWidget* widget)
{
  return layout_->InsertWidget(index, widget);
}

bool Node::IsExpandX () const
{
  return main_layout_->IsExpandX();
}

bool Node::IsExpandY () const
{
  return main_layout_->IsExpandY();
}

Size Node::GetPreferredSize () const
{
  return main_layout_->GetPreferredSize();
}

void Node::SetInnerColor (unsigned int color)
{
}

void Node::SetOuterColor (unsigned int color)
{
}

void Node::PerformSizeUpdate (const AbstractView* source, const AbstractView* target, int width, int height)
{
  if (target == this) {

    set_size(width, height);

    std::vector<GLfloat> inner_verts;
    std::vector<GLfloat> outer_verts;

    GenerateRoundedVertices(Vertical, AbstractWindow::theme()->node().shadetop,
                            AbstractWindow::theme()->node().shadedown,
                            &inner_verts, &outer_verts);

    vbo_.bind(0);
    vbo_.set_sub_data(0, sizeof(GLfloat) * inner_verts.size(),
                      &inner_verts[0]);
    vbo_.bind(1);
    vbo_.set_sub_data(0, sizeof(GLfloat) * outer_verts.size(),
                      &outer_verts[0]);
    vbo_.reset();

    ResizeSubView(main_layout_, size());

    shadow_->Resize(size());

    RequestRedraw();
  }

  if (source == this) {
    report_size_update(source, target, width, height);
  }
}

void Node::PerformRoundTypeUpdate (int round)
{
  set_round_type(round);

  std::vector<GLfloat> inner_verts;
  std::vector<GLfloat> outer_verts;

  GenerateRoundedVertices(Vertical, AbstractWindow::theme()->node().shadetop,
                          AbstractWindow::theme()->node().shadedown,
                          &inner_verts, &outer_verts);

  vbo_.bind(0);
  vbo_.set_data(sizeof(GLfloat) * inner_verts.size(), &inner_verts[0]);
  vbo_.bind(1);
  vbo_.set_data(sizeof(GLfloat) * outer_verts.size(), &outer_verts[0]);
  vbo_.reset();

  RequestRedraw();
}

void Node::PerformRoundRadiusUpdate (float radius)
{
  set_round_radius(radius);

  std::vector<GLfloat> inner_verts;
  std::vector<GLfloat> outer_verts;

  GenerateRoundedVertices(Vertical, AbstractWindow::theme()->node().shadetop,
                          AbstractWindow::theme()->node().shadedown,
                          &inner_verts, &outer_verts);

  vbo_.bind(0);
  vbo_.set_data(sizeof(GLfloat) * inner_verts.size(), &inner_verts[0]);
  vbo_.bind(1);
  vbo_.set_data(sizeof(GLfloat) * outer_verts.size(), &outer_verts[0]);
  vbo_.reset();

  RequestRedraw();
}

Response Node::Draw (AbstractWindow* context)
{
  glm::vec3 v = context->shaders()->widget_model_matrix()
      * glm::vec3(0.f, 0.f, 1.f);

  if (context->active_frame()->has_view_buffer()) {
    shadow_->Draw(0, 0, v.x - context->viewport_origin().x(),
                  v.y - context->viewport_origin().y());
  } else {
    shadow_->Draw(
        0,
        0,
        context->active_frame()->position().x() + v.x
        - context->viewport_origin().x(),
        context->active_frame()->position().y() + v.y
        - context->viewport_origin().y());
  }

  AbstractWindow::shaders()->widget_inner_program()->use();

  glUniform1i(
      AbstractWindow::shaders()->location(Shaders::WIDGET_INNER_GAMMA), 0);
  glUniform1i(
      AbstractWindow::shaders()->location(Shaders::WIDGET_INNER_SHADED),
      context->theme()->node().shaded);
  glUniform4fv(
      AbstractWindow::shaders()->location(Shaders::WIDGET_INNER_COLOR), 1,
      context->theme()->node().inner.data());

  int vertices_count = outline_vertex_count(round_type());

  glBindVertexArray(vao_[0]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, vertices_count + 2);

  AbstractWindow::shaders()->widget_outer_program()->use();

  glUniform2f(
      AbstractWindow::shaders()->location(Shaders::WIDGET_OUTER_OFFSET),
      0.f, 0.f);
  glUniform4fv(
      AbstractWindow::shaders()->location(Shaders::WIDGET_OUTER_COLOR), 1,
      context->theme()->node().outline.data());

  glBindVertexArray(vao_[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0,
               vertices_count * 2 + 2);

  context->icons()->end_point()->Draw(0, 20, Color(Palette::Yellow).data());

  return subview_count() ? Ignore : Finish;
}

}
