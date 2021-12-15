#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "cppl.hpp"

namespace clbrod {

class ClAccelerator {
protected:
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;
    cl::Program program_;

public:
    ClAccelerator (const cl::Device& device,
                   const std::string& source,
                   cl::QueueProperties cmd_queue_prop = cl::QueueProperties::None);
};

class FractalDrawer : protected ClAccelerator {
    using ClAccelerator::device_;
    using ClAccelerator::context_;
    using ClAccelerator::cmd_queue_;
    using ClAccelerator::program_;
    
    cl::KernelFunctor <cl::Buffer, float, float, float, sf::Vector2f, int>
        kernelMandelbrod;
    cl::KernelFunctor <cl::Buffer, float, float, float, int>
        kernelMandelbrod3;

    std::vector <sf::Vertex> verts_;
    sf::Vector2u size_;

public:

    FractalDrawer (const cl::Device& device,
                   sf::Vector2u size);
    void drawMandelbrod (sf::RenderTarget& target,
                         const sf::Vector3f& pos,
                         const sf::Vector2f& C,
                         int count_iters = 1000);
    void drawMandelbrod3 (sf::RenderTarget& target,
                          const sf::Vector3f& pos,
                          int count_iters = 100);
};

} // namespace clbrod