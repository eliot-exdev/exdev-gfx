#include <exdevgfx/ui/ui.h>
#include <exdevgfx/exdev_base.h>

int main() {
    exdev_base_init();

    Application_t app;
    application_init(&app, "test", 640, 480);
    application_run(&app, 40);
    application_destroy(&app);

    exdev_base_deinit();

    return 0;
}