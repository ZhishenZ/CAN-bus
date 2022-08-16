#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>

/*Compile:

gcc -Wno-format -o braking_control braking_control.c -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

*/
// Make the GtkWidgets global

GtkBuilder *builder;

GtkWidget *window, *fixed1;

/*labels*/
GtkWidget *label_force_value, *label_actual_force, *label_empty,
    *label_status, *label_status_value, *label_data_recording;

/*buttons*/
GtkWidget *button_activate, *button_stop, *button_release,
    *button_step_1, *button_step_2;

/*switch*/
GtkWidget *switch_data_recording;

int activated = 0;



static gboolean on_timeout(gpointer user_data)
{

    static unsigned f_times = 0;
    gchar *label_str = g_strdup_printf(" %u N", f_times);
    gtk_label_set_label(GTK_LABEL(label_force_value), label_str);
    f_times++;
    g_free(label_str);
    if (f_times < 100)
        return G_SOURCE_CONTINUE;
    else
        return G_SOURCE_REMOVE;
}


int main(int argc, char *argv[])
{

    gtk_init(&argc, &argv); // init Gtk

    // establish contact with xml code used to adjust widget settings

    builder = gtk_builder_new_from_file("braking_control.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // This will build a signal of all the signals and all the callbacks
    gtk_builder_connect_signals(builder, NULL);

    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

    /*labels*/
    label_force_value = GTK_WIDGET(gtk_builder_get_object(builder, "label_force_value"));
    label_actual_force = GTK_WIDGET(gtk_builder_get_object(builder, "label_actual_force"));
    label_empty = GTK_WIDGET(gtk_builder_get_object(builder, "label_empty"));
    label_status = GTK_WIDGET(gtk_builder_get_object(builder, "label_status"));
    label_status_value = GTK_WIDGET(gtk_builder_get_object(builder, "label_status_value"));
    label_data_recording = GTK_WIDGET(gtk_builder_get_object(builder, "label_data_recording"));

    /*buttons*/
    button_activate = GTK_WIDGET(gtk_builder_get_object(builder, "button_activate"));
    button_stop = GTK_WIDGET(gtk_builder_get_object(builder, "button_stop"));
    button_release = GTK_WIDGET(gtk_builder_get_object(builder, "button_release"));
    button_step_1 = GTK_WIDGET(gtk_builder_get_object(builder, "button_step_1"));
    button_step_2 = GTK_WIDGET(gtk_builder_get_object(builder, "button_step_2"));

    // GdkColor color;
    // gdk_color_parse("#0080FF", &color);
    // gtk_widget_modify_fg(GTK_WIDGET(button_activate), GTK_STATE_SELECTED, &color);

    // gtk_widget_modify_fg(GTK_WIDGET(button_activate), GTK_STATE_NORMAL, &color);

    /* switch */
    switch_data_recording = GTK_WIDGET(gtk_builder_get_object(builder, "switch_data_recording"));

    g_timeout_add(500/*ms*/,on_timeout,label_force_value);

    gtk_widget_show(window);

    gtk_main(); // watch for events

    return EXIT_SUCCESS;
}

void on_button_activate_clicked(GtkButton *b)
{

    activated = 1;
    if (activated)
        printf("button activate clicked.\n");
}

void on_button_stop_clicked(GtkButton *b)
{
    activated = 0;
    if (!activated)
        printf("button stop clicked.\n");
}

void on_button_release_clicked(GtkButton *b)
{
    printf("button release clicked.\n");
}


void on_button_step_1_clicked(GtkButton *b)
{
    printf("BRAKE step 1.\n"); 
}


void on_button_step_2_clicked(GtkButton *b)
{
    printf("BRAKE step 2.\n"); 
}

void on_switch_data_recording_state_set(GtkSwitch *s)
{
    gboolean T = gtk_switch_get_active(s);
    if(T){
        printf("Data recording is on.\n");
    }else{
        printf("Data recording is off.\n");
    }
}

// void on_button1_clicked(GtkButton *b)
// {
//     char tmp[128];
//     gdouble val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin1));
//     sprintf(tmp, "spin=%d", (int)val);
//     gtk_label_set_text(GTK_LABEL(label1), (const char *)tmp);
// }

// void on_radio1_toggled(GtkRadioButton *b)
// {

//     gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
//     if (T)
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Radio 1 Active");
//     }
//     else
//     {
//         gtk_label_set_text(GTK_LABEL(label2), (const gchar *)"Radio 1 Not Active");
//     }
// }

// void on_radio2_toggled(GtkRadioButton *b)
// {

//     gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
//     if (T)
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Radio 2 Active");
//     }
//     else
//     {
//         gtk_label_set_text(GTK_LABEL(label2), (const gchar *)"Radio 2 Not Active");
//     }
// }

// void on_radio3_toggled(GtkRadioButton *b)
// {

//     gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
//     if (T)
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Radio 3 Active");
//     }
//     else
//     {
//         gtk_label_set_text(GTK_LABEL(label2), (const gchar *)"Radio 3 Not Active");
//     }
// }

// void on_check1_toggled(GtkCheckButton *b)
// {
//     gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
//     if (T)
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Check button is \n Active");
//     }
//     else
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Check button is \n Not Active");
//     }
// }

// void on_toggle1_toggled(GtkToggleButton *b)
// {

//     gboolean T = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
//     if (T)
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Toggle 1 is \n Active");
//     }
//     else
//     {
//         gtk_label_set_text(GTK_LABEL(label1), (const gchar *)"Toggle 1 is \n Not Active");
//     }
// }