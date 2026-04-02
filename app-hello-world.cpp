// this example creates an app class which uses c++ to make code more modern
// the thing is this is useless because of gtkmm existence

#include <functional>
#include <gtk/gtk.h>

class HelloWorld {
    GtkApplication* app;
    std::function<void()> on_activate;

public:
    int run(int argc, char** argv) {
        return g_application_run(G_APPLICATION(app), argc, argv);
    }
    HelloWorld() {
        app = gtk_application_new("org.gtk.app-hello-world", G_APPLICATION_DEFAULT_FLAGS);

        on_activate = [this]() {
            GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
            gtk_window_set_title(window, "Hello, Application World!");
            gtk_window_set_default_size(window, 600, 400);

            GtkWidget* label = gtk_label_new("Hello, World!");
            gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
            gtk_window_set_child(window, label);

            gtk_window_present(window);
        };


        g_signal_connect(app, "activate", 
            G_CALLBACK(+[](GtkApplication*, void* d) {
                (*static_cast<std::function<void()>*>(d))(); 
            }), 
        &on_activate);
    }
    ~HelloWorld() {
        g_object_unref(app);
    }
};

int main(int argc, char** argv) {
    HelloWorld app;
    int app_status = app.run(argc, argv);
    return app_status;
}
