#ifndef glmcommob_h
#define glmcommob_h
#pragma once

#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glm_aux {

    constexpr glm::vec3 vec3_000{ 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 vec3_100{ 1.0f, 0.0f, 0.0f };
    constexpr glm::vec3 vec3_010{ 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 vec3_001{ 0.0f, 0.0f, 1.0f };
    constexpr glm::vec3 vec3_111{ 1.0f, 1.0f, 1.0f };

    /// @brief Convert glm::vec3 to string
    /// @param vec Vector to convert
    inline std::string to_string(const glm::vec3& vec)
    {
        std::ostringstream oss;
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return oss.str();
    }

    /// @brief Convert glm::vec4 to string
    /// @param vec Vector to convert
    inline std::string to_string(const glm::vec4& vec)
    {
        std::ostringstream oss;
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return oss.str();
    }

    /// @brief Convert glm::mat4 to string
    /// @param matrix Matrix to convert
    inline std::string to_string(const glm::mat4& mat)
    {
        std::ostringstream oss;
        oss << "[";
        for (int i = 0; i < 4; ++i) {
            oss << "(";
            for (int j = 0; j < 4; ++j) {
                oss << mat[i][j];
                if (j < 3) oss << ", ";
            }
            oss << ")";
            if (i < 3) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    /**
         * @brief Creates a translation matrix.
         * @param translation A 3D vector representing the translation.
         * @return A 4x4 translation matrix.
         */
    inline glm::mat4 T(
        const glm::vec3& translation)
    {
        return glm::translate(glm::mat4(1.0f), translation);
    }

    /**
     * @brief Creates a rotation matrix.
     * @param angle The rotation angle in radians.
     * @param axis A 3D vector representing the axis of rotation (must be normalized).
     * @return A 4x4 rotation matrix.
     */
    inline glm::mat4 R(
        float angle,
        const glm::vec3& axis)
    {
        return glm::rotate(glm::mat4(1.0f), angle, axis);
    }

    /**
     * @brief Creates a scaling matrix.
     * @param scale A 3D vector representing the scale along each axis.
     * @return A 4x4 scaling matrix.
     */
    inline glm::mat4 S(
        const glm::vec3& scale)
    {
        return glm::scale(glm::mat4(1.0f), scale);
    }

    /**
     * @brief Creates a combined Translation * Rotation matrix.
     * @param translation A 3D vector representing the translation.
     * @param angle The rotation angle in radians.
     * @param axis A 3D vector representing the axis of rotation (must be normalized).
     * @return A 4x4 matrix representing Translation * Rotation.
     */
    inline glm::mat4 TR(
        const glm::vec3& translation,
        float angle,
        const glm::vec3& axis)
    {
        return T(translation) * R(angle, axis); // Translation * Rotation
    }

    /**
     * @brief Creates a combined Translation * Scale matrix.
     * @param translation A 3D vector representing the translation.
     * @param scale A 3D vector representing the scale along each axis.
     * @return A 4x4 matrix representing Translation * Scale.
     */
    inline glm::mat4 TS(
        const glm::vec3& translation,
        const glm::vec3& scale)
    {
        return T(translation) * S(scale); // Translation * Scale
    }

    /**
     * @brief Creates a combined Rotation * Scale matrix.
     * @param angle The rotation angle in radians.
     * @param axis A 3D vector representing the axis of rotation (must be normalized).
     * @param scale A 3D vector representing the scale along each axis.
     * @return A 4x4 matrix representing Rotation * Scale.
     */
    inline glm::mat4 RS(
        float angle,
        const glm::vec3& axis,
        const glm::vec3& scale)
    {
        return R(angle, axis) * S(scale); // Rotation * Scale
    }

    /**
     * @brief Creates a combined Translation * Rotation * Scale matrix.
     * @param translation A 3D vector representing the translation.
     * @param angle The rotation angle in radians.
     * @param axis A 3D vector representing the axis of rotation (must be normalized).
     * @param scale A 3D vector representing the scale along each axis.
     * @return A 4x4 matrix representing Translation * Rotation * Scale.
     */
    inline glm::mat4 TRS(
        const glm::vec3& translation,
        float angle,
        const glm::vec3& axis,
        const glm::vec3& scale)
    {
        return T(translation) * R(angle, axis) * S(scale); // Translation * Rotation * Scale
    }

    /// @brief Computes a world-space ray from window coordinates.
    /// @param windowCoordinates Mouse position in window coordinates (e.g., from input events).
    /// @param viewMatrix The camera's view matrix.
    /// @param projectionMatrix The camera's projection matrix.
    /// @param viewport The viewport as a vec4: (x, y, width, height).
    /// @return A pair where:
    ///         - First element is the ray's origin in world space.
    ///         - Second element is the ray's normalized direction in world space.
    ///
    inline std::pair<glm::vec3, glm::vec3> world_ray_from_window_coords(
        const glm::vec2& windowCoordinates,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        const glm::vec4& viewport
    ) {
        // Step 1: Convert window coordinates to normalized device coordinates (NDC)
        float x = (2.0f * (windowCoordinates.x - viewport.x)) / viewport.z - 1.0f;
        float y = 1.0f - (2.0f * (windowCoordinates.y - viewport.y)) / viewport.w; // Invert Y-axis for OpenGL
        float zNear = -1.0f; // Near clip plane in NDC
        float zFar = 1.0f;   // Far clip plane in NDC

        // Step 2: Compute the inverse of the view-projection matrix
        glm::mat4 inverseVP = glm::inverse(projectionMatrix * viewMatrix);

        // Step 3: Unproject the NDC points to world space
        glm::vec4 nearPointNDC(x, y, zNear, 1.0f);
        glm::vec4 farPointNDC(x, y, zFar, 1.0f);

        // Transform NDC points to world space
        glm::vec4 nearPointWorld = inverseVP * nearPointNDC;
        glm::vec4 farPointWorld = inverseVP * farPointNDC;

        // Perform perspective divide
        nearPointWorld /= nearPointWorld.w;
        farPointWorld /= farPointWorld.w;

        // Step 4: Define the ray in world space
        glm::vec3 rayOrigin = glm::vec3(nearPointWorld); // Ray origin
        glm::vec3 rayDirection = glm::normalize(glm::vec3(farPointWorld - nearPointWorld)); // Ray direction

        return { rayOrigin, rayDirection };
    }

}

#endif