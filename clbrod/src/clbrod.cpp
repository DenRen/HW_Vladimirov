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
    verts_ (size.x * size.y),
    size_ (size)
{
    kernels.emplace_back ("Mandelbrod",  program_, "drawMandelbrod");
    kernels.emplace_back ("Mandelbrod3", program_, "drawMandelbrod3");
    kernels.emplace_back ("Julia",  program_, "drawJulia");
    kernels.emplace_back ("Julia3", program_, "drawJulia3");
}

void FractalDrawer::draw (sf::RenderTarget& target,
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

    kernels[fractalNumber_] (args, bufferVertex, scale_factor, shift_x, shift_y, C, count_iters);

    cl::copy (cmd_queue_, bufferVertex, verts_.data (), verts_.data () + verts_.size ());

    target.draw (verts_.data (), verts_.size (), sf::PrimitiveType::Points);
}

void FractalDrawer::processFractalNumber () {
    fractalNumber_ %= kernels.size ();
}
void FractalDrawer::nextFractal () {
    ++fractalNumber_;
    processFractalNumber ();
}
void FractalDrawer::previousFractal () {
    --fractalNumber_;
    processFractalNumber ();
}
void FractalDrawer::setFractalNumber (unsigned fractalNumber) {
    fractalNumber_ = fractalNumber;
    processFractalNumber ();
}

static bool
processEvent (sf::Window& window,
              sf::Vector3f& pos,
              sf::Vector2f& C,
              clbrod::FractalDrawer& fractalDrawer)
{
    bool change = false;
    sf::Event event;
    while (window.pollEvent (event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close ();
                return true;

            case sf::Event::KeyPressed:
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close ();
                        return true;

                    case sf::Keyboard::Right:
                        pos.x += 0.1 * pos.z;
                        change = true;
                        break;
                    case sf::Keyboard::Left:
                        pos.x -= 0.1 * pos.z;
                        change = true;
                        break;

                    case sf::Keyboard::Up:
                        pos.y += 0.1 * pos.z;
                        change = true;
                        break;
                    case sf::Keyboard::Down:
                        pos.y -= 0.1 * pos.z;
                        change = true;
                        break;

                    case sf::Keyboard::Space:
                        pos.z *= 1.2;
                        break;
                    case sf::Keyboard::LShift:
                    case sf::Keyboard::X:
                        pos.z /= 1.2;
                        change = true;
                        break;

                    case sf::Keyboard::D:
                        C.x += 0.02 * pos.z;
                        change = true;
                        break;
                    case sf::Keyboard::A:
                        C.x -= 0.02 * pos.z;
                        change = true;
                        break;

                    case sf::Keyboard::W:
                        C.y += 0.02 * pos.z;
                        change = true;
                        break;
                    case sf::Keyboard::S:
                        C.y -= 0.02 * pos.z;
                        change = true;
                        break;

                    case sf::Keyboard::E:
                        fractalDrawer.nextFractal ();
                        change = true;
                        break;
                    case sf::Keyboard::Q:
                        fractalDrawer.previousFractal ();
                        change = true;
                        break;
                    
                    case sf::Keyboard::R:
                        C.x = 0;
                        C.y = 0;
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }

    return change;
} // bool processEvent (sf::Window& window, sf::Vector3f& pos, 
  //                    sf::Vector2f& C, int& fractalNumber)

int drawFractal (QUALITY quality) {
    hidra::DeviceProvider deviceProvider;
    auto device = deviceProvider.getDefaultDevice ();

    sf::RenderWindow window (sf::VideoMode ((int) quality * 16 / 9, (int) quality),"", sf::Style::Default);

    clbrod::FractalDrawer fractalDrawer (device, window.getSize ());

    sf::RenderTexture target;
    if (!target.create (window.getSize ().x, window.getSize ().y)) {
        std::cout << "Error to create target" << std::endl;
        return 0;
    }
    std::vector <sf::Vertex> verts (target.getSize ().x * target.getSize ().y);

    sf::Vector3f pos (0, 0, 1);
    sf::Vector2f C (0, 0);
    int fractalNumber = 0;

    sf::Clock timer;
    timer.restart ();

    sf::Color colorLine (255, 255, 255, 100);
    sf::Vertex grid_x[] = {
        sf::Vertex (sf::Vector2f (window.getSize ().x / 2, 0), colorLine),
        sf::Vertex (sf::Vector2f (window.getSize ().x / 2, window.getSize ().y), colorLine)
    };

    sf::Vertex grid_y[] = {
        sf::Vertex (sf::Vector2f (0, window.getSize ().y / 2), colorLine),
        sf::Vertex (sf::Vector2f (window.getSize ().x, window.getSize ().y / 2), colorLine)
    };

    do {
        timer.restart ();

        fractalDrawer.draw (target, pos, C);

        window.clear ();
        window.draw (sf::Sprite (target.getTexture ()));
        window.draw (grid_x, 2, sf::PrimitiveType::Lines);
        window.draw (grid_y, 2, sf::PrimitiveType::Lines);
        window.display ();

        float fps = 1000.0 / timer.getElapsedTime ().asMilliseconds ();
        window.setTitle ("FPS: " + std::to_string (fps));

        if (processEvent (window, pos, C, fractalDrawer) == false)
            continue;
    } while (window.isOpen ());
}

} // namespace clbrod