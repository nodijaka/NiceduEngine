//
//  debug_renderer.cpp
//  tau3d
//
//  Created by Carl Johan Gribel on 2016-12-26.
//
//

#include <stdio.h>
#include <cstddef>
#include <array>

//#include "Ray.h"
//#include "glutil.h"
#include <glm/gtc/type_ptr.hpp>
#include "glcommon.h"
#include "config.h"
#include "ShapeRenderer.hpp"
//#include "interp.h"

namespace ShapeRendering {

    const Color4u Color4u::Black = Color4u(0xff000000u);
    const Color4u Color4u::White = Color4u(0xffffffffu);
    const Color4u Color4u::Red = Color4u(0xff0000ffu);
    const Color4u Color4u::Lime = Color4u(0xff00ff00u);
    const Color4u Color4u::Blue = Color4u(0xffff0000u);
    const Color4u Color4u::Yellow = Color4u(0xff00ffffu);
    const Color4u Color4u::Cyan = Color4u(0xffffff00u);
    const Color4u Color4u::Magenta = Color4u(0xffff00ffu);
    const Color4u Color4u::Gray = Color4u(0xff808080u);
    const Color4u Color4u::Maroon = Color4u(0xff000080u);
    const Color4u Color4u::Olive = Color4u(0xff808000u);
    const Color4u Color4u::Green = Color4u(0xff008000u);
    const Color4u Color4u::Purple = Color4u(0xff800080u);
    const Color4u Color4u::Teal = Color4u(0xff808000u);
    const Color4u Color4u::Navy = Color4u(0xff800000u);
    const Color4u Color4u::Orange = Color4u(0xffffa500u);
    const Color4u Color4u::Pink = Color4u(0xffffc0cbu);
    const Color4u Color4u::Brown = Color4u(0xffa52a2au);
    const Color4u Color4u::Silver = Color4u(0xffc0c0c0u);
    const Color4u Color4u::Gold = Color4u(0xffffd700u);
    const Color4u Color4u::Turquoise = Color4u(0xff40e0d0u);

    namespace {

        struct unitcube_t
        {
            //static const int face_stride = 4;

            const std::array<glm::vec3, 8> vertices =
            {
                glm::vec3 {0.5f,-0.5f, 0.5f},
                glm::vec3 {0.5f, 0.5f, 0.5f},
                glm::vec3 {-0.5f, 0.5f, 0.5f},
                glm::vec3 {-0.5f,-0.5f, 0.5f},

                glm::vec3 {0.5f,-0.5f,-0.5f},
                glm::vec3 {0.5f, 0.5f,-0.5f},
                glm::vec3 {-0.5f, 0.5f,-0.5f},
                glm::vec3 {-0.5f,-0.5f,-0.5f}
            };

            const std::array<glm::vec3, 6> tri_normals =
            {
                glm::vec3 {0.0f, 0.0f, 1.0f},
                glm::vec3 {0.0f, 0.0f, -1.0f},
                glm::vec3 {1.0f, 0.0f, 0.0f},
                glm::vec3 {-1.0f, 0.0f, 0.0f},
                glm::vec3 {0.0f, 1.0f, 0.0f},
                glm::vec3 {0.0f, -1.0f, 0.0f}
            };

            const std::array<unsigned, 36> tris =
            {
                0,1,2,
                0,2,3,

                4,7,6,
                4,6,5,

                0,4,5,
                0,5,1,

                3,2,6,
                3,6,7,

                1,5,6,
                1,6,2,

                0,3,7,
                0,7,4
            };

            //    const std::vector<unsigned> quad_faces =
            //    {
            //        0,1,2,3,
            //        4,7,6,5,
            //        0,4,5,1,
            //        3,2,6,7,
            //        1,5,6,2,
            //        0,3,7,4
            //    };

            const std::array<unsigned, 24> edges =
            {
                0,1,
                0,3,
                2,1,
                2,3,
                4,5,
                4,7,
                6,5,
                6,7,
                0,4,
                1,5,
                2,6,
                3,7
            };

        } unitcube;

        struct UnitQuad
        {
            //static const int face_stride = 4;

            const std::vector<glm::vec3> vertices =
            {
                glm::vec3(0.5f,     -0.5f,  0.0f),
                glm::vec3(0.5f,     0.5f,   0.0f),
                glm::vec3(-0.5f,    0.5f,   0.0f),
                glm::vec3(-0.5f,    -0.5f,  0.0f)
            };

            //        const std::vector<unsigned> tri_faces =
            //        {
            //            0,1,2,3,
            //            4,7,6,5,
            //            0,4,5,1,
            //            3,2,6,7,
            //            1,5,6,2,
            //            0,3,7,4
            //        };

            //    const std::vector<unsigned> quad_faces =
            //    {
            //        0,1,2,3,
            //        4,7,6,5,
            //        0,4,5,1,
            //        3,2,6,7,
            //        1,5,6,2,
            //        0,3,7,4
            //    };

            const std::vector<unsigned> edges =
            {
                0,1,
                0,3,
                2,1,
                2,3
            };

        } unitquad;

    } // Anon. namespace

#if 0
    //
    // CONE
    //
    // height = h
    // radius = r
    //
    // Parametric equation (along z with tip in {0,0,h}):
    //
    //                   |(1-u/h) r cos(theta)  |
    //      P(u,theta) = | (1-u/h) r sin(theta) |
    //                   | u                    |
    // where
    //      0 <= u <= h
    //      0 <= theta <= 2 pi
    //
    // Normal direction = dP/dtheta x dP/du
    //                  = [ simplifying while preserving directions ]
    //                  = (-sin(theta), cos(theta), 0) x (-cos(theta), -sin(theta), h/r)
    //
    //                    | h/r cos(theta) |
    //                  = | h/r sin(theta) |
    //                    | 1              |
    //
    // http://mathworld.wolfram.com/Cone.html
    //
    //
    void create_cone(float h,
        float r,
        unsigned hres,
        unsigned rres,
        std::vector<PolyVertex>& vertices,
        std::vector<unsigned>& indices)
    {
        // Vertices
        //
        for (int hi = 0; hi < hres; hi++)
        {
            float z = h * (float)hi / (hres - 1);
            float d = (1.0f - (float)hi / (hres - 1)) * r;

            for (int ri = 0; ri < rres; ri++)
            {
                float theta = 2.0f * 3.14159f * (float)ri / (rres - 1);
                glm::vec3 v = { d * cos(theta), d * sin(theta), z };
                glm::vec3 n = { h / r * cos(theta), h / r * sin(theta), 1 };

                vertices.push_back({ v, normalize(n) });
            }
        }

        // Indices (GL_TRIANGLES)
        //
        for (int hi = 0; hi < hres - 1; hi++) /* once for hi=0 */
            for (int ri = 0; ri < rres - 1; ri++)
            {
                unsigned i = hi * rres + ri;
                unsigned j = hi * rres + ri + 1;
                indices.push_back(i);
                indices.push_back(j);
                indices.push_back(i + rres);

                // Degenerate for top triangle strip
                indices.push_back(j);
                indices.push_back(j + rres);
                indices.push_back(i + rres);
            }
    }

