#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#define MAX_BUFFER 256
#define SOUND_CARD "PCM"

/**
 * Get the current volume status
 * @return A string containing volume info or "mute"
 */
char* get_volume() {
    static char volume[32];
    FILE *fp;
    char buffer[MAX_BUFFER];

    // Execute amixer command and capture output
    fp = popen("amixer get " SOUND_CARD " | grep -oP '\\d+%' | head -n 1", "r");
    if (fp == NULL) {
        return "vol: err";
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    } else {
        strcpy(buffer, "err");
    }
    pclose(fp);

    // Check if muted
    fp = popen("amixer get " SOUND_CARD " | grep -oP '\\[off\\]' | head -n 1", "r");
    if (fp == NULL) {
        return "vol: err";
    }

    char mute_buffer[MAX_BUFFER];
    if (fgets(mute_buffer, sizeof(mute_buffer), fp) != NULL) {
        strcpy(volume, "mute");
    } else {
        strcpy(volume, buffer);
    }
    pclose(fp);

    return volume;
}

/**
 * Get the current date and time
 * @return A string with date and time in format "DD.MM.YYYY HH:MM:SS"
 */
char* get_datetime() {
    static char datetime[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(datetime, sizeof(datetime), "%d.%m.%Y %T", t);
    return datetime;
}

/**
 * Get network interface status
 * @return A string describing the network status
 */
char* get_network_info() {
    static char network[64];
    FILE *fp;
    char eth_status[16] = "down";
    char wifi_status[16] = "down";
    char buffer[MAX_BUFFER];
    // Check Ethernet status
    fp = popen("cat /sys/class/net/eth0/operstate 2>/dev/null", "r");
    if (fp != NULL) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strcmp(buffer, "up") == 0) {
                strcpy(eth_status, "up");
            }
        }
        pclose(fp);
    }
    // Check WiFi status
    fp = popen("cat /sys/class/net/wlan0/operstate 2>/dev/null", "r");
    if (fp != NULL) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strcmp(buffer, "up") == 0) {
                strcpy(wifi_status, "up");
            }
        }
        pclose(fp);
    }
    // Create status string
    if (strcmp(eth_status, "up") == 0) {
        sprintf(network, "e: up");
    } else if (strcmp(wifi_status, "up") == 0) {
        sprintf(network, "w: up");
    } else {
        sprintf(network, "e: down | w: down");
    }
    return network;
}

/**
 * Get keyboard layout
 * @return String containing simple layout code
 */
char *get_kb_layout(Display* display) {
    static char layout[32] = "??"; // Static buffer for return value
    
    // Get current keyboard state
    XkbStateRec state;
    XkbGetState(display, XkbUseCoreKbd, &state);
    
    // Get layout information
    XkbRF_VarDefsRec vd;
    
    // Get layout names from X properties
    if (XkbRF_GetNamesProp(display, NULL, &vd) && vd.layout) {
        // Split comma-separated layouts and find current one
        char *layouts = strdup(vd.layout);
        char *tok = strtok(layouts, ",");
        
        // Iterate to the current group
        for (int i = 0; i < state.group && tok; i++) {
            tok = strtok(NULL, ",");
        }
        
        if (tok) {
            strncpy(layout, tok, sizeof(layout) - 1);
            layout[sizeof(layout) - 1] = '\0'; // Ensure null termination
        }
        
        // Clean up
        free(layouts);
        free(vd.layout);
        free(vd.variant);
        free(vd.model);
        free(vd.options);
    }
    
    return layout;
}

/**
 * Main function to run the status bar using Xlib
 */
int main() {
    Display *display;
    Window root;
    char status[MAX_BUFFER];

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    // Get the root window
    root = DefaultRootWindow(display);

    while (1) {
        // Concatenate all modules
        snprintf(status, sizeof(status), "%s | %s | vol: %s | %s", 
                 get_kb_layout(display), get_network_info(),
                 get_volume(), get_datetime());

        // Set the window name
        XStoreName(display, root, status);
        XFlush(display);

        // Sleep for 1 second before updating again
        sleep(1);
    }

    // Clean up (though we'll never reach this in an infinite loop)
    XCloseDisplay(display);
    return 0;
}
