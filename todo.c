#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define MARGIN_MAINBOX 10

typedef struct {
    GtkWidget* window;
    GtkWidget* del_button;
    GtkWidget* add_button;
    GtkWidget* todos;
    GtkWidget* main_box;
    GtkWidget* entry;
    GtkEntryBuffer* entry_buffer;
    GtkWidget* tool_box;
} TodoState;

void add_todo(TodoState* todo_state) {
    const char* entry_text = gtk_entry_buffer_get_text(todo_state->entry_buffer);

    if (g_strcmp0(entry_text, "") == 0)
        return;

    GtkWidget* label = gtk_label_new(entry_text);

    // -1 means gtk will select every character until the end of the text
    gtk_entry_buffer_delete_text(todo_state->entry_buffer, 0, -1);
    gtk_list_box_append(GTK_LIST_BOX(todo_state->todos), label);
}

static void delete_todo(TodoState* todo_state) {
    GtkListBoxRow* selected_todo = gtk_list_box_get_selected_row(GTK_LIST_BOX(todo_state->todos));
    if (selected_todo == NULL) return;
    gtk_list_box_remove(GTK_LIST_BOX(todo_state->todos), GTK_WIDGET(selected_todo));
}

static void on_add_button(GtkButton* button G_GNUC_UNUSED, gpointer todo_state) {
    add_todo(todo_state);
}

static void entry_on_enter(GtkEntry* entry G_GNUC_UNUSED, gpointer todo_state) {
    add_todo(todo_state);
}

static void on_delete_button(GtkButton* button G_GNUC_UNUSED, gpointer todo_state) {
    delete_todo(todo_state);
}

static gboolean todos_on_key(GtkEventControllerKey* controller G_GNUC_UNUSED, guint keyval, guint keycode G_GNUC_UNUSED, GdkModifierType state G_GNUC_UNUSED, gpointer todo_state) {
    // here we're using keyvals from GDK to identify our Delete key and block everything else
    // this callback returns boolean, true means key was handled so the controller intercepts the key event
    // before it reaches the widget
    // every single gdk keyval defined in the gdk/gdkkeysyms.h header
    if (keyval != GDK_KEY_Delete)
        return FALSE;
    delete_todo(todo_state);
    return TRUE;
}

static void activate(GtkApplication* app G_GNUC_UNUSED, gpointer todo_state) {
    TodoState* state = todo_state;

    // window
    state->window = gtk_application_window_new(app);

    // main box
    state->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(state->window), state->main_box);
    gtk_widget_set_margin_top(state->main_box, MARGIN_MAINBOX);
    gtk_widget_set_margin_bottom(state->main_box, MARGIN_MAINBOX);
    gtk_widget_set_margin_end(state->main_box, MARGIN_MAINBOX);
    gtk_widget_set_margin_start(state->main_box, MARGIN_MAINBOX);

    // tool box
    state->tool_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(state->main_box), state->tool_box);

    // entry
    state->entry = gtk_entry_new();
    gtk_box_append(GTK_BOX(state->tool_box), state->entry);
    gtk_widget_set_hexpand(state->entry, TRUE);
    g_signal_connect(state->entry, "activate", G_CALLBACK(entry_on_enter), state);
    state->entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(state->entry));

    // add button
    state->add_button = gtk_button_new();
    gtk_button_set_label(GTK_BUTTON(state->add_button), "+");
    g_signal_connect(state->add_button, "clicked", G_CALLBACK(on_add_button), state);
    gtk_box_append(GTK_BOX(state->tool_box), state->add_button);

    // delete button
    state->del_button = gtk_button_new();
    gtk_button_set_label(GTK_BUTTON(state->del_button), "-");
    g_signal_connect(state->del_button, "clicked", G_CALLBACK(on_delete_button), state);
    gtk_box_append(GTK_BOX(state->tool_box), state->del_button);

    // todo list box
    state->todos = gtk_list_box_new();
    gtk_box_append(GTK_BOX(state->main_box), state->todos);

    // to handle Delete, we need to add a key controller on the todos ListBox
    // and connect a callback
    GtkEventController* key_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(state->todos, key_controller);
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(todos_on_key), state);

    gtk_window_present(GTK_WINDOW(state->window));
}

int main(int argc, char **argv) {
    TodoState todo_state;

    GtkApplication* app = gtk_application_new("org.gtk.todo", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &todo_state);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
