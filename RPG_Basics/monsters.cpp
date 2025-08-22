#include <algorithm>
#include <cmath>
#include <random>
#include "config.hpp"
#include "monsters.hpp"
#include "monsters_db.hpp"


static int irand(int lo, int hi) {
	static std::mt19937 rng{ std::random_device{}() };
	std::uniform_int_distribution<int> dist(lo, hi);
	return dist(rng);
}

bool isMonsterAt(const Monsters& mons, int x, int y, const Monster** outPtr) {
	for (const auto& m : mons.list) {
		if (m.alive && m.x == x && m.y == y) {
			if (outPtr) *outPtr = &m;
			return true;
		}
	}
	return false;
}

char monsterGlyphAt(const Monsters& mons, int x, int y, bool isVisible, bool /*isExplored*/) {
	if (!isVisible) return 0;
	const Monster* pm{};
	if (!isMonsterAt(mons, x, y, &pm)) return 0;
	return monsterDef(pm->kind).glyph;
}

void spawnMonsters(Monsters& mons, int count, const Tile map[MAP_HEIGHT][MAP_WIDTH]) {
	mons.list.clear();
	for (int i = 0; i < count; ++i) {
		MonsterKind kind = (irand(0, 1) == 0) ? MonsterKind::Rat : MonsterKind::Goblin;
		const auto& def = monsterDef(kind);

		Monster m{};
		m.kind = kind;
		m.hp = def.base_hp;

		bool placed = false;
		for (int tries = 0; tries < 200; ++tries) {
			int x = irand(1, MAP_WIDTH - 2);
			int y = irand(1, MAP_HEIGHT - 2);
			if (isWalkable(x, y, map)) {
				const Monster* tmp{};
				if (!isMonsterAt(mons, x, y, &tmp) && !(x == MAP_WIDTH / 2 && y == MAP_HEIGHT / 2)) {
					m.x = x; m.y = y;
					mons.list.push_back(m);
					placed = true;
					break;
				}
			}
		}
		if (!placed) break;
	}
}

static int manhattan(int x1, int y1, int x2, int y2) {
	return std::abs(x1 - x2) + std::abs(y1 - y2);
}

static bool canStep(const Monsters& mons, const Tile map[MAP_HEIGHT][MAP_WIDTH], int nx, int ny) {
	if (!inBounds(nx, ny)) return false;
	if (!isWalkable(nx, ny, map)) return false;
	return !isMonsterAt(mons, nx, ny, nullptr);
}

static void stepTowards(Monster& m, const Player& p, const Tile map[MAP_HEIGHT][MAP_WIDTH], const Monsters& mons) {
	int dx = (p.x > m.x) ? 1 : (p.x < m.x ? -1 : 0);
	int dy = (p.y > m.y) ? 1 : (p.y < m.y ? -1 : 0);
	bool preferX = (irand(0, 1) == 0);

	auto tryXY = [&](int nx, int ny) {
		if (canStep(mons, map, nx, ny)) { m.x = nx; m.y = ny; return true; }
		return false;
	};

	if (preferX) {
		if (dx && tryXY(m.x + dx, m.y)) return;
		if (dy && tryXY(m.x, m.y + dy)) return;
	}
	else {
		if (dy && tryXY(m.x, m.y + dy)) return;
		if (dx && tryXY(m.x + dx, m.y)) return;
	}

	static const int ox[4] = { 1,-1,0,0 };
	static const int oy[4] = { 0,0,1,-1 };
	int dir = irand(0, 3);
	tryXY(m.x + ox[dir], m.y + oy[dir]);
}

bool playerBumpAttackOrMove(Player& p, Monsters& mons, Command cmd, std::string& outStatus, Tile map[MAP_HEIGHT][MAP_WIDTH]) {
	int dx = 0, dy = 0;
	switch (cmd) {
	case Command::Up: dy = -1; break;
	case Command::Down: dy = 1; break;
	case Command::Left: dx = -1; break;
	case Command::Right: dx = 1; break;
	default: return false;
	}

	const int nx = p.x + dx;
	const int ny = p.y + dy;
	if (!inBounds(nx, ny)) { outStatus = "Bump! Edge of map."; return false; }

	for (auto& m : mons.list) {
		if (!m.alive) continue;
		if (m.x == nx && m.y == ny) {
			// TODO: ADD WEAPON DMG (1DMG NOW)
			int dmg = 1;
			m.hp -= dmg;
			outStatus = "You hit the " + monsterDef(m.kind).name + " for " + std::to_string(dmg) + ".";
			if (m.hp <= 0) {
				m.alive = false;
				outStatus += " It dies.";
			}
			return true;
		}
	}

	if (!isWalkable(nx, ny, map)) {
		outStatus = "Bump! You can't go there.";
		return false;
	}

	p.x = nx; p.y = ny;

	Tile& cell = map[ny][nx];
	const bool wasTrap = (cell == Tile::Trap);
	applyTileEffect(p, cell);
	if (wasTrap) outStatus = "You triggered a trap! (-1 HP)";
	else outStatus = "Moved";

	return true;
}

void monstersTurn(Monsters& mons, Player& p, const Tile map[MAP_HEIGHT][MAP_WIDTH]) {
	for (auto& m : mons.list) {
		if (!m.alive) continue;

		int dist = manhattan(m.x, m.y, p.x, p.y);
		if (dist == 1) {
			const auto& def = monsterDef(m.kind);
			p.hp -= def.attack;
			continue;
		}

		if (dist <= MONSTER_SIGHT) {
			stepTowards(m, p, map, mons);
		}
		else {
			if (irand(0, 3) == 0) {
				static const int ox[4] = { 1,-1,0,0 };
				static const int oy[4] = { 0,0,1,-1 };
				int dir = irand(0, 3);
				int nx = m.x + ox[dir], ny = m.y + oy[dir];
				if (canStep(mons, map, nx, ny)) { m.x = nx; m.y = ny; }
			}
		}
	}
}