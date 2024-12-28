
#include <entt/entt.hpp> // -> Scene source
#include "glmcommon.hpp"
#include "imgui.h"
#include "Scene.hpp"

bool Scene::init()
{
    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();

    // Position of camera/eye
    eyePos = glm::vec3(0.0f, 5.0f, 10.0f);

    // Position to look at
    eyeAt = glm::vec3(0.0f, 0.0f, 0.0f);

    // Do some entt stuff
    // entt::registry registry;
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
    // Sponza
    characterMesh->load("/Users/ag1498/Dropbox/MAU/DA307A-CGM/Rendering/eduRend_2022/assets/crytek-sponza/sponza.obj", false);
#endif
#if 0
    // Character
    characterMesh->load("assets/Ultimate Platformer Pack/Character/Character.fbx", false);
#endif
#if 0
    // Enemy
    characterMesh->load("assets/Ultimate Platformer Pack/Enemies/Bee.fbx", false);
#endif
#if 0
    // Dragon
    // Requires MaxBones = 151;
    // https://sketchfab.com/3d-models/tarisland-dragon-high-poly-ecf63885166c40e2bbbcdf11cd14e65f
    // characterMesh->load("assets/tarisland-dragon-high-poly/M_B_44_Qishilong_skin_Skeleton.FBX");
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

    return true;
}

