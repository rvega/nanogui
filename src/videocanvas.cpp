#include <nanogui/videocanvas.h>
#include <nanogui/opengl.h>

#include <vlc/vlc.h>
#include <mutex>
#include <thread>

VideoCanvas::VideoCanvas(nanogui::Widget *parent, const srd::string& filename): 
  nanogui::GLCanvas(parent) {
      mData = new char[width()*height()*480*320*3];
      needUpdate = false;
      initGL();
      initVLC();
    }
    
    ~VideoCanvas() {
        delete[] mData;
        glDeleteTextures(1, &mTex);
    }

    void initVLC() {
      const char *argv[] = {
        "--no-xlib", 
        "--no-video-title-show"
      };
      int argc = sizeof(argv) / sizeof(*argv);
      vlc = libvlc_new(argc, argv);

      libvlc_media_t *m;
      m = libvlc_media_new_path(vlc, "/home/rvg/Projects/Explora/1.m4v"); 
      mediaPlayer = libvlc_media_player_new_from_media(m);
      libvlc_media_release(m);

      libvlc_video_set_callbacks(mediaPlayer, videoLockCallBack, videoUnlockCallback, NULL, this);
      libvlc_video_set_format(mediaPlayer, "RV24", 480, 320, 480*3);
      libvlc_media_player_play(mediaPlayer);
    }

    void initGL() {
        // create the OpenGL texture
        glGenTextures(1, &mTex);
        glBindTexture(GL_TEXTURE_2D, mTex);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // initialize the video feed shader
        // create shader that will use to display textures
        mTexShader.init(
            /* registered name */
            "video_shader",
            /* vertex shader */
            "#version 330\n"
            "// inputs: should just be the vertices and uv's for a rectangle\n"
            "in vec2 position;\n"
            "in vec2 texcoord;\n"
            "// outputs: interpolated texture coordinates for fragment shader\n"
            "out vec2 pass_texcoord;\n"
            "void main() {\n"
            "    pass_texcoord = texcoord;\n"
            "    gl_Position   = vec4(position.xy, 0.0f, 1.0f);\n"
            "}\n",
            /* fragment shader */
            "#version 330\n"
            "in vec2 pass_texcoord;// interpolated texture coordinate\n"
            "out vec4 outColor;    // output color sampled from texture\n"
            "uniform sampler2D passTex;// sampler to read the texture\n"
            "void main() {\n"
            "    outColor = texture(passTex, pass_texcoord);\n"
            "}\n"
        );

        nanogui::MatrixXu indices(3, 2);
        indices.col(0) << 0, 1, 2;
        indices.col(1) << 2, 3, 1;

        nanogui::MatrixXf positions(2, 4);
        positions.col(0) << -1.0f,  1.0f;
        positions.col(1) <<  1.0f,  1.0f;
        positions.col(2) << -1.0f, -1.0f;
        positions.col(3) <<  1.0f, -1.0f;

        nanogui::MatrixXf texcoords(2, 4);
        texcoords.col(0) << 0.0f, 0.0f;
        texcoords.col(1) << 1.0f, 0.0f;
        texcoords.col(2) << 0.0f, 1.0f;
        texcoords.col(3) << 1.0f, 1.0f;

        mTexShader.bind();
        mTexShader.uploadIndices(indices);
        mTexShader.uploadAttrib("position", positions);
        mTexShader.uploadAttrib("texcoord", texcoords);
    }

    void drawGL() override {
        mTexShader.bind();
        glActiveTexture(1);

        glUniform1i(mTexShader.uniform("passTex", true), 1 - GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTex);

        // upload the image data (only needed when new data comes in from newFrame)
        {
            std::lock_guard<std::mutex> data_lock(mDataMutex);
            if(needUpdate) {
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mData);
              needUpdate = false;
            }
        }

        mTexShader.drawIndexed(GL_TRIANGLES, 0, 2);
    }

    static void *videoLockCallBack(void *object, void **planes) {
      VideoCanvas* c = (VideoCanvas *)object;
      c->mDataMutex.lock();
      planes[0] = c->mData;
      return NULL;
    }

    static void videoUnlockCallback(void *object, void *picture, void * const *planes) {
      VideoCanvas* c = (VideoCanvas *)object;
      c->needUpdate = true;
      c->mDataMutex.unlock();
    }

};

class MyScreen : public nanogui::Screen {
public:
  MyScreen() : nanogui::Screen({600, 600}, "Texture Testing") {
    auto *window = new nanogui::Window(this, "Video Data");
    window->setLayout(new nanogui::GroupLayout());

    mVideoCanvas = new VideoCanvas(window, 480, 320);
    mVideoCanvas->setSize({480, 320});
  }

  ~MyScreen() { }

protected:
    VideoCanvas *mVideoCanvas = nullptr;
};

int main(int argc, const char **argv) {
    nanogui::init();
    auto *screen = new MyScreen();
    screen->performLayout();
    screen->setVisible(true);
    nanogui::mainloop();
    nanogui::shutdown();
    return 0;
}
