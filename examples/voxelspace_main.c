#include <exdevgfx/exdev_base.h>
#include <exdevgfx/voxelspace.h>
#include <exdevgfx/window.h>
#include <exdevgfx/helper.h>
#include <exdevgfx/matrix.h>
#include <exdevgfx/args.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <stdlib.h>
#include <string.h>

#ifdef __AMIGA__
#define ASSETS_PREFIX ""
#else
#define ASSETS_PREFIX "../share/exdev/"
#endif

#define VERSION "voxelspace 0.7 (26.12.2020)"

#ifndef __linux__
#ifdef __VBCC__
__entry
#endif
unsigned char versiontag[] = "\0$VER: " VERSION;
#endif

#ifdef LOW_RESOLUTION
#define WIDTH 320
#define HEIGHT 240
#define DEFAULT_DISTANCE 200.0f
#define SCALE_HEIGHT 80.0f
#else
#define WIDTH 640
#define HEIGHT 480
#define DEFAULT_DISTANCE 320.0f
#define SCALE_HEIGHT 160.0f
#endif

#define ROTATION_STEP_SIZE 4.0f
#define MOVEMENT_STEP_SIZE 3.0f

static const char COLORMAP_ONE[] = ASSETS_PREFIX"assets/first_color_map_8bit.dat";
static const char COLORMAP_TWO[] = ASSETS_PREFIX"assets/second_color_map_8bit.dat";
static const char COLORMAP_THREE[] = ASSETS_PREFIX"assets/third_color_map_8bit.dat";
static const char COLORMAP_FOUR[] = ASSETS_PREFIX"assets/fourth_color_map_8bit.dat";
static const char COLORMAP_FIVE[] = ASSETS_PREFIX"assets/fifth_color_map_8bit.dat";

static const char HEIGHTMAP_ONE[] = ASSETS_PREFIX"assets/first_height_map.dat";
static const char HEIGHTMAP_TWO[] = ASSETS_PREFIX"assets/second_height_map.dat";
static const char HEIGHTMAP_THREE[] = ASSETS_PREFIX"assets/third_height_map.dat";
static const char HEIGHTMAP_FOUR[] = ASSETS_PREFIX"assets/fourth_height_map.dat";
static const char HEIGHTMAP_FIVE[] = ASSETS_PREFIX"assets/fifth_height_map.dat";

static const char PALETTE_ONE[] = ASSETS_PREFIX"assets/first_color_map_palette.dat";
static const char PALETTE_TWO[] = ASSETS_PREFIX"assets/second_color_map_palette.dat";
static const char PALETTE_THREE[] = ASSETS_PREFIX"assets/third_color_map_palette.dat";
static const char PALETTE_FOUR[] = ASSETS_PREFIX"assets/fourth_color_map_palette.dat";
static const char PALETTE_FIVE[] = ASSETS_PREFIX"assets/fifth_color_map_palette.dat";

static const char *heightmap_path = HEIGHTMAP_ONE;
static const char *colormap_path = COLORMAP_ONE;
static const char *palette_path = PALETTE_ONE;

static float distance = DEFAULT_DISTANCE;
static char demo_mode = 0;

static void print_help() {
    printf("voxelspace [ARGUMENTS]...\n"
           "arguments:\n"
           " -h, --help              print help message and exit\n"
           " -w, --world <number>    select world 1, 2, 3, 4 or 5 (default 1)\n"
           " -d, --demo              enable demo mode\n"
           " -v, --version           print version\n"
           "\n"
           "controls:\n"
           "     w                   move up\n"
           "     s                   move down\n"
           "     a                   strafe left\n"
           "     d                   strafe right\n"
           "     cursors             navigate forward/backward and turn left/right\n"
           "\n"
           "options:\n"
           "     ESC or q            quit\n"
           "     F1                  enable/disable show fps\n"
           "     F2                  reduce rendering quality\n"
           "     F3                  increase rendering quality\n"
           "     F4                  decrease distance\n"
           "     F5                  increase distance\n"
           "     F6                  enable/disable dither x\n"
    );
}

static void print_version() {
    printf(VERSION "\n");
}

