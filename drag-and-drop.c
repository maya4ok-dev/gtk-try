// this example shows how to make a drag-and-drop widget

#include <gdk/gdk.h>
#include <gtk/gtk.h>

static gboolean on_drop(GtkDropTarget* target, const GValue* value, double x, double y, gpointer data) {
    if (!G_VALUE_HOLDS(value, G_TYPE_FILE)) return FALSE;

    GFile* file = G_FILE(g_value_get_object(value));
    char* uri = g_file_get_uri(file);
    g_print("got file: %s\n", uri);

    g_free(uri);
    return TRUE;
}

void activate(GtkApplication* app) {
    GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_window_set_title(window, "Hello, World!");
    gtk_window_set_default_size(window, 600, 400);

    // create an drag-and-drop event controller
    GtkDropTarget* drop_target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    g_signal_connect(drop_target, "drop", G_CALLBACK(on_drop), drop_target);
    // attach a controller to the window's widget
    gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(drop_target));

    gtk_window_present(window);
}

int main(int argc, char** argv) {
    GtkApplication* app = gtk_application_new("org.gtk.let-window-be-window", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