    //
    // CYLINDER
    //
    // Note:
    // GL_TRIANGLE_STRIP works for a cylinder if the indices are generated from a wrapping grid,
    // since triangles straddling each strip then automatically become degenerate (and are discarded
    // automatically).
    // For a flat mesh or something non-wrapping, additional degenerate triangles has to be added in order to
    // separate the strips correctly. This is done by duplicating indices at the start and end of each strip.
    // http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
    //
    void create_cylinder(float h,
        float r,
        unsigned hres,
        unsigned rres,
        std::vector<PolyVertex>& vertices,
        std::vector<unsigned>& indices)
    {
        // Vertices
        //
        for (int hi = 0;hi < hres;hi++)
        {
            float z = h * hi / (hres - 1);
            for (int ri = 0; ri < rres; ri++)
            {
                float alpha = 2.0f * fPI * ri / (rres - 1);
                vec3f v = { r * cos(alpha), r * sin(alpha), z };
                vec3f n = { r * cos(alpha), r * sin(alpha), 0 };

                vertices.push_back({ v, normalize(n) });
            }
        }

#if 0
        // Indices (GL_TRIANGLE_STRIP)
        //
        for (int hi = 0;hi < hres - 1;hi++)
            for (int ri = 0; ri <= rres; ri++)
            {
                unsigned i = hi * rres + ri % rres;
                cylinder_i.push_back(i + rres);
                cylinder_i.push_back(i);
            }
#endif

        // Indices (GL_TRIANGLES)
        //
        for (int hi = 0; hi < hres - 1; hi++)
            for (int ri = 0; ri < rres - 1; ri++)
            {
                unsigned i = hi * rres + ri % rres;
                unsigned j = hi * rres + (ri + 1) % rres;
                indices.push_back(i);
                indices.push_back(j);
                indices.push_back(i + rres);

                indices.push_back(j);
                indices.push_back(j + rres);
                indices.push_back(i + rres);
            }
    }

    //
    // SPHERE
    //
    // radius = r
    //
    // Parametric equation:
    //
    //                      | r sin(theta)sin(phi)  |
    //      P(theta, phi) = | -r cos(phi)           |
    //                      | -r sin(theta)sin(phi) |
    // where
    //      0 <= theta <= 2pi
    //      0 <= phi <= pi
    //
    // Normal direction = t x b
    //                  = dP/dtheta x dP/dphi
    //                  = [ simplifying while preserving directions ]
    //                    | sin(theta)sin(phi) |
    //                  = | -cos(phi)          |
    //                    | cos(theta)sin(phi) |
    //
    void create_sphere(float r,
        unsigned thetares,
        unsigned phires,
        std::vector<PolyVertex>& vertices,
        std::vector<unsigned>& indices,
        bool wireframe = false)
    {
        // Vertices
        //
        float   theta = 0.0f,			/* 'stepping'-variable for theta: will go 0 - 2PI */
            dtheta = 2.0f * fPI / (thetares - 1),	/* step size, depending on the resolution */
            phi = 0.0f,						/* 'stepping'-variable for phi: will go 0 - 2PI */
            dphi = fPI / (phires - 1);			/* step size, depending on the resolution */

        for (int phii = 0; phii < phires; phii++)
        {
            float   cos_phi = cos(phi),
                sin_phi = sin(phi);
            theta = 0;

            for (int thetai = 0; thetai < thetares; thetai++)
            {
                float   cos_theta = cos(theta),
                    sin_theta = sin(theta);

                vec3f v = { r * sin_theta * sin_phi, -r * cos_phi, r * cos_theta * sin_phi };
                vec3f n = { sin_theta * sin_phi, -cos_phi, cos_theta * sin_phi };

                vertices.push_back({ v, normalize(n) });

                theta += dtheta;
            }
            phi += dphi;
        }

        if (wireframe)
        {
            // Indices (GL_LINES)
            //
            for (int phii = 0; phii < phires - 1; phii++)
                for (int thetai = 0; thetai < thetares - 1; thetai++)
                {
                    unsigned i = thetares * phii + thetai;
                    indices.push_back(i);
                    indices.push_back(i + 1);

                    indices.push_back(i);
                    indices.push_back(i + thetares);
                }
        }
        else
        {
            // Indices (GL_TRIANGLES)
            //
            for (int phii = 0; phii < phires - 1; phii++)
                for (int thetai = 0; thetai < thetares - 1; thetai++)
                {
                    unsigned i = thetares * phii + thetai;
                    indices.push_back(i);
                    indices.push_back(i + 1);
                    indices.push_back(i + thetares);

                    indices.push_back(i + 1);
                    indices.push_back(i + 1 + thetares);
                    indices.push_back(i + thetares);
                }
        }
    }

    void generate_normals(std::vector<PolyVertex>& vertices, std::vector<unsigned>& indices)
    {
        std::vector<vec3f>* v_bin = new std::vector<vec3f>[vertices.size()];

        /* Compute triangle normals and bin to vertices */
        for (int i = 0; i < indices.size(); i += 3)
        {
            vec3f   v0 = vertices[indices[i + 0]].p,
                v1 = vertices[indices[i + 1]].p,
                v2 = vertices[indices[i + 2]].p;
            vec3f n = linalg::normalize((v1 - v0) % (v2 - v0));

            v_bin[indices[i + 0]].push_back(n);
            v_bin[indices[i + 1]].push_back(n);
            v_bin[indices[i + 2]].push_back(n);
        }

        /* Average binned normals */
        for (int i = 0; i < vertices.size(); i++)
        {
            vec3f n = vec3f(0, 0, 0);
            for (int j = 0; j < v_bin[i].size(); j++)
            {
                n += v_bin[i][j];
            }
            n = linalg::normalize(n);

            vertices[i].normal = n;
        }

        delete[] v_bin;
    }

#endif

