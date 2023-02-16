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

class Entity {
public:
	Rectangle bounds;
	Vector2 velocity;
	float maxSpeed = 10.0f;
	Color color;

	Entity(Rectangle b, Color c) :
		bounds(b),
		velocity({ 0.0f, 0.0f }),
		color(c)
		{}

	void UpdateBounds(std::vector<Rectangle> &walls) {
		float deltaTime = GetFrameTime();

		//bool verticalCollision = false;
		bool hitGround = false;

		Rectangle nextBounds { bounds };

		nextBounds.y += velocity.y*deltaTime;
		velocity.y += G*deltaTime;

		for (size_t i = 0; i < walls.size(); ++i) {
			if (Colliding(nextBounds, walls[i])) {
				//should this be a virtual function call for a HitGround function?
				//verticalCollision = true;
				hitGround = true;
				velocity.y = 0.0f;
				nextBounds.y = walls[i].y - bounds.height;
				break;
			}
		}
		bounds = nextBounds;

		if (hitGround && velocity.x != 0.0f) {
			//if (abs(velocity.x) < 2.0f) velocity.x = 0.0f;
			//else velocity.x += (velocity.x > 0.0f ? -1.0f : 1.0f) * friction;
			velocity.x += (velocity.x > 0.0f ? -1.0f : 1.0f) * 1.0f;
		}
		bounds.x += velocity.x;


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
};

void InitializeWindow(Vector2 screenSize) {
	InitWindow(screenSize.x, screenSize.y, "FUN -- ?");
}

void InitializeCamera(Camera2D &camera, Player &player, Vector2 screenSize) {
	camera = { 0 };
	camera.target = (Vector2){ player.bounds.x, player.bounds.y } ;
	camera.offset = (Vector2){ screenSize.x/2.0f, screenSize.y/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
}

int main() {
	Vector2 screenSize = { 1200, 900 };
	InitializeWindow(screenSize);

	Player player;
	//player.velocity.x = 10.0f;

	Camera2D camera;
	InitializeCamera(camera, player, screenSize);

	std::vector<Rectangle> level_walls { 
		{ 0, 100, 600, 100 },
	};

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		BeginMode2D(camera);


		if (IsKeyDown(KEY_LEFT)) {
			if (player.velocity.x > -player.maxSpeed) player.velocity.x -= 2.0f;
			else                                      player.velocity.x = -player.maxSpeed;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			if (player.velocity.x < player.maxSpeed) player.velocity.x += 2.0f;
			else                                     player.velocity.x = player.maxSpeed;
		}


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
