/**
@file App.h

The default starter app is configured for OpenGL 3.3 and relatively recent
GPUs.
*/
#ifndef App_h
#define App_h

#include "BaseApp.h"
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace basicgraphics {
	class App : public BaseApp {
	public:

		App(int argc, char** argv, std::string windowName, int windowWidth, int windowHeight);
		~App();

	protected:

		const float BALL_RADIUS = 2.0;
		const float TABLE_HALF_WIDTH = 76.25;
		const float TABLE_HALF_LENGTH = 137;
		const float TABLE_THICKNESS = 4.0;
		const float NET_HEIGHT = 15.25;
		const float NET_OVERHANG = 15.25;

		const int NUM_NET_HORIZ_LINES = 10;
		const int NUM_NET_VERT_LINES = 100;
		const float NET_LINE_RADIUS = 0.25;

		const float HORIZ_LINE_HALF_LENGTH = TABLE_HALF_WIDTH + NET_OVERHANG;

		const float PADDLE_COLLISION_RADIUS = 8.0 + BALL_RADIUS;

		const float SHADOW_HEIGHT = 0.2f;

		void onRenderGraphics() override;
		void onEvent(std::shared_ptr<Event> event) override;
		void resetBall();
		void onSimulation(double rdt);

		// Use these functions to access the current state of the paddle!
		glm::vec3 getPaddlePosition() { return glm::column(paddleFrame, 3); }
		glm::vec3 getPaddleNormal() { return glm::vec3(0, 0, -1); }
		glm::vec3 getPaddleVelocity() { return paddleVel; }

		// The paddle is drawn with two cylinders
		std::unique_ptr<Cylinder> paddle;
		std::unique_ptr<Cylinder> handle;

		std::unique_ptr<Box> table_box;
		std::unique_ptr<Sphere> pBall;
		
		std::unique_ptr<Line> midLine;
		std::unique_ptr<Line> leftLine;
		std::unique_ptr<Line> rightLine;
		std::unique_ptr<Line> frontLine;
		std::unique_ptr<Line> backLine;

		std::unique_ptr<Cylinder> ballShadow;
		std::unique_ptr<Cylinder> paddleShadow;

		std::vector<std::unique_ptr<Line>> netLines;


		// This 4x4 matrix stores position and rotation data for the paddle.
		glm::mat4 paddleFrame;

		// This vector stores the paddle's current velocity.
		glm::vec3 paddleVel;
		glm::vec3 ballVel;
		glm::vec3 ballPos;
		glm::vec3 prevBallPos;
		glm::vec3 prevPaddlePos;

		int bouncesThisSide;
		bool winnerDeclared;
		bool doSimulation;

		// This holds the time value for the last time onSimulate was called
		double lastTime;
	};
}

#endif