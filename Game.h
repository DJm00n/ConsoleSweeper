#pragma once

#include <vector>

struct Pos
{
	int x;
	int y;
};

struct Cell
{
	bool is_opened = false;
	bool has_mine = false;
	bool has_flag = false;
	int neighbour_mine_count = 0; // 0..8
};

class Game
{
public:
	Game(const Pos& _size, int _mine_count);

	void start();
private:
	Cell& get_cell(const Pos& _pos);
	void print(bool _all = false) const;

	// Returns 0 = ok
	// 1 - win
	// -1 - lose
	int step(const Pos& _pos, bool _flag_cell);

	std::vector<std::vector<Cell>> cells_;
	int flags_;
};
