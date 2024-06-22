#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <iostream>
#include <vector>
#include <cmath>

const int WIDTH = 1200;
const int HEIGHT = 900;
const float FPS = 60.0;
const int PLAYER_SIZE = 32;
const int BULLET_SIZE = 8;
const int PLAYER_HEALTH = 4;
const int ENEMY_HEALTH = 4;
const float GRAVITY = 0.05;

enum BulletType { NORMAL, SPREAD, ENEMYNORMAL };
enum GameMode { NORMAL_MODE, GRAVITY_MODE };

class Obstacle {
public:
    float x, y, width, height;

    Obstacle(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

    void draw() const {
        al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(128, 128, 128));
    }

    bool is_collidingx(float ox, float size) const {
        return !(ox + size < x || ox - size > x + width );
    }

    bool is_collidingy(float oy, float size) const{
        return !(oy + size < y || oy - size > y + height );
    }
};



class Bull {
public:
    float x, y, dx, dy;
    BulletType type;

    Bull(float x, float y, float dx, float dy, BulletType type) : x(x), y(y), dx(dx), dy(dy), type(type) {}

    void update() {
        x += dx;
        y += dy;
    }

    void draw() const {
        if (type == NORMAL) {
            al_draw_filled_circle(x, y, BULLET_SIZE, al_map_rgb(0, 0, 255));
        } else if (type == SPREAD) {
            al_draw_filled_circle(x, y, BULLET_SIZE, al_map_rgb(0, 255, 0));
        } else if (type == ENEMYNORMAL) {
            al_draw_filled_circle(x, y, BULLET_SIZE, al_map_rgb(255, 0, 0));
        }
    }

    bool operator==(const Bull &other) const {
        return x == other.x && y == other.y && dx == other.dx && dy == other.dy && type == other.type;
    }
};

class Enem {
public:
    float x, y;
    float dx, dy; // 速度
    int health;
    std::vector<Bull> bullets;

    Enem(float x, float y) : x(x), y(y), health(ENEMY_HEALTH) {
        dx = (rand() % 3 - 1) * 2;
        dy = (rand() % 3 - 1) * 2;
    }

    void update(const std::vector<Obstacle>& obstacles) {
        if (rand() % 100 == 0) {
            dx = (rand() % 3 - 1) * 2;
            dy = (rand() % 3 - 1) * 2;
        }

        x += dx;
        y += dy;

        // 檢查與障礙物的碰撞
        for (const auto& obs : obstacles) {
            if (obs.is_collidingx(x,  PLAYER_SIZE / 2)&&obs.is_collidingy(y,PLAYER_SIZE/2)) {
                x -= dx;
                y -= dy;
                dx = -dx;
                dy = -dy;
                break;
            }
        }

        if (x < 0 || x > WIDTH) dx = -dx;
        if (y < 0 || y > HEIGHT) dy = -dy;

        if (rand() % 50 == 0) {
            float angle = (rand() % 360) * ALLEGRO_PI / 180.0;
            bullets.push_back(Bull(x, y, cos(angle) * 2, sin(angle) * 2, ENEMYNORMAL));
        }

        for (auto &bullet : bullets) {
            bullet.update();
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](Bull &bullet) {
            for (const auto &obs : obstacles) {
                if (obs.is_collidingx(bullet.x, BULLET_SIZE / 2)&&obs.is_collidingy(bullet.y,BULLET_SIZE/2)) {
                    return true;
                }
            }
            return bullet.x < 0 || bullet.x > WIDTH || bullet.y < 0 || bullet.y > HEIGHT;d
        }), bullets.end());
    }

    void draw() const {
        int red = 255 * health / ENEMY_HEALTH;
        al_draw_filled_rectangle(x - PLAYER_SIZE / 2, y - PLAYER_SIZE / 2, x + PLAYER_SIZE / 2, y + PLAYER_SIZE / 2, al_map_rgb(red, 0, 0));
        for (const auto &bullet : bullets) {
            bullet.draw();
        }
    }

    bool operator==(const Enem &other) const {
        return x == other.x && y == other.y && health == other.health;
    }
};

class Player {
public:
    float x, y, dy;
    int health;
    std::vector<Bull> bullets;
    BulletType currentBulletType;
    bool mousePressed;
    int score;
    GameMode mode;

    Player(GameMode mode) : x(WIDTH / 2), y(HEIGHT - 50), dy(0), health(PLAYER_HEALTH), currentBulletType(NORMAL), mousePressed(false), score(0), mode(mode) {}