    void ShapeRenderer::init()
    {
        //
        // Prepare unit primitives
        //

// TODO
        // create_cone(1, 1, 2, 8, unitcone_vbo.vertices, unitcone_vbo.indices);
        // create_cylinder(1, 1, 2, 8, unitcylinder_vbo.vertices, unitcylinder_vbo.indices);
        // create_sphere(1, 8, 8, unitsphere_vbo.vertices, unitsphere_vbo.indices, false);
        // create_sphere(1, 8, 8, unitspherewireframe_vbo.vertices, unitspherewireframe_vbo.indices, true);

        push_states(BackfaceCull::True, DepthTest::True, Color4u::White, glm::mat4{ 1.0f });

        const GLchar* poly_vshader =
            "#version 410 core\n"
            "layout (location = 0) in vec3 attr_Pos;"
            "layout (location = 1) in vec3 attr_Normal;"
            "layout (location = 2) in vec4 attr_Color;"
            "uniform mat4 PROJ_VIEW;"
            "out vec3 pos, normal;"
            "out vec4 color;"
            ""
            "void main()"
            "{"
            "   pos = attr_Pos;"
            "   normal = attr_Normal;"
            "   color = attr_Color;"
            "   gl_Position = PROJ_VIEW * vec4(attr_Pos, 1);"
            "}";

        const GLchar* poly_fshader =
            "#version 410 core\n"
            //    "uniform vec3 color;"
            "uniform float ambient_ratio;"
            ""
            "in vec3 pos, normal;"
            "in vec4 color;"
            "out vec4 fragcolor;"
            "void main()"
            "{"
            "   vec3 l = vec3(1000,1000,1000);"
            "   float lambert = ambient_ratio + (1.0-ambient_ratio) * dot(normal, normalize(l-pos));"
            "   fragcolor = vec4(color.xyz*lambert, color.w);"
            //"   fragcolor = vec4(normal*0.5+0.5,1);"
            "}";

        const GLchar* line_vshader =
            "#version 410 core\n"
            "layout (location = 0) in vec3 attr_Pos;"
            "layout (location = 1) in vec4 attr_Color;"
            "uniform mat4 PROJ_VIEW;"
            "out vec4 color;"
            ""
            "void main()"
            "{"
            "   color = attr_Color;"
            "   gl_Position = PROJ_VIEW * vec4(attr_Pos, 1);"
            "}";

        const GLchar* line_fshader =
            "#version 410 core\n"
            //    "uniform vec3 color;"
            "in vec4 color;"
            "out vec4 fragcolor;"
            "void main()"
            "{"
            "   fragcolor = color;"
            //    "   fragcolor = vec4(color,1);"
            "}";

        const GLchar* point_vshader =
            "#version 410 core\n"
            "layout (location = 0) in vec3 attr_Pos;"
            "layout (location = 1) in vec4 attr_Color;"
            "uniform mat4 PROJ_VIEW;"
            "out vec4 color;"
            ""
            "void main()"
            "{"
            "   color = attr_Color;"
            "   gl_Position = PROJ_VIEW * vec4(attr_Pos, 1);"
            "}";

        const GLchar* point_fshader =
            "#version 410 core\n"
            "in vec4 color;"
            "out vec4 fragcolor;"
            ""
            "void main()"
            "{"
            "   fragcolor = color;"
            "}";

        const GLuint poly_pos_location = 0;
        const GLuint poly_normal_location = 1;
        const GLuint poly_color_location = 2;

        const GLuint line_pos_location = 0;
        const GLuint line_color_location = 1;

        const GLuint point_pos_location = 0;
        const GLuint point_color_location = 1;

        lambert_shader = createShaderProgram(poly_vshader, poly_fshader);
        line_shader = createShaderProgram(line_vshader, line_fshader);
        point_shader = createShaderProgram(point_vshader, point_fshader);

        //
        // Init polygon buffers
        //

        //glUseProgram(lambert_shader); // because we use
        glGenVertexArrays(1, &polygon_vao);
        glGenBuffers(1, &polygon_vbo);
        glGenBuffers(1, &polygon_ibo);

        glBindVertexArray(polygon_vao);
        glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon_ibo);

        assert(std::is_trivial_v<PolyVertex> || std::is_standard_layout_v<PolyVertex>);
        glEnableVertexAttribArray(poly_pos_location);
        glEnableVertexAttribArray(poly_normal_location);
        glEnableVertexAttribArray(poly_color_location);
        glVertexAttribPointer(poly_pos_location,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(PolyVertex),
            (GLvoid*)offsetof(PolyVertex, p));
        glVertexAttribPointer(poly_normal_location,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(PolyVertex),
            (GLvoid*)offsetof(PolyVertex, normal));
        glVertexAttribPointer(poly_color_location,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(PolyVertex),
            (GLvoid*)offsetof(PolyVertex, color));

        glBindVertexArray(0);

        //
        // Init line buffers
        //

        glGenVertexArrays(1, &lines_VAO);
        glGenBuffers(1, &lines_VBO);
        glGenBuffers(1, &lines_IBO);

        glBindVertexArray(lines_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, lines_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lines_IBO);

        assert(std::is_trivial_v<LineVertex> || std::is_standard_layout_v<LineVertex>);
        glEnableVertexAttribArray(line_pos_location);
        glEnableVertexAttribArray(line_color_location);
        glVertexAttribPointer(line_pos_location,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(LineVertex),
            (GLvoid*)offsetof(LineVertex, p));
        glVertexAttribPointer(line_color_location,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(LineVertex),
            (GLvoid*)offsetof(LineVertex, color));

        glBindVertexArray(0);

        //
        // Init point buffers
        //

        glGenVertexArrays(1, &point_vao);
        glGenBuffers(1, &point_vbo);

        glBindVertexArray(point_vao);
        glBindBuffer(GL_ARRAY_BUFFER, point_vbo);

        assert(std::is_trivial_v<PointVertex> || std::is_standard_layout_v<PointVertex>);
        glEnableVertexAttribArray(point_pos_location);
        glEnableVertexAttribArray(point_color_location);
        glVertexAttribPointer(point_pos_location,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(PointVertex),
            (GLvoid*)offsetof(PointVertex, p));
        glVertexAttribPointer(point_color_location,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(PointVertex),
            (GLvoid*)offsetof(PointVertex, color));

        glBindVertexArray(0);
        CheckAndThrowGLErrors();

        initialized = true;
    }

