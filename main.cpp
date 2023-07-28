
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

int main(int, char**){

    // window setup
    int ww = 600;
    int wh = 600;
    SDL_Window* window = SDL_CreateWindow("RRT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ww, wh, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    // screen texture setup
    int wtw = 600;
    int wth = 600;
    std::vector<unsigned char> pixels(wtw * wth * 4, 0);
    SDL_Texture* win_tex = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, wtw, wth );

    // camera setup
    glm::vec3 p = {0.0f, 0.0f, 0.0f};
    float y_rot = 0.0f;
    float x_rot = 0.0f;
    float fov = glm::radians(100);

    // triangles (won't be using indices for now)
    std::vector<glm::vec3> vertices = {
        {1.0f, 1.0f, 1.0f},
        {0.5f, 0.5f, 1.0f},
        {0.75f, 1.0f, 0.5f}
    };

    // light sources
    std::vector<glm::vec3> light_sources = {
        {-1.0f,-1.0f, -1.0f}
    };

    // setting up for raytracing
    int max_ray_dist = 100.0f;
    float z0 = 0.5f / glm::tan(fov/2);

    bool running = true;
    SDL_Event ev;
    while (running) {

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    running = false;
            }
        }

        glm::mat4 rot_mat(0.0f);
        rot_mat = glm::rotate(rot_mat, y_rot, glm::vec3(0.0f, 1.0f, 0.0f));
        rot_mat = glm::rotate(rot_mat, x_rot, glm::vec3(1.0f, 0.0f, 0.0f));

        for (int i = 0; i < wth; i++) {
            for (int j = 0; j < wtw; j++) {
                
                glm::vec3 start_pos = glm::vec3(j/wtw-0.5f - p.x, i/wth-0.5f, z0);
                glm::vec3 norm_vec = (rot_mat * glm::vec4(glm::normalize(start_pos - p), 0.0f));

                glm::vec3 p1 = start_pos;
                glm::vec3 p2 = start_pos;

                bool stopped = false;

                // kolla

                /*
                int steps = 0;
                while (steps < max_steps && (!stopped)) {

                    p1 = p2;
                    p2 += step_vec;

                    // check if the line between p1 and p2 intersects with any triangle
                    for (int ti = 0; ti < vertices.size()/3; ti += 3) {
                        
                        // borde räkna ut surface matriserna innan detta så att det endast behövs göras en gång

                        // los computaciones caros
                        glm::vec3 axis1 = vertices[ti] - vertices[ti+1];
                        glm::vec3 normal = glm::cross(axis1, vertices[ti] - vertices[ti+2]);
                        glm::vec3 axis2 = glm::cross(axis1, axis2);

                        glm::mat4 surface_mat = glm::mat4(glm::vec4(normal, 0.0f), glm::vec4(axis1, 0.0f), glm::vec4(axis2, 0.0f), glm::vec4(0.0f));
                        glm::mat4 surface_inverse = glm::inverse(surface_mat);

                        float triangle_normal_pos = (surface_inverse * glm::vec4(vertices[ti], 0.0f)).x;

                        float p1_normal_pos = (surface_inverse * glm::vec4(p1, 0.0f)).x;
                        float p2_normal_pos = (surface_inverse * glm::vec4(p2, 0.0f)).x;

                        if ((triangle_normal_pos - p1_normal_pos < 0) != (triangle_normal_pos - p2_normal_pos)) {
                            stopped = true;
                            break;
                        }

                    }

                }
                */

                if (stopped) {

                    // do essentially the same process, but from the position where the ray was stopped, to every light source

                }



            }
        }

        SDL_UpdateTexture(win_tex, nullptr, pixels.data(), wtw * 4);
        SDL_RenderCopy(renderer, win_tex, nullptr, nullptr);

        SDL_RenderPresent(renderer);
        
    }

}

// plan: modellera programmet utan användningen av shaders först, för att sedan börja använda OpenGL för renderingen. Blir nog enklast om jag bara börjar rendera skärmen på en
// texture i SDL2. 

// TODO: ha så att ljuset av ett fragment beror på fragmentets distans till ljuskällor och till kameran
