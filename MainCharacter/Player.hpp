//
// Created by User on 2024/6/12.
//

#ifndef INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_PLAYER_HPP
#define INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_PLAYER_HPP

#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"


class Bullet;
class PlayScene;
class Turret;

class Player : public Engine::Sprite {
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    PlayScene* getPlayScene();
    virtual void OnExplode();
public:
    float reachEndTime;
    std::list<Bullet*> lockedBullets;
    Player(std::string img, float x, float y, float radius, float speed, float hp);
    void Hit(float damage);
    void UpdatePath();
    void Update(float deltaTime) override;
    void Draw() const override;
};





#endif //INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_PLAYER_HPP