static void parse_args(int argc, char **argv) {
    if (args_find_option(argc, argv, 'h', "help")) {
        print_help();
        exit(0);
    }
    if (args_find_option(argc, argv, 'v', "version")) {
        print_version();
        exit(0);
    }
    if (args_find_option(argc, argv, 'd', "demo")) {
        demo_mode = 1;
    }
    const char *ptr = NULL;
    if ((ptr = args_get_option_parameter(argc, argv, 'w', "world")) != NULL) {
        const char world = atoi(ptr);
        switch (world) {
            case 1:
                colormap_path = COLORMAP_ONE;
                heightmap_path = HEIGHTMAP_ONE;
                palette_path = PALETTE_ONE;
                break;
            case 2:
                colormap_path = COLORMAP_TWO;
                heightmap_path = HEIGHTMAP_TWO;
                palette_path = PALETTE_TWO;
                break;
            case 3:
                colormap_path = COLORMAP_THREE;
                heightmap_path = HEIGHTMAP_THREE;
                palette_path = PALETTE_THREE;
                break;
            case 4:
                colormap_path = COLORMAP_FOUR;
                heightmap_path = HEIGHTMAP_FOUR;
                palette_path = PALETTE_FOUR;
                break;
            case 5:
                colormap_path = COLORMAP_FIVE;
                heightmap_path = HEIGHTMAP_FIVE;
                palette_path = PALETTE_FIVE;
                break;
            default:
                log_warning_fmt("there is now world %d", world);
                exit(0);
        }
    }
}

static void move(Vertex3d_t p, const char move_flag, const char strafe_flag, const int phi) {
    if (move_flag == 0 && strafe_flag == 0) {
        return;
    }

    float move = 0.0f;
    float strafe = 0.0f;

    if (move_flag == 1) {
        move = MOVEMENT_STEP_SIZE;
    } else if (move_flag == 2) {
        move = -MOVEMENT_STEP_SIZE;
    }

    if (strafe_flag == 1) {
        strafe = -MOVEMENT_STEP_SIZE;
    } else if (strafe_flag == 2) {
        strafe = MOVEMENT_STEP_SIZE;
    }

    Vertex3d_t v1 = {strafe, 0, -move};
    Vertex3d_t v2 = {p[0], p[2], p[1]};
    Vertex3d_t v3;

    MATRIX_DEFAULT(m);
    matrix_rotateY(m, deg_to_rad((float) phi), m);
    matrix_mul_vector(m, v1, v3);
    vertex3d_add(v3, v2, v3);

    p[0] = v3[0];
    p[1] = v3[2];
    p[2] = v3[1];
}

#define NUM_EVENTS 5

