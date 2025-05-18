#include <exdevgfx/exdev_base.h>
#include <exdevgfx/voxelspace.h>
#include <exdevgfx/window.h>
#include <exdevgfx/helper.h>
#include <exdevgfx/args.h>
#include <exdevgfx/vertex2d.h>

#define EXDEVGFX2_LOG_LEVEL 2

#include <exdevgfx/logger.h>

#include <stdlib.h>
#include <string.h>

#ifdef __AMIGA__
#define ASSETS_PREFIX ""
#else
#define ASSETS_PREFIX ""
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
#define HORIZON 120.0f
#else
#define WIDTH 640
#define HEIGHT 480
#define DEFAULT_DISTANCE 320.0f
#define SCALE_HEIGHT 160.0f
#define HORIZON 240.0f
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

#ifdef LOW_RESOLUTION
static const char SKY_TEXTURE_ONE[] = ASSETS_PREFIX"assets/first_sky_lowres.dat";
static const char SKY_TEXTURE_TWO[] = ASSETS_PREFIX"assets/second_sky_lowres.dat";
static const char SKY_TEXTURE_THREE[] = ASSETS_PREFIX"assets/third_sky_lowres.dat";
static const char SKY_TEXTURE_FOUR[] = ASSETS_PREFIX"assets/fourth_sky_lowres.dat";
static const char SKY_TEXTURE_FIVE[] = ASSETS_PREFIX"assets/fifth_sky_lowres.dat";
#else
static const char SKY_TEXTURE_ONE[] = ASSETS_PREFIX"assets/first_sky.dat";
static const char SKY_TEXTURE_TWO[] = ASSETS_PREFIX"assets/second_sky.dat";
static const char SKY_TEXTURE_THREE[] = ASSETS_PREFIX"assets/third_sky.dat";
static const char SKY_TEXTURE_FOUR[] = ASSETS_PREFIX"assets/fourth_sky.dat";
static const char SKY_TEXTURE_FIVE[] = ASSETS_PREFIX"assets/fifth_sky.dat";
#endif

static const char *heightmap_path = HEIGHTMAP_ONE;
static const char *colormap_path = COLORMAP_ONE;
static const char *palette_path = PALETTE_ONE;
static const char *sky_path = SKY_TEXTURE_ONE;

static float distance = DEFAULT_DISTANCE;
static char demo_mode = 0;

#ifdef PROFILE_APPLICATION
static exdev_timestamp_t tp;
#define begin_profile() tp = now();
#define update_profile(M) {const exdev_timestamp_t tp_tmp = now(); log_info_fmt("time for %s: %d ms", M, tp_tmp-tp); tp = tp_tmp;}
#else
#define begin_profile()
#define update_profile(M)
#endif

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
           "     F6                  enable/disable dither x\n");
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
                sky_path = SKY_TEXTURE_ONE;
                break;
            case 2:
                colormap_path = COLORMAP_TWO;
                heightmap_path = HEIGHTMAP_TWO;
                palette_path = PALETTE_TWO;
                sky_path = SKY_TEXTURE_TWO;
                break;
            case 3:
                colormap_path = COLORMAP_THREE;
                heightmap_path = HEIGHTMAP_THREE;
                palette_path = PALETTE_THREE;
                sky_path = SKY_TEXTURE_THREE;
                break;
            case 4:
                colormap_path = COLORMAP_FOUR;
                heightmap_path = HEIGHTMAP_FOUR;
                palette_path = PALETTE_FOUR;
                sky_path = SKY_TEXTURE_FOUR;
                break;
            case 5:
                colormap_path = COLORMAP_FIVE;
                heightmap_path = HEIGHTMAP_FIVE;
                palette_path = PALETTE_FIVE;
                sky_path = SKY_TEXTURE_FIVE;
                break;
            default:
                log_warning_fmt("there is now world %d", world);
                exit(0);
        }
    }
}

#define MAX_HEIGHT 120.0f
#define MIN_HEIGHT 5.0f

static void move(Vertex3d_t p, const char move_flag, const char strafe_flag, const char up_down_flag, const int rot) {
    if (move_flag == 0 && strafe_flag == 0 && up_down_flag == 0) {
        return;
    }

    Vertex2d_t movement = {0.f, 0.f};

    // move forward/backward
    if (move_flag == 1) {
        movement[1] = -MOVEMENT_STEP_SIZE;
    } else if (move_flag == 2) {
        movement[1] = MOVEMENT_STEP_SIZE;
    }

    // strafe left/right
    if (strafe_flag == 1) {
        movement[0] = -MOVEMENT_STEP_SIZE;
    } else if (strafe_flag == 2) {
        movement[0] = MOVEMENT_STEP_SIZE;
    }

    // rotate
    Vertex2d_t dst;
    vertex2d_rotate(movement, dst, deg_to_rad((float) -rot));

    p[0] += dst[0];
    p[1] += dst[1];

    // move up/down
    if (up_down_flag == 1 && p[2] < MAX_HEIGHT) {
        p[2] += MOVEMENT_STEP_SIZE;
    } else if (up_down_flag == 2 && p[2] > MIN_HEIGHT) {
        p[2] -= MOVEMENT_STEP_SIZE;
    }
}

