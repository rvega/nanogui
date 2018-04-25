/*
    src/example5.cpp -- C++ version of an example application that shows
    how to use the VideoView widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/nanogui.h>
#include <iostream>
#include <string>


class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(800, 600), "NanoGUI Test", false) {
        using namespace nanogui;

        Window *window = new Window(this, "Video Demo");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());

        mVideo = new VideoView(window, "./videos/cat.m4v");
        mVideo->setBackgroundColor({100, 100, 100, 255});
        mCanvas->setSize({320, 213});

        performLayout();
    }

    /* virtual void draw(NVGcontext *ctx) { */
    /*     /1* Draw the user interface *1/ */
    /*     Screen::draw(ctx); */
    /* } */
private:
    VideoView* mVideo;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<ExampleApplication> app = new ExampleApplication();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
