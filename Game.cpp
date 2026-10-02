#include "stdafx.h"
#include "Game.h"

Game::Game()
{
	Reset();
}

void Game::Reset()
{
	Console::SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Console::CursorVisible(false);

	isWon = false;
	isLost = false;

	paddle.width = 12;
	paddle.height = 2;
	paddle.x_position = 32;
	paddle.y_position = 30;

	ball.visage = 'O';
	ball.color = ConsoleColor::Cyan;
	ResetBall();

	// TODO #2 - Add this brick and 4 more bricks to the vector
	bricks.clear();

	const int numBricks = 5;
	const int brickWidth = 10;
	const int brickHeight = 2;
	const int spacing = (WINDOW_WIDTH - (numBricks * brickWidth)) / (numBricks + 1);

	for (int i = 0; i < numBricks; ++i)
	{
		Box b;
		b.width = brickWidth;
		b.height = brickHeight;
		b.x_position = spacing + i * (brickWidth + spacing);
		b.y_position = 5;
		b.doubleThick = true;
		b.color = ConsoleColor::DarkGreen; // Starts at DarkGreen (2 hits remaining to reach Black/0)

		bricks.push_back(b);
	}
}

void Game::ResetBall()
{
	ball.x_position = paddle.x_position + paddle.width / 2;
	ball.y_position = paddle.y_position - 1;
	ball.x_velocity = rand() % 2 ? 1 : -1;
	ball.y_velocity = -1;
	ball.moving = false;
}

bool Game::Update()
{
	if (GetAsyncKeyState(VK_ESCAPE) & 0x1)
		return false;

	if (GetAsyncKeyState(VK_RIGHT) && paddle.x_position < WINDOW_WIDTH - paddle.width)
		paddle.x_position += 2;

	if (GetAsyncKeyState(VK_LEFT) && paddle.x_position > 0)
		paddle.x_position -= 2;

	if (GetAsyncKeyState(VK_SPACE) & 0x1)
		ball.moving = !ball.moving;

	if (GetAsyncKeyState('R') & 0x1)
		Reset();

	ball.Update();
	CheckCollision();
	return true;
}

//  All rendering, including text, should occur in the Render function
void Game::Render() const
{
	Console::Lock(true);
	Console::Clear();

	paddle.Draw();
	ball.Draw();

	// TODO #3 - Update render to render all bricks
	for (const auto& b : bricks)
	{
		b.Draw();
	}

	// Render victory or defeat text when triggered
	if (isWon)
	{
		const char* winMsg = "You win! Press 'R' to play again.";
		int textX = (WINDOW_WIDTH - (int)strlen(winMsg)) / 2;
		int textY = WINDOW_HEIGHT / 2;

		Console::SetCursorPosition(textX, textY);
		Console::ForegroundColor(ConsoleColor::Yellow);
		std::cout << winMsg;
	}
	else if (isLost)
	{
		const char* loseMsg = "You lose. Press 'R' to play again.";
		int textX = (WINDOW_WIDTH - (int)strlen(loseMsg)) / 2;
		int textY = WINDOW_HEIGHT / 2;

		Console::SetCursorPosition(textX, textY);
		Console::ForegroundColor(ConsoleColor::Red);
		std::cout << loseMsg;
	}

	Console::Lock(false);
}

void Game::CheckCollision()
{
	// TODO #4 - Update collision to check all bricks
	for (size_t i = 0; i < bricks.size(); ++i)
	{
		if (bricks[i].Contains(ball.x_position + ball.x_velocity, ball.y_position + ball.y_velocity))
		{
			bricks[i].color = ConsoleColor(bricks[i].color - 1);
			ball.y_velocity *= -1;

			// TODO #5 - If the ball hits the same brick 3 times (color == black), remove it from the vector
			if (bricks[i].color == ConsoleColor::Black)
			{
				bricks.erase(bricks.begin() + i);
				--i; // Decrement index to prevent skipping the next brick
			}
			break;
		}
	}

	// TODO #6 - If no bricks remain, pause ball and display (render) victory text with R to reset
	if (bricks.empty())
	{
		ball.moving = false;
		isWon = true;
	}

	if (paddle.Contains(ball.x_position + ball.x_velocity, ball.y_velocity + ball.y_position))
	{
		ball.y_velocity *= -1;
	}

	// TODO #7 - If ball touches bottom of window, pause ball and display (render) defeat text with R to reset
	if (ball.y_position >= WINDOW_HEIGHT - 1)
	{
		ball.moving = false;
		isLost = true;
	}
}