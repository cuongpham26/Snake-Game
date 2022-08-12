#include <chrono> // for std::chrono
#include <conio.h> // for _kbhit() and _getch()
#include <iostream>
#include <thread> // for std::this_thread
#include <windows.h> // for SetConsoleCursorPosition

bool gameOver;
constexpr int width{ 20 };
constexpr int height{ 20 };
int headX, headY, fruitX, fruitY, score;
int tailX[(width - 2) * (height - 2) - 1], tailY[(width - 2) * (height - 2) - 1];
int tailNum;
enum Direction { stop = 0, up, right, down, left };
Direction direction;

void setup()
{
	gameOver = false;
	direction = stop;
	headX = width / 2;
	headY = height / 2;

	bool wrongFruitPosition{ false }; // If the fruit is in the same position as head or one of the tails
	do {
		fruitX = (rand() % (width - 2)) + 1; // to ensure that fruitX is within the range [1, width-2]
		fruitY = (rand() % (height - 2)) + 1; // to ensure that fruitY is within the range [1, height-2]

		if (fruitX == headX && fruitY == headY) {
			wrongFruitPosition = true;
			continue;
		}

		for (int i = 0; i < tailNum; ++i) {
			if (fruitX == tailX[i] && fruitY == tailY[i]) {
				wrongFruitPosition = true;
			}
		}
	} while (wrongFruitPosition);

	score = 0;
}

void draw()
{
	/* system("cls"); // clear the console window (but there is flickering) */

	// A better solution that doesn't have flickering
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	// What this does is jsut takes your cursor to first word of top line and starts to
	// overwirte the text so the flickering just stops

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
			else if (j == headX && i == headY) {
				std::cout << 'O';
			}
			else if (j == fruitX && i == fruitY) {
				std::cout << '@';
			}
			else {
				bool tail_is_printed{ false };
				for (int k = 0; k < tailNum; ++k) {
					if (j == tailX[k] && i == tailY[k]) {
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
	std::cout << "\nScore: " << score;
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
			if (direction != down || tailNum == 0) {
				direction = up;
			}
			break;
		case 'd':
			if (direction != left || tailNum == 0) {
				direction = right;
			}
			break;
		case 's':
			if (direction != up || tailNum == 0) {
				direction = down;
			}
			break;
		case 'a':
			if (direction != right || tailNum == 0) {
				direction = left;
			}
			break;
		case ' ': // for testing purpose only
			direction = stop;
			break;
		case 'x':
			gameOver = true;
			break;
		}
	}
}

void logic()
{
	int currX, currY, prevX{ headX }, prevY{ headY };
	for (int i = 0; i < tailNum; ++i)
	{
		// Save the current tail position
		currX = tailX[i];
		currY = tailY[i];

		// Update the current tail position
		tailX[i] = prevX;
		tailY[i] = prevY;

		// Update the previous tail position as the current tail position (for the next iteration)
		prevX = currX;
		prevY = currY;
	}

	// Update the position of the snake's head based on its direction
	switch (direction)
	{
	case up:
		--headY; // since the y-axis is reversed for this program
		break;
	case right:
		++headX;
		break;
	case down:
		++headY; // since the y-axis is reversed for this program
		break;
	case left:
		--headX;
		break;
	}

	// If the snake's head pass hit the wall
	if (headX >= width - 1 || headX <= 0 || headY <= 0 || headY >= height - 1) {
		gameOver = true;
	}

	// If the snake eats the fruit
	if (headX == fruitX && headY == fruitY) {
		++tailNum;
		score += 10;

		bool wrongFruitPosition{ false }; // If the fruit is in the same position as head or one of the tails
		do {
			fruitX = (rand() % (width - 2)) + 1; // to ensure that fruitX is within the range [1, width-2]
			fruitY = (rand() % (height - 2)) + 1; // to ensure that fruitY is within the range [1, height-2]

			if (fruitX == headX && fruitY == headY) {
				wrongFruitPosition = true;
				continue;
			}

			for (int i = 0; i < tailNum; ++i) {
				if (fruitX == tailX[i] && fruitY == tailY[i]) {
					wrongFruitPosition = true;
					continue;
				}
			}
			wrongFruitPosition = false; // If this line is not hear, wrongFruitPosition
			// once becomes true, it will remain true forever. Therefore, this while loop
			// becomes an infinite while loop
		} while (wrongFruitPosition);
	}

	// If the snake eats itself, stops the game
	for (int i = 0; i < tailNum; ++i) {
		if (headX == tailX[i] && headY == tailY[i]) {
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