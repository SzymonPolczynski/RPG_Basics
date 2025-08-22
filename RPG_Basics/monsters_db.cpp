#include <array>
#include <cassert>
#include "monsters_db.hpp"


static constexpr std::array<MonsterDef, 2> DB = { {
	{MonsterKind::Rat, "Rat", 3, 1, 'r'},
	{MonsterKind::Goblin, "Goblin", 6, 2, 'g'}
} };

std::span<const MonsterDef> allMonstersDefs() { return DB; }

const MonsterDef& monsterDef(MonsterKind kind) {
	const auto idx = static_cast<std::size_t>(kind);
	assert(idx < DB.size());
	return DB[idx];
}
