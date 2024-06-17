#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "ScoreBoardScene.hpp"

void ScoreBoardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    record.clear();
    std::ifstream fin("../Resource/score.txt");
    std::string name;
    int score;
    while(fin>>name && fin>>score)
    {
        record.insert(std::pair<int,std::string>(score,name));
        int i=record.size();
        if(i>5*maxpage)maxpage++;
        std::cout<<maxpage;
    }
    AddNewObject(new Engine::Label("Score Board", "pirulen.ttf", 80, halfW, halfH / 3 , 10, 255, 0, 255, 0.5, 0.5));
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 + 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2+100, 0, 0, 0, 255, 0.5, 0.5));
    if(nowpage!=maxpage)
    {
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300 , halfH * 3 / 2 + 50, 400, 100);
        btn->SetOnClickCallback(std::bind(&ScoreBoardScene::NextOnClick, this,1));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Next", "pirulen.ttf", 48, halfW + 500, halfH * 3 / 2+100, 0, 0, 0, 255, 0.5, 0.5));
    }
    if(nowpage!=1)
    {
        btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700, halfH * 3 / 2 + 50, 400, 100);
        btn->SetOnClickCallback(std::bind(&ScoreBoardScene::PrevOnClick, this, 1));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 48, halfW-500, halfH * 3 / 2+100, 0, 0, 0, 255, 0.5, 0.5));
    }
    for(int i=0;i<5&&5*(nowpage-1)+1+i<=record.size();i++)
    {
        auto it=record.end();
        for(int j=0;j<5*(nowpage-1)+1+i;j++)it--;
        std::string line=it->second+"    "+std::to_string(it->first);
        AddNewObject(new Engine::Label(line, "pirulen.ttf", 32, halfW, halfH / 3+100+75*i , 100, 100, 200, 255, 0.5, 0.5));
    }

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    fin.close();
}
void ScoreBoardScene::Terminate() {
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void ScoreBoardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void ScoreBoardScene::NextOnClick(int stage) {
    if(nowpage<maxpage)nowpage++;
    Engine::GameEngine::GetInstance().ChangeScene("score");
}
void ScoreBoardScene::PrevOnClick(int stage) {
    if(nowpage>1)nowpage--;
    Engine::GameEngine::GetInstance().ChangeScene("score");
}
