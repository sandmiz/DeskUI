#pragma once

#include "DeskUI.h"

namespace Desk {
	class Button : public Widget {
	private:
		GLFWmousebuttonfun callback;
		void initialize(Window* window) override {
			glfwSetMouseButtonCallback(getGLWindow(window), callback);
		}
	public:
		Button(const char* id, std::array<float, 3> posi, std::array<float, 2> siz, GLFWmousebuttonfun callback, const char* tex_path = "DeskUI/examples/wheatley.png") : callback(callback), Widget(id, posi, siz, tex_path) {
		}
	};
}