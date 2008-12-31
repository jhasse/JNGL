/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "window.hpp"

#include <boost/assign/ptr_map_inserter.hpp>

namespace jngl
{
	double Window::GetTextWidth(const std::string& text)
	{
		return fonts_[fontSize_][fontName_].GetTextWidth(text);
	}

	void Window::Print(const std::string& text, const double xposition, const double yposition)
	{
		fonts_[fontSize_][fontName_].Print(xposition, yposition, text);
	}

	void Window::SetFont(const std::string& filename)
	{
		try
		{
			if(fonts_[fontSize_].find(filename) == fonts_[fontSize_].end()) // Only load font if it doesn't exist yet
			{
				boost::assign::ptr_map_insert(fonts_[fontSize_])(filename, filename.c_str(), fontSize_);
			}
		}
		catch(std::exception& e)
		{
			throw e;
		}
		fontName_ = filename;
	}

	void Window::SetFontByName(const std::string& name)
	{
		Window::SetFont(GetFontFileByName(name));
	}

	void Window::FontSize(const int size)
	{
		int oldSize = fontSize_;
		fontSize_ = size;
		try
		{
			SetFont(fontName_); // We changed the size we also need to reload the current font
		}
		catch(std::exception& e) // Someting went wrong ...
		{
			fontSize_ = oldSize; // ... so let's set fontSize_ back to the previous size
			throw e;
		}
	}

	bool Window::MouseDown(mouse::Button button)
	{
		return mouseDown_.at(button);
	}

	bool Window::MousePressed(mouse::Button button)
	{
		if(mousePressed_.at(button))
		{
			mousePressed_[button] = false;
			return true;
		}
		return false;
	}

	bool Window::GetFullscreen() const
	{
		return fullscreen_;
	}

	bool Window::GetMouseVisible() const
	{
		return isMouseVisible_;
	}

	int Window::GetWidth() const
	{
		return width_;
	}

	int Window::GetHeight() const
	{
		return height_;
	}

	bool Window::IsMultisampleSupported() const
	{
		return isMultisampleSupported_;
	}

	bool Window::Running()
	{
		return running_;
	}

	void Window::Quit()
	{
		running_ = false;
	}

	void Window::Continue()
	{
		running_ = true;
	}
}
