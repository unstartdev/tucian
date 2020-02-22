#include <stdio.h>
#include "../include/gtk_gui.h"
#include "../include/fetchrm.h"

/* 读取json member */
static const char *member_reader(JsonReader *reader, const char *member_name)
{
    if (!json_reader_read_member(reader, member_name))
    {
        json_reader_end_member(reader);
        return NULL;
    }
    const char *member_value = json_reader_get_string_value(reader);
    json_reader_end_member(reader);
    return member_value;
}

void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window = gtk_application_window_new(app);

    GtkWidget *label2 = gtk_label_new("Todo:\nIt's coming!");

    GtkWidget *grid_today = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid_today), TRUE);

    curl_global_init(CURL_GLOBAL_ALL);
    JsonNode *td_data = today_img();
    curl_global_cleanup();
    if (td_data)
    {
        JsonReader *reader = json_reader_new(td_data);
        size_t number_of_pics = json_reader_count_elements(reader);
        GtkWidget **imagewg = (GtkWidget **)calloc(number_of_pics, sizeof(GtkWidget *));
        for (size_t sel = 0; sel < number_of_pics; sel++)
        {
            if (json_reader_read_element(reader, sel))
            {
                const char *pic_PID  = member_reader(reader, "PID");
                const char *pic_local_url = member_reader(reader, "local_url");
                const char *pic_nativePath = member_reader(reader, "nativePath");
                if (pic_PID && pic_local_url && pic_nativePath)
                {
                    imagewg[sel] = gtk_image_new_from_file(cache_image(pic_PID, pic_nativePath, pic_local_url));
                }
            }
            json_reader_end_element(reader);
            gtk_grid_attach(GTK_GRID(grid_today), imagewg[sel], sel, 0, 1, 1);
        }
    }

    GtkWidget *stack1 = gtk_stack_new();
    gtk_stack_add_titled(GTK_STACK(stack1), grid_today, "today", "今日");
    gtk_stack_add_titled(GTK_STACK(stack1), label2, "history", "以往");

    GtkWidget *stacksw1 = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stacksw1), GTK_STACK(stack1));

    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box1), stacksw1, TRUE, FALSE, 0);

    GtkWidget *box0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(box0), box1, FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(box0), stack1, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), box0);

    GdkRectangle workarea_ra;
    gdk_monitor_get_workarea(gdk_display_get_primary_monitor(gdk_display_get_default()), &workarea_ra);
    gint window_width = (gint)(workarea_ra.width * 0.75);
    gint window_height = (gint)(workarea_ra.height * 0.75);

    gtk_window_set_title(GTK_WINDOW(window), "Tucian");
    gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
    gtk_widget_show_all(window);
}