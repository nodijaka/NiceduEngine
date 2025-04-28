
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include "imgui.h"
#include "Log.hpp"
#include "Game.hpp"

bool Game::init()
{
    win_open = true;
    p_win_open = &win_open;
    boneGizmo = new BoneGizmo();

    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();

    // Do some entt stuff
    entity_registry = std::make_shared<entt::registry>();
    auto ent1 = entity_registry->create();
    struct Tfm
    {
        float x, y, z;
    };
    entity_registry->emplace<Tfm>(ent1, Tfm{});

    // Grass
    grassMesh = std::make_shared<eeng::RenderableMesh>();
    grassMesh->load("assets/grass/grass_trees_merged2.fbx", false);

    // Horse
    horseMesh = std::make_shared<eeng::RenderableMesh>();
    horseMesh->load("assets/Animals/Horse.fbx", false);

    // Character
    characterMesh = std::make_shared<eeng::RenderableMesh>();
#if 0
    // Character
    characterMesh->load("assets/Ultimate Platformer Pack/Character/Character.fbx", false);
#endif
#if 0
    // Enemy
    characterMesh->load("assets/Ultimate Platformer Pack/Enemies/Bee.fbx", false);
#endif
#if 0
    // ExoRed 5.0.1 PACK FBX, 60fps, No keyframe reduction
    characterMesh->load("assets/ExoRed/exo_red.fbx");
    characterMesh->load("assets/ExoRed/idle (2).fbx", true);
    characterMesh->load("assets/ExoRed/walking.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif
#if 1
    // Amy 5.0.1 PACK FBX
    characterMesh->load("assets/Amy/Ch46_nonPBR.fbx");
    characterMesh->load("assets/Amy/idle.fbx", true);
    characterMesh->load("assets/Amy/walking.fbx", true);
    characterMesh->load("assets/Amy/jump.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif
#if 0
    // Eve 5.0.1 PACK FBX
    // Fix for assimp 5.0.1 (https://github.com/assimp/assimp/issues/4486)
    // FBXConverter.cpp, line 648: 
    //      const float zero_epsilon = 1e-6f; => const float zero_epsilon = Math::getEpsilon<float>();
    characterMesh->load("assets/Eve/Eve By J.Gonzales.fbx");
    characterMesh->load("assets/Eve/idle.fbx", true);
    characterMesh->load("assets/Eve/walking.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif

    grassWorldMatrix = glm_aux::TRS(
        { 0.0f, 0.0f, 0.0f },
        0.0f, { 0, 1, 0 },
        { 100.0f, 100.0f, 100.0f });

    horseWorldMatrix = glm_aux::TRS(
        { 30.0f, 0.0f, -35.0f },
        35.0f, { 0, 1, 0 },
        { 0.01f, 0.01f, 0.01f });
    CreateEntities();
    InitPlayer();
    return true;
}

void Game::update(
    float time,
    float deltaTime,
    InputManagerPtr input)
{
    Time(time);
    updateCamera(input);

    //updatePlayer(deltaTime, input);
    PlayerControllerSystem(input);
    NPCControllerSystem();
    MovementSystem(deltaTime);

    pointlight.pos = glm::vec3(
        glm_aux::R(time * 0.1f, { 0.0f, 1.0f, 0.0f }) *
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f));

    characterWorldMatrix1 = glm_aux::TRS(
        player.pos,
        0.0f, { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

    characterWorldMatrix2 = glm_aux::TRS(
        { -3, 0, 0 },
        time * glm::radians(50.0f), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

    characterWorldMatrix3 = glm_aux::TRS(
        { 3, 0, 0 },
        time * glm::radians(50.0f), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

    // Intersect player view ray with AABBs of other objects 
    glm_aux::intersect_ray_AABB(player.viewRay, character_aabb2.min, character_aabb2.max);
    glm_aux::intersect_ray_AABB(player.viewRay, character_aabb3.min, character_aabb3.max);
    glm_aux::intersect_ray_AABB(player.viewRay, horse_aabb.min, horse_aabb.max);

    // We can also compute a ray from the current mouse position,
    // to use for object picking and such ...
    if (input->GetMouseState().rightButton)
    {
        glm::ivec2 windowPos(camera.mouse_xy_prev.x, matrices.windowSize.y - camera.mouse_xy_prev.y);
        auto ray = glm_aux::world_ray_from_window_coords(windowPos, matrices.V, matrices.P, matrices.VP);
        // Intersect with e.g. AABBs ...

        eeng::Log("Picking ray origin = %s, dir = %s",
            glm_aux::to_string(ray.origin).c_str(),
            glm_aux::to_string(ray.dir).c_str());


    }
}

void Game::render(
    float time,
    int windowWidth,
    int windowHeight)
{
    renderMyWindow(p_win_open);
    renderUI();

    matrices.windowSize = glm::ivec2(windowWidth, windowHeight);

    // Projection matrix
    const float aspectRatio = float(windowWidth) / windowHeight;
    matrices.P = glm::perspective(glm::radians(60.0f), aspectRatio, camera.nearPlane, camera.farPlane);

    // View matrix
    matrices.V = glm::lookAt(camera.pos, camera.lookAt, camera.up);

    matrices.VP = glm_aux::create_viewport_matrix(0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f);

    // Begin rendering pass
    forwardRenderer->beginPass(matrices.P, matrices.V, pointlight.pos, pointlight.color, camera.pos);

    RenderSystem(time);
    BoneTest(time);
    // Grass
    forwardRenderer->renderMesh(grassMesh, grassWorldMatrix);
    grass_aabb = grassMesh->m_model_aabb.post_transform(grassWorldMatrix);

    // Horse
    horseMesh->animate(3, time);
    forwardRenderer->renderMesh(horseMesh, horseWorldMatrix);
    horse_aabb = horseMesh->m_model_aabb.post_transform(horseWorldMatrix);

    // Character, instance 1
    characterMesh->animate(characterAnimIndex, time * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix1);
    character_aabb1 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix1);

    // Character, instance 2
    characterMesh->animate(1, time * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix2);
    character_aabb2 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix2);

    // Character, instance 3
    characterMesh->animate(2, time * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix3);
    character_aabb3 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix3);

    // End rendering pass
    drawcallCount = forwardRenderer->endPass();

    // Draw player view ray
    if (player.viewRay)
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xff00ff00 });
        shapeRenderer->push_line(player.viewRay.origin, player.viewRay.point_of_contact());
    }
    else
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xffffffff });
        shapeRenderer->push_line(player.viewRay.origin, player.viewRay.origin + player.viewRay.dir * 100.0f);
    }
    shapeRenderer->pop_states<ShapeRendering::Color4u>();

    // Draw object bases
    {
        shapeRenderer->push_basis_basic(characterWorldMatrix1, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix2, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix3, 1.0f);
        shapeRenderer->push_basis_basic(grassWorldMatrix, 1.0f);
        shapeRenderer->push_basis_basic(horseWorldMatrix, 1.0f);
    }

    // Draw AABBs
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
        shapeRenderer->push_AABB(character_aabb1.min, character_aabb1.max);
        shapeRenderer->push_AABB(character_aabb2.min, character_aabb2.max);
        shapeRenderer->push_AABB(character_aabb3.min, character_aabb3.max);
        shapeRenderer->push_AABB(horse_aabb.min, horse_aabb.max);
        shapeRenderer->push_AABB(grass_aabb.min, grass_aabb.max);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
    }

