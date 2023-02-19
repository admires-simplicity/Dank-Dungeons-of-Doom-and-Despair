#include <iostream>
#include <vector>

#include <raylib.h>
#include <raymath.h>

#define G	500

bool Colliding(Rectangle &a, Rectangle &b) {
	if (b.x + b.width > a.x &&
            b.y + b.height > a.y &&
	    a.x + a.width > b.x &&
	    a.y + a.height > b.y)
		return true;
	else return false;
}


namespace dddd {
	class Rectangle : public ::Rectangle {
	public:
		int Top() {
			return y;
		}

		int Bottom() {
			return y + height;
		}

		int Left() {
			return x;
		}

		int Right() {
			return x + width;
		}

		bool Above(Rectangle r) {
			return Bottom() <= r.Top();
		}

		Rectangle(::Rectangle r) : ::Rectangle(r) {}
	};

}

class Entity {
public:
	dddd::Rectangle bounds;
	Vector2 velocity;
	float maxSpeed = 200.0f;
	Color color;
	bool moving;
	bool onGround;

	Entity(Rectangle b, Color c) :
		bounds(b),
		velocity({ 0.0f, 0.0f }),
		color(c),
		moving(0),
		onGround(0)
		{}

	void UpdateBounds(std::vector<dddd::Rectangle> &walls) {
		float deltaTime = GetFrameTime();

		dddd::Rectangle nextBounds { bounds };

		nextBounds.y += velocity.y*deltaTime;
		velocity.y += G*deltaTime;

		for (size_t i = 0; i < walls.size(); ++i) {
			if (Colliding(nextBounds, walls[i])) {
				if (bounds.Above(walls[i])) {
					onGround = true;
					nextBounds.y = walls[i].y - bounds.height;
				} else {
					nextBounds.y = walls[i].y + walls[i].height;
				}
				velocity.y = 0.0f;
				break;
			}
		}

		if (!moving && velocity.x != 0.0f) {
			const float friction = 20.0f;
			velocity.x += (velocity.x > 0.0f ? -1.0f : 1.0f) * friction;
		}

		nextBounds.x += velocity.x*deltaTime;

		for (size_t i = 0; i < walls.size(); ++i) {
			if (Colliding(nextBounds, walls[i])) {
				if (bounds.Left() >= walls[i].Right()) nextBounds.x = walls[i].Right();
				else                                   nextBounds.x = walls[i].Left() - bounds.width;
				velocity.x = 0;
				break;
			}
		}

		bounds = nextBounds;
	}
};

class Player : public Entity {
#define PLAYER_WIDTH	20
#define PLAYER_HEIGHT	60
#define PLAYER_COLOR	WHITE
public:
	Player() :
		Entity( { 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT }, PLAYER_COLOR )
		{}

	Player(float x, float y) : 
		Entity( { x, y, PLAYER_WIDTH, PLAYER_HEIGHT }, PLAYER_COLOR )
		{}

	void Move() {
		std::cout << "player.velocity.x = " << velocity.x << '\n';
		if (IsKeyDown(KEY_SPACE) && onGround) {
#define PLAYER_JUMP_SPD	325.0f
			velocity.y = -PLAYER_JUMP_SPD;
			onGround = false;
		}
		const float hAccel = 30.0f;
		if (IsKeyDown(KEY_LEFT)) {
			moving = true;
			if (velocity.x - hAccel > -maxSpeed) velocity.x -= hAccel;
			else                                      velocity.x = -maxSpeed;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			moving = true;
			if (velocity.x + hAccel < maxSpeed) velocity.x += hAccel;
			else                                     velocity.x = maxSpeed;
		}
		if (IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT)) {
			moving = false;
		}
	}
};

void InitializeWindow(Vector2 screenSize) {
	InitWindow(screenSize.x, screenSize.y, "FUN -- ?");
}

void InitializeCamera(Camera2D &camera, Player &player, Vector2 screenSize) {
	camera = { 0 };
	camera.target = (Vector2){ 0, 0 };
	camera.target = (Vector2){ 0, 0 };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
}
int main() {
	Vector2 screenSize = { 1200, 900 };
	InitializeWindow(screenSize);

	Player player;
//	player.bounds.x += 650;

	Camera2D camera;
	InitializeCamera(camera, player, screenSize);

	std::vector<dddd::Rectangle> level_walls { 
		dddd::Rectangle({ 0, 100, 600, 100 }),
		dddd::Rectangle({ 700, 200, 500, 100 }),
		dddd::Rectangle({ 0, 300, 1200, 100 }),
	};

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		BeginMode2D(camera);




		player.Move();

		player.UpdateBounds(level_walls);
		DrawRectangleRec(player.bounds, player.color);

		for (auto &r : level_walls) {
			DrawRectangleRec(r, LIGHTGRAY);
		}

		EndMode2D();
		EndDrawing();
	}

	return 0;
}