#if 0
    void ShapeRenderer::push_quad(const v3f points[4],
        const v3f& n)
    {
        const auto [color, depth_test, cull_face] = get_states<Color4u, DepthTest, BackfaceCull>();
        static const unsigned tri_indices[] = { 0, 1, 2, 0, 2, 3 };

        unsigned vertex_ofs = (unsigned)polygon_vertices.size();
        GLsizei index_ofs = (GLsizei)polygon_indices.size();

        for (int i = 0; i < 4; i++)
            polygon_vertices.push_back(PolyVertex{ points[i], n, color });

        polygon_indices.insert(polygon_indices.end(),
            tri_indices,
            tri_indices + 6);

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {index_ofs, 6, (GLint)vertex_ofs}
            });
    }

    // TODO: What's this? A 2D quad in the xy-plane?
    void ShapeRenderer::push_quad(const vec3f& pos,
        float scale)
    {
        const auto [color, depth_test, cull_face] = get_states<Color4u, DepthTest, BackfaceCull>();

        static const v3f quad_vertices[] =
        {
            v3f {-0.5,-0.5,0},
            v3f {0.5,-0.5,0},
            v3f {0.5,0.5,0},
            v3f {-0.5,0.5,0}
        };
        static const unsigned tri_indices[] = { 0, 1, 2, 0, 2, 3 };

        unsigned vertex_ofs = (unsigned)polygon_vertices.size();
        GLsizei index_ofs = (GLsizei)polygon_indices.size();
        vec3f N = v3f_001; // = linalg::normalize(vec3f(qua))

        for (auto& v : quad_vertices)
            polygon_vertices.push_back({ pos + v * scale, N, color });

        polygon_indices.insert(polygon_indices.end(),
            tri_indices,
            tri_indices + 6);

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {index_ofs, 6, (GLint)vertex_ofs}
            });
    }

    void ShapeRenderer::push_quad_wireframe()
    {
        const auto& transform = get_states<m4f>();

        auto vertices = unitquad.vertices;
        std::for_each(vertices.begin(),
            vertices.end(),
            [&transform](vec3f& v) { v = (transform * v.xyz1()).xyz(); });

        push_lines(vertices, unitquad.edges);
    }

    void ShapeRenderer::push_cube()
    {
        const auto [color, depth_test, cull_face, M] = get_states<Color4u, DepthTest, BackfaceCull, m4f>();
        unsigned vertex_ofs = (unsigned)polygon_vertices.size();
        GLsizei index_ofs = (GLsizei)polygon_indices.size();

#if 0
        // Use unique normals per vertex (36 vertices)
        v3f n;
        for (int i = 0; i < unitcube.tris.size(); i++)
        {
            const unsigned index = unitcube.tris[i];
            const v3f& v = xyz(M * xyz1(unitcube.vertices[index]));
            if (i % 6 == 0) n = normalize(xyz(M * xyz0(unitcube.tri_normals[i / 6])));
            polygon_vertices.push_back(PolyVertex{ v, n, color });
            polygon_indices.push_back(i);
        }
#else
        // Use normals á la sphere (8 vertices)
        for (auto& v : unitcube.vertices)
        {
            const v3f& vm = xyz(M * xyz1(v));
            const v3f nm = normalize(xyz(M * xyz0(v)));
            polygon_vertices.push_back(PolyVertex{ vm, nm, color });
        }

        polygon_indices.insert(polygon_indices.end(),
            unitcube.tris.begin(),
            unitcube.tris.end());
#endif

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {index_ofs, unitcube.tris.size(), (GLint)vertex_ofs}
            });
    }

    void ShapeRenderer::push_cube_wireframe()
    {
        const auto& transform = get_states<m4f>();

        auto vertices = unitcube.vertices;
        std::for_each(vertices.begin(),
            vertices.end(),
            [&transform](vec3f& v) { v = (transform * v.xyz1()).xyz(); });

        push_lines(vertices.data(),
            vertices.size(),
            unitcube.edges.data(),
            unitcube.edges.size());
    }

    //void ShapeRenderer::push_circle_ring(float r,
    //                                           const m4f& m,
    //                                           float subdiv,
    //                                           Color4u color,
    //                                           DepthTest depth_test)
    //{
    //    vec3f p0 = pos + vec3f(r, 0, 0);
    //    vec3f p1;
    //    
    //    for (int i = 0; i < subdiv; i++)
    //    {
    //        float theta = i*2.0f*fPI/(subdiv-1);
    //        p1 = pos + vec3f(cos(theta), sin(theta), 0)*r;
    //        push_line(p0, p1, color, depth_test);
    //        p0 = p1;
    //    }
    //}
#endif

    void ShapeRenderer::push_line(const glm::vec3& pos0, const glm::vec3& pos1)
    {
        const auto [color, depth_test] = get_states<Color4u, DepthTest>();

        const LineDrawcall ldc{ GL_LINES, depth_test };
        unsigned vertex_ofs = (unsigned)line_vertices.size();

        line_vertices.push_back(LineVertex { pos0, color });
        line_vertices.push_back(LineVertex { pos1, color });

        line_hash[ldc].push_back(vertex_ofs + 0);
        line_hash[ldc].push_back(vertex_ofs + 1);
    }