#if 0
    // Demo draw other shapes
    {
        shapeRenderer->push_states(glm_aux::T(glm::vec3(0.0f, 0.0f, -5.0f)));
        ShapeRendering::DemoDraw(shapeRenderer);
        shapeRenderer->pop_states<glm::mat4>();
    }
#endif

    // Draw shape batches
    shapeRenderer->render(matrices.P * matrices.V);
    shapeRenderer->post_render();
}

void Game::renderMyWindow(bool* p_open) 
{
    ImGui::Begin("MyWindow", p_open);
    if (ImGui::Button("BoneGizmo")) {
        boneGizmo->toggle_bone_gizmo();
    }  
    ImGui::End();
}

void Game::renderUI()
{
    ImGui::Begin("Game Info");

    ImGui::Text("Drawcall count %i", drawcallCount);

    ImGui::Text("Total Time %i:%i", time_minutes, time_seconds);
    if (ImGui::ColorEdit3("Light color",
        glm::value_ptr(pointlight.color),
        ImGuiColorEditFlags_NoInputs))
    {
    }

    if (characterMesh)
    {
        // Combo (drop-down) for animation clip
        int curAnimIndex = characterAnimIndex;
        std::string label = (curAnimIndex == -1 ? "Bind pose" : characterMesh->getAnimationName(curAnimIndex));
        if (ImGui::BeginCombo("Character animation##animclip", label.c_str()))
        {
            // Bind pose item
            const bool isSelected = (curAnimIndex == -1);
            if (ImGui::Selectable("Bind pose", isSelected))
                curAnimIndex = -1;
            if (isSelected)
                ImGui::SetItemDefaultFocus();

            // Clip items
            for (int i = 0; i < characterMesh->getNbrAnimations(); i++)
            {
                const bool isSelected = (curAnimIndex == i);
                const auto label = characterMesh->getAnimationName(i) + "##" + std::to_string(i);
                if (ImGui::Selectable(label.c_str(), isSelected))
                    curAnimIndex = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
            characterAnimIndex = curAnimIndex;
        }

        // In-world position label
        const auto VP_P_V = matrices.VP * matrices.P * matrices.V;
        auto world_pos = glm::vec3(horseWorldMatrix[3]);
        glm::ivec2 window_coords;
        if (glm_aux::window_coords_from_world_pos(world_pos, VP_P_V, window_coords))
        {
            ImGui::SetNextWindowPos(
                ImVec2{ float(window_coords.x), float(matrices.windowSize.y - window_coords.y) },
                ImGuiCond_Always,
                ImVec2{ 0.0f, 0.0f });
            ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x80000000);
            ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoInputs |
                // ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin("window_name", nullptr, flags))
            {
                ImGui::Text("In-world GUI element");
                ImGui::Text("Window pos (%i, %i)", window_coords.x, window_coords.x);
                ImGui::Text("World pos (%1.1f, %1.1f, %1.1f)", world_pos.x, world_pos.y, world_pos.z);
                ImGui::End();
            }
            ImGui::PopStyleColor(2);
        }
    }

    ImGui::SliderFloat("Animation speed", &characterAnimSpeed, 0.1f, 5.0f);
    ImGui::SliderFloat("character speed", character_speed, 0.1f, 50.0f);

    ImGui::End(); // end info window
}

