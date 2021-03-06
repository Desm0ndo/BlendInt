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

#include <blendint/opengl/gl-buffer.hpp>

#include <blendint/gui/linear-layout.hpp>
#include <blendint/gui/abstract-node.hpp>
#include <blendint/gui/widget-shadow.hpp>

namespace BlendInt {

  class String;

  /**
   * @brief A special view used to display and manage a node in NodeView
   *
   * ColorScheme name in theme: "node"
   *
   * @ingroup blendint_gui_nodes
   */
  class Node: public AbstractNode
  {
  public:

    Node (const String& title);

    virtual ~Node ();

    bool AddWidget (AbstractWidget* widget);

    bool InsertWidget (int index, AbstractWidget* widget);

    virtual bool IsExpandX () const;

    virtual bool IsExpandY () const;

    virtual Size GetPreferredSize () const;

    void SetInnerColor (unsigned int color);

    void SetOuterColor (unsigned int color);

  protected:

    virtual void PerformSizeUpdate (const AbstractView* source, const AbstractView* target, int width, int height);

    virtual void PerformRoundTypeUpdate (int round_type);

    virtual void PerformRoundRadiusUpdate (float radius);

    virtual Response Draw (AbstractWindow* context);

  private:

    /**
     * 0: inner
     * 1: outer
     * 2: header
     */
    GLuint vao_[3];

    GLBuffer<ARRAY_BUFFER, 3> vbo_;

    LinearLayout* main_layout_;

    LinearLayout* layout_;

    RefPtr<WidgetShadow> shadow_;

  };

}
