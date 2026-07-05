/*
				SPRITE DIVISION GAME
				


*/
#pragma once



namespace game {


    void set_sprites_texture(std::vector<sf::Sprite>& spr, sf::Texture& tex, sf::Vector2u sprSize)
    {
        spr.clear();

        auto imgSize = tex.getSize();
        // calculate n and m 
        auto n = imgSize.x / sprSize.x;
        auto m = imgSize.y / sprSize.y;

        // n and m should be superior or equal to 2
        if (n < 2) {
            n = 2;
            sprSize = imgSize / n;
        }

        if (m < 2) {
            m = 2;
            sprSize = imgSize / m;
        }

        util::fRG<float>        frand;
        // set size of vector
        spr.resize(n * m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                spr[i + j * (n - 1)].setTexture(tex);
                spr[i + j * (n - 1)].setTextureRect(
                    sf::IntRect(
                        i * sprSize.x,
                        j * sprSize.y,
                        sprSize.x,
                        sprSize.y
                    ));

                spr[i + j * (n - 1)].setPosition(
                    { frand(0.f, 1200.f - sprSize.x), frand(0.f, 800.f - sprSize.y) });
            }
        }
    }

    void set_sprites_texture_number(
        std::vector<sf::Sprite>& spr,
        sf::Texture& tex,
        int column,
        int line,
        float scale = 1.f
    )
    {
        util::fRG<float>        frand;
        // set size of vector
        spr.resize(column * line);

        sf::Vector2u sprSize(tex.getSize().x / column, tex.getSize().y / line);

        for (int j = 0; j < line; ++j) {
            for (int i = 0; i < column; ++i) {
                int N = i + j * (column);
                spr[N].setTexture(tex);
                spr[N].setTextureRect(
                    sf::IntRect(
                        i * sprSize.x,
                        j * sprSize.y,
                        sprSize.x,
                        sprSize.y
                    ));

                spr[N].setPosition(
                    { frand(0.f, 1200.f - sprSize.x), frand(0.f, 800.f - sprSize.y) });

                spr[N].setScale(scale, scale);
            }
        }
    }

    void order_position_sprites(std::vector<sf::Sprite>& Sprites, const fvec2& v0, int n, int m)
    {
        if (Sprites.size() != n * m) return;

        const auto sprWidth = Sprites[0].getGlobalBounds().width;
        const auto sprHight = Sprites[0].getGlobalBounds().height;

        for (int j = 0; j < m; ++j)
            for (int i = 0; i < n; ++i)
            {
                Sprites[i + j * n].setPosition(
                    v0 + fvec2(i * sprWidth, j * sprHight)
                );
            }
    }

    void random_position_sprites_grid(
        VSprites& Sprites,
        const fvec2& v0,
        int n, int m
    )
    {
        if (Sprites.size() != n * m) return;

        const auto sprWidth = Sprites[0].getGlobalBounds().width;
        const auto sprHight = Sprites[0].getGlobalBounds().height;

        auto iv = util::make_random_map(n);
        auto jv = util::make_random_map(m);

        for (int j = 0; j < m; ++j)
            for (int i = 0; i < n; ++i)
            {
                Sprites[i + j * n].setPosition(
                    v0 + fvec2(iv[i] * sprWidth, jv[j] * sprHight)
                );
            }
    }

    void random_position_sprites(std::vector<sf::Sprite>& Sprites, const sf::FloatRect& rect) {

        util::fRG<float>            frand;

        const float                 sprX = Sprites[0].getGlobalBounds().width;
        const float                 sprY = Sprites[0].getGlobalBounds().height;

        const auto                  a = rect.getPosition();
        const auto                  d = rect.getSize();

        for (auto& s : Sprites) {
            s.setPosition(
                frand(a.x, a.x + d.x - sprX),
                frand(a.y, a.y + d.y - sprY));

        }
    }

    bool object_is_close_to(const sf::Sprite& a, const sf::Sprite& b, float eps) {
        auto A = util::Quadra(a.getGlobalBounds());
        auto B = util::Quadra(b.getGlobalBounds());

        if (util::is_vector_close(A[3], B[0], eps)) return true;

        if (util::is_vector_close(A[0], B[1], eps)) return true;

        if (util::is_vector_close(B[3], A[0], eps)) return true;

        if (util::is_vector_close(B[0], A[1], eps)) return true;
        return false;
    }

    bool magnet_stick(const sf::Sprite& a, sf::Sprite& b, float eps) {
        auto A = util::Quadra(a.getGlobalBounds());
        auto B = util::Quadra(b.getGlobalBounds());

        if (util::is_vector_close(A[3], B[0], eps)) {
            b.setPosition(A[3]); return true;
        }

        if (util::is_vector_close(A[1], B[0], eps)) {
            b.setPosition(A[1]); return true;
        }

        if (util::is_vector_close(B[3], A[0], eps)) {
            b.setPosition(A[0] + B[0] - B[3]); return true;
        }

        if (util::is_vector_close(B[1], A[0], eps)) {
            b.setPosition(A[0] + B[0] - B[1]); return true;
        }
    }

    bool object_is_close_to(const std::vector<sf::Sprite>& Sprite, sf::Sprite* spr, float eps)
    {
        for (auto& s : Sprite) {
            if (spr != &s)
            {
                if (magnet_stick(s, *spr, eps)) return true;
            }
        }

        return false;
    }

    void select_one_object(
        std::vector<sf::Sprite>& Sprites,
        sf::Event& event,
        sf::Sprite*& ptr_Spr,
        const sf::Vector2f& fmouse,
        sf::Vector2f& dl,
        int& index
    ) {
        // select object by mouse
        int i{};
        if (event.type == sf::Event::MouseButtonPressed) {

            if (event.key.code == sf::Mouse::Left) {

                if (ptr_Spr == nullptr) {
                    for (auto& s : Sprites) {
                        if (s.getGlobalBounds().contains(fmouse)) {
                            ptr_Spr = &s;
                            dl = fmouse - s.getPosition();
                            index = i;
                            break;
                        }

                        ++i;
                    }
                }

            }

        }

        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.key.code == sf::Mouse::Left) {
                ptr_Spr = nullptr;
            }
        }

    }



}