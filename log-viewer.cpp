// this example app shows a log viewer using mayak-logger
// it also wraps the GTK C API into C++ API, so it's a bit complicated
// gtkmm probably does this better

#include <functional>
#include <gtk/gtk.h>
#include <mayak/logger.hpp>
#include <vector>

#define GRID_SIZE 3

// add a new label with the message to a listbox
class Sink : public mayak::logger::core::Sink {
    GtkListBox* log_box;
public:
    Sink(GtkListBox* log_box) : log_box(log_box) {}
    void log(const std::string& msg) override {
        GtkWidget* label = gtk_label_new(msg.data());
        gtk_list_box_append(log_box, label);
    }
    void flush() override {}
};

// create a structure to store the logger and the button's index
struct ButtonCallbackData {
    int index;
    mayak::logger::core::Logger& logger;
};

class LogViewer {
    GtkApplication* app;
    GtkWindow* window;
    GtkWindow* log_window;
    GtkListBox* log_box;
    GtkGrid* grid;
    std::function<void()> on_activate;
    mayak::logger::core::Logger logger;
    std::vector<ButtonCallbackData> button_cb_data;

public:
    int run(int argc, char** argv) {
        return g_application_run(G_APPLICATION(app), argc, argv);
    }
    LogViewer() {
        app = gtk_application_new("org.gtk.app-hello-world", G_APPLICATION_DEFAULT_FLAGS);
        // pointer's address changes when vector is resizing,
        // so if you don't know the size, create it dynamically
        button_cb_data.reserve(GRID_SIZE * GRID_SIZE);

        on_activate = [this]() {
            window = GTK_WINDOW(gtk_application_window_new(app));
            gtk_window_set_title(window, "Hello, World!");
            gtk_window_set_default_size(window, 200, 200);
            // we need to force quit the app cuz for some reason it doesn't quit
            // when we're closing all of the windows
            // it's not because we hide log_window instead of destroying, i tried
            // TODO: figure out why application stays opened after closing the window
            g_signal_connect(window, "destroy", G_CALLBACK(+[](GtkWindow*, void* app){
                g_application_quit(G_APPLICATION(app));}), app);

            // we'll put the logs into the listbox
            log_box = GTK_LIST_BOX(gtk_list_box_new());
            logger.addSink<Sink>(log_box);

            grid = GTK_GRID(gtk_grid_new());

            // set some grid properties so everything isn't ugly
            gtk_grid_set_column_homogeneous(grid, true);
            gtk_grid_set_row_homogeneous(grid, true);
            gtk_grid_set_column_spacing(grid, 5);
            gtk_grid_set_row_spacing(grid, 5);
            gtk_widget_set_margin_bottom(GTK_WIDGET(grid), 5);
            gtk_widget_set_margin_top(GTK_WIDGET(grid), 5);
            gtk_widget_set_margin_start(GTK_WIDGET(grid), 5);
            gtk_widget_set_margin_end(GTK_WIDGET(grid), 5);

            // generate the buttons
            int index = 0;
            for (int y = 0; y < GRID_SIZE; ++y) {
                for (int x = 0; x < GRID_SIZE; ++x) {
                    index++;
                    button_cb_data.push_back({index, logger});
                    auto& cb_data = button_cb_data.back();
                    
                    GtkWidget* button = gtk_button_new_with_label(std::to_string(index).c_str());
                    g_signal_connect(button, "clicked", G_CALLBACK(+[](GtkButton*, void* d) {
                        ButtonCallbackData* cb_data = static_cast<ButtonCallbackData*>(d);
                        cb_data->logger << "Button " << cb_data->index << " clicked" << mayak::logger::core::flush;
                    }), &cb_data);
                    gtk_grid_attach(grid, button, x, y, 1, 1);
                }
            }

            log_window = GTK_WINDOW(gtk_application_window_new(app));
            gtk_window_set_title(log_window, "Log viewer");
            gtk_window_set_child(log_window, GTK_WIDGET(log_box));
            // hide the window instead of destroying it
            gtk_window_set_hide_on_close(log_window, true);

            // button to show logs window
            GtkWidget* log_button = gtk_button_new_with_label("Show logs");
            gtk_grid_attach(grid, log_button, 0, GRID_SIZE, GRID_SIZE, 1);
            g_signal_connect(log_button, "clicked", G_CALLBACK(+[](GtkButton*, void* d) {
                gtk_window_present(GTK_WINDOW(d));
            }), log_window);

            gtk_window_set_child(window, GTK_WIDGET(grid));
            gtk_window_present(window);
        };

        g_signal_connect(app, "activate", 
            G_CALLBACK(+[](GtkApplication*, void* d) {
                (*static_cast<std::function<void()>*>(d))(); 
            }), 
        &on_activate);
    }
    ~LogViewer() {
        g_object_unref(app);
    }
};


int main(int argc, char** argv) {
    LogViewer viewer;
    return viewer.run(argc, argv);
}
