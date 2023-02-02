#include <exdevgfx/exdev_base.h>
#include <exdevgfx/window.h>
#include <exdevgfx/helper.h>
#include <exdevgfx/font.h>
#include <exdevgfx/sw_renderer_8bit.h>
#include <exdevgfx/args.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <string.h>
#include <stdlib.h>

#define VERSION "test_3d 0.1 (18.12.2020)"

#ifdef __AMIGA__
#ifdef __VBCC__
__entry
#endif
unsigned char versiontag[] = "\0$VER: " VERSION;
#endif

#ifndef LOW_RESOLUTION
#define WIDTH 640
#define HEIGHT 480
#define TRIANGLE_SIZE 150
#else
#define WIDTH 320
#define HEIGHT 240
#define TRIANGLE_SIZE 75
#endif

static void print_help() {
    printf("voxelspace [ARGUMENTS]...\n"
           "arguments:\n"
           " -h, --help              print help message and exit\n"
           " -v, --version           print version\n"
           "commands:\n"
           " cursor keys              translate up, dow, left, right\n"
           " +/-                      translate Z axis/zoom in/out\n"
           " x/X                      rotate X axis\n"
           " y/Y                      rotate Y axis\n"
           " z/Z                      rotate Z axis\n"
           " p/P                      change projection constant\n");
}

static void print_version() {
    printf(VERSION"\n");
}

void parse_args(int argc, char **argv) {
    if (args_find_option(argc, argv, 'h', "help")) {
        print_help();
        exit(0);
    }
    if (args_find_option(argc, argv, 'v', "version")) {
        print_version();
        exit(0);
    }
}

#define MOVE_STEP_SIZE 0.25f
#define ROT_STEP_SIZE 4.0f
#define AUTO_ROTATE 1.0f

#define BLACK 0
#define WHITE 1
#define CYAN 2
#define RED 3
#define BLUE 4
#define GRAY 5
#define GREEN 6
#define YELLOW 7

