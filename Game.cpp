#include "Game.h"

#include <iostream>
#include <random>
#include <cassert>
#include <string>

using namespace std;

template <typename T>
T RandomInt(T _min, T _max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> dis(_min, _max);
    return dis(gen);
}

Game::Game(
	const Pos& _size,
	int _mine_count)
{
	assert(_size.x * _size.y > 0);
	assert(_mine_count < _size.x * _size.y);

	flags_ = _mine_count;

	// init cells
	cells_.clear();
	cells_.resize(_size.x);
	for (auto& line : cells_)
	{
		line.clear();
		line.resize(_size.y);
	}
	// fill mines
	for (auto i = 0; i < _mine_count;)
	{
		const Pos p{RandomInt(0, _size.x - 1), RandomInt(0, _size.y - 1)};
		auto& cell = get_cell(p);
		if (!cell.has_mine)
		{
			cell.has_mine = true;
			++i;
		}
	}

	// fill neighbour mine count
	for (auto i = 0, i_end = _size.x - 1; i <= i_end; ++i)
	{
		for (auto j = 0, j_end = _size.y - 1; j <= j_end; ++j)
		{
			auto& cell = get_cell({i, j});
			if (cell.has_mine) continue; // do not count neighbour mine count on mined cells
			for (auto k = max(0, i - 1), k_end = min(i_end, i + 1); k <= k_end; ++k)
			{
				for (auto l = max(0, j - 1), l_end = min(j_end, j + i); l <= l_end; ++l)
				{
					if (k == i && l == j) continue; // skip self
					if (get_cell({k, l}).has_mine)
					{
						++cell.neighbour_mine_count;
					}
				}
			}
		}
	}
}

void Game::start()
{
	cout << u8"MineSweeper Game" << endl;
	cout << "* = mine, f = flag, - = undiscovered cell" << endl;
	cout << "To make your turn, please enter X and Y after space (valid from 1 to " << cells_.size() << ")" << endl;
	cout << "After them you can enter \"f\" to put or remove flag from that cell." << endl;
	int res = 0;
	while (!res)
	{
		print();

		cout << "Available flags count: " << flags_ << endl;
		cout << "Enter X Y [f]: ";

		string input;
		getline(cin, input, '\n');
		char flag = 0;
		Pos pos{};
		int count = sscanf(input.data(), "%d %d %c", &pos.x, &pos.y, &flag);
		if (count < 2 || count > 3 || pos.x < 1 || pos.y < 1 || pos.x > int(cells_.size()) || pos.y > int(cells_[0].size()))
		{
			cout << "Wrong input. Try again." << endl;
			continue;
		}

		res = step({pos.x - 1, pos.y - 1}, flag == 'f');
	}

	print(true);
	cout << (res > 0 ? "You win! Great!!!" : "Boom! You're lose!") << endl;
}

Cell& Game::get_cell(
	const Pos& _pos)
{
	assert(_pos.x >= 0 && _pos.y >= 0 && _pos.x < int(cells_.size()) && _pos.y < int(cells_[0].size()));
	return cells_[_pos.x][_pos.y];
}

void Game::print(
	bool _all) const
{
	using namespace std;
	cout << u8" ";
	for(int j = 0, j_e = int(cells_[0].size()) - 1; j <= j_e; ++j)
	{
		cout << j + 1;
	}
	cout << endl << u8"┌";
	for(int j = 0, j_e = int(cells_[0].size()) - 1; j <= j_e; ++j)
	{
		cout << u8"─";
	}
	cout << u8"┐" << endl << flush;

	for(auto i = 0, i_e = int(cells_.size()) - 1; i <= i_e; ++i)
	{
		cout << u8"│";
		for (const auto& c : cells_[i])
		{
			if (_all || c.is_opened)
			{
				if (c.has_mine)
				{
					cout << u8"*";
				}
				else if (c.has_flag)
				{
					cout << u8"f";
				}
				else if (!_all && c.neighbour_mine_count)
				{
					cout << c.neighbour_mine_count;
				}
				else
				{
					cout << u8" ";
				}
			}
			else if (c.has_flag)
			{
				cout << u8"f";
			}
			else
			{
				cout << u8"-";
			}
		}
		cout << u8"│ " << i + 1 << endl;
	}

	cout << u8"└";
	for(int j = 0, j_e = int(cells_[0].size()) - 1; j <= j_e; ++j)
	{
		cout << u8"─";
	}
	cout << u8"┘" << endl << flush;
}

int Game::step(
	const Pos& _pos,
	bool _flag_cell)
{
	assert(_pos.x >= 0 && _pos.y >= 0 && _pos.x < int(cells_.size()) && _pos.y < int(cells_[0].size()));

	auto& cell = get_cell(_pos);
	if (cell.is_opened)
	{
		cout << "That cell is already discovered." << endl;
		return 0;
	}
	if (_flag_cell)
	{
		if (cell.has_flag)
		{
			cout << "Flag is already set on that cell. Removing it." << endl;
			cell.has_flag = false;
			--flags_;
		}
		else if (flags_ > 0)
		{
			cell.has_flag = true;
			--flags_;
		}
		else
		{
			cout << "No flags available." << endl;
		}
		return 0;
	}
	if (cell.has_mine)
	{
		return -1; // game over, lose
	}

	cell.is_opened = true;
	if (cell.neighbour_mine_count == 0)
	{
		const auto i = _pos.x, i_end = int(cells_.size()) - 1, j = _pos.y, j_end = int(cells_[0].size()) - 1;
		for (auto k = max(0, i - 1), k_end = min(i_end, i + 1); k <= k_end; ++k)
		{
			for (auto l = max(0, j - 1), l_end = min(j_end, j + i); l <= l_end; ++l)
			{
				if (k == i && l == j) continue; // skip self
				if (get_cell({k, l}).has_flag || get_cell({k, l}).is_opened) continue; // skip flagged and already opened;
				const auto res = step({k, l}, false);
				if (res) return res;
			}
		}
	}

	// if all non mined cells is opened - win
	for (auto i = 0, i_end = int(cells_.size()) - 1; i <= i_end; ++i)
	{
		for (auto j = 0, j_end = int(cells_[i].size()) - 1; j <= j_end; ++j)
		{
			if (get_cell({i, j}).has_mine || get_cell({i, j}).is_opened) continue;
			return 0;
		}
	}

	return 1;
}
