// an audio player in gtk and gstreamer
// ts was pretty hard to develop though

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <stdio.h>

typedef struct {
    GstElement* gst_pipeline;
    GstBus* gst_bus;
    GtkDropTarget* drop_target;
    gboolean is_playing;
    gboolean is_seeking;
    GtkWidget* play_button;
    GtkWidget* slider;
} AudioPlayer;

static void on_gst_message(GstBus* bus, GstMessage* msg, gpointer user_data) {
    AudioPlayer* player = user_data;
    if (GST_MESSAGE_TYPE(msg) != GST_MESSAGE_ERROR) return;

    GError* err;
    gchar* dbg;

    gst_message_parse_error(msg, &err, &dbg);
    gst_printerrln("Error: %s", err->message);

    g_error_free(err);
    g_free(dbg);

    player->is_playing = FALSE;
}

static gboolean on_drop(GtkDropTarget* target, const GValue* value, double x, double y, gpointer user_data) {
    if (!G_VALUE_HOLDS(value, G_TYPE_FILE)) return FALSE;

    GFile* file = G_FILE(g_value_get_object(value));
    char* uri = g_file_get_uri(file);
    g_print("[%s] loading file: %s\n", __FUNCTION__, uri);

    AudioPlayer* player = user_data;

    gtk_button_set_label(GTK_BUTTON(player->play_button), "Play");
    player->is_playing = FALSE;

    if (player->gst_bus) {
        gst_bus_remove_signal_watch(player->gst_bus);
        gst_object_unref(player->gst_bus);
        player->gst_bus = NULL;
    }

    if (player->gst_pipeline) {
        gst_element_set_state(player->gst_pipeline, GST_STATE_NULL);
        gst_object_unref(player->gst_pipeline);
    }
    gchar* description = g_strdup_printf("playbin uri=%s", uri);
    player->gst_pipeline = gst_parse_launch(description, NULL);
    player->gst_bus = gst_element_get_bus(player->gst_pipeline);

    gst_bus_add_signal_watch(player->gst_bus);
    g_signal_connect(player->gst_bus, "message", G_CALLBACK(on_gst_message), player);

    g_free(uri);
    g_free(description);
    return TRUE;
}

static void on_play(GtkButton* play_button, gpointer user_data) {
    AudioPlayer* player = user_data;

    if (!player->gst_pipeline) return;

    if (player->is_playing == TRUE) {
        g_print("[%s] pausing...\n", __FUNCTION__);
        gtk_button_set_label(GTK_BUTTON(player->play_button), "Play");
        gst_element_set_state(player->gst_pipeline, GST_STATE_PAUSED);
        player->is_playing = FALSE;
        return;
    }

    g_print("[%s] playing...\n", __FUNCTION__);
    gtk_button_set_label(GTK_BUTTON(player->play_button), "Pause");
    gst_element_set_state(player->gst_pipeline, GST_STATE_PLAYING);
    player->is_playing = TRUE;
}

static void cleanup(GtkWindow* window, gpointer user_data) {
    AudioPlayer* player = user_data;

    if (player->gst_bus) {
        g_print("[%s] freeing bus\n", __FUNCTION__);
        gst_bus_remove_signal_watch(player->gst_bus);
        gst_object_unref(player->gst_bus);
    }
    if (player->gst_pipeline) {
        g_print("[%s] freeing pipeline\n", __FUNCTION__);
        gst_element_set_state(GST_ELEMENT(player->gst_pipeline), GST_STATE_NULL);
        gst_object_unref(player->gst_pipeline);
    }
}

static int update_slider(gpointer user_data) {
    AudioPlayer* player = user_data;

    if (player->is_seeking) return TRUE;
    if (!player->gst_pipeline) return TRUE;

    gint64 duration, position;
    gst_element_query_duration(GST_ELEMENT(player->gst_pipeline), GST_FORMAT_TIME, &duration);
    gst_element_query_position(GST_ELEMENT(player->gst_pipeline), GST_FORMAT_TIME, &position);

    int target_slider_position = (position * 100) / duration;
    if ((int)gtk_range_get_value(GTK_RANGE(player->slider)) == target_slider_position) return TRUE;
    gtk_range_set_value(GTK_RANGE(player->slider), target_slider_position);

    return TRUE;
}

