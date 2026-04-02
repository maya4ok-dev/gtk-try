// this example shows how to create a grid, expand it and attach some widgets

#include <gtk/gtk.h>

#define GRID_SIZE 3

void activate(GtkApplication* app) {
    GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Hello, World!");
    gtk_window_set_default_size(window, 200, 200);

    GtkGrid* grid = GTK_GRID(gtk_grid_new());
 
    // homogeneous expands the grid and its widgets
    gtk_grid_set_column_homogeneous(grid, TRUE);
    gtk_grid_set_row_homogeneous(grid, TRUE);
    gtk_grid_set_column_spacing(grid, 5);
    gtk_grid_set_row_spacing(grid, 5);

    // set margin so the grid doesn't stick to the edges of the window
    gtk_widget_set_margin_bottom(GTK_WIDGET(grid), 10);
    gtk_widget_set_margin_end(GTK_WIDGET(grid), 10);
    gtk_widget_set_margin_start(GTK_WIDGET(grid), 10);
    gtk_widget_set_margin_top(GTK_WIDGET(grid), 10);
    
    GtkWidget* label = gtk_label_new("Grid!!");
    // grid will center it so you don't have to
    // attach label at the very top of the grid
    gtk_grid_attach(grid, label, 0, 0, GRID_SIZE, 1);

    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 1; y < GRID_SIZE; y++) {
            GtkWidget* button = gtk_button_new();
            // anything else attach to its position
            gtk_grid_attach(grid, button, x, y, 1, 1);
        }
    }

    gtk_window_set_child(window, GTK_WIDGET(grid));
    gtk_window_present(window);
}

int main(int argc, char** argv) {
    GtkApplication* app = gtk_application_new("org.gtk.grid", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
