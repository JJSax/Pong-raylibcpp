
extern "C" {
#include <raylib.h>
}

const int windowWidth = 1280;
const int windowHeight = 720;

const Color Green = Color{38, 185, 154, 255};
const Color Dark_Green = Color{20, 160, 133, 255};
const Color Light_Green = Color{129, 204, 184, 255};
const Color Yellow = Color{243, 213, 154, 255};

struct vec2 {
	float x, y;

	//									v const for "this"
	vec2 operator+(const vec2 &other) const {
		return vec2 {this->x + other.x, this->y + other.y};
	}

	vec2& operator+=(const vec2 &other) {
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
};

class Paddle {
public:
	vec2 pos;
	vec2 lastPos;
	int score = 0;

	static constexpr vec2 dim = {10, 100};
	Paddle(vec2 pos) {
		this->pos = pos;
	}

	void setHeight(int y) {
		this->lastPos = this->pos;
		this->pos.y = y;
		if (this->pos.y < 0) {
			this->pos.y = 0;
		}
		if (this->pos.y + this->dim.y > windowHeight) {
			this->pos.y = windowHeight - this->dim.y;
		}
	}

	void draw() {
		DrawRectangleRounded(Rectangle{pos.x, pos.y, dim.x, dim.y}, 0.7, 0, WHITE);
	}

};

class Ball {
public:
	vec2 pos;
	vec2 speed;
	int radius = 10;

	Ball(vec2 pos) {
		this->reset(pos);
	}

	void reset(vec2 pos) {
		this->pos = pos;
		this->speed = {7, 7};
	}

	void flipYSpeed() {
		this->speed.y *= -1;
	}

	void flipXSpeed() {
		this->speed.x *= -1;
	}

	bool checkCollision(const Paddle& p) {
		return CheckCollisionCircleRec(
			Vector2{this->pos.x, this->pos.y},
			this->radius,
			Rectangle{p.pos.x, p.pos.y, p.dim.x, p.dim.y}
		);
	}

	void update() {
		this->pos += this->speed;
	}

	void draw() {
		DrawCircle(this->pos.x, this->pos.y, this->radius, Yellow);
	}

};


int main(void) {
	const int paddlePadding = 10;

	Ball ball(vec2 {windowWidth/2, windowHeight/2});
	Paddle player(vec2 {paddlePadding, windowHeight/2 - Paddle::dim.y/2});
	Paddle opponent(vec2 {windowWidth - Paddle::dim.x - paddlePadding, windowHeight/2 - Paddle::dim.y/2});

	InitWindow(windowWidth, windowHeight, "Pong");
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		// update
		ball.update();
		if (ball.pos.y - ball.radius < 0) {
			ball.pos.y = ball.radius;
			ball.flipYSpeed();
		}
		if (ball.pos.y + ball.radius > windowHeight) {
			ball.pos.y = windowHeight - ball.radius;
			ball.flipYSpeed();
		}

		if (ball.pos.x - ball.radius < 0) {
			ball.reset(vec2 {windowWidth/2, windowHeight/2});
			opponent.score += 1;
		}
		if (ball.pos.x + ball.radius > windowWidth) {
			ball.reset(vec2 {windowWidth/2, windowHeight/2});
			player.score += 1;
		}

		if (ball.checkCollision(player) | ball.checkCollision(opponent)) {
			ball.speed.x *= -1;
			// get difference in height
			Paddle other = ball.pos.x > windowWidth/2 ? opponent : player;
			int midy = other.pos.y + other.dim.y/2;
			int diff = ball.pos.y - midy;
			ball.speed.y = diff/7 + (other.pos.y - other.lastPos.y)*0.7;
			ball.pos.x = ball.pos.x > windowWidth/2 ? other.pos.x - ball.radius : other.pos.x + other.dim.x + ball.radius;
		}

		player.setHeight(GetMouseY() - player.dim.y/2);
		opponent.setHeight(ball.pos.y - opponent.dim.y/2);


		//draw
		BeginDrawing();
		ClearBackground(Dark_Green);
		DrawRectangle(windowWidth/2, 0, windowWidth/2, windowHeight, Green);
		DrawCircle(windowWidth/2, windowHeight/2, 150, Light_Green);
		DrawLine(windowWidth/2, 0, windowWidth/2, windowHeight, WHITE);
		DrawText(TextFormat("%i", player.score), 40, windowHeight - 50, 40, WHITE);
		DrawText(TextFormat("%i", opponent.score), windowWidth - 60, windowHeight - 50, 40, WHITE);
		ball.draw();

		player.draw();
		opponent.draw();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}