/* Copyright (C) 2017 adlo
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

#ifndef _LIGHTDASH_APP_BUTTON_H_
#define _LIGHTDASH_APP_BUTTON_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LIGHTDASH_TYPE_APP_BUTTON (lightdash_app_button_get_type())
#define LIGHTDASH_APP_BUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIGHTDASH_TYPE_APP_BUTTON, LightdashAppButton))
#define LIGHTDASH_APP_BUTTON_CLASS (klass) (G_TYPE_CHECK_CLASS_CAST (klass, LIGHTDASH_TYPE_APP_BUTTON, LightdashAppButtonClass))
#define IS_LIGHTDASH_APP_BUTTON (obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIGHTDASH_TYPE_APP_BUTTON))
#define IS_LIGHTDASH_APP_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LIGHTDASH_TYPE_APP_BUTTON))

typedef struct _LightdashAppButton LightdashAppButton;
typedef struct _LightdashAppButtonClass LightdashAppButtonClass;

struct _LightdashAppButton
{
	GtkButton parent_instance;
};

struct _LightdashAppButtonClass
{
	GtkButtonClass parent_class;
};

GtkWidget * lightdash_app_button_new ();

G_END_DECLS

#endif
