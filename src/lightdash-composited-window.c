/* Copyright (C) 2016 adlo
 * 
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>
 */
 
 #include <cairo/cairo.h>
 #include <cairo/cairo-xlib.h>
 #include <cairo/cairo-xlib-xrender.h>
 
 #include "lightdash-composited-window.h"
 
 enum 
 {
	 DAMAGE_SIGNAL,
	 LAST_SIGNAL
 };
 
 static guint lightdash_composited_window_signals [LAST_SIGNAL]={0};
 static int _lightdash_composited_window_damage_event_base = 0;
 
 G_DEFINE_TYPE (LightdashCompositedWindow, lightdash_composited_window, G_TYPE_OBJECT)
 
 static void lightdash_composited_window_finalize (GObject *object);
 static GdkFilterReturn lightdash_composited_window_event (GdkXEvent *xevent, GdkEvent *event, LightdashCompositedWindow *self);
 
 static void lightdash_composited_window_class_init (LightdashCompositedWindowClass *klass)
 {
	 GObjectClass *object_class = G_OBJECT_CLASS (klass);
	 
	 object_class->finalize = lightdash_composited_window_finalize;
	 
	 lightdash_composited_window_signals [DAMAGE_SIGNAL] = g_signal_new ("damage-event",
				G_TYPE_FROM_CLASS (klass),
				G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
				0,
				NULL,
				NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);
 }
 
 static void lightdash_composited_window_init (LightdashCompositedWindow *self)
 {
	 int dr;
	 self->surface = NULL;
	 self->compositor = NULL;
	 self->window = NULL;
	 self->xid = None;
	 self->gdk_window = NULL;
	 self->damage = None;
	 self->compositor = lightdash_compositor_get_default ();
	 
	 XDamageQueryExtension (self->compositor->dpy, &_lightdash_composited_window_damage_event_base, &dr);
	 
 }
 
 static void lightdash_composited_window_finalize (GObject *object)
 {
	 LightdashCompositedWindow *task;
	 gint error;
	 task = LIGHTDASH_COMPOSITED_WINDOW (object);
	 
	 if (task->damage != None)
	{
		gdk_error_trap_push ();
		XDamageDestroy (task->compositor->dpy, task->damage);
		task->damage = None;
		error = gdk_error_trap_pop ();
	}
	
	
	gdk_window_remove_filter (task->gdk_window, (GdkFilterFunc) lightdash_composited_window_event, task);
	
	if (task->surface)
	{	
		cairo_surface_destroy (task->surface);
		task->surface = NULL;
	}
	
	g_object_unref (task->compositor);
	
	G_OBJECT_CLASS (lightdash_composited_window_parent_class)->finalize (object);
}

void lightdash_composited_window_get_size (LightdashCompositedWindow *self, gint *width, gint *height)
{
	if (width)
		*width = self->attr.width;
		
	if (height)
		*height = self->attr.height;
}
 
 static GdkFilterReturn lightdash_composited_window_event (GdkXEvent *xevent, GdkEvent *event, LightdashCompositedWindow *self)
 {
	XEvent *ev;
	XDamageNotifyEvent *e;
	XConfigureEvent *ce;
	
	int width, height;
	gint error;
	
	ev = (XEvent*)xevent;
	e = (XDamageNotifyEvent *)ev;
	
	switch (ev->type)
	{
		case DestroyNotify:
			gdk_error_trap_push ();
			cairo_surface_destroy (self->surface);
			self->surface = NULL;
			XDamageDestroy (self->compositor->dpy, self->damage);
			XSync (self->compositor->dpy, False);
			error = gdk_error_trap_pop ();
		break;
	
		case ConfigureNotify:
			ce = &ev->xconfigure;
		
		//if (ce->height == self->attr.height && ce->width == self->attr.width)
			//return GDK_FILTER_CONTINUE;
			
			self->attr.width = ce->width;
			self->attr.height = ce->height;
			cairo_xlib_surface_set_size (self->surface,
								self->attr.width,
								self->attr.height);
							
			g_signal_emit (self, lightdash_composited_window_signals[DAMAGE_SIGNAL], 0);

		break;
		default:
		break;
	}
	
	if (ev->type == _lightdash_composited_window_damage_event_base + XDamageNotify)
	{
	XDamageSubtract (self->compositor->dpy, e->damage, None, None);
	g_signal_emit (self, lightdash_composited_window_signals[DAMAGE_SIGNAL], 0);
	
	}
	
	
	return GDK_FILTER_CONTINUE;
}
 
 cairo_surface_t *
lightdash_composited_window_get_window_picture (LightdashCompositedWindow *task)
{
	cairo_surface_t *surface;
	XRenderPictFormat *format;
		
	format = None;
			
	format = XRenderFindVisualFormat (task->compositor->dpy, task->attr.visual);	

	surface = cairo_xlib_surface_create_with_xrender_format (task->compositor->dpy,
			task->xid,
			task->attr.screen,
			format,
			task->attr.width,
			task->attr.height);
			
	return surface;
}
 
 LightdashCompositedWindow * lightdash_composited_window_new_from_window (WnckWindow *window)
 {
	LightdashCompositedWindow *composited_window;
	gint error;
	 
	composited_window = LIGHTDASH_COMPOSITED_WINDOW (g_object_new (LIGHTDASH_TYPE_COMPOSITED_WINDOW, NULL));
	 
	composited_window->window = window;
	composited_window->xid = wnck_window_get_xid (composited_window->window);
	XGetWindowAttributes (composited_window->compositor->dpy, composited_window->xid, &composited_window->attr);
	composited_window->gdk_window = gdk_x11_window_foreign_new_for_display 
		(gdk_screen_get_display (composited_window->compositor->gdk_screen), composited_window->xid);
	
	//if (task->tasklist->composited 
		//&& task->attr.height != 0)
		//{
			gdk_error_trap_push ();
			
			composited_window->surface = lightdash_composited_window_get_window_picture (composited_window);
	
			/* Ignore damage events on excluded windows */
			if (composited_window->gdk_window != composited_window->compositor->excluded_gdk_window)
				{
					composited_window->damage = XDamageCreate (composited_window->compositor->dpy, 
									composited_window->xid, 
									XDamageReportBoundingBox);
					XDamageSubtract (composited_window->compositor->dpy, composited_window->damage, None, None);				
					gdk_window_add_filter (composited_window->gdk_window, 
								(GdkFilterFunc) lightdash_composited_window_event, 
								composited_window);
				}
			error = gdk_error_trap_pop ();
		//}
		
	return composited_window;
}
