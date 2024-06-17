#ifndef TURRET4_HPP
#define TURRET4_HPP
#include "Turret.hpp"

class FTurret: public Turret {
public:
	static const int Price;
    FTurret(float x, float y);
    void CreateBullet() override;
};
#endif // MISSILETURRET_HPP
