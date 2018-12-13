#include "Inventory.h"

void Inventory::UseHealthPotion(std::shared_ptr<Entity> e)
{
	auto hp = e->getComponent<CHealth>();
	auto items = e->getComponent<CInventory>();
	if (items->hPotCount == 0)
	{
		return;
	}
	hp->currentHP += 25;
	if (hp->currentHP >= hp->maxHP)
	{
		hp->currentHP = hp->maxHP;
	}
	items->hPotCount -= 1;
}

void Inventory::UseShieldPotion(std::shared_ptr<Entity> e)
{
	auto hp = e->getComponent<CHealth>();
	auto items = e->getComponent<CInventory>();
	if (items->sPotCount == 0)
	{
		return;
	}
	hp->currentShield += 25;
	if (hp->currentShield >= hp->maxShield)
	{
		hp->currentShield = hp->maxShield;
	}
	items->sPotCount -= 1;
}