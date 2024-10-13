#define EXDEVGFX2_LOG_LEVEL 1

#include <exdevgfx/logger.h>

int main(){
    log_debug("debug message");
    log_debug_fmt("debug message with format %d", 10);

    log_info("info message");
    log_info_fmt("info message with format %d", 10);

    log_warning("warning message");
    log_warning_fmt("warning message with format %d", 10);
    return 0;
}