int main(int argc, char **argv) {
    parse_args(argc, argv);

    log_info("--> init all ...");

    int res = exdev_base_init();
    if (res) {
        log_warning_fmt("could no init exdev base=%d", res);
        return 1;
    }

    log_info_fmt("heightmap path=%s", heightmap_path);
    log_info_fmt("colormap path=%s", colormap_path);
    log_info_fmt("palette path=%s", palette_path);
    log_info_fmt("distance=%f", distance);
    log_info_fmt("demo mode=%d", (int) demo_mode);

    // read height map
    log_info("--> reading height map ...");
    Framebuffer8Bit_t height_map;
    res = framebuffer_8bit_read_from_dat(&height_map, heightmap_path);
    log_info_fmt("<-- result reading height map=%d", res);
    if (res) {
        exit(0);
    }

    // read color map
    log_info("--> reading color map ...");
    Framebuffer8Bit_t color_map;
    res = framebuffer_8bit_read_from_dat(&color_map, colormap_path);
    log_info_fmt("<-- result reading color map=%d", res);
    if (res) {
        exit(0);
    }

    // read palette
    log_info("--> reading palette ...");
    Palette8Bit_t palette;
    res = palette_8bit_read_from_dat(&palette, palette_path);
    log_info_fmt("<--- result reading palette=%d", res);
    if (res) {
        exit(0);
    }

    // setup framebuffer to render
    struct Framebuffer8Bit fb;
    framebuffer_8bit_init(&fb, WIDTH, HEIGHT);

    // create font
    Font_t mia1;
    font_init_mia_1(&mia1);
    const unsigned char font_color = palette_8bit_add_pen(&palette, &PEN_YELLOW);
    char fps_text[10];
    memset(fps_text, 0, 10);

    // create voxelspace
    Voxelspace_t v;
    voxelspace_init(&v, &height_map, &color_map, &fb, SCALE_HEIGHT, palette.numPens - 2);

    // cleanup
    framebuffer_8bit_deinit(&height_map);
    framebuffer_8bit_deinit(&color_map);
    log_info("<-- init all done");

    // local variables
    int rotation = 0;
    float horizon = 120.0f;
    int show_fps = 1;
    int skip_x = 0;
    TIMESTAMP before = 0, after = 0;
#ifdef LOW_RESOLUTION
    int dz = 5;
#else
    int dz = 1;
#endif
    Vertex3d_t p;
    vertex3d_set(p, 550, 320, 70);

    char close_event = 0;
    KeyEvent_t events[NUM_EVENTS];

    char ctrl_move = 0; // 0=no, 1=forward, 2=backward
    char ctrl_rotate = 0; // 0=no, 1=right, 2=left
    char ctrl_up_down = 0; // 0=no, 1=up, 2=down
    char ctrl_strafe = 0; // 0=no, 1=left, 2=right

    // show window
    Window_t window = window_create(WIDTH, HEIGHT, "voxel", FS_8_BIT);
    if (!window) {
        log_warning("could not create window or screen");
        exit(0);
    }
    window_update_palette(window, &palette);

    // game loop
    while (!close_event) {
        // read inputs
        //log_debug("before get events");
        const int numKeyEvents = window_poll_events(window, &close_event, events, NUM_EVENTS);
        //log_debug_fmt("num events: %i", numKeyEvents);
        for (int i = 0; i < numKeyEvents; ++i) {
            if (events[i].event == KEY_EVENT_PRESSED) {
                switch (events[i].type) {
                    case KEY_TYPE_ESC:
                        close_event = 1;
                        break;
                    case KEY_TYPE_RIGHT:
                        ctrl_rotate = 1;
                        break;
                    case KEY_TYPE_LEFT:
                        ctrl_rotate = 2;
                        break;
                    case KEY_TYPE_UP:
                        ctrl_move = 1;
                        break;
                    case KEY_TYPE_DOWN:
                        ctrl_move = 2;
                        break;
                    case KEY_TYPE_F1:
                        show_fps = !show_fps;
                        break;
                    case KEY_TYPE_F2:
                        ++dz;
                        break;
                    case KEY_TYPE_F3:
                        if (dz > 1)
                            --dz;
                        break;
                    case KEY_TYPE_F4:
                        if (distance >= 30.0f) {
                            distance -= 10.f;
                        }
                        break;
                    case KEY_TYPE_F5:
                        if (distance <= 600.0f) {
                            distance += 10.f;
                        }
                        break;
                    case KEY_TYPE_F6:
                        skip_x = !skip_x;
                        break;
                    case KEY_TYPE_CODE:
                        if (events[i].code == 'w') {
                            ctrl_up_down = 1;
                        } else if (events[i].code == 's') {
                            ctrl_up_down = 2;
                        } else if (events[i].code == 'a') {
                            ctrl_strafe = 1;
                        } else if (events[i].code == 'd') {
                            ctrl_strafe = 2;
                        } else if (events[i].code == 'q') {
                            close_event = 1;
                        }
                        break;
                }
            } else if (events[i].event == KEY_EVENT_RELEASED) {
                switch (events[i].type) {
                    case KEY_TYPE_RIGHT:
                    case KEY_TYPE_LEFT:
                        ctrl_rotate = 0;
                        break;
                    case KEY_TYPE_UP:
                    case KEY_TYPE_DOWN:
                        ctrl_move = 0;
                        break;
                    case KEY_TYPE_CODE:
                        if (events[i].code == 'w' || events[i].code == 's') {
                            ctrl_up_down = 0;
                        } else if (events[i].code == 'a' || events[i].code == 'd') {
                            ctrl_strafe = 0;
                        }
                }
            }
        }

        // demo mode
        if (demo_mode) {
            ctrl_move = 1;
            ctrl_rotate = 0;
            ctrl_up_down = 0;
            ctrl_strafe = 0;
            p[2] = -1.0f; // use auto height
        }

        // update
        move(p, ctrl_move, ctrl_strafe, rotation);

        switch (ctrl_rotate) {
            case 1:
                rotation -= ROTATION_STEP_SIZE;
                rotation = rotation % 360;
                break;
            case 2:
                rotation += ROTATION_STEP_SIZE;
                rotation = rotation % 360;
                break;
        }

        switch (ctrl_up_down) {
            case 1:
                if (p[2] <= 120.0f) {
                    p[2] += MOVEMENT_STEP_SIZE;
                }
                break;
            case 2:
                if (p[2] >= 5.0f) {
                    p[2] -= MOVEMENT_STEP_SIZE;
                }
                break;
        }

        // render
        log_debug("--> render");
        p[0] = normalize_float(p[0], v.heightmap.height);
        p[1] = normalize_float(p[1], v.heightmap.width);
        voxelspace_render(p,
                          deg_to_rad((float) rotation),
                          horizon,
                          distance,
                          (float) dz,
                          skip_x,
                          &v);

        after = now();
        // draw text
        if (show_fps) {
            sprintf(fps_text, "%li", 1000 / (after - before));
            framebuffer_8bit_draw_text(&fb, &mia1, fps_text, strlen(fps_text), font_color, 10, fb.height - 10);
        }
        before = now();
        window_fill_8bit(window, &fb);
        log_debug("<-- render");
    }

    // cleanup
    log_info("--> cleanup ...");
    window_destroy(window);
    voxelspace_deinit(&v);
    font_deinit(&mia1);
    log_info("<-- cleanup done");

    return exdev_base_deinit();
}