#if 0
    void ShapeRenderer::push_lines_from_cyclic_source(const LineVertex* vertices,
        int start_index,
        int nbr_vertices,
        int max_vertices)
    {
        const auto& depth_test = get_states<DepthTest>();

        const LineDrawcall ldc{ GL_LINES, depth_test };
        unsigned vertex_ofs = (unsigned)line_vertices.size();

        //    line_vertices.insert(line_vertices.end(),
        //                         vertices,
        //                         vertices + nbr_vertices);
        //    for (int i = 0; i < nbr_vertices; i++)
        //        line_vertices.push_back({vertices[i], color});
        for (int i = 0; i < nbr_vertices; i++)
        {
            unsigned index = (start_index + i) % max_vertices;
            line_vertices.push_back(vertices[index]);
        }

        // TODO: No need to hash for color anymore. Using an arbitrary hash (color) value.
        for (int i = 0; i < nbr_vertices - 1; i++)
        {
            line_hash[ldc].push_back(vertex_ofs + i);
            line_hash[ldc].push_back(vertex_ofs + i + 1);
        }
        // TODO: Wrapping should be optional
    //    line_hash[0].push_back(vertex_ofs + (unsigned)nbr_vertices - 1);
    //    line_hash[0].push_back(vertex_ofs);
    }

    void ShapeRenderer::push_lines(const std::vector<v3f>& vertices,
        const std::vector<unsigned>& indices)
    {
        push_lines(vertices.data(),
            vertices.size(),
            indices.data(),
            indices.size());
    }

    void ShapeRenderer::push_lines(const v3f* vertices,
        size_t nbr_vertices,
        const unsigned* indices,
        size_t nbr_indices)
    {
        const auto [color, depth_test] = get_states<Color4u, DepthTest>();

        const LineDrawcall ldc{ GL_LINES, depth_test };
        unsigned vertex_ofs = (unsigned)line_vertices.size();

        for (int i = 0; i < nbr_vertices; i++)
            line_vertices.push_back({ vertices[i], color });

        for (int i = 0; i < nbr_indices; i++)
            line_hash[ldc].push_back(vertex_ofs + indices[i]);
    }

    void ShapeRenderer::push_lines(const v3f* vertices,
        size_t nbr_vertices)
    {
        const auto [transform, color, depth_test] = get_states<m4f, Color4u, DepthTest>();

        assert(vertices);
        assert(nbr_vertices > 0);
        const LineDrawcall ldc{ GL_LINES, depth_test };
        unsigned vertex_ofs = (unsigned)line_vertices.size();

        //    if (M)
        //    {
        for (int i = 0; i < nbr_vertices; i++)
            line_vertices.push_back({ xyz(transform * xyz1(vertices[i])), color });
        //    }
        //    else
        //    {
        //        for (int i = 0; i < nbr_vertices; i++)
        //            line_vertices.push_back({vertices[i], color});
        //    }

        for (int i = 0; i < nbr_vertices - 1; i++)
        {
            line_hash[ldc].push_back(vertex_ofs + i);
            line_hash[ldc].push_back(vertex_ofs + i + 1);
        }
        // TODO: Wrapping should be optional
        line_hash[ldc].push_back(vertex_ofs + (unsigned)nbr_vertices - 1);
        line_hash[ldc].push_back(vertex_ofs);
    }

    void ShapeRenderer::push_grid(const vec3f& pos,
        unsigned size,
        unsigned resolution)
    {
        float sizef = float(size);

        for (int i = 0; i < resolution; i++)
        {
            push_line(
                { pos.x - sizef / 2 + i * sizef / (resolution - 1), pos.y, pos.z - sizef / 2 },
                { pos.x - sizef / 2 + i * sizef / (resolution - 1), pos.y, pos.z + sizef / 2 });
            push_line(
                { pos.x - sizef / 2, pos.y, pos.z - sizef / 2 + i * sizef / (resolution - 1) },
                { pos.x + sizef / 2, pos.y, pos.z - sizef / 2 + i * sizef / (resolution - 1) });
        }
    }

    void ShapeRenderer::push_cone(const vec3f& from,
        const vec3f to,
        float r)
    {
        const vec3f conev = (to - from);
        const float conel = conev.norm2();

        // Main transform
        const mat4f R = mat4f(mat3f::base(conev));
        const mat4f M = mat4f::translation(from) * R;

        push_states(M);
        push_cone(conel, r);
        pop_states<m4f>();
    }

    void ShapeRenderer::push_cone(float h,
        float r,
        bool flip_normals)
    {
        const auto [color, depth_test, cull_face, M] = get_states<Color4u, DepthTest, BackfaceCull, m4f>();
        const auto vertex_ofs = polygon_vertices.size();
        GLsizei index_ofs = (GLsizei)polygon_indices.size();

        const mat4f N = M * mat4f::scaling(r, r, h);
        mat4f Nit = N.inverse(); Nit.transpose();

        for (auto& v : unitcone_vbo.vertices)
        {
            vec3f vw = (N * (v.p.xyz1())).xyz();
            vec3f nw = (Nit * (v.normal.xyz0())).xyz() * (flip_normals ? -1.0f : 1.0f);
            polygon_vertices.push_back({ vw, normalize(nw), color });
        }

        polygon_indices.insert(polygon_indices.end(),
            unitcone_vbo.indices.begin(),
            unitcone_vbo.indices.end());

        // Hacky way to flip the cone inside out
        if (flip_normals)
            for (int i = index_ofs; i < polygon_indices.size(); i += 3)
                std::swap(polygon_indices[i], polygon_indices[i + 1]);

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {index_ofs, (GLsizei)unitcone_vbo.indices.size(), (GLint)vertex_ofs}
            });

#if 0
        // Add normals (last [vertex_ofs] added normals)
        for (auto it = std::next(varray.begin(), vertex_ofs); it < varray.end(); it++)
        {
            add_line2(it->p, it->p + it->normal * 0.2f, { 0,1,0 });
        }
#endif
    }

    void ShapeRenderer::push_cylinder(float h,
        float r,
        Ray* ray)
    {
        const auto [color, depth_test, cull_face, M] = get_states<Color4u, DepthTest, BackfaceCull, m4f>();
        const auto vertex_ofs = polygon_vertices.size();
        const auto index_ofs = polygon_indices.size();

        mat4f N = M * mat4f::scaling(r, r, h);
        mat4f Nit = N; // Scaling is uniform in the plane of all normals (xy), so inverse-transform not needed.
        //        mat4f Nit = N.inverse(); Nit.transpose();

        for (auto& v : unitcylinder_vbo.vertices)
        {
            vec3f vw = (N * (v.p.xyz1())).xyz();
            vec3f nw = (Nit * (v.normal.xyz0())).xyz();
            polygon_vertices.push_back({ vw, normalize(nw), color });
        }
        polygon_indices.insert(polygon_indices.end(),
            unitcylinder_vbo.indices.begin(),
            unitcylinder_vbo.indices.end());

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {(GLsizei)index_ofs, (GLsizei)unitcylinder_vbo.indices.size(), (GLint)vertex_ofs}
            });

        if (ray)
            for (int i = 0; i < unitcylinder_vbo.indices.size(); i += 3) {
                const v3f& v0 = polygon_vertices[vertex_ofs + unitcylinder_vbo.indices[i + 0]].p;
                const v3f& v1 = polygon_vertices[vertex_ofs + unitcylinder_vbo.indices[i + 1]].p;
                const v3f& v2 = polygon_vertices[vertex_ofs + unitcylinder_vbo.indices[i + 2]].p;
                RayTriangleIntersection(*ray, v0, v1, v2);
            }

#if 0
        // Add normals (last [vertex_ofs] added normals)
        for (auto it = std::next(varray.begin(), vertex_ofs); it < varray.end(); it++)
        {
            add_line2(it->p, it->p + it->normal * 0.2f, { 0,1,0 });
        }