void Scene::update(
    float time_s,
    float deltaTime_s,
    InputManagerPtr input)
{
    auto mouse = input->GetMouseState();
    glm::vec2 mouse_xy{ mouse.x, mouse.y }; // INT
    glm::vec2 mouse_dxdy{ 0.0f, 0.0f };
    if (mouse_xy_prev.x >= 0.0f) mouse_dxdy = mouse_xy_prev - mouse_xy;
    mouse_xy_prev = mouse_xy;

    // Move camera
    if (mouse.leftButton)
    {
        // std::cout << mouse_dxdy.x << ", " << mouse_dxdy.y << std::endl;
    }
    else
        mouse_dxdy = glm::vec2(0.0f, 0.0f);
    updateCamera(mouse_dxdy.x, mouse_dxdy.y);

    using Key = eeng::InputManager::Key;
    // if (input->IsKeyPressed(Key::A))
    // {
    //     std::cout << "A\n";
    // }
    bool W = input->IsKeyPressed(Key::W);
    bool A = input->IsKeyPressed(Key::A);
    bool S = input->IsKeyPressed(Key::S);
    bool D = input->IsKeyPressed(Key::D);
    auto fwd = glm::vec3(glm_aux::R(yaw, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    // std::cout << glm_aux::to_string(fwd) << std::endl;
    auto right = glm::cross(fwd, glm_aux::vec3_010);
    //
    auto movement =
        fwd * player_velocity * deltaTime_s * ((W ? 1.0f : 0.0f) + (S ? -1.0f : 0.0f)) +
        right * player_velocity * deltaTime_s * ((A ? -1.0f : 0.0f) + (D ? 1.0f : 0.0f));

    // 
    player_pos += movement;
    eyeAt += movement;
    eyePos += movement;

    // std::cout << "mouse (" << mouse.x << ", " << mouse.y << ")\n";

    // std::cout << "Connected controllers: " << input->GetConnectedControllerCount() << std::endl;
    // auto controller = input->GetControllerState(0);

    lightPos = glm::vec3(glm_aux::TRS(
        { 1000.0f, 1000.0f, 1000.0f },
        time_s * 0.0f,
        { 0.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f }) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    characterWorldMatrix1 = glm_aux::TRS(
        player_pos,
        glm::pi<float>(), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

    characterWorldMatrix2 = glm_aux::TRS(
        { -3, 0, 0 },
        time_s * glm::radians(50.0f), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

    characterWorldMatrix3 = glm_aux::TRS(
        { 3, 0, 0 },
        time_s * glm::radians(50.0f), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });
}

void Scene::renderUI()
{
    ImGui::Text("Drawcall count %i", drawcallCount);

    if (ImGui::ColorEdit3("Light color",
        glm::value_ptr(lightColor),
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
        auto world_pos = glm::vec3(characterWorldMatrix1[3]);
        glm::vec2 window_coords;
        if (glm_aux::window_coords_from_world_pos(world_pos, VP * P * V, window_coords))
        {
            ImGui::SetNextWindowPos(
                ImVec2{ window_coords.x, 900 - window_coords.y },
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
                ImGui::Text("Window pos (%i, %i)", (int)window_coords.x, (int)window_coords.x);
                ImGui::Text("World pos (%1.1f, %1.1f, %1.1f)", world_pos.x, world_pos.y, world_pos.z);
                ImGui::End();
            }
            ImGui::PopStyleColor(2);
        }
    }

    ImGui::SliderFloat("Animation speed", &characterAnimSpeed, 0.1f, 5.0f);
}

void Scene::render(
    float time_s,
    int windowWidth,
    int windowHeight)
{
    // If we want to draw AABBs
    eeng::AABB character_aabb1, character_aabb2, character_aabb3, horse_aabb, grass_aabb;

    // Projection matrix
    const float aspectRatio = float(windowWidth) / windowHeight;
    P = glm::perspective(glm::radians(60.0f), aspectRatio, nearPlane, farPlane);

    // View matrix
    //glm::mat4 V = glm::inverse(TRS(eyePos, 0.0f, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
    V = glm::lookAt(eyePos, eyeAt, eyeUp);

    VP = glm_aux::create_viewport_matrix(0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f);

    // Compute world ray from window position (e.g. mouse), to use for picking or such
    {
        glm::vec2 mousePos{ windowWidth / 2, windowHeight / 2 }; // middle of the window as placeholder
        glm_aux::Ray ray = glm_aux::world_ray_from_window_coords(mousePos, V, P, VP);
        std::cout << "ray origin " << glm_aux::to_string(ray.origin)
            << ", ray dir " << glm_aux::to_string(ray.dir) << ")\n";
    }

    // Begin rendering pass
    forwardRenderer->beginPass(P, V, lightPos, lightColor, eyePos);

    // Grass
    forwardRenderer->renderMesh(grassMesh, grassWorldMatrix);
    grass_aabb = grassMesh->m_model_aabb.post_transform(grassWorldMatrix);

    // Horse
    horseMesh->animate(3, time_s);
    forwardRenderer->renderMesh(horseMesh, horseWorldMatrix);
    horse_aabb = horseMesh->m_model_aabb.post_transform(horseWorldMatrix);

    // Character, instance 1
    characterMesh->animate(characterAnimIndex, time_s * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix1);
    character_aabb1 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix1);

    // Character, instance 2
    characterMesh->animate(1, time_s * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix2);
    character_aabb2 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix2);

    // Character, instance 3
    characterMesh->animate(2, time_s * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix3);
    character_aabb3 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix3);

    // End rendering pass
    drawcallCount = forwardRenderer->endPass();

    // Character view ray (not in render())
    auto fwd = glm::vec3(glm_aux::R(yaw, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    glm_aux::Ray character_view_ray{ player_pos + glm::vec3(0.0f, 2.0f, 0.0f), fwd };
    bool hit = false;
    hit |= glm_aux::intersect_ray_AABB(character_view_ray, character_aabb2.min, character_aabb2.max);
    hit |= glm_aux::intersect_ray_AABB(character_view_ray, character_aabb3.min, character_aabb3.max);
    hit |= glm_aux::intersect_ray_AABB(character_view_ray, horse_aabb.min, horse_aabb.max);
    if (hit)
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xff00ff00 });
        shapeRenderer->push_line(character_view_ray.origin, character_view_ray.point_of_contact());
    }
    else
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xffffffff });
        shapeRenderer->push_line(character_view_ray.origin, character_view_ray.origin + character_view_ray.dir * 100.0f);
    }
    shapeRenderer->pop_states<ShapeRendering::Color4u>();

    // Just a line
    glm::vec3 p0{ 0.0f, 0.0f, 0.0f }, p1{ 1.0f, 1.0f, 0.0f };
    shapeRenderer->push_line(p0, p1);

    // Bases
    {
        shapeRenderer->push_basis_basic(characterWorldMatrix1, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix2, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix3, 1.0f);
        shapeRenderer->push_basis_basic(grassWorldMatrix, 1.0f);
        shapeRenderer->push_basis_basic(horseWorldMatrix, 1.0f);
    }

#if 0
    {
        // const ShapeRendering::ArrowDescriptor arrowdesc
        // {
        //     .cone_fraction = 0.2,
        //     .cone_radius = 0.05f,
        //     .cylinder_radius = 0.025f
        // };
        const auto arrowdesc = ShapeRendering::ArrowDescriptor
        {
            .cone_fraction = 0.2,
            .cone_radius = 0.15f,
            .cylinder_radius = 0.075f
        };
        shapeRenderer.push_basis(grassWorldMatrix, 1.0f, arrowdesc);
    }