int main(int argc, char **argv) {
    parse_args(argc, argv);

    log_info("--> init all ...");

    int res = exdev_base_init();
    if (res) {
        log_warning_fmt("could no init exdev base=%d", res);
        return 1;
    }

    log_info_fmt("resolution=%d:%d", WIDTH, HEIGHT);
    log_info_fmt("heightmap path=%s", heightmap_path);
    log_info_fmt("colormap path=%s", colormap_path);
    log_info_fmt("palette path=%s", palette_path);
    log_info_fmt("sky texture path=%s", sky_path);
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

    // read sky texture
    Framebuffer8Bit_t sky_texture;
    log_info("--> reading sky texture ...");
    res = framebuffer_8bit_read_from_dat(&sky_texture, sky_path);
    log_info_fmt("<--- result reading sky texture=%d", res);
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
    voxelspace_init(&v, &height_map, &color_map, &fb, SCALE_HEIGHT, palette.numPens - 2, &sky_texture);

    // cleanup
    framebuffer_8bit_deinit(&height_map);
    framebuffer_8bit_deinit(&color_map);
    log_info("<-- init all done");

    // local variables
    int rotation = 0;
    int show_fps = 1;
    int skip_x = 0;
    exdev_timestamp_t before = 0, after = 0;
#ifdef LOW_RESOLUTION
    int dz = 5;
#else
    int dz = 1;
#endif
    Vertex3d_t position;
    vertex3d_set(position, 512, 512, 80);

    char ctrl_move = 0; // 0=no, 1=forward, 2=backward
    char ctrl_rotate = 0; // 0=no, 1=right, 2=left
    char ctrl_up_down = 0; // 0=no, 1=up, 2=down
    char ctrl_strafe = 0; // 0=no, 1=left, 2=right

    // show window
    Window_t *window = window_create(WIDTH, HEIGHT, "voxel", FS_8_BIT);
    if (!window) {
        log_warning("could not create window or screen");
        exit(0);
    }
    window_update_palette(window, &palette);

    //events
    char close_event = 0;
    Event_t event;
    // game loop
    while (!close_event) {
        begin_profile();
        // read inputs
        window_poll_events(window, &close_event, &event, 1);
        // keyboard events
        if (event.type == EVENT_KEY && event.key_event.event == KEY_EVENT_PRESSED) {
            switch (event.key_event.key) {
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
                    if (event.key_event.code == 'w') {
                        ctrl_up_down = 1;
                    } else if (event.key_event.code == 's') {
                        ctrl_up_down = 2;
                    } else if (event.key_event.code == 'a') {
                        ctrl_strafe = 1;
                    } else if (event.key_event.code == 'd') {
                        ctrl_strafe = 2;
                    } else if (event.key_event.code == 'q') {
                        close_event = 1;
                    }
                    break;
                default:
                    break;
            }
        } else if (event.type == EVENT_KEY && event.key_event.event == KEY_EVENT_RELEASED) {
            switch (event.key_event.key) {
                case KEY_TYPE_RIGHT:
                case KEY_TYPE_LEFT:
                    ctrl_rotate = 0;
                    break;
                case KEY_TYPE_UP:
                case KEY_TYPE_DOWN:
                    ctrl_move = 0;
                    break;
                case KEY_TYPE_CODE:
                    if (event.key_event.code == 'w' || event.key_event.code == 's') {
                        ctrl_up_down = 0;
                    } else if (event.key_event.code == 'a' || event.key_event.code == 'd') {
                        ctrl_strafe = 0;
                    }
                default:
                    break;
            }
        }

        // demo mode
        if (demo_mode) {
            ctrl_move = 1;
            ctrl_rotate = 0;
            ctrl_up_down = 0;
            ctrl_strafe = 0;
            position[2] = -1.0f; // use auto height
        }

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

        // update
        move(position, ctrl_move, ctrl_strafe, ctrl_up_down, rotation);

        // render
        log_debug("--> render");
        position[0] = normalize_float(position[0], (float) v.heightmap.height);
        position[1] = normalize_float(position[1], (float) v.heightmap.width);
        update_profile("update world");

        voxelspace_render(position, rotation, HORIZON, distance, (float) dz, skip_x, &v);

        // draw text
        if (show_fps) {
            after = now();
            long time_elapsed = after - before;
            if (time_elapsed == 0) {
                time_elapsed = 1;
            }
            sprintf(fps_text, "%li", 1000 / time_elapsed);
            framebuffer_8bit_draw_text(&fb, &mia1, fps_text, strlen(fps_text), font_color, 20, HEIGHT - 20);
        }
        before = now();
        update_profile("render world");

        window_fill_8bit(window, &fb);
        update_profile("blit image");
        log_debug("<-- render");
    }

    // cleanup
    log_info("--> cleanup ...");
    window_destroy(window);
    voxelspace_deinit(&v);
    font_deinit(&mia1);
    framebuffer_8bit_deinit(&sky_texture);
    framebuffer_8bit_deinit(&fb);
    log_info("<-- cleanup done");

    return exdev_base_deinit();
}
