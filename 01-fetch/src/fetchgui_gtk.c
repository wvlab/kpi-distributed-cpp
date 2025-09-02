#include "fetchbase.h"
#include <gtk/gtk.h>
#include <stdio.h>

typedef struct {
        GtkLabel *os_label;
        GtkLabel *cpu_label;
        GtkLabel *ram_label;
        GtkLabel *hostname_label;
        GtkLabel *ip_label;
        GtkLabel *uptime_label;
} AppWidgets;

static void on_window_destroy(GtkWidget *widget, gpointer user_data)
{
        g_free(user_data);
        return;
}

void update_info(AppWidgets *widgets)
{
        struct KDC_OSInfo os;
        if (get_os_info(&os) == 0) {
                char buffer[256];
                snprintf(
                    buffer, sizeof(buffer), "%s %s (%s)", os.os_name,
                    os.os_version, os.arch
                );
                gtk_label_set_text(widgets->os_label, buffer);
        } else {
                gtk_label_set_text(widgets->os_label, "Failed to get OS info");
        }

        struct KDC_CPUInfo cpu;
        if (get_cpu_info(&cpu) == 0) {
                char buffer[256];
                snprintf(
                    buffer, sizeof(buffer), "%s, cores=%d, frequency=%lu Hz",
                    cpu.name, cpu.cores, cpu.frequency_hz
                );
                gtk_label_set_text(widgets->cpu_label, buffer);
        } else {
                gtk_label_set_text(
                    widgets->cpu_label, "Failed to get CPU info"
                );
        }

        struct KDC_RAMInfo ram;
        if (get_ram_info(&ram) == 0) {
                char buffer[256];
                snprintf(
                    buffer, sizeof(buffer),
                    "total=%lu MB, used=%lu MB, free=%lu MB",
                    ram.total_bytes / 1024 / 1024, ram.used_bytes / 1024 / 1024,
                    ram.free_bytes / 1024 / 1024
                );
                gtk_label_set_text(widgets->ram_label, buffer);
        } else {
                gtk_label_set_text(
                    widgets->ram_label, "Failed to get RAM info"
                );
        }

        struct KDC_NetworkInfo net;
        if (get_network_info(&net) == 0) {
                gtk_label_set_text(widgets->hostname_label, net.hostname);
                gtk_label_set_text(widgets->ip_label, net.ip_address);
        } else {
                gtk_label_set_text(
                    widgets->hostname_label, "Failed to get Hostname"
                );
                gtk_label_set_text(
                    widgets->ip_label, "Failed to get IP Address"
                );
        }

        struct KDC_UptimeInfo up;
        if (get_uptime_info(&up) == 0) {
                char buffer[64];
                snprintf(
                    buffer, sizeof(buffer), "%lu seconds", up.uptime_seconds
                );
                gtk_label_set_text(widgets->uptime_label, buffer);
        } else {
                gtk_label_set_text(
                    widgets->uptime_label, "Failed to get Uptime"
                );
        }
}

gboolean refresh_callback(gpointer user_data)
{
        update_info((AppWidgets *)user_data);
        return G_SOURCE_CONTINUE;
}

static void activate(GtkApplication *app, gpointer user_data)
{
        GtkWidget *main_window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(main_window), "System Information");
        gtk_window_set_default_size(GTK_WINDOW(main_window), 400, 300);

        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
        gtk_widget_set_margin_start(grid, 10);
        gtk_widget_set_margin_end(grid, 10);
        gtk_widget_set_margin_top(grid, 10);
        gtk_widget_set_margin_bottom(grid, 10);
        gtk_window_set_child(GTK_WINDOW(main_window), grid);

        AppWidgets *widgets = g_new(AppWidgets, 1);

        GtkWidget *os_label_title = gtk_label_new("OS:");
        gtk_widget_set_halign(os_label_title, GTK_ALIGN_START);
        GtkWidget *os_label_value = gtk_label_new("N/A");
        gtk_widget_set_halign(os_label_value, GTK_ALIGN_START);
        widgets->os_label = GTK_LABEL(os_label_value);
        gtk_grid_attach(GTK_GRID(grid), os_label_title, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), os_label_value, 1, 0, 1, 1);

        GtkWidget *cpu_label_title = gtk_label_new("CPU:");
        gtk_widget_set_halign(cpu_label_title, GTK_ALIGN_START);
        GtkWidget *cpu_label_value = gtk_label_new("N/A");
        gtk_widget_set_halign(cpu_label_value, GTK_ALIGN_START);
        widgets->cpu_label = GTK_LABEL(cpu_label_value);
        gtk_grid_attach(GTK_GRID(grid), cpu_label_title, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), cpu_label_value, 1, 1, 1, 1);

        GtkWidget *ram_label_title = gtk_label_new("RAM:");
        gtk_widget_set_halign(ram_label_title, GTK_ALIGN_START);
        GtkWidget *ram_label_value = gtk_label_new("N/A");
        gtk_widget_set_halign(ram_label_value, GTK_ALIGN_START);
        widgets->ram_label = GTK_LABEL(ram_label_value);
        gtk_grid_attach(GTK_GRID(grid), ram_label_title, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), ram_label_value, 1, 2, 1, 1);

        GtkWidget *hostname_label_title = gtk_label_new("Hostname:");
        gtk_widget_set_halign(hostname_label_title, GTK_ALIGN_START);
        GtkWidget *hostname_label_value = gtk_label_new("N/A");
        gtk_widget_set_halign(hostname_label_value, GTK_ALIGN_START);
        widgets->hostname_label = GTK_LABEL(hostname_label_value);
        gtk_grid_attach(GTK_GRID(grid), hostname_label_title, 0, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), hostname_label_value, 1, 3, 1, 1);

        GtkWidget *ip_label_title = gtk_label_new("IP Address:");
        gtk_widget_set_halign(ip_label_title, GTK_ALIGN_START);
        GtkWidget *ip_label_value = gtk_label_new("N/A");
        gtk_widget_set_halign(ip_label_value, GTK_ALIGN_START);
        widgets->ip_label = GTK_LABEL(ip_label_value);
        gtk_grid_attach(GTK_GRID(grid), ip_label_title, 0, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), ip_label_value, 1, 4, 1, 1);

        GtkWidget *uptime_label_title = gtk_label_new("Uptime:");
        gtk_widget_set_halign(uptime_label_title, GTK_ALIGN_START);
        GtkWidget *uptime_label_value = gtk_label_new("N/A");
        gtk_widget_set_halign(uptime_label_value, GTK_ALIGN_START);
        widgets->uptime_label = GTK_LABEL(uptime_label_value);
        gtk_grid_attach(GTK_GRID(grid), uptime_label_title, 0, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), uptime_label_value, 1, 5, 1, 1);

        update_info(widgets);

        g_timeout_add(5000, refresh_callback, widgets);

        g_signal_connect(
            main_window, "destroy", G_CALLBACK(on_window_destroy), widgets
        );

        gtk_widget_set_visible(main_window, TRUE);
}

int main(int argc, char *argv[])
{
        GtkApplication *app;
        int status;

        app = gtk_application_new(
            "org.kpi.distributed.fetchgui", G_APPLICATION_DEFAULT_FLAGS
        );
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);

        return status;
}
