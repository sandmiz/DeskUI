# DeskUI

> &emsp;Well this is meant for people who want to develop their own UI frameworks in OpenGL, well at least for the beginners, this includes a lot of simple things that take a lot of work.

&emsp;This is my first try of making an UI framework, because I was rebellious and don't wanted to use GTK... well I failed, but I learned a valuable lesson out of this, don't use OpenGL for normal software. This is a library to render squares with textures, and add some sort of primitive interactivity. If you want to continue this project by yourself... I wish you good luck.

# OPENING
1. Clone or download the repository
2. Open Visual Studio 2022
3. Click in open a project and solution
4. Go to the cloned repo and select the Desk.sln file
5. You can now compile or edit

# FUNCTIONS
Use the main namespace `Desk` to get all the functions, here is an example code:

```cpp
#include <iostream>

#include "DeskUI\include\DeskUI\DeskUI.h"
#include "DeskUI\include\DeskUI\DeskWidgets.h"

void onClick(Desk::Window* window) {
    std::cout << "Hello" << std::endl;
}

int main() {
    Desk::Window window = Desk::Window(640, 480, "Hello World", true);
    Desk::Button widget = Desk::Button("wid", {0, -0.5, 0}, {1, 1}, onClick, "C:/Users/João/Documents/DeskUI/DeskUI/img/wheatley.png");
    Desk::Widget widget2 = Desk::Widget("wid2", { -1, -0.5, 0 }, {1, 1}, "C:/Users/João/Documents/DeskUI/DeskUI/img/wheatley.png");

    window += &widget;
    widget += &widget2;

    window.start();
}

```