    void update(ALLEGRO_KEYBOARD_STATE &keyState, ALLEGRO_MOUSE_STATE &mouseState, const std::vector<Obstacle> &obstacles) {
        float old_x = x, old_y = y;
        if (al_key_down(&keyState, ALLEGRO_KEY_W)) y -= 4;
        if (al_key_down(&keyState, ALLEGRO_KEY_S)) y += 4;
        if (al_key_down(&keyState, ALLEGRO_KEY_A)) x -= 4;
        if (al_key_down(&keyState, ALLEGRO_KEY_D)) x += 4;

        for (const auto &obs : obstacles) {
            if(obs.is_collidingx(x,PLAYER_SIZE/2)&&obs.is_collidingy(y,PLAYER_SIZE/2))
            {
                if(obs.is_collidingx(old_x,PLAYER_SIZE/2)&&obs.is_collidingy(y,PLAYER_SIZE/2)){
                    y=old_y;
                }
                if(obs.is_collidingx(x,PLAYER_SIZE/2)&&obs.is_collidingy(old_y,PLAYER_SIZE/2)){
                    x=old_x;
                }
                break;
            }
        }

        if (al_key_down(&keyState, ALLEGRO_KEY_1)) currentBulletType = NORMAL;
        if (al_key_down(&keyState, ALLEGRO_KEY_2)) currentBulletType = SPREAD;

        if (mouseState.buttons & 1) {
            if (!mousePressed) {
                mousePressed = true;
                float dx = mouseState.x - x;
                float dy = mouseState.y - y;
                float length = std::sqrt(dx * dx + dy * dy);
                dx /= length;
                dy /= length;

                if (currentBulletType == NORMAL) {
                    bullets.push_back(Bull(x, y, dx * 4, dy * 4, NORMAL));
                } else if (currentBulletType == SPREAD) {
                    bullets.push_back(Bull(x, y, dx * 4, dy * 4, SPREAD));
                    bullets.push_back(Bull(x, y, dx * 4 + 0.5, dy * 4, SPREAD));
                    bullets.push_back(Bull(x, y, dx * 4 - 0.5, dy * 4, SPREAD));
                }
            }
        } else {
            mousePressed = false;
        }

        for (auto &bullet : bullets) {
            bullet.update();
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](Bull &bullet) {
            for (const auto &obs : obstacles) {
                if (obs.is_collidingx(bullet.x, BULLET_SIZE / 2)&&obs.is_collidingy(bullet.y,BULLET_SIZE/2)) {
                    return true;
                }
            }
            return bullet.x < 0 || bullet.x > WIDTH || bullet.y < 0 || bullet.y > HEIGHT;
        }), bullets.end());

        if (mode == GRAVITY_MODE) {
            dy += GRAVITY;
            y += dy;
            if (y > HEIGHT - PLAYER_SIZE / 2) {
                y = HEIGHT - PLAYER_SIZE / 2;
                dy = 0;
            }
        }
    }

    void draw() const {
        al_draw_filled_rectangle(x - PLAYER_SIZE / 2, y - PLAYER_SIZE / 2, x + PLAYER_SIZE / 2, y + PLAYER_SIZE / 2, al_map_rgb(0, 0, 255));
        for (const auto &bullet : bullets) {
            bullet.draw();
        }
        draw_health();
        draw_score();
    }

    void draw_health() const {
        for (int i = 0; i < health; ++i) {
            al_draw_filled_rectangle(10 + i * 20, 10, 30 + i * 20, 30, al_map_rgb(255, 0, 0));
        }
    }

    void draw_score() const {
        ALLEGRO_FONT *font = al_load_ttf_font("../Resource/fonts/pirulen.ttf", 32, 0);
        if (!font) {
            std::cerr << "Failed to load font for score display!" << std::endl;
            return;
        }
        al_draw_textf(font, al_map_rgb(255, 255, 255), WIDTH - 100, 10, ALLEGRO_ALIGN_CENTER, "Score: %d", score);
        al_destroy_font(font);
    }

    void increase_score(int amount) {
        score += amount;
    }
};

bool check_collision(float x1, float y1, float x2, float y2, float size1, float size2) {
    float distance = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
    return distance < size1 + size2;
}

