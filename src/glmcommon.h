#ifndef glmcommob_h
#define glmcommob_h
#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

inline void printMat4(const glm::mat4& matrix)
{
    const float* ptr = glm::value_ptr(matrix);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout << ptr[j * 4 + i] << " ";
        }
        std::cout << std::endl;
    }
}

inline glm::mat4 TRS(const glm::vec3& translation,
    float angle,
    const glm::vec3& axis,
    const glm::vec3& scale)
{
    const glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
    const glm::mat4 TR = glm::rotate(T, glm::radians(angle), axis);
    const glm::mat4 TRS = glm::scale(TR, scale);
    return TRS;
}

#endif