#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
#include <set>
#include <map>
#include <iostream>
class ScoreBoardScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
    int nowpage=1;
    int maxpage=0;
    std::multimap<int,std::string> record;
	explicit ScoreBoardScene() = default;
	void Initialize() override;
	void Terminate() override;
	void BackOnClick(int stage);
    void NextOnClick(int);
    void PrevOnClick(int);
};



#endif // SCOREBOARDSCENE_HPP
