#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
#include <conio.h>
#include <ctime>


using namespace std;

int sign(int z)
{
	if (z > 0)
		return 1;
	if (z < 0)
		return -1;
	//if (z == 0)
	return 0;
}


struct Point
{
	int x, y;

	Point()
	{
		x = 0;
		y = 0;
	}

	Point(int c)
	{
		x = c;
		y = c;
	}


	Point(int c, int d)
	{
		x = c;
		y = d;
	}

	double dist(Point b) const
	{
		return sqrt((b.x - x)*(b.x - x) + (b.y - y)*(b.y - y));
	}

	double dist() const
	{
		Point b;
		return dist(b);
	}

	bool operator==(Point b)
	{
		return x == b.x && y == b.y;
	}

};

struct Direction
{
	int a;

	Direction()
	{
		a = 0;
	}

	Direction(int q)
	{
		a = q;
	}

	bool operator==(Direction b)
	{
		return a == b.a;
	}

};


enum class block { empty, wall, p, berry };

class Field
{
private:
	int up, down, left, right;
	vector<vector<block>> blocks;
public:

	Field()
	{
		up = 1;
		down = -1;
		right = 1;
		left = -1;
		blocks.resize(right - left + 1, vector<block>(up - down + 1, block::empty));
		blocks[1][1] = block::wall;
		blocks[0][0] = block::p;
		blocks[0][2] = block::p;
		blocks[2][0] = block::p;
		blocks[2][2] = block::p;
	}

	block& get_block(Point q)
	{
		if (q.x - left == -1)
			q.x = right;
		if (q.y - down == -1)
			q.y = up;
		if (q.y - up == 1)
			q.y = down;
		if (q.x - right == 1)
			q.x = left;

		return blocks[q.x - left][q.y - down];
	}

	bool check_path(Point start, Point finish)
	{
		while (!(start == finish))
		{
			if (get_block(start) == block::wall)
				return false;

			start.x += sign(finish.x - start.x);
			start.y += sign(finish.y - start.y);
		}
		
		return true;
	}


	Field(Point u, Point i)
	{
		up = i.y;
		left = u.x;
		down = u.y;
		right = i.x;
		blocks.resize(right - left + 1, vector<block>(up - down + 1, block::p));
	}

	int get_up() const
	{
		return up;
	}

	int get_down() const
	{
		return down;
	}

	int get_right() const
	{
		return right;
	}

	int get_left() const
	{
		return left;
	}

	int get_score_left() const
	{
		int score = 0;
		for (const auto& v : blocks)
			for (auto x : v)
				score += (x == block::p) ? 1 : 0;
		return score;
	}


};

class Unit
{
protected:
	int speed;
	Point pos;
	Direction d;
	Field* f;
public:
	void move()
	{
		if (d == Direction(0) && f->get_block(Point(pos.x, pos.y + 1)) != block::wall)
		{
			pos.y++;
			if (pos.y > f->get_up())
				pos.y = f->get_down();
		}
		if (d == Direction(1) && f->get_block(Point(pos.x + 1, pos.y)) != block::wall)
		{
			pos.x++;
			if (pos.x > f->get_right())
				pos.x = f->get_left();
		}
		if (d == Direction(2) && f->get_block(Point(pos.x, pos.y - 1)) != block::wall)
		{
			pos.y--;
			if (pos.y < f->get_down())
				pos.y = f->get_up();
		}
		if (d == Direction(3) && f->get_block(Point(pos.x - 1, pos.y)) != block::wall)
		{
			pos.x--;
			if (pos.x < f->get_left())
				pos.x = f->get_right();
		}

		interact_block();
	}

	virtual void interact_block()
	{
	}

	void turn(Direction a)
	{
		d = a;
	}

	Point get_pos()
	{
		return pos;
	}

	virtual int get_speed()
	{
		return speed;
	}
};

