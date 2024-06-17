#include <allegro5/color.h>

#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "TurretButton.hpp"

PlayScene* TurretButton::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
TurretButton::TurretButton(std::string img, std::string imgIn, Engine::Sprite Base, Engine::Sprite Turret, float x, float y, int money) :
	ImageButton(img, imgIn, x, y), money(money), Base(Base), Turret(Turret) {
}
void TurretButton::Update(float deltaTime) {
}
void TurretButton::Draw() const {
	ImageButton::Draw();
	Base.Draw();
	Turret.Draw();
}
