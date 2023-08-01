
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
    int wtw = 100;
    int wth = 100;
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

                        case SDLK_w:
                            p += glm::vec3(glm::rotate(glm::mat4(1.0f), y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f)) * dt;
                            break;
                        case SDLK_s:
                            p -= glm::vec3(glm::rotate(glm::mat4(1.0f), y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f)) * dt;
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

                float brightness = ambient_light;
                
                glm::vec3 start_pos = glm::vec3((float)j/wtw-0.5f, (float)i/wth-0.5f, z0);
                glm::vec3 angled_vec = rot_mat * glm::vec4(glm::normalize(start_pos), 0.0f);

                bool stopped = false;
                int stopped_triangle_index = 0;

                float lowest_dist = max_ray_dist;

                for (int ti = 0; ti < vertices.size(); ti+=3) {
                    
                    glm::vec3 normal = glm::normalize(glm::cross(vertices[ti+1] - vertices[ti], vertices[ti+2] - vertices[ti]));
                    glm::vec3 plane_point = vertices[ti] - p;
                    // verkar bli fel med denna
                    // antar att det kan vara fel med att angled_vec går från p och inte origo

                    // angled_vec verkar orsaka felet
                    
                    //std::cout << "angled vec: " << angled_vec.x << " " << angled_vec.y << " " << angled_vec.z << std::endl;

                    float d = (normal.x*plane_point.x + normal.y*plane_point.y + normal.z*plane_point.z) / (normal.x*angled_vec.x + normal.y*angled_vec.y + normal.z*angled_vec.z);

                    if (d > 0.0f && d < lowest_dist) {

                        // kolla om punkten som går genom planet går genom triangeln

                        glm::vec3 ray_point = d * angled_vec;

                        glm::vec3 A = vertices[ti] - p;
                        glm::vec3 B = vertices[ti+1] - p;
                        glm::vec3 C = vertices[ti+2] - p;

                        // kan lagras i normalen
                        float triangle_area = glm::length(glm::cross(B - A, C - A)) / 2;

                        float alpha = (glm::length(glm::cross(A - ray_point, B - ray_point)) / 2) / triangle_area;
                        float beta = (glm::length(glm::cross(B - ray_point, C - ray_point)) / 2) / triangle_area;
                        float gamma = (glm::length(glm::cross(C - ray_point, A - ray_point)) / 2) / triangle_area;

                        if (alpha >= 0 && beta >= 0 && gamma >= 0 && alpha + beta + gamma > 0.98f && alpha + beta + gamma < 1.02f) {

                            lowest_dist = d;
                            stopped_triangle_index = ti/3;
                            if (!stopped) {
                                stopped = true;
                            }

                        }

                    }

                }

                if (stopped) {

                    for (int li = 0; li < light_sources.size(); li++) {

                        bool reaches = true;

                        glm::vec3 ttl = light_sources[li] - (p + angled_vec * lowest_dist);
                        glm::vec3 ttl_normalized = glm::normalize(ttl);

                        for (int ti = 0; ti < vertices.size()/3; ti++) {
                            
                            if (ti != stopped_triangle_index) {
                                
                                glm::vec3 A = vertices[ti*3] - p;
                                glm::vec3 B = vertices[ti*3+1] - p;
                                glm::vec3 C = vertices[ti*3+2] - p;

                                glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));

                                float d = (normal.x*A.x + normal.y*A.y + normal.z*A.z) / (normal.x*ttl.x + normal.y*ttl.y + normal.z*ttl.z);

                                if (d >= 0.0f && d <= glm::length(ttl)) {

                                    glm::vec3 ray_point = d * ttl_normalized;

                                    float triangle_area = glm::length(glm::cross(B - A, C - A)) / 2;

                                    float alpha = (glm::length(glm::cross(A - ray_point, B - ray_point)) / 2) / triangle_area;
                                    float beta = (glm::length(glm::cross(B - ray_point, C - ray_point)) / 2) / triangle_area;
                                    float gamma = (glm::length(glm::cross(C - ray_point, A - ray_point)) / 2) / triangle_area;

                                    if (alpha >= 0 && beta >= 0 && gamma >= 0 && alpha + beta + gamma > 0.98f && alpha + beta + gamma < 1.02f) {
                                        //glm::vec3 n = glm::normalize(glm::cross(vertices[stopped_triangle_index*3+1] - vertices[stopped_triangle_index*3+1], vertices[stopped_triangle_index*3+2] - vertices[stopped_triangle_index*3]));

                                        reaches = false;
                                        break;

                                    }

                                }

                            }

                        }

                        if (reaches) {

                            glm::vec3 normal = glm::normalize(glm::cross(vertices[stopped_triangle_index*3+1] - vertices[stopped_triangle_index*3], vertices[stopped_triangle_index*3+2] - vertices[stopped_triangle_index*3]));
                            float dist = glm::length(ttl);
                            brightness += glm::dot(normal, ttl_normalized) / (dist*dist);


                            std::cout << "a test" << std::endl;
                        }

                    }

                    //float inv_sqrt = 1.0f / (lowest_dist*lowest_dist);

                    brightness = (brightness > 1.0f) ? 1.0f : brightness;

                    //brightness /= lowest_dist*lowest_dist;

                    const unsigned int offset = ( wtw * i * 4 ) + j * 4;
                    pixels[offset]      = 255 * brightness;
                    pixels[offset+1]    = 255 * brightness;
                    pixels[offset+2]    = 255 * brightness;
                    pixels[offset+3]    = SDL_ALPHA_OPAQUE;

                    // do essentially the same process, but from the position where the ray was stopped, to every light source

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