class Ghost : public Unit
{
private:
	int colour;
	bool mode;

public:
	Ghost(Field& f1, Point p, int l)
	{
		f = &f1;
		pos = p;
		colour = l;
		mode = 0;
		speed = 2;
	}

	bool& get_mode()
	{
		return mode;
	}


	int get_speed()
	{
		if (mode)
			speed = 1;
		return speed;
	}

};


class Pacman : public Unit
{
private:
	int score = 0;
	bool mode = false;

public:
	Pacman()
	{
		pos = Point();
		speed = 1;
	}

	Pacman(Field& f1, Point p)
	{
		f = &f1;
		pos = p;
		speed = 1;
	}

	void interact_block() override
	{
		if (f->get_block(pos) == block::p)
		{
			f->get_block(pos) = block::empty;
			score++;
		}

		if (f->get_block(pos) == block::berry)
		{
			f->get_block(pos) = block::empty;
			mode = true;
			cout << "God mode activated!" << endl;
		}
	}

	int& get_score()
	{
		return score;
	}

	bool get_mode()
	{
		return mode;
	}
};


int random_num(int left, int right)
{
	return rand() % (right - left + 1) + left;
}



int main()
{
	Point w(-2, -2), e(2, 2);
	Field f(w, e);
	f.get_block(Point(-2, -2)) = block::wall;
	f.get_block(Point(2, -2)) = block::wall;
	f.get_block(Point(2, 2)) = block::berry;

	Pacman n(f, Point(0, 0));

	vector<Ghost> ghosts;
	for (int i = 0; i < 2; ++i)
		ghosts.emplace_back(f, Point(random_num(f.get_left(), f.get_right()), random_num(f.get_down(), f.get_up())), rand() % 4);

	bool v = true;

	while (v)
	{
		string s;

		cout << "Pacman position: " << n.get_pos().x << " " << n.get_pos().y << endl;
		cout << "Pacman score: " << n.get_score() << endl;
		if (!ghosts.empty())
			cout << "Ghosts positions:" << endl;
		for (auto x : ghosts)
		{
			cout << x.get_pos().x << " " << x.get_pos().y << endl;
		}

		for (auto it = ghosts.begin(); it != ghosts.end(); ++it)
		{
			if (n.get_pos() == it->get_pos())
			{
				if (!it->get_mode())
				{
					cout << "Pacman died" << endl;
					cout << "Final score = " << n.get_score() << endl;
					v = false;
				}
				else
				{
					n.get_score() += 100;
					ghosts.erase(it);
					break;
				}
			}
		}

		int c = getch();
		bool t = false;


		int d;

		if (c == 27)
			v = false;

		if (c == 0 || c == 224)
		{
			switch (_getch())
			{
			case 72:
				d = 0;
				break;

			case 80:
				d = 2;
				break;

			case 75:
				d = 3;
				break;

			case 77:
				d = 1;
				break;
			}

			n.turn(Direction(d));
			n.move();

			if (f.get_score_left() == 0)
			{
				cout << "YOU WON!" << endl;
				v = false;
			}

			for (auto& x : ghosts)
			{

				for (int i = 0; i < x.get_speed(); i++)
				{
					x.turn(Direction(rand() % 4));

					if (n.get_pos().x == x.get_pos().x && f.check_path(x.get_pos(), n.get_pos()))
					{
						if (n.get_pos().y > x.get_pos().y)
							x.turn(Direction(0));
						else
							x.turn(Direction(2));
					}

					if (n.get_pos().y == x.get_pos().y && f.check_path(x.get_pos(), n.get_pos()))
					{
						if (n.get_pos().x > x.get_pos().x)
							x.turn(Direction(1));
						else
							x.turn(Direction(3));
					}

					x.move();
				}

				x.get_mode() = n.get_mode();
			}

		}


	}

	cout << "Enter anything to exit" << endl;
	string s;
	cin >> s;
	return 0;
}