int main(int argc, char **argv) {
    parse_args(argc, argv);

    // init
    int res = exdev_base_init();
    if (res) {
        log_warning_fmt("could no init exdev base=%d", res);
        return 1;
    }

    // framebuffer
    Framebuffer8Bit_t framebuffer;
    framebuffer_8bit_init(&framebuffer, WIDTH, HEIGHT);

    // software renderer
    SWRenderer8bit_t swRenderer;
    sw_renderer_8bit_init(&swRenderer, &framebuffer);

    // colors
    Palette8Bit_t palette;
    res = palette_8bit_read_from_dat(&palette, "assets/texture_8bit_palette.dat");
    if (res) {
        log_warning_fmt("could not read palette=%d", res);
        return 2;
    }

    // texture
    Framebuffer8Bit_t texture;
    res = framebuffer_8bit_read_from_dat(&texture, "assets/texture_8bit.dat");
    if (res) {
        log_warning_fmt("could not read palette=%d", res);
        return 3;
    }

    // window
    Window_t window = window_create(WIDTH, HEIGHT, "test_3d", FS_8_BIT);
    if (!window) {
        log_warning("could not create window");
        return 2;
    }
    window_update_palette(window, &palette);

    // font
    Font_t mia1;
    font_init_mia_1(&mia1);

    // cube
    Vertex3d_t cube_triangles[36] = {{-1, -1, -1}, // front
                                     {1,  1,  -1},
                                     {-1, 1,  -1},
                                     {-1, -1, -1},
                                     {1,  -1, -1},
                                     {1,  1,  -1},
                                     {-1, -1, 1}, // left
                                     {-1, 1,  -1},
                                     {-1, 1,  1},
                                     {-1, -1, 1},
                                     {-1, -1, -1},
                                     {-1, 1,  -1},
                                     {1,  -1, 1}, // right
                                     {1,  1,  -1},
                                     {1,  -1, -1},
                                     {1,  -1, 1},
                                     {1,  1,  1},
                                     {1,  1,  -1},
                                     {-1, -1, 1}, // back
                                     {-1, 1,  1},
                                     {1,  1,  1},
                                     {-1, -1, 1},
                                     {1,  1,  1},
                                     {1,  -1, 1},
                                     {-1, 1,  -1}, // top
                                     {1,  1,  1},
                                     {-1, 1,  1},
                                     {-1, 1,  -1},
                                     {1,  1,  -1},
                                     {1,  1,  1},
                                     {-1, -1, -1}, // bottom
                                     {-1, -1, 1},
                                     {1,  -1, 1},
                                     {-1, -1, -1},
                                     {1,  -1, 1},
                                     {1,  -1, -1}};
//    Vertex3d_t cube_normals[12] = {
//            {0,  0,  -1}, // front
//            {0,  0,  -1},
//            {-1, 0,  0}, // left
//            {-1, 0,  0},
//            {1,  0,  0}, // right
//            {1,  0,  0},
//            {0,  0,  1}, // back
//            {0,  0,  1},
//            {0,  1,  0}, // top
//            {0,  1,  0},
//            {0,  -1, 0}, // bottom
//            {0,  -1, 0}
//    };
//    const float texture_coordinates[6][2] = {{0.0f, 1.0f},
//                                             {1.0f, 1.0f},
//                                             {0.0f, 0.0f},
//                                             {0.0f, 1.0f},
//                                             {1.0f, 1.0f},
//                                             {1.0f, 0.0f}};

    Color8Bit_t cube_colors[12] = {RED, RED, GREEN, GREEN, BLUE, BLUE, CYAN, CYAN, WHITE, WHITE, GRAY, GRAY};
    Vertex3d_t cube_translation = {0, 0, 5};
    Vertex3d_t cube_rotation = {0, 0, 0};

    // work loop
    Event_t event;
    char close_event = 0;
    TIMESTAMP before = 0;
    TIMESTAMP after = 0;
    char fps_text[128];
    int show_fps = 1;
    while (!close_event) {
        // read inputs
        window_poll_events(window, &close_event, &event, 1);
        if (event.type == EVENT_KEY && event.key_event.event == KEY_EVENT_PRESSED) {
            switch (event.key_event.key) {
                case KEY_TYPE_ESC:
                    close_event = 1;
                    break;
                case KEY_TYPE_F1:
                    show_fps = !show_fps;
                    break;
                case KEY_TYPE_UP:
                    cube_translation[1] += MOVE_STEP_SIZE;
                    break;
                case KEY_TYPE_DOWN:
                    cube_translation[1] -= MOVE_STEP_SIZE;
                    break;
                case KEY_TYPE_LEFT:
                    cube_translation[0] -= MOVE_STEP_SIZE;
                    break;
                case KEY_TYPE_RIGHT:
                    cube_translation[0] += MOVE_STEP_SIZE;
                    break;
                case KEY_TYPE_CODE: {
                    if (event.key_event.code == '+') {
                        cube_translation[2] -= MOVE_STEP_SIZE;
                    } else if (event.key_event.code == '-') {
                        cube_translation[2] += MOVE_STEP_SIZE;
                    } else if (event.key_event.code == 'p') {
                        swRenderer.pc -= MOVE_STEP_SIZE;
                        log_info_fmt("pc=%f\n", swRenderer.pc);
                    } else if (event.key_event.code == 'P') {
                        swRenderer.pc += MOVE_STEP_SIZE;
                        log_info_fmt("pc=%f\n", swRenderer.pc);
                    } else if (event.key_event.code == 'y') {
                        cube_rotation[1] -= deg_to_rad(ROT_STEP_SIZE);
                    } else if (event.key_event.code == 'Y') {
                        cube_rotation[1] += deg_to_rad(ROT_STEP_SIZE);
                    } else if (event.key_event.code == 'x') {
                        cube_rotation[0] -= deg_to_rad(ROT_STEP_SIZE);
                    } else if (event.key_event.code == 'X') {
                        cube_rotation[0] += deg_to_rad(ROT_STEP_SIZE);
                    } else if (event.key_event.code == 'z') {
                        cube_rotation[2] -= deg_to_rad(ROT_STEP_SIZE);
                    } else if (event.key_event.code == 'Z') {
                        cube_rotation[2] += deg_to_rad(ROT_STEP_SIZE);
                    } else if (event.key_event.code == 'q') {
                        close_event = 1;
                    }
                }
                    break;
                default:
                    break;
            }
        }

        // update
        cube_rotation[1] += deg_to_rad(AUTO_ROTATE);

        // clear framebuffer
        framebuffer_8bit_fill(&framebuffer, BLACK);

        // render cube color
        sw_renderer_8bit_fill_polygon_color_new(&swRenderer, cube_triangles, // first rect will have texture
                                                12, cube_colors, cube_translation, cube_rotation);
        // render cube texture
//        sw_renderer_8bit_fill_polygon_texture(&swRenderer,
//                                              cube_triangles,
//                                              texture_coordinates,
//                                              2,
//                                              &texture,
//                                              cube_translation,
//                                              cube_rotation);

        // render texture
        framebuffer_8bit_draw_framebuffer(&framebuffer, framebuffer.width - texture.width, framebuffer.height - texture.height, &texture);
        // fps
        after = now();
        if (show_fps) {
            sprintf(fps_text, "%li", 1000 / (after - before));
            framebuffer_8bit_draw_text(&framebuffer, &mia1, fps_text, (int) strlen(fps_text), YELLOW, 10, framebuffer.height - 10);
        }
        before = now();

        window_fill_8bit(window, &framebuffer);
    }

    window_destroy(window);
    font_deinit(&mia1);
    sw_renderer_8bit_deinit(&swRenderer);
    framebuffer_8bit_deinit(&framebuffer);
    framebuffer_8bit_deinit(&texture);
    exdev_base_deinit();

    return 0;
}
