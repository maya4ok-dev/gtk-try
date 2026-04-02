#include <gtk/gtk.h>

int score = 0;

// passing the label as user_data
// in the real project you can pass app's state
void clicked(GtkButton* button G_GNUC_UNUSED, gpointer label) {
    char char_score[12];
    sprintf(char_score, "%d", ++score);
    // now we're using label that we've passed
    gtk_label_set_text(GTK_LABEL(label), char_score);
}

void activate(GtkApplication* app) {
    GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Clicker");
    gtk_window_set_default_size(window, 400, 400);

    GtkWidget* score_label = gtk_label_new("0");
    gtk_widget_set_halign(score_label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(score_label, GTK_ALIGN_CENTER);

    GtkWidget* click_button = gtk_button_new_with_label("Click!");
    gtk_widget_set_halign(click_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(click_button, GTK_ALIGN_CENTER);
    g_signal_connect(click_button, "clicked", G_CALLBACK(clicked), score_label);

    // create a box that contains widgets
    // necessary cuz you can't append multiple widgets to a window
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(box), score_label);
    gtk_box_append(GTK_BOX(box), click_button);

    gtk_window_set_child(window, box);
    gtk_window_present(window);
}

int main(int argc, char** argv) {
    GtkApplication* app = gtk_application_new("org.gtk.clicker", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(G_APPLICATION(app), "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
