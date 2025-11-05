#include <exdevgfx/ui/ui.h>

int main() {
    Application_t app;
    application_init(&app, "test", 640, 480);
    application_run(&app, 40);
    application_destroy(&app);

    return 0;
}