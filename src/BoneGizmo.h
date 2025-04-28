#pragma once
#include "RenderableMesh.hpp"
#include "ShapeRenderer.hpp"
class BoneGizmo {

	bool drawSkeleton;
public:

	BoneGizmo();
	void draw_bone_gizmo(std::shared_ptr<eeng::RenderableMesh> characterMesh, ShapeRendererPtr shapeRenderer,glm::mat4 characterWorldMatrix) const;
	void toggle_bone_gizmo();

	~BoneGizmo();
};