#endif
    }

    void ShapeRenderer::push_sphere(float h, float r)
    {
        const auto [color, depth_test, cull_face, M] = get_states<Color4u, DepthTest, BackfaceCull, m4f>();
        const auto vertex_ofs = polygon_vertices.size();
        const auto index_ofs = polygon_indices.size();

        mat4f N = M * mat4f::scaling(r, h, r);
        mat4f Nit;
        if (h == r) Nit = N;
        else { Nit = N.inverse(); Nit.transpose(); }

        for (auto& v : unitsphere_vbo.vertices)
        {
            vec3f vw = (N * (v.p.xyz1())).xyz();
            vec3f nw = (Nit * (v.normal.xyz0())).xyz();
            polygon_vertices.push_back({ vw, normalize(nw), color });
        }

        polygon_indices.insert(polygon_indices.end(),
            unitsphere_vbo.indices.begin(),
            unitsphere_vbo.indices.end());

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {(GLsizei)index_ofs, (GLsizei)unitsphere_vbo.indices.size(), (GLint)vertex_ofs}
            });

#if 0
        // Add normals (last [vertex_ofs] added normals)
        for (auto it = std::next(varray.begin(), vertex_ofs); it < varray.end(); it++)
        {
            add_line2(it->p, it->p + it->normal * 0.2f, { 0,1,0 });
        }
#endif
    }

    void ShapeRenderer::push_sphere_wireframe(float h, float r)
    {
        const auto [color, depth_test, cull_face, M] = get_states<Color4u, DepthTest, BackfaceCull, m4f>();
        const auto vertex_ofs = polygon_vertices.size();
        const auto index_ofs = polygon_indices.size();

        mat4f N = M * mat4f::scaling(r, h, r);
        mat4f Nit = N; // N.inverse(); Nit.transpose(); // ugly, only inv-transpose when needed

        for (auto& v : unitspherewireframe_vbo.vertices)
        {
            vec3f vw = (N * (v.p.xyz1())).xyz();
            vec3f nw = (Nit * (v.normal.xyz0())).xyz();
            polygon_vertices.push_back({ vw, normalize(nw), color });
        }

        polygon_indices.insert(polygon_indices.end(),
            unitspherewireframe_vbo.indices.begin(),
            unitspherewireframe_vbo.indices.end());

        polygon_hash.insert({
            PolygonDrawcall {GL_LINES, depth_test, cull_face},
            IndexRange {(GLsizei)index_ofs, (GLsizei)unitspherewireframe_vbo.indices.size(), (GLint)vertex_ofs}
            });

#if 0
        // Add normals (last [vertex_ofs] added normals)
        for (auto it = std::next(varray.begin(), vertex_ofs); it < varray.end(); it++)
        {
            add_line2(it->p, it->p + it->normal * 0.2f, { 0,1,0 });
        }
#endif
    }

    void ShapeRenderer::push_arrow(const vec3f& from,
        const vec3f& to,
        ArrowDescriptor arrow_desc,
        Ray* ray)
    {
        vec3f arrowv = (to - from);
        float arrowl = arrowv.norm2();

        // Main arrow transform
        mat4f R = mat4f(mat3f::base(arrowv));
        mat4f M = mat4f::translation(from) * R;

        // Cone
        float conel = arrowl * arrow_desc.cone_fraction; // cone_frac;
        // Skip cone part if arrow is very short
        if (conel > 0.0f)
        {
            mat4f Mcone = M * mat4f::translation({ 0,0,arrowl - conel });
            push_states(Mcone);
            push_cone(conel, arrow_desc.cone_radius);
            pop_states<m4f>();
        }

        // Cylinder
        float cyll = arrowl - conel;
        push_states(M);
        push_cylinder(cyll, arrow_desc.cylinder_radius, ray);
        pop_states<m4f>();

        // Additional debug line
        //        push_line(from, to, {0,1,0});
    }

    void ShapeRenderer::push_helix(const vec3f& from,
        const vec3f& to,
        float r_outer,
        float r_inner,
        float revs)
    {
        vec3f helixv = (to - from);
        float helixl = helixv.norm2();

        // Main helix transform
        mat4f R = mat4f(mat3f::base(helixv, { 1,0,0 }));
        mat4f M = mat4f::translation(from) * R;

        push_states(M);
        push_helix(helixl,
            r_outer,
            r_inner,
            revs);
        pop_states<m4f>();
    }

    void ShapeRenderer::push_helix(float length,
        float r_outer,
        float r_inner,
        float revs)
    {
        const auto [color, depth_test, cull_face, Mpos] = get_states<Color4u, DepthTest, BackfaceCull, m4f>();
        std::vector<PolyVertex> hvert;
        std::vector<unsigned> hind;

        // Base cylinder along +z
        const int res_per_rev = 12;
        create_cylinder(1,
            r_inner,
            res_per_rev * revs + 1,
            8,
            hvert,
            hind);

        // Re-aligns cylinder from +z to -x at y=r_outer.
        // From here, the cylinder is rotated and displaced wrt z.
        mat4f Minit = mat4f::translation(0, r_outer, 0) * mat4f::rotation(-fPI / 2, 0, 1, 0);

        // For each vertex: extract z as a parameter (for angle and
        // final z-position), re-align and then rotate into a helix
        for (int i = 0; i < hvert.size(); i++)
        {
            PolyVertex* v = &(hvert[i]);
            float zparam = v->p.z;
            // Collapse position back to z=0
            v->p.z = 0;

            float theta = linalg::lerp(0.0f, 2.0f * fPI * revs, zparam);
            mat4f Mspin = mat4f::translation(0, 0, length * zparam) * mat4f::rotation(theta, 0, 0, 1);
            mat4f M = Mpos * Mspin * Minit;

            v->p = (M * v->p.xyz1()).xyz();
            v->normal = normalize((M * v->normal.xyz0()).xyz());
            v->color = color;
        }

        // Now push the helix:ified vertex & index arrays to the main batch

        unsigned vertex_ofs = (unsigned)polygon_vertices.size();
        GLsizei index_ofs = (GLsizei)polygon_indices.size();

        polygon_vertices.insert(polygon_vertices.end(),
            hvert.begin(),
            hvert.end());
        polygon_indices.insert(polygon_indices.end(),
            hind.begin(),
            hind.end());

        polygon_hash.insert({
            PolygonDrawcall {GL_TRIANGLES, depth_test, cull_face},
            IndexRange {index_ofs, (GLsizei)hind.size(), (GLint)vertex_ofs}
            });

#if 0
        // Add normals (last [vertex_ofs] added normals)
        for (auto it = std::next(varray.begin(), vertex_ofs); it < varray.end(); it++)
        {
            push_line(it->p, it->p + it->normal * 0.2f, { 0,1,0 });
        }
#endif
    }

    void ShapeRenderer::push_frustum(const mat4f& invProjView)
    {
        // Frustum corners in 3D Clip space
        const vec4f f_points_clip[8] =
        {
            // near
            { -1, -1, -1, 1 },  // bottom-left
            { 1, -1, -1, 1 },   // bottom-right
            { 1, 1, -1, 1 },    // top-right
            { -1, 1, -1, 1 },   // top-left
            // far
            { -1, -1, 1, 1 },  // bottom-left
            { 1, -1, 1, 1 },   // bottom-right
            { 1, 1, 1, 1 },    // top-right
            { -1, 1, 1, 1 }   // top-left
        };
        vec4f f_points_world[8];

        for (int i = 0; i < 8; i++)
        {
            // 3D Clip -> 3DH World
            f_points_world[i] = invProjView * f_points_clip[i];

            // Project from 3DH World to 3D World. Needed for perspective
            // projection since the w-component is not submitted (otherwise
            // this step would be performed in vertex post-processing on GPU).
            const float winv = (1.0f / f_points_world[i].w);
            f_points_world[i] = f_points_world[i] * winv;
        }

        for (int i = 0; i < 4; i++)
        {
            push_line(f_points_world[i].xyz(), f_points_world[(i + 1) % 4].xyz());
            push_line(f_points_world[4 + i].xyz(), f_points_world[4 + (i + 1) % 4].xyz());
            push_line(f_points_world[i].xyz(), f_points_world[i + 4].xyz());
        }
    }

    void ShapeRenderer::push_basis_basic(const m4f& basis, float arrlen)
    {
        const vec3f wpos = basis.column(3).xyz();

        push_states(Color4u::Red);
        push_line(wpos, wpos + normalize(basis.col[0].xyz()) * arrlen);
        pop_states<Color4u>();

        push_states(Color4u::Lime);
        push_line(wpos, wpos + normalize(basis.col[1].xyz()) * arrlen);
        pop_states<Color4u>();

        push_states(Color4u::Blue);
        push_line(wpos, wpos + normalize(basis.col[2].xyz()) * arrlen);
        pop_states<Color4u>();
    }

    void ShapeRenderer::push_basis_basic2d(const m4f& basis, float arrlen)
    {
        const vec3f wpos = basis.column(3).xyz();

        push_states(Color4u::Red);
        push_line(wpos, wpos + normalize(basis.col[0].xyz()) * arrlen);
        pop_states<Color4u>();

        push_states(Color4u::Lime);
        push_line(wpos, wpos + normalize(basis.col[1].xyz()) * arrlen);
        pop_states<Color4u>();
    }

    void ShapeRenderer::push_basis(const m4f& basis,
        float arrlen,
        const ArrowDescriptor& arrdesc,
        Ray* ray)
    {
        const vec3f wpos = basis.column(3).xyz();

        push_states(Color4u::Red);
        push_arrow(wpos, wpos + normalize(basis.col[0].xyz()) * arrlen, arrdesc, ray);
        pop_states<Color4u>();

        push_states(Color4u::Lime);
        push_arrow(wpos, wpos + normalize(basis.col[1].xyz()) * arrlen, arrdesc, ray);
        pop_states<Color4u>();

        push_states(Color4u::Blue);
        push_arrow(wpos, wpos + normalize(basis.col[2].xyz()) * arrlen, arrdesc, ray);
        pop_states<Color4u>();
    }

    void ShapeRenderer::push_point(const vec3f& p, unsigned size)
    {
        const auto [color, depth_test] = get_states<Color4u, DepthTest>();
        const PointDrawcall dc{ size, depth_test };
        point_hash[dc].push_back(PointVertex{ p, color });
    }

    void ShapeRenderer::push_points(const PointVertex* points,
        unsigned nbr_points,
        unsigned size)
    {
        const auto& depth_test = get_states<DepthTest>();
        const PointDrawcall dc{ size, depth_test };

        auto& point_vector = point_hash[dc];
        point_hash[dc].insert(point_vector.end(),
            points,
            points + nbr_points);
    }