void add_enemies(std::vector<Enem> &enemies, int level, const std::vector<Obstacle>& obstacles) {
    for (int i = 0; i < level; ++i) {
        Enem enemy(rand() % WIDTH, rand() % HEIGHT);
        // 確保敵人不會生成在障礙物內
        bool colliding;
        do {
            colliding = false;
            for (const auto& obs : obstacles) {
                if (obs.is_collidingx(enemy.x, PLAYER_SIZE / 2)&&obs.is_collidingy(enemy.y,PLAYER_SIZE/2)) {
                    enemy.x = rand() % WIDTH;
                    enemy.y = rand() % HEIGHT;
                    colliding = true;
                    break;
                }
            }
        } while (colliding);
        enemies.push_back(enemy);
    }
}

void add_obstacles(std::vector<Obstacle> &obstacles) {
    obstacles.push_back(Obstacle(200, 200, 100, 300));
    obstacles.push_back(Obstacle(600, 500, 200, 50));
    obstacles.push_back(Obstacle(900, 300, 100, 200));
}

GameMode select_mode(ALLEGRO_DISPLAY *display) {
    ALLEGRO_FONT *font = al_load_ttf_font("../Resource/fonts/pirulen.ttf", 32, 0);
    if (!font) {
        std::cerr << "Failed to load font for mode selection!" << std::endl;
        exit(-1);
    }

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    GameMode mode = NORMAL_MODE;
    bool selected = false;

    while (!selected) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_1) {
                mode = NORMAL_MODE;
                selected = true;
            } else if (ev.keyboard.keycode == ALLEGRO_KEY_2) {
                mode = GRAVITY_MODE;
                selected = true;
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        if (font) {
            al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 - 20, ALLEGRO_ALIGN_CENTER, "Select Game Mode:");
            al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 20, ALLEGRO_ALIGN_CENTER, "1. Normal Mode");
            al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 60, ALLEGRO_ALIGN_CENTER, "2. Gravity Mode");
        }
        al_flip_display();
    }

    al_destroy_font(font);
    al_destroy_event_queue(event_queue);

    return mode;
}

int main() {
    if (!al_init()) {
        al_show_native_message_box(NULL, "Error", NULL, "Failed to initialize Allegro!", NULL, 0);
        return -1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);
    if (!display) {
        al_show_native_message_box(NULL, "Error", NULL, "Failed to create display!", NULL, 0);
        return -1;
    }

    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    if (!al_init_ttf_addon()) {
        std::cerr << "Failed to initialize TTF addon!" << std::endl;
        return -1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    GameMode mode = select_mode(display);
    Player player(mode);
    std::vector<Enem> enemies;
    std::vector<Obstacle> obstacles;
    add_obstacles(obstacles);

    int level = 1;
    add_enemies(enemies, level, obstacles);

    bool running = true;
    bool redraw = true;

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            ALLEGRO_KEYBOARD_STATE keyState;
            ALLEGRO_MOUSE_STATE mouseState;
            al_get_keyboard_state(&keyState);
            al_get_mouse_state(&mouseState);
            player.update(keyState, mouseState, obstacles);

            for (auto &enemy : enemies) {
                enemy.update(obstacles);
                for (auto &bullet : enemy.bullets) {
                    if (check_collision(player.x, player.y, bullet.x, bullet.y, PLAYER_SIZE / 2, BULLET_SIZE / 2)) {
                        player.health--;
                        enemy.bullets.erase(std::remove(enemy.bullets.begin(), enemy.bullets.end(), bullet), enemy.bullets.end());
                        if (player.health <= 0) {
                            running = false;
                            al_show_native_message_box(display, "Game Over", "You Lost!", "Press OK to exit.", NULL, ALLEGRO_MESSAGEBOX_WARN);
                        }
                    }
                }
            }

            for (auto &bullet : player.bullets) {
                for (auto &enemy : enemies) {
                    if (check_collision(bullet.x, bullet.y, enemy.x, enemy.y, BULLET_SIZE / 2, PLAYER_SIZE / 2)) {
                        enemy.health--;
                        player.bullets.erase(std::remove(player.bullets.begin(), player.bullets.end(), bullet), player.bullets.end());
                        if (enemy.health <= 0) {
                            player.increase_score(10);
                            enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());
                        }
                        break;
                    }
                }
            }

            if (enemies.empty()) {
                level++;
                add_enemies(enemies, level, obstacles);
            }

            redraw = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            player.draw();
            for (const auto &enemy : enemies) {
                enemy.draw();
            }
            for (const auto &obs : obstacles) {
                obs.draw();
            }

            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}