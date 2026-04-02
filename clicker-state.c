// in clicker.c we were passing label, and I said that in real projects it's better to pass a state
// this example shows how to use states in GTK applications
// more comments about clicker are located in clicker.c

#include <gtk/gtk.h>

typedef struct ClickerState {
    int score;
    // store needed widgets right in the state
    GtkWidget* score_label;
} ClickerState;

void clicked(GtkButton* button G_GNUC_UNUSED, gpointer raw_state) {
    ClickerState* state = (ClickerState*)raw_state;
    char char_score[12];
    sprintf(char_score, "%d", ++state->score);
    gtk_label_set_text(GTK_LABEL(state->score_label), char_score);
}

void activate(GtkApplication* app, gpointer raw_state) {
    // cast gpointer (aka void*) to a ClickerState*
    ClickerState* state = (ClickerState*)raw_state;
    GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(window, "Clicker");
    gtk_window_set_default_size(window, 400, 400);

    state->score_label = gtk_label_new("0");
    gtk_widget_set_halign(state->score_label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(state->score_label, GTK_ALIGN_CENTER);

    GtkWidget* click_button = gtk_button_new_with_label("Click!");
    gtk_widget_set_halign(click_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(click_button, GTK_ALIGN_CENTER);
    g_signal_connect(click_button, "clicked", G_CALLBACK(clicked), state);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(box), state->score_label);
    gtk_box_append(GTK_BOX(box), click_button);

    gtk_window_set_child(window, box);
    gtk_window_present(window);
}

int main(int argc, char** argv) {
    ClickerState state;
    state.score = 0;
    GtkApplication* app = gtk_application_new("org.gtk.clicker-state", G_APPLICATION_DEFAULT_FLAGS);
    // pass a reference to the state as user_data
    g_signal_connect(G_APPLICATION(app), "activate", G_CALLBACK(activate), &state);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
