#include <raylib.h>
#include <raygui.h>

#define SHADER_FILENAME "shader.glsl"

typedef struct {
	float time;
	Vector2 mouse;
	Vector2 resolution;
	int timeLoc;
	int mouseLoc;
	int resLoc;
} Uniforms;

typedef struct {
	void *data;
	char name[255];
	int location;
	void (*update_func)(void);
} Uniform;

void draw(Texture text, Shader sh);
void updateShader(Shader *sh);
void handleInput(Shader *sh, Uniforms *u);
void resizeTexture(Texture *text, int width, int height);
void setupUniforms(Shader *sh, Uniforms *u);
void updateUniforms(Shader *sh, Uniforms *u);
void addUniform(Shader *sh, Uniform u);

Uniform uniforms[255];
int uniformcnt = 0;

int
main(int argc, char **argv)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);
	InitWindow(1920, 1080, "Shader tester");

	Texture text;
	long lastmodtime = GetFileModTime(SHADER_FILENAME);

	resizeTexture(&text, 1920, 1080);

	Shader shader = LoadShader(0, SHADER_FILENAME);

	Uniforms u = {
		.mouse = (Vector2){0, 0},
		.time = 0.0f,
		.resolution = (Vector2) {0, 0}
	};

	setupUniforms(&shader, &u);

	while (!WindowShouldClose())
	{
		updateUniforms(&shader, &u);
		draw(text, shader);
		handleInput(&shader, &u);

		if (IsWindowResized())
			resizeTexture(&text, GetScreenWidth(), GetScreenHeight());

		if (GetFileModTime(SHADER_FILENAME) != lastmodtime) {
			lastmodtime = GetFileModTime(SHADER_FILENAME);
			updateShader(&shader);
			updateUniforms(&shader, &u);
		}
	}

	UnloadShader(shader);
	UnloadTexture(text);
}

void
draw(Texture text, Shader sh)
{
	BeginDrawing();
		ClearBackground(WHITE);
		BeginShaderMode(sh);
			DrawTexture(text, 0, 0, BLANK);
		EndShaderMode();
		DrawFPS(10, 10);
	EndDrawing();
}

void
updateShader(Shader *sh)
{
	UnloadShader(*sh);
	*sh = LoadShader(0, "shader.glsl");
}

void
handleInput(Shader *sh, Uniforms *u)
{
	if (IsKeyPressed(KEY_R)) {
		updateShader(sh);
		setupUniforms(sh, u);
	}
}

void
resizeTexture(Texture *text, int width, int height)
{
	UnloadTexture(*text);
	Image blank = GenImageColor(width, height, BLANK);
	*text = LoadTextureFromImage(blank);
	UnloadImage(blank);
}

void
setupUniforms(Shader *sh, Uniforms *u)
{
	u->timeLoc = GetShaderLocation(*sh, "u_time");
	u->mouseLoc = GetShaderLocation(*sh, "u_mouse");
	u->resLoc = GetShaderLocation(*sh, "u_resolution");
}

void
updateUniforms(Shader *sh, Uniforms *u)
{
	u->time = (float)GetTime();
	u->mouse = GetMousePosition();
	u->resolution = (Vector2) {GetScreenWidth(), GetScreenHeight()};

	SetShaderValue(*sh, u->timeLoc, &u->time, SHADER_UNIFORM_FLOAT);
	SetShaderValue(*sh, u->mouseLoc, &u->mouse, SHADER_UNIFORM_VEC2);
	SetShaderValue(*sh, u->resLoc, &u->resolution, SHADER_UNIFORM_VEC2);
}

void
addUniform(Shader *sh, Uniform u)
{
	if (uniformcnt == 255) {
		TraceLog(LOG_ERROR, "Max uniform count reached!");
		return;
	}

	u.location = GetShaderLocation(*sh, u.name);
	uniforms[uniformcnt++] = u;
}
