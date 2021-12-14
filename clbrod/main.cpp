#include <iostream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

template <typename T>
bool belongsMandelbot (T x_0, T y_0, std::size_t count_iters = 100) {
    T x = x_0, y = y_0;

    while (count_iters--) {
        T tmp_x = x * x - y * y + x_0;
        T tmp_y = 2 * x * y + y_0;

        x = tmp_x;
        y = tmp_y;
    }

    return (x * x + y * y) < 4;
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

void drawFractal (sf::RenderTarget& target,
                  const sf::Vector3f& pos) {
    sf::Vector2u size = target.getSize ();

    std::vector <sf::Vertex> verts (size.x * size.y);

    const float gamma = 1.0;
    const float kx = gamma * pos.z;
    
    const sf::Vector2f scale_factor (1.0 / size.y, 1.0 / size.y);
    const float height = pos.z;

    auto ivertex = verts.begin ();
    for (unsigned ix = 0; ix < size.x; ++ix) {
        for (unsigned iy = 0; iy < size.y; ++iy) {
            sf::Vector2f ipos (ix, iy);
            sf::Vertex vertex (ipos);

            ipos *= scale_factor;

            float x = pos.z * (scale_factor.x * ix - 0.5) + pos.x;
            float y = pos.z * (scale_factor.y * iy - 0.5) + pos.y;

            if (belongsMandelbot <float> (x, y)) {
                vertex.color = sf::Color::Yellow;
            } else {
                vertex.color = sf::Color::Black;
            }

            *ivertex++ = vertex;
        }
    }

    target.draw (verts.data (), size.x * size.y, sf::PrimitiveType::Points);
}

void printPosition (const sf::Vector3f& pos) {
    std::cout << "pos x:" << pos.x << ", y: " << pos.y << ", z: " << pos.z << std::endl;
}

int main () {
    sf::RenderWindow window (sf::VideoMode (1280, 720), "Fractal");

    sf::RenderTexture target;
    if (!target.create (window.getSize ().x, window.getSize ().y)) {
        std::cout << "Error to create target" << std::endl;
        return 0;
    }

    sf::Vector3f pos (0, 0, 1);

    sf::Clock timer;
    timer.restart ();

    sf::Vertex grid_x[] = {
        sf::Vertex (sf::Vector2f (window.getSize ().x / 2, 0)),
        sf::Vertex (sf::Vector2f (window.getSize ().x / 2, window.getSize ().y))
    };

    sf::Vertex grid_y[] = {
        sf::Vertex (sf::Vector2f (0, window.getSize ().y / 2)),
        sf::Vertex (sf::Vector2f (window.getSize ().x, window.getSize ().y / 2))
    };

    sf::Event event;
    while (window.isOpen ()) {
        while (window.pollEvent (event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close ();
                    break;
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
                            pos.z /= 1.2;
                            printPosition (pos);
                            break;
                        
                    }
            } 
        }
        drawFractal (target, pos);

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