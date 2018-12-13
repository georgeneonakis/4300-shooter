#pragma once

#include "Common.h"
#include "Entity.h"
#include "Components.h"

namespace Inventory
{
	void UseHealthPotion(std::shared_ptr<Entity> e);
	void UseShieldPotion(std::shared_ptr<Entity> e);
	void UseStealthPotion(std::shared_ptr<Entity> e);
	void UseSpeedPotion(std::shared_ptr<Entity> e);
}