void Game::destroy()
{

}

void Game::updateCamera(
    InputManagerPtr input)
{
    // Fetch mouse and compute movement since last frame
    auto mouse = input->GetMouseState();
    glm::ivec2 mouse_xy{ mouse.x, mouse.y };
    glm::ivec2 mouse_xy_diff{ 0, 0 };
    if (mouse.leftButton && camera.mouse_xy_prev.x >= 0)
        mouse_xy_diff = camera.mouse_xy_prev - mouse_xy;
    camera.mouse_xy_prev = mouse_xy;

    // Update camera rotation from mouse movement
    camera.yaw += mouse_xy_diff.x * camera.sensitivity;
    camera.pitch += mouse_xy_diff.y * camera.sensitivity;
    camera.pitch = glm::clamp(camera.pitch, -glm::radians(89.0f), 0.0f);

    // Update camera position
    const glm::vec4 rotatedPos = glm_aux::R(camera.yaw, camera.pitch) * glm::vec4(0.0f, 0.0f, camera.distance, 1.0f);
    camera.pos = camera.lookAt + glm::vec3(rotatedPos);
}

void Game::updatePlayer(
    float deltaTime,
    InputManagerPtr input)
{
    // Fetch keys relevant for player movement
    using Key = eeng::InputManager::Key;
    bool W = input->IsKeyPressed(Key::W);
    bool A = input->IsKeyPressed(Key::A);
    bool S = input->IsKeyPressed(Key::S);
    bool D = input->IsKeyPressed(Key::D);
    
    // Compute vectors in the local space of the player
    player.fwd = glm::vec3(glm_aux::R(camera.yaw, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    player.right = glm::cross(player.fwd, glm_aux::vec3_010);

    // Compute the total movement as a 3D vector
    auto movement =
        player.fwd * player.velocity * deltaTime * ((W ? 1.0f : 0.0f) + (S ? -1.0f : 0.0f)) +
        player.right * player.velocity * deltaTime * ((A ? -1.0f : 0.0f) + (D ? 1.0f : 0.0f));

    // Update player position and forward view ray
    player.pos += movement;
    player.viewRay = glm_aux::Ray{ player.pos + glm::vec3(0.0f, 2.0f, 0.0f), player.fwd };

    // Update camera to track the player
    /*camera.lookAt += movement;
    camera.pos += movement;*/

}

void Game::MovementSystem(float deltaTime) {
    auto view = entity_registry->view<TransformComponent, LinearVelocityComponent>();

    for (auto entity : view) {

        auto [transform, velocity] = view.get<TransformComponent, LinearVelocityComponent>(entity);
        transform.translation += velocity.velocity * velocity.speed * deltaTime;
        //transform.translation.z += velocity.velocity.z * velocity.speed * deltaTime;
    }


    //oh dear maybe do this in you know... player?
    auto view_player = entity_registry->view<LinearVelocityComponent, PlayerControllerComponent>();

    for (auto entity : view_player) {
        auto [velocity, c_player] = 
            view_player.get< 
            LinearVelocityComponent, 
            PlayerControllerComponent>(entity);

        camera.lookAt += velocity.velocity * velocity.speed * deltaTime;
        camera.pos += velocity.velocity * velocity.speed * deltaTime;
    }
}

void Game::PlayerControllerSystem(InputManagerPtr input) {
    auto view = entity_registry->view<TransformComponent, LinearVelocityComponent, PlayerControllerComponent>();

    bool W, A, S, D, spacebar = false;
    
    for (auto entity : view) {

        auto [transform, velocity, c_player] = view.get<TransformComponent, LinearVelocityComponent, PlayerControllerComponent>(entity);
        W = input->IsKeyPressed(c_player.W);
        A = input->IsKeyPressed(c_player.A);
        S = input->IsKeyPressed(c_player.S);
        D = input->IsKeyPressed(c_player.D);
        spacebar = input->IsKeyPressed(c_player.spacebar);
        
        glm::vec3 fwd = glm::vec3(glm_aux::R(camera.yaw, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        glm::vec3 right = glm::cross(fwd, glm_aux::vec3_010);

        velocity.velocity =
            fwd * ((W ? 1.0f : 0.0f) + (S ? -1.0f : 0.0f)) +
            right * ((A ? -1.0f : 0.0f) + (D ? 1.0f : 0.0f));

        /*camera.lookAt = transform.translation;
        camera.pos = transform.translation;*/
    }
}

void Game::NPCControllerSystem() {
    auto view = entity_registry->view<NPCControllerComponent, TransformComponent, LinearVelocityComponent>();
    int point_index;
    int point_max;
    float distance_check;

    for (auto entity : view) {
        
        auto [npc_controller, transform, linear_velocity] = view.get<NPCControllerComponent, TransformComponent, LinearVelocityComponent>(entity);
        point_index = npc_controller.pp_index;
        distance_check = npc_controller.proximity_value;
        point_max = npc_controller.pp_max;

        if (glm::distance(transform.translation, npc_controller.path_points[point_index]) <= distance_check) {
            point_index++;
            point_index %= point_max;
            npc_controller.pp_index = point_index;            
        }
        
        linear_velocity.velocity = glm::normalize(npc_controller.path_points[point_index] - transform.translation);
    }
}

void Game::RenderSystem(float time) {
    auto view = entity_registry->view<TransformComponent, MeshComponent, AABBComponent>();

    for (auto entity : view) {

        auto [transform, mesh_ptr, aabb] = view.get<TransformComponent, MeshComponent, AABBComponent>(entity);

        glm::mat4 T = glm_aux::T(transform.translation);
        glm::mat4 R = glm_aux::R(transform.yaw, transform.pitch);
        glm::mat4 S = glm_aux::S(transform.scale);
        glm::mat4 TRS = T * R * S;

        mesh_ptr.renderable_mesh->animate(9, time * 3);
        forwardRenderer->renderMesh(mesh_ptr.renderable_mesh, TRS);
        aabb.mesh_aabb = mesh_ptr.renderable_mesh->m_model_aabb.post_transform(TRS);

        shapeRenderer->push_basis_basic(TRS, 1.0f);

        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
        shapeRenderer->push_AABB(aabb.mesh_aabb.min, aabb.mesh_aabb.max);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
        
    }
}

void Game::CreateEntities()
{
    entt::entity entity; 
    
    for (int i = 0; i < 5; ++i) {
        entity = entity_registry->create();

        entity_registry->emplace<TransformComponent>(
            entity, glm::vec3(0.f, 0.f, 0.f),
            0.f, 0.f, 0.f,
            glm::vec3(0.01f, 0.01f, 0.01f));

        entity_registry->emplace<LinearVelocityComponent>(
            entity, glm::vec3(1, 0, 0), (float(RandomInt(5, 20))));

        entity_registry->emplace<MeshComponent>(entity, horseMesh);

        entity_registry->emplace<AABBComponent>(entity);

        entity_registry->emplace<NPCControllerComponent>(entity);
        GeneratePath(entity);
    }
}

void Game::CreateAnimEntity() 
{
    entt::entity entity;

    entity = entity_registry->create();

    entity_registry->emplace<TransformComponent>(
        entity, glm::vec3(5.f, 0.f, 0.f),
        0.f, 0.f, 0.f,
        glm::vec3(0.03f, 0.03f, 0.03f));
    entity_registry->emplace<MeshComponent>(entity, characterMesh);
}

void Game::InitPlayer() {
    
    auto entity = entity_registry->create();

    entity_registry->emplace<TransformComponent>(
        entity, glm::vec3(5.f, 0.f, 0.f),
        0.f, 0.f, 0.f,
        glm::vec3(0.03f, 0.03f, 0.03f));

    //why am i trolling myself?
    entity_registry->emplace<LinearVelocityComponent>(
        entity, glm::vec3(0, 0, 0), (float(RandomInt(10, 20))));

    character_speed = &entity_registry->get<LinearVelocityComponent>(entity).speed;

    entity_registry->emplace<MeshComponent>(entity, characterMesh);

    entity_registry->emplace<AABBComponent>(entity);

    entity_registry->emplace<PlayerControllerComponent>(entity);

    camera.lookAt = entity_registry->get<TransformComponent>(entity).translation;
}

void Game::GeneratePath(entt::entity& e)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(-100.0, 100.0);
    
    for (int i = 0; i < 5; ++i) {
        entity_registry->get<NPCControllerComponent>(e).path_points.push_back({ distr(gen), 0, distr(gen)});
    }
}

int Game::RandomInt(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(min, max);

    return distr(gen);
}

void Game::Time(float time) {

    time_seconds = (int)time % 60;

    if (time_seconds % 60 > 0) {
        minute_cooldown = true;
    }

    if (time_seconds % 60 == 0 && minute_cooldown) {
        time_minutes++;
        minute_cooldown = false;
    }
}

void Game::BoneTest(float time) {
    auto view = entity_registry->view<TransformComponent, MeshComponent, AABBComponent, PlayerControllerComponent>();

    for (auto entity : view) {

        auto [transform, mesh_ptr, aabb] = view.get<TransformComponent, MeshComponent, AABBComponent>(entity);

        glm::mat4 T = glm_aux::T(transform.translation);
        glm::mat4 R = glm_aux::R(transform.yaw, transform.pitch);
        glm::mat4 S = glm_aux::S(transform.scale);
        glm::mat4 TRS = T * R * S;

        mesh_ptr.renderable_mesh->animate(9, time * 3);
        forwardRenderer->renderMesh(mesh_ptr.renderable_mesh, TRS);
        aabb.mesh_aabb = mesh_ptr.renderable_mesh->m_model_aabb.post_transform(TRS);

        shapeRenderer->push_basis_basic(TRS, 1.0f);

        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
        shapeRenderer->push_AABB(aabb.mesh_aabb.min, aabb.mesh_aabb.max);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
        boneGizmo->draw_bone_gizmo(mesh_ptr.renderable_mesh, shapeRenderer, TRS);
    }
}