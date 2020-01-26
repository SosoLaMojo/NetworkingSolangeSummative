#pragma once
/*
 MIT License

 Copyright (c) 2019 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include <comp_graph/sample_program.h>
#include "graphics/graphics.h"
#include "gl/shader.h"
#include "gl/shape.h"

namespace neko
{
class HelloTriangleProgram : public SampleProgram
{
public:
    enum class RenderType
    {
        VaoProgram,
        EboProgram,
        NekoQuad,
        Length
    };

    void Init() override;

    void Update(seconds dt) override;

    void Destroy() override;

    void Render() override;

    void DrawUi(seconds dt) override;

    void OnEvent(const SDL_Event& event) override;

private:
    RenderType renderType_ = RenderType::VaoProgram;
    struct VaoProgram
    {
        float vertices[18] = {
                0.5f, 0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, 0.5f, 0.0f,   // top left
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f, 0.5f, 0.0f   // top left
        };
        float colors[18] = {
                1.0f, 0.0f, 1.0f,// top right
                0.0f, 1.0f, 0.0f, // bottom right
                0.5f, 0.0f, 0.5f,// top left
                0.0f, 1.0f, 0.0f, // bottom right
                0.0f, 0.0f, 1.0f,// bottom left
                0.5f, 0.0f, 0.5f,// top left
        };

        unsigned int VBO[2] = {}; //Vertex Buffer Object
        unsigned int VAO = 0; //Vertex Array Object
    };
    VaoProgram vaoProgam_;
    struct EboProgram
    {
        float vertices[12] = {
                0.5f, 0.5f, 0.0f,  // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f, 0.5f, 0.0f   // top left
        };
        float colors[12] = {
                1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
                0.5f, 0.0f, 0.5f
        };
        unsigned int indices[6] = {
                // note that we start from 0!
                0, 1, 3,   // first triangle
                1, 2, 3    // second triangle
        };

        unsigned int EBO = 0; // Element Buffer Object
        unsigned int VBO[2] = {}; //Vertex Buffer Object
        unsigned int VAO = 0; //Vertex Array Object
    };
    EboProgram eboProgram_;

    neko::gl::Quad quad_{Vec2f::Zero, Vec2f::One};

    seconds timeSinceInit_ = seconds(0.0f);
    gl::Shader shader_;
    gl::Shader quadShader_;
};
}
