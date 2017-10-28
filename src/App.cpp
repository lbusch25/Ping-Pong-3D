

/** \file App.cpp */
#include "App.h"
#include <iostream>
using namespace std;
using namespace glm;

namespace basicgraphics {

	App::App(int argc, char** argv, std::string windowName, int windowWidth, int windowHeight) : BaseApp(argc, argv, windowName, windowWidth, windowHeight) {
		lastTime = glfwGetTime();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0);

		// Initialize the cylinders that make up the model. We're using unique_ptrs here so they automatically deallocate.
		paddle.reset(new Cylinder(vec3(0, 0, -0.5), vec3(0, 0, 0.5), 8.0, vec4(0.5, 0, 0, 1.0)));
		handle.reset(new Cylinder(vec3(0, -7.5, 0), vec3(0, -16, 0), 1.5, vec4(0.3, 0.4, 0, 1.0)));
		//CHANGE COLOR
		table_box.reset(new Box(vec3(-TABLE_HALF_WIDTH, -TABLE_THICKNESS, -TABLE_HALF_LENGTH), vec3(TABLE_HALF_WIDTH, 0, TABLE_HALF_LENGTH), vec4(0.0, 1.0, 0.0, 1.0)));

		pBall.reset(new Sphere(vec3(0, 0, 0), BALL_RADIUS, vec4(1.0, 1.0, 1.0, 1.0)));

		ballShadow.reset(new Cylinder(vec3(0), vec3(0, 0.1, 0), BALL_RADIUS, vec4(0, 0, 0, 1.0)));
		paddleShadow.reset(new Cylinder(vec3(0), vec3(0, 0.1, 0), 8.0, vec4(0, 0, 0, 1.0)));

		//Table lines
		midLine.reset(new Line(vec3(0, 0.1, -TABLE_HALF_LENGTH), vec3(0, 0.1, TABLE_HALF_LENGTH), vec3(0, 1, 0), 1.0, vec4(1)));

		leftLine.reset(new Line(vec3(-TABLE_HALF_WIDTH + 0.6f, 0.1, TABLE_HALF_LENGTH), vec3(-TABLE_HALF_WIDTH + 0.6f, 0.1, -TABLE_HALF_LENGTH), vec3(0, 1, 0), 1.0, vec4(1)));
		rightLine.reset(new Line(vec3(TABLE_HALF_WIDTH - 0.6f, 0.1, TABLE_HALF_LENGTH), vec3(TABLE_HALF_WIDTH - 0.6f, 0.1, -TABLE_HALF_LENGTH), vec3(0, 1, 0), 1.0, vec4(1)));

		frontLine.reset(new Line(vec3(-TABLE_HALF_WIDTH, 0.1, TABLE_HALF_LENGTH - 0.5f), vec3(TABLE_HALF_WIDTH, 0.1, TABLE_HALF_LENGTH - 0.5f), vec3(0, 1, 0), 1.0, vec4(1)));

		backLine.reset(new Line(vec3(-TABLE_HALF_WIDTH, 0.1, -TABLE_HALF_LENGTH + 0.5f), vec3(TABLE_HALF_WIDTH, 0.1, -TABLE_HALF_LENGTH + 0.5f), vec3(0, 1, 0), 1.0, vec4(1)));

		//Net
		for (int i = 0; i <= NUM_NET_HORIZ_LINES; i++)
		{
			vec4 color;
			float radius;
			float z;
			if (i == 0 || i == NUM_NET_HORIZ_LINES) {
				color = vec4(0.5, 0.5, 0.5, 1);
				radius = 0.5;
				z = 1;
			}
			else {
				color = vec4(0, 0, 0, 1);
				radius = NET_LINE_RADIUS;
				z = 0;
			}

			float y = (i / (float)NUM_NET_HORIZ_LINES) * NET_HEIGHT;
			Line *line = new Line(vec3(-HORIZ_LINE_HALF_LENGTH, y + 0.5f, z), vec3(HORIZ_LINE_HALF_LENGTH, y + 0.5f, z), vec3(0, 0, 1), radius, color);
			netLines.emplace_back((std::move(line)));
		}

