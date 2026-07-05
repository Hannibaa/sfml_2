// 0. Include imgui.h and imgui-SFML.h
#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../include/util-sfml.hpp"
#include "../include/grid.hpp"
#include "../include/sprite_division.hpp"

#define     EPS         0.01f


int main()
{
    constexpr float Width = 1200.f;
    constexpr float Hight = 800.f;
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(int(Width), int(Hight)), "SFML window");
    // 1. initializing imgui
    ImGui::SFML::Init(window);

    // files resource
    std::string         image_filename("resource\\tri.jpg");
    const float         Epsilon = 0.01f;

    const char*         text_true = "Object is closed";
    const char*         text_false = "Object not close";
    bool                bool_obj{ false };

    // Texture
    sf::Texture         image;
    if (!image.loadFromFile(image_filename)) {
        return EXIT_FAILURE;
    }

    // box of image
    float               box_img_width = 800.f;
    float               box_img_hight = 600.f;
    const float         gridX = 200.f;
    const float         gridY = 200.f;

    // reduse image size to 
    // calculate scale:
    float scaleX = box_img_width / (float)image.getSize().x;
    float scaleY = box_img_hight / (float)image.getSize().y;

    float scale = std::max(scaleX, scaleY);

    std::vector<sf::Sprite>     Sprites;
    sf::Sprite*                 ptr_Spr{nullptr};

    const float subdiv_x = 4;
    const float subdiv_y = 4;


    game::set_sprites_texture_number(Sprites, image, subdiv_x , subdiv_y , scale);

    //Sprites.resize(10);

    // 2. set clock for imgui
    sf::Clock           imclock;
    bool                game_close{ false };
    bool                show_grid{ true };
	sf::Vector2i        imouse;
    sf::Vector2f        fmouse;
	sf::Vector2i        gmouse;
    sf::Vector2f        dl;
    float               x{ 100.f};
    float               y{ 100.f};
    int                 index{};

    const float grid_width = Sprites[0].getGlobalBounds().width * subdiv_x;

    util::Grid                grid(sf::FloatRect(gridX, gridY, grid_width , box_img_hight), 4, 4);

	// Start the game loop
    while (window.isOpen())
    {
		// get mouse position
		imouse = sf::Mouse::getPosition(window);
		fmouse.x = (float)imouse.x;
		fmouse.y = (float)imouse.y;
		gmouse = sf::Mouse::getPosition();
        
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 3. Prossess event to imgui
            ImGui::SFML::ProcessEvent(event);
            // Close window: exit
            if (event.type == sf::Event::Closed || game_close)
                window.close();

            game::select_one_object(Sprites, event, ptr_Spr, fmouse, dl, index);
        }

        // 4. Update imgui
        ImGui::SFML::Update(window, imclock.restart());



        std::string  text;
        // Drag system
		if (ptr_Spr) {
			ptr_Spr->setPosition(fmouse.x - dl.x, fmouse.y - dl.y);
            bool_obj = game::object_is_close_to(Sprites, ptr_Spr, 10.f);
		}

		if (bool_obj) {
			text = text_true;
		}
		else text = text_false;

        // Principale Menu
        ImGui::Begin("Menu");
        ImGui::Text("Sprite size        : [ %d ]", Sprites.size());
        ImGui::Text("index Spr selected : [ %d ]", index);
        if (ImGui::Button("Close")) game_close = true;

        if (ImGui::Button("Random")) {
            game::random_position_sprites(Sprites, sf::FloatRect(gridX, gridY, 800.f, 700.f));
        }

        if (ImGui::Button("Order")) {
            game::order_position_sprites(Sprites, { gridX, gridY }, 4,4);
        }

        if (ImGui::Button("Random Grid")) {
            game::random_position_sprites_grid(Sprites, { gridX, gridY }, 4, 4);
        }

        ImGui::Text("Obj : %s ", text.c_str());
        ImGui::SliderFloat("X", &x, 0.f, Width);
        ImGui::SliderFloat("Y", &y, 0.f, Hight);

        ImGui::Checkbox("Show Grid", &show_grid);

        ImGui::End(); // End Menu



        // Clear screen
        window.clear();

        for (auto& s : Sprites) window.draw(s);

        if (show_grid)
        window.draw(grid);

        // Update the window
        ImGui::SFML::Render(window);
        window.display();
    }

    // 6. Shut down imgui
    ImGui::SFML::Shutdown();


    return EXIT_SUCCESS;
}