#endif

    void ShapeRenderer::render(const glm::mat4& PROJ_VIEW /* Proj * WorldToView */)
    {
        //push_arrow({1,2,0}, {1,0,0}, YELLOW);
        //push_sphere(1, 1, linalg::mat4f_identity, BLUE);
        //add_cylinder(5, 0.5, mat4f::translation(-2, 2, 0), BLUE);
        //add_cone2(5, 0.5, mat4f::translation(0, 2, 0), BLUE);
        //push_helix(3, 0.5, 0.1, 6, mat4f::translation(0, 1, 3));
        //push_helix({1,1,0}, {4,4,1}, 0.5, 0.1, 3);
        //push_cone({1,1,0}, {4,4,1}, 0.5, LIME);
        //push_cube_wireframe(mat4f::translation(0,2,0)*mat4f::scaling(2), {1,1,1});

        assert(initialized);
        framenbr++;

#if 0
        bool wireframe = false;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else {
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE); //glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
#endif
        //        glPolygonOffset(-1, -1);

#if 1
    //
    // STORE STATE
    //
#ifdef GL_POLYGON_MODE
//    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
        GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
        GLfloat last_poin_size; glGetFloatv(GL_POINT_SIZE, &last_poin_size);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        //    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

            //
            // IMGUI STATE
            //
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glCullFace(GL_BACK);
        //    glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    //
#endif


#if 1
    //
    // Render polygons
    //
        if (polygon_hash.size())
        {
            glUseProgram(lambert_shader);
            glBindVertexArray(polygon_vao);

#if 1
            // Reallocate buffers every frame

            glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(PolyVertex) * (int)polygon_vertices.size(), &polygon_vertices[0], GL_STREAM_DRAW);
            CheckAndThrowGLErrors();

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon_ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (int)polygon_indices.size(), &polygon_indices[0], GL_STREAM_DRAW);
            CheckAndThrowGLErrors();
#else

            // Reallocate buffers only when needed + stream draw & orphaning
            // This should be a lot faster in theory, at least for large buffers

            static size_t allocated_nbr_poly_vertices = 0;
            static size_t allocated_nbr_poly_indices = 0;

            // Resize only if needed
            if (polygon_vertices.size() > allocated_nbr_poly_vertices)
            {
                glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(PolyVertex) * (int)polygon_vertices.size(), &polygon_vertices[0], GL_STREAM_DRAW);
                allocated_nbr_poly_vertices = polygon_vertices.size();
            }
            if (polygon_indices.size() > allocated_nbr_poly_indices)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon_ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (int)polygon_indices.size(), &polygon_indices[0], GL_STREAM_DRAW);
                allocated_nbr_poly_indices = polygon_vertices.size();
            }

            // == VBO ==
            {
                GLintptr ofs = 0;
                GLsizeiptr size = polygon_vertices.size() * sizeof(PolyVertex);
                const void* data = polygon_vertices.data();

                // Orphan
                glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
                glMapBufferRange(GL_ARRAY_BUFFER,
                    0,
                    size,
                    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                glUnmapBuffer(GL_ARRAY_BUFFER);

                // Map data
                glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
                void* destptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                memcpy((char*)destptr + ofs, data, size);
                glUnmapBuffer(GL_ARRAY_BUFFER);
            }

            // == IBO ==
            {
                GLintptr ofs = 0;
                GLsizeiptr size = polygon_indices.size() * sizeof(unsigned int);
                const void* data = polygon_indices.data();

                // Orphan
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon_ibo);
                glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,
                    0,
                    size,
                    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

                // Map data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygon_ibo);
                void* destptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
                memcpy((char*)destptr + ofs, data, size);
                glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            }
