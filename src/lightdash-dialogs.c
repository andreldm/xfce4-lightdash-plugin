/*
 * Copyright (C) 2015 adlo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>
 */
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>
#include "lightdash.h"
#include "lightdash-dialogs.h"
 
static void lightdash_configure_response (GtkWidget *dialog,
											gint response,
											LightdashPlugin *lightdash)
{
	/* remove the dialog data from the plugin */
      g_object_set_data (G_OBJECT (lightdash->plugin), "dialog", NULL);

      /* unlock the panel menu */
      xfce_panel_plugin_unblock_menu (lightdash->plugin);

      /* destroy the properties dialog */
      gtk_widget_destroy (dialog);
}

static void lightdash_opacity_slider_changed (GtkRange *range, LightdashPlugin *lightdash)
{
	lightdash->opacity = gtk_range_get_value (range);
}

static void lightdash_preferences_entry_changed (GtkEditable *editable, LightdashPlugin *lightdash)
{
	const gchar *text = gtk_entry_get_text (GTK_ENTRY (editable));
	gtk_label_set_text (GTK_LABEL (lightdash->button_label), text);
	lightdash->button_title = g_strdup (text);
}

static void lightdash_show_desktop_button_toggled (GtkToggleButton *button, LightdashPlugin *lightdash)
{
	lightdash->show_desktop = gtk_toggle_button_get_active (button);
}
				
void lightdash_configure (XfcePanelPlugin *plugin,
							LightdashPlugin *lightdash)
{
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *opacity_slider;
	GtkWidget *show_desktop_button;
	GtkWidget *toplevel;
	GtkWindow *window;
	
	window = NULL;
	
	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (plugin));
	
	if (gtk_widget_is_toplevel (toplevel))
	{
		window = GTK_WINDOW (toplevel);
	}
	
	xfce_panel_plugin_block_menu (plugin);
	
	dialog = xfce_titled_dialog_new_with_buttons (_("lightdash"),
						window,
						GTK_DIALOG_DESTROY_WITH_PARENT 
							#if GTK_CHECK_VERSION (3, 0, 0)
							#else
							| GTK_DIALOG_NO_SEPARATOR
							#endif
						,
						GTK_STOCK_HELP, GTK_RESPONSE_HELP,
						GTK_STOCK_CLOSE, GTK_RESPONSE_OK,
						NULL);
	
	#if GTK_CHECK_VERSION (3, 0, 0)
	hbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	#else
	hbox = gtk_hbox_new (FALSE, 0);
	#endif
	
	label = gtk_label_new_with_mnemonic (_("_Title:"));
	
	entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (entry), gtk_label_get_text (GTK_LABEL (lightdash->button_label)));
	
	g_signal_connect (G_OBJECT (entry), "changed",
      G_CALLBACK (lightdash_preferences_entry_changed), lightdash);
	
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), 
		hbox, TRUE, TRUE, 6);
	
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 6);
	
	gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 6);
	gtk_widget_show_all (hbox);
	
	
	
	#if GTK_CHECK_VERSION (3, 0, 0)
	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	#else
	hbox = gtk_hbox_new (FALSE, 0);
	#endif
	
	opacity_slider = gtk_hscale_new_with_range (0.0, 100.0, 1.0);
	gtk_scale_set_value_pos (GTK_SCALE (opacity_slider), GTK_POS_RIGHT);
	gtk_widget_set_size_request (GTK_WIDGET (opacity_slider), 200, -1);
	gtk_range_set_value (GTK_RANGE (opacity_slider), lightdash->opacity);
	
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), 
		hbox, TRUE, TRUE, 6);
	
	label = gtk_label_new_with_mnemonic (_("Background opacit_y:"));
	
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 6);
	
	gtk_box_pack_start (GTK_BOX (hbox), opacity_slider, TRUE, TRUE, 6);
	
	g_signal_connect (G_OBJECT (opacity_slider), "value-changed",
			G_CALLBACK (lightdash_opacity_slider_changed), lightdash);
	
	gtk_widget_show_all (hbox);
	
	show_desktop_button = gtk_check_button_new_with_label (_("Show desktop behind dashboard"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (show_desktop_button), lightdash->show_desktop);
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), 
		show_desktop_button, TRUE, TRUE, 6);
	g_signal_connect (G_OBJECT (show_desktop_button), "toggled",
		G_CALLBACK (lightdash_show_desktop_button_toggled), lightdash);
	
	 				
	g_object_set_data (G_OBJECT (plugin), "dialog", dialog);
	
	g_signal_connect (G_OBJECT (dialog), "response",
			G_CALLBACK (lightdash_configure_response), lightdash);
						
	gtk_widget_show_all (dialog);
}
