#pragma once
#include "BoneGizmo.h"

BoneGizmo::BoneGizmo() { this->drawSkeleton = true; }

void BoneGizmo::draw_bone_gizmo(std::shared_ptr<eeng::RenderableMesh> characterMesh, ShapeRendererPtr shapeRenderer, glm::mat4 characterWorldMatrix) const {

	float axisLen = 25.0f;
	
	if (drawSkeleton) {
		
		for (int i = 0; i < characterMesh->boneMatrices.size(); ++i) {
			auto IBinverse = glm::inverse(characterMesh->m_bones[i].inversebind_tfm);

			glm::mat4 global = characterWorldMatrix * characterMesh->boneMatrices[i] * IBinverse;
			glm::vec3 pos = glm::vec3(global[3]);
			glm::vec3 right = glm::vec3(global[0]);
			glm::vec3 up = glm::vec3(global[1]);
			glm::vec3 fwd = glm::vec3(global[2]);

			shapeRenderer->push_states(ShapeRendering::Color4u::Red);
			shapeRenderer->push_line(pos, pos + axisLen * right);

			shapeRenderer->push_states(ShapeRendering::Color4u::Green);
			shapeRenderer->push_line(pos, pos + axisLen * up);

			shapeRenderer->push_states(ShapeRendering::Color4u::Blue);
			shapeRenderer->push_line(pos, pos + axisLen * fwd);

			shapeRenderer->pop_states<ShapeRendering::Color4u>();
			shapeRenderer->pop_states<ShapeRendering::Color4u>();
			shapeRenderer->pop_states<ShapeRendering::Color4u>();
		}
	}
}

void BoneGizmo::toggle_bone_gizmo() {
	std::cout << drawSkeleton << std::endl;
	drawSkeleton = !drawSkeleton;
	std::cout << drawSkeleton << std::endl;
}

BoneGizmo::~BoneGizmo() {};