#endif

            // Uniforms
            glUniformMatrix4fv(
                glGetUniformLocation(lambert_shader, "PROJ_VIEW"), 
                1, 
                0, 
                glm::value_ptr(PROJ_VIEW));
            glUniform1f(
                glGetUniformLocation(lambert_shader, "ambient_ratio"), 
                0.6);

            CheckAndThrowGLErrors();

            // geometry v2 (iterate over unique keys)
            //
            // Note: Elements with equal keys (color, topology) are isolated by manual stepping of the
            // iterator. Another way is to use 'count', which returns the number of elements with the same key,
            // but this call has linear time complexity worst case (nbr of found keys on average; which is
            // actually the cost of manual stepping...)

            for (auto it = polygon_hash.begin(); it != polygon_hash.end(); )
            {
                const PolygonDrawcall& dcgroup = it->first;

                // TODO: keep track of state and only actually change GL state when needed

                // Depth test
                if (to_integral(dcgroup.depth_test))
                    glEnable(GL_DEPTH_TEST);
                else
                    glDisable(GL_DEPTH_TEST);

                // Face culling
                if (to_integral(dcgroup.cull_face))
                    glEnable(GL_CULL_FACE);
                else
                    glDisable(GL_CULL_FACE);

                static std::vector<void*> start;
                static std::vector<GLsizei> size;
                static std::vector<GLint> ofs;
                start.clear();
                size.clear();
                ofs.clear();

                while (it != polygon_hash.end() && dcgroup == it->first)
                {
                    start.push_back(BUFOFS(it->second.start * sizeof(GLuint)));
                    size.push_back(it->second.size);
                    ofs.push_back(it->second.ofs);

                    ++it;
                }
                int count = (int)start.size();
                glMultiDrawElementsBaseVertex(dcgroup.topology, // GLenum mode
                    &size[0],         // const GLsizei *count
                    GL_UNSIGNED_INT,  // GLenum type
                    &start[0],        // const void * const *indices
                    count,            // GLsizei drawcount
                    &ofs[0]);         // const GLint *basevertex
            }

            glBindVertexArray(0);
            glUseProgram(0);
            CheckAndThrowGLErrors();
        }
#endif


        // test triangle
        /*
         index_range_t irange = { 0, 3 };
         //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, BUFOFS(0 * sizeof(GLuint)));
         glDrawElements(GL_TRIANGLES, irange.size, GL_UNSIGNED_INT, BUFOFS(irange.start * sizeof(GLuint)));
         */

#if 1
         //
         // Render lines
         //
        if (line_hash.size())
        {
            glUseProgram(line_shader);
            glBindVertexArray(lines_VAO);

            glLineWidth(1);

            glBindBuffer(GL_ARRAY_BUFFER, lines_VBO);
            glBufferData(GL_ARRAY_BUFFER,
                sizeof(LineVertex) * (int)line_vertices.size(),
                &line_vertices[0],
                GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lines_IBO);

            glUniformMatrix4fv(
                glGetUniformLocation(line_shader, "PROJ_VIEW"), 
                1, 
                0, 
                glm::value_ptr(PROJ_VIEW));

            // lines v3 (unorderded_map with vectors)

            for (auto& it : line_hash)
            {
                int size = (int)it.second.size();
                if (!size) continue;

                const LineDrawcall& dc = it.first;

                if (to_integral(dc.depth_test)) glEnable(GL_DEPTH_TEST);
                else                            glDisable(GL_DEPTH_TEST);

                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * size, &it.second[0], GL_STREAM_DRAW);
                glDrawElements(dc.topology, size, GL_UNSIGNED_INT, BUFOFS(0));
            }
            glLineWidth(1);
            glBindVertexArray(0);
            glUseProgram(0);
            CheckAndThrowGLErrors();
        }
#endif

#if 1
        //
        // Render points
        //

        // If point array is empty, glDrawElements for depth rendering crashes. Why!?
        //push_point({0,0,0}, {1,0,0}, 1);

        if (point_hash.size())
        {
            //        glDisable(GL_DEPTH_TEST);
            glUseProgram(point_shader);
            glBindVertexArray(point_vao);

            glBindBuffer(GL_ARRAY_BUFFER, point_vbo);

            glUniformMatrix4fv(
                glGetUniformLocation(point_shader, "PROJ_VIEW"), 
                1, 
                0, 
                glm::value_ptr(PROJ_VIEW));

            for (auto& it : point_hash)
            {
                int size = (int)it.second.size();
                if (!size) continue;

                const PointDrawcall& dc = it.first;
                if (to_integral(dc.depth_test))
                    glEnable(GL_DEPTH_TEST);
                else
                    glDisable(GL_DEPTH_TEST);

                glBufferData(GL_ARRAY_BUFFER, sizeof(PointVertex) * size, &it.second[0], GL_STREAM_DRAW);
                glPointSize(dc.size);
                glDrawArrays(GL_POINTS, 0, size);
            }
            CheckAndThrowGLErrors();
        }
#endif
        glUseProgram(0);
        //    glEnable(GL_DEPTH_TEST);


            //
            // RESTORE STATE
            //
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        //    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        glPointSize(last_poin_size);

#ifdef GL_POLYGON_MODE
        //    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    }

    void ShapeRenderer::post_render()
    {
        // Todo - move clear stuff

        polygon_vertices.clear();
        polygon_indices.clear();
        polygon_hash.clear();

        line_vertices.clear();
        for (auto& it : line_hash) // Clear key vectors
            it.second.clear();
        // lines.clear(); // Clear keys as well? Keys are likely to be reused so why really.

        for (auto& it : point_hash)
        {
            //            printf("%d ", it.second.size());
            it.second.clear();
        }
        //        printf("\n");
        //        printf("%d\n", point_hash.size());
        // point_array.clear(); // Clear keys as well?
    }

} // namespace gl_batch_renderer
