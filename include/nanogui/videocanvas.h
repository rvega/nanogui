/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/videocanvas.h
 *
 * \brief Defines the \ref nanogui::VideoCanvas widget.
 */

#pragma once

#include <nanogui/glcanvas.h>

NAMESPACE_BEGIN(nanogui)
/**
 * \class VideoCanvas videocanvas.h nanogui/videocanvas.h
 *
 * \brief Video Canvas widget.
 */
class NANOGUI_EXPORT VideoCanvas: public GLCanvas {
public:
    /**
     * \brief Creates a video canvas attached to the specified parent.
     *
     * \param parent
     *     The \ref nanogui::Widget this VideoCanvas will be attached to.
     *
     * \param filename 
     *     The path to the video file that will be played.
     */
    VideoCanvas(Widget *parent, const std::string &filename);

protected:
    /// The gl shader used to render the video content
    nanogui::GLShader mTexShader;

    /// The id of the gl texture used to render the video content
    GLuint mTex;

    /// A mutex used internally to keep bad stuff from happening
    std::mutex mDataMutex;

    /// The pixel data loaded from the video at each frame
    char *mData = nullptr;

    /// Wether new data is ready to be drawn or not
    bool needUpdate;

    /// An instance of Libvlc used to decode video file
    libvlc_instance_t *vlc;

    /// The media player object from lib vlc
    libvlc_media_player_t *mediaPlayer;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
}
NAMESPACE_END(nanogui)
