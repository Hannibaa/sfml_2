// 0. Include imgui.h and imgui-SFML.h
#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>




int main()
{
    constexpr float Width = 1200.f;
    constexpr float Hight = 800.f;
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(int(Width), int(Hight)), "SFML window");
    // 1. initializing imgui
    ImGui::SFML::Init(window);

    sf::CircleShape     circle(30.f);

    std::string         image_filename("resource\\cute_image.jpg");
    std::string         font_filename("resource\\font1.ttf");

    // load font 
    sf::Font            font;
    font.loadFromFile(font_filename);

    // load texture
    sf::Texture         image;
    image.loadFromFile(image_filename);

    float img_width = (float)image.getSize().x;
    float img_hight = (float)image.getSize().y;

    // Creating sprite
    sf::Sprite          sprite;
    sprite.setTexture(image);
    sprite.setTextureRect(sf::IntRect(10, 10, 50, 50));
    sprite.setPosition(10.f, 50.f);

    float           spx{ 20.f };
    float           spy{ 20.f };
    float           sx{10.f};  // sprite position x in texture
    float           sy{10.f};  // sprite position y in texture
    float           sw{50.f};  // sprite width
    float           sh{50.f};  // sprite hight

    const float     sx_min{1.f};
    const float     sx_max{(float)img_width};
    const float     sy_max{(float)img_hight};
    const float     sy_min{1.f};

    // Create text
    sf::Text            text("ImGui+SFML", font, 45);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Green);
    text.setPosition(10.f,  50.f);

    // 2. set clock for imgui
    sf::Clock           imclock;
    bool                game_close{ false };

    float               cx{ 100.f };
    float               cy{ 200.f };
    float               r{50.f};

    constexpr float     R_MIN = 10.f;
    constexpr float     R_MAX = 300.f;
    int                 segment{ 30 };

    constexpr int       SEG_MIN = 2;
    constexpr int       SEG_MAX = 45;

    float cColor[3] = { 204.f / 255.f, 7.f / 255.f, 204.f / 255.f };


    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 3. Prossess event to imgui
            ImGui::SFML::ProcessEvent(event);
            // Close window: exit
            if (event.type == sf::Event::Closed || game_close)
                window.close();
        }

        // 4. Update imgui
        ImGui::SFML::Update(window,imclock.restart());

        ImGui::Begin("Circle Option");
        ImGui::Text("Circle of radius %.2f ", circle.getRadius());
        ImGui::SliderFloat("Radius", &r, R_MIN, R_MAX);
        ImGui::SliderInt("Segment", &segment, SEG_MIN, SEG_MAX);
        if (ImGui::Button("Close")) game_close = true;
        ImGui::ColorEdit3("Circle Color", cColor);

        ImGui::End();

        ImGui::Begin("Circle Position");
        const float         cx_min = r;
        const float         cx_max = Width - r;
        const float         cy_min = r;
        const float         cy_max = Hight - r;
        ImGui::SliderFloat("X", &cx, cx_min, cx_max);
        ImGui::SliderFloat("Y", &cy, cy_min, cy_max);
        ImGui::End(); // End Circle Position Menu

        ImGui::Begin("Sprite Positioning");
        float spx_max = Width - sw;
        float spy_max = Hight - sh;
        ImGui::SliderFloat("PosX", &spx, 0.f, spx_max);
        ImGui::SliderFloat("PosY", &spy, 0.f, spy_max);
        ImGui::SliderFloat("X", &sx, 1.f, Width/10.f);
        ImGui::SliderFloat("Y", &sy, 1.f, Hight/10.f);
        ImGui::SliderFloat("Width", &sw, 1.f, sx_max);
        ImGui::SliderFloat("Hight", &sh, 1.f, sy_min);

        ImGui::End();


        circle.setPointCount(segment);
        circle.setRadius(r);
        circle.setOrigin(r, r);
        circle.setPosition(cx, cy);
        circle.setFillColor(sf::Color(
            (int)(cColor[0] * 255),
            (int)(cColor[1] * 255),
            (int)(cColor[2] * 255)
        ));

        // sprite control
        sprite.setTextureRect(sf::IntRect(sx, sy, sw, sh));
        sprite.setPosition(spx,spy);


        // Clear screen
        window.clear();

        window.draw(sprite);

        window.draw(circle);
        window.draw(text);

        // Update the window
        ImGui::SFML::Render(window);
        window.display();
    }

    // 6. Shut down imgui
    ImGui::SFML::Shutdown();


    return EXIT_SUCCESS;
}
