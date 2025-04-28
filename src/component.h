#pragma once
#include <glm/glm.hpp>
#include "iostream"
#include "RenderableMesh.hpp"

struct TransformComponent {
	glm::vec3 translation;
	float pitch, yaw, roll;
	glm::vec3 scale;
};

struct LinearVelocityComponent {
	glm::vec3 velocity;
	float speed = 20;
};

struct MeshComponent {
	std::shared_ptr<eeng::RenderableMesh> renderable_mesh;
};

struct PlayerControllerComponent {
	glm::vec3 fwd, right, vertical;
	bool grounded = true; //should probably be in a "physics"-esque component
	float jumpforce = 10;
	float gravity = 4.9;
	eeng::InputManager::Key W = eeng::InputManager::Key::W;
	eeng::InputManager::Key A = eeng::InputManager::Key::A;
	eeng::InputManager::Key S = eeng::InputManager::Key::S;
	eeng::InputManager::Key D = eeng::InputManager::Key::D;
	eeng::InputManager::Key spacebar = eeng::InputManager::Key::Space;
};

struct NPCControllerComponent {
	const float angular_velocity = 0.5;
	int pp_index = 0;
	const int pp_max = 5;
	const float proximity_value = 0.1f;
	std::vector<glm::vec3> path_points;
};

struct RayComponent {

};

struct AABBComponent {
	eeng::AABB mesh_aabb;
};