static void slider_pressed(GtkGestureClick* slider_gesture, gint n_press, gdouble x, gdouble y, gpointer user_data) {
    AudioPlayer* player = user_data;
    g_print("[%s] starting seeking...\n", __FUNCTION__);
    player->is_seeking = TRUE;
}

static void slider_released(GtkGestureClick* slider_gesture, gint n_press, gdouble x, gdouble y, gpointer user_data) {
    AudioPlayer* player = user_data;
    g_print("[%s] seeking...\n", __FUNCTION__);

    if (!player->gst_pipeline) return;
    
    gint64 duration;
    gst_element_query_duration(GST_ELEMENT(player->gst_pipeline), GST_FORMAT_TIME, &duration);

    gint64 target_seek_time = gtk_range_get_value(GTK_RANGE(player->slider)) * duration / 100;
    
    if (!gst_element_seek_simple(player->gst_pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT, target_seek_time))
        gst_printerrln("[%s] seek error", __FUNCTION__);

    player->is_seeking = FALSE;
}

static void activate(GtkApplication* app, gpointer user_data) {
    AudioPlayer* player = user_data;

    // window
    GtkWindow* window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(GTK_WINDOW(window), "Audio Player");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(cleanup), player);

    // Drag-and-Drop controller
    GtkDropTarget* drop_target = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    g_signal_connect(drop_target, "drop", G_CALLBACK(on_drop), player);
    gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(drop_target));

    // app's body
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_bottom(GTK_WIDGET(main_box), 10);
    gtk_widget_set_margin_top(GTK_WIDGET(main_box), 10);
    gtk_widget_set_margin_start(GTK_WIDGET(main_box), 10);
    gtk_widget_set_margin_end(GTK_WIDGET(main_box), 10);

    // song's cover box
    GtkWidget* cover_box = gtk_center_box_new();
    gtk_widget_set_vexpand(cover_box, TRUE);
    gtk_box_append(GTK_BOX(main_box), cover_box);

    GtkWidget* initial_cover = gtk_label_new("Drag-and-Drop some audio here!");
    gtk_center_box_set_center_widget(GTK_CENTER_BOX(cover_box), initial_cover);
    gtk_widget_set_halign(initial_cover, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(initial_cover, GTK_ALIGN_CENTER);

    GtkWidget* interface_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(main_box), interface_box);

    player->play_button = gtk_button_new_with_label("Play");
    g_signal_connect(GTK_BUTTON(player->play_button), "clicked", G_CALLBACK(on_play), player);
    gtk_box_append(GTK_BOX(interface_box), player->play_button);

    player->slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 5);
    gtk_widget_set_hexpand(GTK_WIDGET(player->slider), TRUE);

    g_timeout_add(100, update_slider, player);

    // box for a slider
    // gesture doesn't work with a scale
    GtkWidget* slider_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append(GTK_BOX(slider_box), player->slider);
    gtk_box_append(GTK_BOX(interface_box), slider_box);

    GtkGesture* slider_gesture = gtk_gesture_click_new();
    // capture clicks at the lower level than the scale
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(slider_gesture), GTK_PHASE_CAPTURE);
    gtk_widget_add_controller(slider_box, GTK_EVENT_CONTROLLER(slider_gesture));
    g_signal_connect(slider_gesture, "pressed", G_CALLBACK(slider_pressed), player);
    g_signal_connect(slider_gesture, "released", G_CALLBACK(slider_released), player);

    gtk_window_set_child(window, main_box);
    gtk_window_present(window);
}

int main(int argc, char** argv) {
    gst_init(&argc, &argv);

    AudioPlayer player = {0};

    GtkApplication* app = gtk_application_new("org.gtk.audio-player", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &player);
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