#endif

    // Draw AABBs
    shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
    shapeRenderer->push_AABB(character_aabb1.min, character_aabb1.max);
    shapeRenderer->push_AABB(character_aabb2.min, character_aabb2.max);
    shapeRenderer->push_AABB(character_aabb3.min, character_aabb3.max);
    shapeRenderer->push_AABB(horse_aabb.min, horse_aabb.max);
    shapeRenderer->push_AABB(grass_aabb.min, grass_aabb.max);
    shapeRenderer->pop_states<ShapeRendering::Color4u>();

    // Push quads
    {
        glm::vec3 points[4]{ {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f} };
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0x8000ffff });

        shapeRenderer->push_states(glm_aux::TS(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(2.0f, 1.0f, 1.0f)));
        shapeRenderer->push_quad(points, glm_aux::vec3_001);
        shapeRenderer->pop_states<glm::mat4>();

        shapeRenderer->push_states(glm_aux::TS(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.0f)));
        shapeRenderer->push_quad_wireframe();
        shapeRenderer->pop_states<glm::mat4>();

        shapeRenderer->pop_states<ShapeRendering::Color4u>();
    }

    // Push cube
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0x8000ffff });
        shapeRenderer->push_states(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f)));

        shapeRenderer->push_states(glm_aux::T(glm::vec3(0.0f, 4.0f, 0.0f)));
        shapeRenderer->push_cube();

        shapeRenderer->push_states(glm_aux::T(glm::vec3(0.0f, 1.5f, 0.0f)));
        shapeRenderer->push_cube_wireframe();

        shapeRenderer->pop_states<ShapeRendering::Color4u, glm::mat4, glm::mat4, glm::mat4>();
    }

    // Push grid
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xff808080 });
        shapeRenderer->push_grid(glm::vec3(0.0f, 1.0e-6f, 0.0f), 20.0f, 21);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
    }

    // Cones, Cylinders
    {
        const auto arrowdesc = ShapeRendering::ArrowDescriptor
        {
            .cone_fraction = 0.2,
            .cone_radius = 0.15f,
            .cylinder_radius = 0.075f
        };
        shapeRenderer->push_states(glm_aux::T(glm::vec3(0.0f, 0.0f, 2.0f)));
        shapeRenderer->push_basis(arrowdesc, glm::vec3(5.0f, 1.0f, 3.0f));
        shapeRenderer->pop_states<glm::mat4>();
    }

    // Points
    {
        shapeRenderer->push_states(ShapeRendering::Color4u::Red);
        shapeRenderer->push_point(glm::vec3(2.0f, 1.0f, 0.0f), 4);
        shapeRenderer->push_point(glm::vec3(2.0f, 2.0f, 0.0f), 4);
        shapeRenderer->push_point(glm::vec3(2.0f, 3.0f, 0.0f), 4);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
    }

    // Circle ring
    {
        shapeRenderer->push_states(ShapeRendering::Color4u::Blue);
        shapeRenderer->push_states(glm_aux::TS(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
        shapeRenderer->push_circle_ring<8>();
        shapeRenderer->pop_states<ShapeRendering::Color4u, glm::mat4>();
    }

    shapeRenderer->render(P * V);
    shapeRenderer->post_render();
}

void Scene::destroy()
{

}



// Update camera rotation based on mouse input
void Scene::updateCamera(
    float deltaX,
    float deltaY) {
    // Update yaw and pitch angles based on input
    yaw += deltaX * 0.005f;   // Sensitivity factor
    pitch += deltaY * 0.005f;

    // Clamp pitch to prevent flipping
    const float pitchLimit = glm::radians(89.0f); // Prevent gimbal lock at poles
    if (pitch > pitchLimit) pitch = pitchLimit;
    if (pitch < -pitchLimit) pitch = -pitchLimit;

    // Create the rotation matrix
    // glm::mat4 rotationMatrix = R(yaw, pitch);

    // Rotate the initial position (0, 0, radius) around the target
    glm::vec4 rotatedPos = glm_aux::R(yaw, pitch) * glm::vec4(0.0f, 0.0f, radius, 1.0f);
    //glm::vec4 rotatedPos = glm::vec4(computeRotatedPosition(yaw, pitch, radius), 1.0f);

    // Update eyePos based on the rotated position
    eyePos = eyeAt + glm::vec3(rotatedPos);
}