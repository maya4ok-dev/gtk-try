// this is a test of how GtkListBox behaves when a row contains more than just a label

#include <gtk/gtk.h>

void activate(GtkApplication* app) {
    GtkWidget* window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ListBox test");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget* tasks_box = gtk_list_box_new();

    // this is our box that we'll put in the ListBox
    GtkWidget* task_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* task_check_button = gtk_check_button_new();
    gtk_box_append(GTK_BOX(task_box), task_check_button);
    GtkWidget* task_label = gtk_label_new("Buy some milk");
    gtk_box_append(GTK_BOX(task_box), task_label);

    gtk_list_box_append(GTK_LIST_BOX(tasks_box), task_box);

    gtk_window_set_child(GTK_WINDOW(window), tasks_box);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv) {
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.box-in-list-box", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}

// and it worked! but there's one important thing:
// clicking the check button also activates the row
// this is fine though
