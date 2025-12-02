#include "fetchbase.h"
#include <cstdio>
#include <gtk/gtk.h>
#include <string>

struct AppWidgets {
        GtkLabel *os_label;
        GtkLabel *cpu_label;
        GtkLabel *ram_label;
        GtkLabel *hostname_label;
        GtkLabel *ip_label;
        GtkLabel *uptime_label;
};

static void on_window_destroy(GtkWidget *, gpointer user_data)
{
        delete static_cast<AppWidgets *>(user_data);
}

void update_info(AppWidgets *widgets)
{
        char buffer[256];

        KDC_OSInfo os;
        if (get_os_info(&os) == 0) {
                snprintf(
                    buffer, sizeof(buffer), "%s %s (%s)", os.os_name,
                    os.os_version, os.arch
                );
                gtk_label_set_text(widgets->os_label, buffer);
        }

        KDC_CPUInfo cpu;
        if (get_cpu_info(&cpu) == 0) {
                snprintf(
                    buffer, sizeof(buffer), "%s, cores=%d, freq=%lu Hz",
                    cpu.name, cpu.cores, cpu.frequency_hz
                );
                gtk_label_set_text(widgets->cpu_label, buffer);
        }

        KDC_RAMInfo ram;
        if (get_ram_info(&ram) == 0) {
                snprintf(
                    buffer, sizeof(buffer),
                    "Total=%lu MB, Used=%lu MB, Free=%lu MB",
                    ram.total_bytes / 1024 / 1024, ram.used_bytes / 1024 / 1024,
                    ram.free_bytes / 1024 / 1024
                );
                gtk_label_set_text(widgets->ram_label, buffer);
        }

        KDC_NetworkInfo net;
        if (get_network_info(&net) == 0) {
                gtk_label_set_text(widgets->hostname_label, net.hostname);
                gtk_label_set_text(widgets->ip_label, net.ip_address);
        }

        KDC_UptimeInfo up;
        if (get_uptime_info(&up) == 0) {
                snprintf(
                    buffer, sizeof(buffer), "%lu seconds", up.uptime_seconds
                );
                gtk_label_set_text(widgets->uptime_label, buffer);
        }
}

gboolean refresh_callback(gpointer user_data)
{
        update_info(static_cast<AppWidgets *>(user_data));
        return G_SOURCE_CONTINUE;
}

static void activate(GtkApplication *app, gpointer)
{
        GtkWidget *main_window = gtk_application_window_new(app);
        gtk_window_set_title(
            GTK_WINDOW(main_window), "System Information (C++)"
        );
        gtk_window_set_default_size(GTK_WINDOW(main_window), 400, 300);

        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
        // Margins logic simplified for brevity, assume GTK4
        gtk_widget_set_margin_start(grid, 10);
        gtk_widget_set_margin_end(grid, 10);
        gtk_widget_set_margin_top(grid, 10);
        gtk_widget_set_margin_bottom(grid, 10);
        gtk_window_set_child(GTK_WINDOW(main_window), grid);

        AppWidgets *widgets = new AppWidgets();

        auto create_row = [&](const char *title, int row) -> GtkLabel * {
                GtkWidget *l_title = gtk_label_new(title);
                gtk_widget_set_halign(l_title, GTK_ALIGN_START);
                GtkWidget *l_value = gtk_label_new("Loading...");
                gtk_widget_set_halign(l_value, GTK_ALIGN_START);
                gtk_grid_attach(GTK_GRID(grid), l_title, 0, row, 1, 1);
                gtk_grid_attach(GTK_GRID(grid), l_value, 1, row, 1, 1);
                return GTK_LABEL(l_value);
        };

        widgets->os_label = create_row("OS:", 0);
        widgets->cpu_label = create_row("CPU:", 1);
        widgets->ram_label = create_row("RAM:", 2);
        widgets->hostname_label = create_row("Hostname:", 3);
        widgets->ip_label = create_row("IP Address:", 4);
        widgets->uptime_label = create_row("Uptime:", 5);

        update_info(widgets);
        g_timeout_add(5000, refresh_callback, widgets);
        g_signal_connect(
            main_window, "destroy", G_CALLBACK(on_window_destroy), widgets
        );
        gtk_widget_set_visible(main_window, TRUE);
}

int main(int argc, char *argv[])
{
        GtkApplication *app = gtk_application_new(
            "org.kpi.distributed.fetchgui", G_APPLICATION_DEFAULT_FLAGS
        );
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        int status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);
        return status;
}