		for (int i = 0; i <= NUM_NET_VERT_LINES; i++)
		{
			vec4 color;
			float radius;
			float z;
			if (i == 0 || i == NUM_NET_VERT_LINES) {
				color = vec4(0.5, 0.5, 0.5, 1);
				radius = 0.5;
				z = 1;
			}
			else {
				color = vec4(0, 0, 0, 1);
				radius = NET_LINE_RADIUS;
				z = 0;
			}

			float x = -HORIZ_LINE_HALF_LENGTH + i * 1.83;
			Line *line = new Line(vec3(x, 0.5f, z), vec3(x, NET_HEIGHT + 0.5f, z), vec3(0, 0, 1), radius, color);
			netLines.emplace_back((std::move(line)));
		}

		prevPaddlePos = getPaddlePosition();
		bouncesThisSide = 0;
		winnerDeclared = false;

		doSimulation = false;
		ballPos = vec3(200, -1000, 0);
	}

	App::~App()
	{
	}


	void App::onEvent(shared_ptr<Event> event) {
		string name = event->getName();
		if (name == "kbd_ESC_down") {
			glfwSetWindowShouldClose(_window, 1);
		}
		else if (name == "mouse_pointer") {
			vec2 mouseXY = event->get2DData();

			int width, height;
			glfwGetWindowSize(_window, &width, &height);

			// This block of code maps the 2D position of the mouse in screen space to a 3D position
			// 20 cm above the ping pong table.  It also rotates the paddle to make the handle move
			// in a cool way.  It also makes sure that the paddle does not cross the net and go onto
			// the opponent's side.
			float xneg1to1 = mouseXY.x / width * 2.0 - 1.0;
			float y0to1 = mouseXY.y / height;
			mat4 rotZ = toMat4(angleAxis(glm::sin(-xneg1to1), vec3(0, 0, 1)));

			glm::vec3 lastPaddlePos = glm::column(paddleFrame, 3);
			paddleFrame = glm::translate(mat4(1.0), vec3(xneg1to1 * 100.0, 20.0, glm::max(y0to1 * 137.0 + 20.0, 0.0))) * rotZ;
			vec3 newPos = glm::column(paddleFrame, 3);

			// This is a weighted average.  Update the velocity to be 10% the velocity calculated 
			// at the previous frame and 90% the velocity calculated at this frame.
			paddleVel = 0.1f*paddleVel + 0.9f*(newPos - lastPaddlePos);
		}
		else if (name == "kbd_SPACE_up") {
			// This is where you can "serve" a new ball from the opponent's side of the net 
			// toward you when the spacebar is released. I found that a good initial position for the ball is: (0, 30, -130).  
			// And, a good initial velocity is (0, 200, 400).  As usual for this program, all 
			// units are in cm.
			ballPos = vec3(0.0, 30.0, -137.0);
			prevBallPos = vec3(0.0, 30.0, -137.0);
			ballVel = vec3(0, 200, 400);
			winnerDeclared = false;
			bouncesThisSide = 0;
			doSimulation = true;
		}

	}


	void App::onSimulation(double rdt) {
		if (!doSimulation)
			return;
		
		// rdt is the change in time (dt) in seconds since the last call to onSimulation
		// So, you can slow down the simulation by half if you divide it by 2.
		rdt *= 0.25;

		vec3 paddlePos = getPaddlePosition();

		// Here are a few other values that you may find useful..
		// Radius of the ball = 2cm
		// Radius of the paddle = 8cm
		// Acceleration due to gravity = 981cm/s^2 in the negative Y direction
		// See the diagram in the assignment handout for the dimensions of the ping pong table
		if (ballPos.y <= BALL_RADIUS && ballVel.y < 0 &&
			abs(ballPos.x) <= TABLE_HALF_WIDTH && abs(ballPos.z) < TABLE_HALF_LENGTH) {
			//Since the normal vector is always (0, 1, 0), the math works out such that
			//reflection just negates y.
			ballVel.y = -ballVel.y;
			ballPos.y = BALL_RADIUS;
			ballVel *= 0.85;

			bouncesThisSide++;
		}

		if (!winnerDeclared) {
			if (ballPos.y < 0) {
				if (ballVel.z > 0) {
					if (bouncesThisSide == 1) {
						cout << "You lost! You missed." << endl;
					}
					else {
						cout << "You won! The other player hit the ball off the table." << endl;
					}
				}
				else {
					if (bouncesThisSide == 1) {
						cout << "You won! The other player missed." << endl;
					}
					else {
						cout << "You lost! You hit the ball off the table." << endl;
					}
				}
				winnerDeclared = true;
			}

			if (bouncesThisSide == 2) {
				if (ballPos.z > 0)
					cout << "You lost! 2 bounces on your side." << endl;
				else
					cout << "You won! 2 bounces on the other player's side." << endl;
				winnerDeclared = true;
			}
		}

		//Test for ball passing through net
		if (sign(ballPos.z) != sign(prevBallPos.z)) {
			bouncesThisSide = 0;
			if (ballPos.y < NET_HEIGHT) {

				ballPos.z = -ballPos.z;
				ballVel.z = -ballVel.z;

				ballVel.y *= 0.1;
				ballVel.x *= 0.1;
				ballVel.z *= 0.5;
			}
		}
		//Test for ball hitting paddle
		if (ballVel.z > 0) { //If ball is moving towards paddle
            if (paddlePos - prevPaddlePos == vec3(0)) {
                if(glm::distance(ballPos, paddlePos) < PADDLE_COLLISION_RADIUS) {
                    ballPos.z = paddlePos.z - (BALL_RADIUS * 2);
                    ballVel.z = -ballVel.z;
                }
            } else {
                Line paddleMovementLine(paddlePos, prevPaddlePos, vec3(0), 0, vec4(0));

                if (glm::distance(ballPos, paddleMovementLine.closestPoint(ballPos)) < PADDLE_COLLISION_RADIUS) {
                    ballPos.z = paddlePos.z - (BALL_RADIUS * 2);
                    
                    ballVel.z = -ballVel.z;
                    
                    ballVel.x += paddleVel.x * 200.0f;
                    ballVel.z += paddleVel.z * 100.0f;
                }
            }
		}


		ballVel.y -= 981 * rdt;
		prevBallPos = ballPos;
		ballPos += ballVel * (float)rdt;
		prevPaddlePos = paddlePos;
	}


	void App::onRenderGraphics() {

		double curTime = glfwGetTime();
		onSimulation(curTime - lastTime);
		lastTime = curTime;

		// Setup the camera with a good initial position and view direction to see the table
		glm::vec3 eye_world = glm::vec3(0, 100, 250);
		glm::mat4 view = glm::lookAt(eye_world, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//eye_world = glm::vec3(glm::column(glm::inverse(view), 3));

		// Setup the projection matrix so that things are rendered in perspective
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)_windowWidth / (GLfloat)_windowHeight, 0.1f, 500.0f);
		// Setup the model matrix
		glm::mat4 model = glm::mat4(1.0);

		//Translation Matrix for pBall
		glm::mat4 translate = glm::translate(model, ballPos);

		// Update shader variables
		_shader.setUniform("view_mat", view);
		_shader.setUniform("projection_mat", projection);
		_shader.setUniform("model_mat", model);
		_shader.setUniform("eye_world", eye_world);

		for (int i = 0; i < netLines.size(); i++) {
			netLines[i]->draw(_shader, model);
		}

		midLine->draw(_shader, model);
		leftLine->draw(_shader, model);
		rightLine->draw(_shader, model);
		frontLine->draw(_shader, model);
		backLine->draw(_shader, model);

		// Draw the paddle using two cylinders
		paddle->draw(_shader, paddleFrame);
		handle->draw(_shader, paddleFrame);

		table_box->draw(_shader, model);
		pBall->draw(_shader, translate);
		if (abs(ballPos.x) <= TABLE_HALF_WIDTH && abs(ballPos.z) < TABLE_HALF_LENGTH) {
			ballShadow->draw(_shader, glm::translate(mat4(1), vec3(ballPos.x, SHADOW_HEIGHT, ballPos.z)));
		}
		paddleShadow->draw(_shader, glm::translate(mat4(1), vec3(getPaddlePosition().x, SHADOW_HEIGHT, getPaddlePosition().z)));

		// Check for any opengl errors
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		}
	}

}
