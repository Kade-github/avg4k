#include "Display.h"

int AvgEngine::Render::Display::height = 0;
int AvgEngine::Render::Display::width = 0;

std::vector<AvgEngine::Render::Vertex> AvgEngine::Render::Display::batch_buffer{};
GLuint AvgEngine::Render::Display::batch_vao{};
GLuint AvgEngine::Render::Display::batch_vbo{};