#include <cassert>
#include <chrono> // for std::chrono
#include <conio.h> // for _kbhit() and _getch()
#include <iostream>
#include <thread> // for std::this_thread
#include <windows.h> // for SetConsoleCursorPosition

class Point
{
private:
	int m_x;
	int m_y;

public:
	Point(int x = 0, int y = 0) : m_x{ x }, m_y{ y }
	{
		assert(x >= 0 && y >= 0);
	}
	int& getX() { return m_x; }
	int& getY() { return m_y; }
	int getX() const { return m_x; }
	int getY() const { return m_y; }

	friend bool operator==(const Point& p1, const Point& p2)
	{
		return (p1.m_x == p2.m_x) && (p1.m_y == p2.m_y);
	}
};

bool gameOver;
constexpr int width{ 20 }, height{ 20 };
Point head, fruit, tail[(width - 2) * (height - 2) - 1];
int tailNum, score;
enum class Direction { stop = 0, up, right, down, left } direction;

void setup()
{
	gameOver = false;
	direction = Direction::stop;
	head = Point(width / 2, height / 2);

	bool wrongFruitPosition{ false }; // If the fruit is in the same position as the head or one of the tails
	do {
		// To ensure that fruit.m_x and fruit.m_y is within the range [1, width-2]
		fruit = Point(rand() % (width - 2) + 1, rand() % (width - 2) + 1);

		// If the fruit is in the same position as the head
		if (fruit == head) {
			wrongFruitPosition = true;
			continue;
		}

		// If the fruit is in the same position as one of the tails
		for (int i = 0; i < tailNum; ++i) {
			if (fruit == tail[i]) {
				wrongFruitPosition = true;
			}
		}
	} while (wrongFruitPosition);

	score = 0;
}

void draw()
{
	/* system("cls"); // clear the console window (but there is flickering) */

	// A better solution
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	// What this does is jsut takes your cursor to first word of top line and starts to
	// overwirte the text so the flickering decreases

	// Print the top wall
	for (int i = 0; i < width; ++i) {
		std::cout << '#';
	}
	std::cout << '\n';

	// Print the middle section
	for (int i = 1; i < height - 1; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j == 0 || j == width - 1) {
				std::cout << '#';
			}
			else if (j == head.getX() && i == head.getY()) {
				std::cout << 'O';
			}
			else if (j == fruit.getX() && i == fruit.getY()) {
				std::cout << '@';
			}
			else {
				bool tail_is_printed{ false };
				for (int k = 0; k < tailNum; ++k) {
					if (j == tail[k].getX() && i == tail[k].getY()) {
						std::cout << 'o';
						tail_is_printed = true;
						break;
					}
				}
				if (!tail_is_printed) {
					std::cout << ' ';
				}
			}
		}
		std::cout << '\n';
	}

	// Print the bottom wall
	for (int i = 0; i < width; ++i) {
		std::cout << '#';
	}
	std::cout << "\n\nScore: " << score;
}

void input()
{
	// if a keyboard is hit
	if (_kbhit())
	{
		// return the ASCII value of the keyboard that was pressed
		switch (_getch())
		{
		case 'w':
			if (direction != Direction::down || tailNum == 0) {
				direction = Direction::up;
			}
			break;
		case 'd':
			if (direction != Direction::left || tailNum == 0) {
				direction = Direction::right;
			}
			break;
		case 's':
			if (direction != Direction::up || tailNum == 0) {
				direction = Direction::down;
			}
			break;
		case 'a':
			if (direction != Direction::right || tailNum == 0) {
				direction = Direction::left;
			}
			break;
		case ' ': // for testing purpose only
			direction = Direction::stop;
			break;
		case 'x':
			gameOver = true;
			break;
		}
	}
}

void logic()
{
	// Update the position of the tails depend on the position of the head
	// This portion must be executed before the updating the position of the head because
	// otherwise, we will lose the old position (before updating) of the head so we can't
	// update the position of tail[0]
	Point curr, prev{ head };
	for (int i = 0; i < tailNum; ++i)
	{
		// Save the current tail position
		curr = tail[i];

		// Update the current tail position
		tail[i] = prev;

		// Update the previous tail position as the current tail position (for the next iteration)
		prev = curr;
	}

	// Update the position of the snake's head based on its direction
	switch (direction)
	{
	case Direction::up:
		--head.getY(); // since the y-axis is reversed for this program
		break;
	case Direction::right:
		++head.getX();
		break;
	case Direction::down:
		++head.getY(); // since the y-axis is reversed for this program
		break;
	case Direction::left:
		--head.getX();
		break;
	}

	// If the snake's head hit the wall
	if (head.getX() >= width - 1 || head.getX() <= 0 || head.getY() <= 0 || head.getY() >= height - 1) {
		gameOver = true;
	}

	// If the snake eats the fruit
	if (head == fruit) {
		++tailNum;
		score += 10;

		bool wrongFruitPosition{ false }; // If the fruit is in the same position as head or one of the tails
		do {
			// To ensure that fruit.m_x and fruit.m_y is within the range [1, width-2]
			fruit = Point(rand() % (width - 2) + 1, rand() % (width - 2) + 1);

			if (fruit == head) {
				wrongFruitPosition = true;
				continue;
			}

			for (int i = 0; i < tailNum; ++i) {
				if (fruit == tail[i]) {
					wrongFruitPosition = true;
					continue;
				}
			}
			wrongFruitPosition = false; // If this line is not here, wrongFruitPosition
			// once becomes true, it will remain true forever. Therefore, this while loop
			// becomes an infinite while loop
		} while (wrongFruitPosition);
	}

	// If the snake eats itself, stops the game
	for (int i = 0; i < tailNum; ++i) {
		if (head == tail[i]) {
			gameOver = true;
			break;
		}
	}
}

void sleep(int miliseconds)
{
	std::chrono::milliseconds timespan(miliseconds); // or whatever
	std::this_thread::sleep_for(timespan);
}

int main()
{
	setup();

	while (!gameOver)
	{
		draw();
		input();
		logic();
		sleep(100); // pause the screen for 100 miliseconds
	}
}