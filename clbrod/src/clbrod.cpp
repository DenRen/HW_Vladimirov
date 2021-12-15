#include "clbrod.hpp"

namespace clbrod {

ClAccelerator::ClAccelerator (const cl::Device& device,
                              const std::string& source,
                              cl::QueueProperties cmd_queue_prop) :
    device_ (device),
    context_ (device),
    cmd_queue_ (context_, cmd_queue_prop),
    program_ (hidra::buildProgram (context_, source))
{}

FractalDrawer::FractalDrawer (const cl::Device& device,
                              sf::Vector2u size) :
    ClAccelerator (device, "kernels/fractal.cl"),
    kernelMandelbrod  (program_, "drawMandelbrod"),
    kernelMandelbrod3 (program_, "drawMandelbrod3"),
    verts_ (size.x * size.y),
    size_ (size)
{}

void FractalDrawer::drawMandelbrod (sf::RenderTarget& target,
                                    const sf::Vector3f& pos,
                                    const sf::Vector2f& C,
                                    int count_iters)
{
    cl::Buffer bufferVertex (context_, CL_MEM_READ_WRITE, verts_.size () * sizeof (verts_[0]));
    cl::NDRange global (size_.x, size_.y);
    cl::EnqueueArgs args {cmd_queue_, global};

    const float scale_factor = pos.z / size_.y;
    const float shift_x = pos.x - scale_factor * (size_.x / 2);
    const float shift_y = pos.y - scale_factor * (size_.y / 2);

    kernelMandelbrod (args, bufferVertex, scale_factor, shift_x, shift_y, C, count_iters);

    cl::copy (cmd_queue_, bufferVertex, verts_.data (), verts_.data () + verts_.size ());

    target.draw (verts_.data (), verts_.size (), sf::PrimitiveType::Points);
}

void FractalDrawer::drawMandelbrod3 (sf::RenderTarget& target,
                                     const sf::Vector3f& pos,
                                     int count_iters)
{
    cl::Buffer bufferVertex (context_, CL_MEM_READ_WRITE, verts_.size () * sizeof (verts_[0]));
    cl::NDRange global (size_.x, size_.y);
    cl::EnqueueArgs args {cmd_queue_, global};

    const float scale_factor = pos.z / size_.y;
    const float shift_x = pos.x - scale_factor * (size_.x / 2);
    const float shift_y = pos.y - scale_factor * (size_.y / 2);

    kernelMandelbrod3 (args, bufferVertex, scale_factor, shift_x, shift_y, count_iters);

    cl::copy (cmd_queue_, bufferVertex, verts_.data (), verts_.data () + verts_.size ());

    target.draw (verts_.data (), verts_.size (), sf::PrimitiveType::Points);
}

} // namespace clbrod