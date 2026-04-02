// a basic example of gtk application

#include <gtk/gtk.h>

// create the interface when the app is ready
void activate(GtkApplication* app) {
    // window is a widget too (see let-window-be-window.c)
    GtkWidget* window;
    GtkWidget* label;

    // create a window, set its properties
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Hello, World!");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // create a centered label
    label = gtk_label_new("Hello, World!");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    // attach to the window
    gtk_window_set_child(GTK_WINDOW(window), label);

    // present to the user
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv) {
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.hello-world", G_APPLICATION_DEFAULT_FLAGS);
    // a callback to create the interface when the app is ready
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
