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

#include <blendint/gui/abstract-form.hpp>

#include <blendint/opengl/gl-texture2d.hpp>
#include <blendint/opengl/gl-buffer.hpp>

namespace BlendInt {

class ViewBuffer: public AbstractForm
{
 public:

  ViewBuffer () = delete;  // disable the default constructor

  ViewBuffer (int width, int height);

  virtual ~ViewBuffer ();

  virtual void Draw (int x = 0,
                     int y = 0,
                     const float* color_ptr = 0,
                     short gamma = 0,
                     float rotate = 0.f,
                     float scale_x = 1.f,
                     float scale_y = 1.f) const;

  virtual void DrawInRect (const Rect& rect,
                           int align,
                           const float* color_ptr = 0,
                           short gamma = 0,
                           float rotate = 0.f,
                           bool scale = false) const;

  inline GLTexture2D* texture ()
  {
    return &texture_;
  }

#ifdef DEBUG
  void SaveToFile (const char* file);
#endif

 protected:

  virtual void PerformSizeUpdate (int width, int height);

 private:

  GLuint vao_;

  GLBuffer<> vbo_;

  GLTexture2D texture_;

};

}
