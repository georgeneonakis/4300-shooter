#include "Inventory.h"

void Inventory::UseHealthPotion(std::shared_ptr<Entity> e)
{
	auto hp = e->getComponent<CHealth>();
	auto items = e->getComponent<CInventory>();
	if (items->hPotCount > 0 && hp->currentHP < hp->maxHP)
	{
		hp->currentHP += 25;
		if (hp->currentHP >= hp->maxHP)
		{
			hp->currentHP = hp->maxHP;
		}
		items->hPotCount -= 1;
	}
}

void Inventory::UseShieldPotion(std::shared_ptr<Entity> e)
{
	auto hp = e->getComponent<CHealth>();
	auto items = e->getComponent<CInventory>();
	if (items->sPotCount > 0 && hp->currentShield < hp->maxShield)
	{
		hp->currentShield += 25;
		if (hp->currentShield >= hp->maxShield)
		{
			hp->currentShield = hp->maxShield;
		}
		items->sPotCount -= 1;
	}
}

void Inventory::UseSpeedPotion(std::shared_ptr<Entity> e)
{
	auto items = e->getComponent<CInventory>();
	auto buffs = e->getComponent<CBuffs>();
	if (items->speedCount > 0 && !(buffs->isFast))
	{
		buffs->isFast = true;
		buffs->speedTimer = buffs->speedDuration + buffs->clock.getElapsedTime().asMilliseconds();
		items->speedCount -= 1;
	}
}

void Inventory::UseStealthPotion(std::shared_ptr<Entity> e)
{
	auto items = e->getComponent<CInventory>();
	auto buffs = e->getComponent<CBuffs>();
	if (items->stealthCount > 0 && !(buffs->isStealthed))
	{
		buffs->isStealthed = true;
		e->getComponent<CBoundingBox>()->blockVision = true;
		buffs->stealthTimer = buffs->stealthDuration + buffs->clock.getElapsedTime().asMilliseconds();
		items->stealthCount -= 1;
	}
}
