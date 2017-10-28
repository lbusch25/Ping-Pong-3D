//
//  Sphere.cpp
//  
//
//  Created by Bret Jackson on 2/3/17.
//
//

#include "Sphere.h"

namespace basicgraphics {

	Sphere::Sphere(const glm::vec3 &position, const float radius, const glm::vec4 &color) : _position(position), _radius(radius), _color(color)
	{
		_model.reset(new Model("sphere.obj", radius, _color));
		//_model.reset(new Model("s 2.0 2.0 0.0 5.0"));

	}

	Sphere::~Sphere()
	{

	}

	void Sphere::draw(GLSLProgram &shader, const glm::mat4 &modelMatrix) {

		glm::mat4 translate = glm::translate(glm::mat4(1.0), _position);
		glm::mat4 model = modelMatrix * translate;
		shader.setUniform("model_mat", model);
		_model->draw(shader);
		shader.setUniform("model_mat", modelMatrix);
	}

}