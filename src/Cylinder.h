//
//  Cylinder.hpp
//  
//
//  Created by Bret Jackson on 2/3/17.
//
//

#ifndef Cylinder_hpp
#define Cylinder_hpp

#include <stdio.h>
#include <Model.h>
#include <GLSLProgram.h>
#define GLM_FORCE_RADIANS
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_access.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtx/quaternion.hpp>

namespace basicgraphics {

	class Cylinder {
	public:

		/*!
		 * Creates a cylinder where the base is centered at point 1 and the top is centered at point2 with the specified radius and color.
		 */
		Cylinder(const glm::vec3 &point1, const glm::vec3 &point2, const float radius, const glm::vec4 &color);
		virtual ~Cylinder();

		virtual void draw(GLSLProgram &shader, const glm::mat4 &modelMatrix);

	protected:
		std::unique_ptr<Model> _model;
		const glm::vec3 _point1;
		const glm::vec3 _point2;
		glm::mat4 _localMat;
		const float _radius;
		const glm::vec4 _color;
	};

}

#endif /* Cylinder_hpp */
