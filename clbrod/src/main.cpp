#include <iostream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "clbrod.hpp"

template <typename T>
T belongsMandelbot (T x_0, T y_0, std::size_t count_iters = 100) {
    T x = x_0, y = y_0;

    while (count_iters--) {
        T tmp_x = x * x - y * y + x_0;
        T tmp_y = 2 * x * y + y_0;

        x = tmp_x;
        y = tmp_y;

        if ((x * x + y * y) > 4) {
            return count_iters;
        }
    }

    // return x * x + y * y;

    return 0;
}

sf::Vector2f operator *= (sf::Vector2f& lhs, const sf::Vector2f& rhs) {
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;

    return lhs;
}

sf::Vector2f operator * (const sf::Vector2f& lhs, const sf::Vector2f& rhs) {
    sf::Vector2f res (lhs);
    res.x *= rhs.x;
    res.y *= rhs.y;

    return res;
}

template <typename T>
void drawFractal (sf::RenderTarget& target,
                  std::vector <sf::Vertex>& verts,
                  const sf::Vector3f& pos) {
    const sf::Vector2u size = target.getSize ();
    const sf::Vector2i half_size (size.x / 2, size.y / 2);

    const T scale_factor = 1.0 / size.y;

    auto ivertex = verts.begin ();
    for (int ix = 0; ix < size.x; ++ix) {
        for (int iy = 0; iy < size.y; ++iy) {
            sf::Vertex vertex (sf::Vector2f (ix, iy));

            T x = (scale_factor * (ix - half_size.x)) * pos.z + pos.x;
            T y = (scale_factor * (iy - half_size.y)) * pos.z + pos.y;

            T res = belongsMandelbot <T> (x, y, 100);
            res *= 255;
            T r = 255*0.0 + res * 0.2;
            T g = 255*0.0 + res * 0.2;
            T b = 255*0.0 + res * 0.9;

            vertex.color = sf::Color (r, g, b, 255);

            *ivertex++ = vertex;
        }
    }

    target.draw (verts.data (), size.x * size.y, sf::PrimitiveType::Points);
}

void printPosition (const sf::Vector3f& pos) {
    std::cout << "pos x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
}

int drawFractal ();

int main () {
    try {
        drawFractal ();
    } catch (cl::Error& exc) {
        std::cout << exc.what () << ", error code: " << exc.err ()
        << " => " << hidra::getErrorString (exc.err ()) << std::endl;
        throw;
    }
}

int drawFractal () {
    hidra::DeviceProvider deviceProvider;
    auto device = deviceProvider.getDefaultDevice ();

    clbrod::FractalDrawer drawer (device, sf::Vector2u (1280, 720));

    sf::RenderWindow window (sf::VideoMode (1280, 720), "Fractal");

    sf::RenderTexture target;
    if (!target.create (window.getSize ().x, window.getSize ().y)) {
        std::cout << "Error to create target" << std::endl;
        return 0;
    }
    std::vector <sf::Vertex> verts (target.getSize ().x * target.getSize ().y);

    sf::Vector3f pos (0, 0, 1);

    sf::Clock timer;
    timer.restart ();

    sf::Vertex grid_x[] = {
        sf::Vertex (sf::Vector2f (window.getSize ().x / 2, 0), sf::Color (255, 255, 255, 20)),
        sf::Vertex (sf::Vector2f (window.getSize ().x / 2, window.getSize ().y), sf::Color (255, 255, 255, 20))
    };

    sf::Vertex grid_y[] = {
        sf::Vertex (sf::Vector2f (0, window.getSize ().y / 2), sf::Color (255, 255, 255, 20)),
        sf::Vertex (sf::Vector2f (window.getSize ().x, window.getSize ().y / 2), sf::Color (255, 255, 255, 20))
    };

    sf::Event event;
    while (window.isOpen ()) {
        while (window.pollEvent (event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close ();
                    return 0;

                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            window.close ();
                            break;

                        case sf::Keyboard::Right:
                            pos.x += 0.1 * pos.z;
                            printPosition (pos);
                            break;
                        case sf::Keyboard::Left:
                            pos.x -= 0.1 * pos.z;
                            printPosition (pos);
                            break;

                        case sf::Keyboard::Up:
                            pos.y += 0.1 * pos.z;
                            printPosition (pos);
                            break;
                        case sf::Keyboard::Down:
                            pos.y -= 0.1 * pos.z;
                            printPosition (pos);
                            break;

                        case sf::Keyboard::Space:
                            pos.z *= 1.2;
                            printPosition (pos);
                            break;
                        case sf::Keyboard::LShift:
                        case sf::Keyboard::X:
                            pos.z /= 1.2;
                            printPosition (pos);
                            break;

                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        drawer.draw (target, pos);
        // drawFractal <double> (target, verts, pos);

        window.clear ();
        window.draw (sf::Sprite (target.getTexture ()));
        window.draw (grid_x, 2, sf::PrimitiveType::Lines);
        window.draw (grid_y, 2, sf::PrimitiveType::Lines);
        window.display ();

        float fps = 1000.0 / timer.getElapsedTime ().asMilliseconds ();
        timer.restart ();
        window.setTitle ("FPS: " + std::to_string (fps));
    }
}