
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>


struct mesh {

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

};


int main(int, char**){

    // window setup
    int ww = 600;
    int wh = 600;
    SDL_Window* window = SDL_CreateWindow("RRT", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ww, wh, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    // screen texture setup
    int wtw = 300;
    int wth = 300;
    std::vector<unsigned char> pixels(wtw * wth * 4, 0);
    SDL_Texture* win_tex = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, wtw, wth );

    // camera setup
    glm::vec3 p = {0.0f, 0.0f, 0.0f};
    float y_rot = 0.0f;
    float x_rot = 0.0f;
    float fov = glm::radians(100.0f);

    // triangles (won't be using indices for now)
    std::vector<glm::vec3> vertices = {
        
        {1.0f, 1.0f, 1.0f},
        {0.5f, 0.5f, 1.0f},
        {0.75f, 1.0f, 0.5f},

        {3.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.5f},
        {0.75f, 0.0f, 2.0f},

        {4.0f, 2.0f, 4.0f},
        {-4.0f, 2.0f, 4.0f},
        {-4.0f, 2.0f, -4.0f},

        {4.0f, 2.0f, 4.0f},
        {4.0f, 2.0f, -4.0f},
        {-4.0f, 2.0f, -4.0f}

    };
    std::vector<glm::vec3> normals(vertices.size()/3);
    for (int i = 0; i < normals.size(); i++) {
        normals[i] = glm::normalize(glm::cross(vertices[i*3+1]-vertices[i*3], vertices[i*3+2]-vertices[i*3]));
    }

    mesh m = {vertices, normals};

    // light sources
    std::vector<glm::vec3> light_sources = {
        {-1.0f,-1.0f, -1.0f}
    };

    // setting up for raytracing
    int max_ray_dist = 100.0f;
    float z0 = 0.5f / glm::tan(fov/2);

    float ambient_light = 0.5f;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    
    float dt = 0.0f;

    bool running = true;
    SDL_Event ev;
    while (running) {

        int start_time = SDL_GetTicks();

        glm::mat4 rot_mat(1.0f);
        rot_mat = glm::rotate(rot_mat, y_rot, glm::vec3(0.0f, 1.0f, 0.0f));
        rot_mat = glm::rotate(rot_mat, x_rot, glm::vec3(1.0f, 0.0f, 0.0f));

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                
                case SDL_QUIT:
                    running = false;
                
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {

                        case SDLK_ESCAPE:
                            running = false;
                            break;

                        case SDLK_LEFT:
                            y_rot -= dt;
                            break;
                        case SDLK_RIGHT:
                            y_rot += dt;
                            break;
                        case SDLK_UP:
                            x_rot += dt;
                            break;
                        case SDLK_DOWN:
                            x_rot -= dt;
                            break;

                        case SDLK_SPACE:
                            p.y -= dt;
                            break;
                        case SDLK_LSHIFT:
                            p.y += dt;
                            break;
                        
                    }

            }
        }

        // clear pixel buffer
        for (int i = 0; i < wth*wtw; i++) {
            
            pixels[i*4] = 0;
            pixels[i*4 + 1] = 0;
            pixels[i*4 + 2] = 0;
            pixels[i*4 + 3] = SDL_ALPHA_OPAQUE;

        }

        for (int i = 0; i < wth; i++) {
            for (int j = 0; j < wtw; j++) {

                glm::vec3 angled_vec = rot_mat * glm::vec4(glm::normalize(glm::vec3((float)j/wtw - 0.5f, (float)i/wth - 0.5f, z0)), 0.0f);

                float lowest_d = max_ray_dist;
                float lowest_ti;
                
                bool stopped = false;

                for (int ti = 0; ti < vertices.size(); ti+=3) {

                    glm::vec3 normal = normals[ti/3];
                    
                    glm::vec3 a = vertices[ti] - p;
                    glm::vec3 b = vertices[ti+1] - p;
                    glm::vec3 c = vertices[ti+2] - p;

                    float d = (normal.x*a.x + normal.y*a.y + normal.z*a.z) / (normal.x*angled_vec.x + normal.y*angled_vec.y + normal.z*angled_vec.z);

                    if (d > 0.0f && d < lowest_d) {

                        glm::vec3 rp = d * angled_vec;

                        float t_area = glm::length(glm::cross(b - a, c - a)) / 2;

                        float alpha = (glm::length(glm::cross(a - rp, b - rp)) / 2) / t_area;
                        float beta = (glm::length(glm::cross(b - rp, c - rp)) / 2) / t_area;
                        float gamma = (glm::length(glm::cross(c - rp, a - rp)) / 2) / t_area;

                        float area_sum = alpha + beta + gamma;

                        if (area_sum > 0.98f && area_sum < 1.02f) {
                            
                            std::cout << "test" << std::endl;

                            stopped = true;

                            lowest_d = d;
                            lowest_ti = ti;

                        }

                    }

                }

                if (stopped) {

                    for (int li = 0; li < light_sources.size(); li++) {

                    }                    

                    int wi = i*wtw+j;

                    pixels[wi*4] = 255;
                    pixels[wi*4 + 1] = 255;
                    pixels[wi*4 + 2] = 255;
                    pixels[wi*4 + 3] = SDL_ALPHA_OPAQUE;

                }

            }
        }

        SDL_UpdateTexture(win_tex, nullptr, pixels.data(), wtw * 4);

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, win_tex, nullptr, nullptr);

        SDL_RenderPresent(renderer);

        dt = (SDL_GetTicks() - start_time) / 1000.0f;

    }

}

// plan: modellera programmet utan användningen av shaders först, för att sedan börja använda OpenGL för renderingen. Blir nog enklast om jag bara börjar rendera skärmen på en
// texture i SDL2. 

// TODO: ha så att ljuset av ett fragment beror på fragmentets distans till ljuskällor och till kameran
