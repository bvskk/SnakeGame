#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <vector>
#include <algorithm>

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

struct Snakeblock {
	int x;
	int y;
};

bool operator==(Snakeblock sb1, Snakeblock sb2)
{
	return sb1.x == sb2.x && sb1.y == sb2.y;
}

typedef Snakeblock Fruit;

struct Snake {
	int size;
	Direction dir;
	std::vector<Snakeblock> body;
};

void processInput(GLFWwindow* window, Snake* snake);
bool progressSnake(Snake* snake);
void generateFruit(Fruit* fruit, std::mt19937* mt, Snake* snake);
inline void growSnake(Snake* snake);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Snake", NULL, NULL);
	if (window == NULL)
	{
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeLimits(window, 800, 800, 800, 800);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -1;
	}

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec4 aPos;\n"
		"uniform mat4 model;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = model * aPos;\n"
		"}\0";

	const char* bodyfragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(0.0f, 0.5f, 0.0f, 1.0f);\n"
		"}\0";

	const char* fruitfragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
		"}\0";

	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &bodyfragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	unsigned int bodyshaderProgram, fruitshaderProgram;
	bodyshaderProgram = glCreateProgram();
	glAttachShader(bodyshaderProgram, vertexShader);
	glAttachShader(bodyshaderProgram, fragmentShader);
	glLinkProgram(bodyshaderProgram);
	glShaderSource(fragmentShader, 1, &fruitfragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	fruitshaderProgram = glCreateProgram();
	glAttachShader(fruitshaderProgram, vertexShader);
	glAttachShader(fruitshaderProgram, fragmentShader);
	glLinkProgram(fruitshaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		0.05f, 0.05f, 0.0f, 1.0f,
		-0.05f, 0.05f, 0.0f, 1.0f,
		-0.05f, -0.05f, 0.0f, 1.0f,
		0.05f, -0.05f, 0.0f, 1.0f,
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	unsigned int bodyBlockVAO, fruitVAO, blockVBO, blockEBO;
	glGenVertexArrays(1, &bodyBlockVAO);
	glGenVertexArrays(1, &fruitVAO);
	glGenBuffers(1, &blockEBO);
	glGenBuffers(1, &blockVBO);
	glBindVertexArray(bodyBlockVAO);
	glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blockEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(fruitVAO);
	glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blockEBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glViewport(0, 0, 800, 800);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Snake snake;
	snake.size = 2;
	snake.dir = RIGHT;
	snake.body = std::vector<Snakeblock>(400);
	snake.body[0] = { 0, 0 };
	snake.body[1] = { 0, 1 };

	float last = 0.0f;
	float current;

	Fruit fruit = {19, 19};

	std::mt19937 mt{ std::random_device{}() };

	while (!glfwWindowShouldClose(window))
	{
		current = (float)glfwGetTime();
		if (current - last > 0.125f)
		{
			last = current;
			if (!progressSnake(&snake))
			{
				return 0;
			}
		}
		if (snake.body[0] == fruit)
		{
			growSnake(&snake);
			generateFruit(&fruit, &mt, &snake);
		}
		processInput(window, &snake);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(fruitVAO);
		glUseProgram(fruitshaderProgram);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.95f + fruit.x * 0.1f, 0.95f - fruit.y * 0.1f, 0.0f));
		unsigned int modelloc = glGetUniformLocation(fruitshaderProgram, "model");
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(bodyBlockVAO);
		glUseProgram(bodyshaderProgram);

		for (int i = 0; i < snake.size; ++i) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-0.95f + snake.body[i].x * 0.1f, 0.95f - snake.body[i].y * 0.1f, 0.0f));
			unsigned int modelloc = glGetUniformLocation(bodyshaderProgram, "model");
			glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void processInput(GLFWwindow* window, Snake* snake)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && snake->dir != DOWN)
	{
		snake->dir = UP;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && snake->dir != UP)
	{
		snake->dir = DOWN;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && snake->dir != LEFT)
	{
		snake->dir = RIGHT;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && snake->dir != RIGHT)
	{
		snake->dir = LEFT;
	}
}

bool progressSnake(Snake* snake)
{
	for (int i = snake->size - 1; i > 0; --i)
	{
		snake->body[i] = snake->body[i - 1];
	}
	switch (snake->dir)
	{
	case UP:
	{
		snake->body[0].y -= 1;
		if (snake->body[0].y < 0) snake->body[0].y = 19;
	} break;
	case DOWN:
	{
		snake->body[0].y += 1;
		if (snake->body[0].y > 19) snake->body[0].y = 0;
	} break;
	case RIGHT:
	{
		snake->body[0].x += 1;
		if (snake->body[0].x > 19) snake->body[0].x = 0;
	} break;
	case LEFT:
	{
		snake->body[0].x -= 1;
		if (snake->body[0].x < 0) snake->body[0].x = 19;
	} break;
	}
	if (std::find(snake->body.begin() + 1, snake->body.begin() + snake->size, snake->body[0]) != snake->body.begin() + snake->size)
	{
		return false;
	}
	return true;
}

void generateFruit(Fruit* fruit, std::mt19937* mt, Snake* snake)
{
	std::uniform_int_distribution<> tiles{0, 19};
	while (std::find(snake->body.begin(), snake->body.end(), *fruit) != snake->body.end())
	{
		fruit->x = tiles(*mt);
		fruit->y = tiles(*mt);
	}
}

void growSnake(Snake* snake)
{
	if (snake->body[snake->size - 1].x - 1 == snake->body[snake->size - 2].x)
	{
		snake->body[snake->size].x = snake->body[snake->size - 1].x + 1;
	}
	else
	{
		snake->body[snake->size].x = snake->body[snake->size - 1].x - 1;
	}
	if (snake->body[snake->size - 1].y - 1 == snake->body[snake->size - 2].y)
	{
		snake->body[snake->size].y = snake->body[snake->size - 1].y + 1;
	}
	else
	{
		snake->body[snake->size].y = snake->body[snake->size - 1].y - 1;
	}
	++snake->size;
}