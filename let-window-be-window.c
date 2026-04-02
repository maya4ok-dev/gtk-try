// window is a widget, but we're still using GTK_WINDOW
// so what can we do with it instead of writing GTK_WINDOW(window) every time?
// this example is still hello-world though

#include <gtk/gtk.h>

void activate(GtkApplication* app) {
    // create the window widget and cast it to a GtkWindow at once
    GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
    GtkWidget* label;

    gtk_window_set_title(window, "Hello, World!");
    gtk_window_set_default_size(window, 600, 400);

    label = gtk_label_new("Hello, World!");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_window_set_child(window, label);

    gtk_window_present(window);
}

int main(int argc, char** argv) {
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.let-window-be-window", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}

// and the answer is yes! it actually works
