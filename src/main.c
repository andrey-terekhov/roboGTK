#include <gtk/gtk.h>


/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;
static int i = 0;


static void draw_brush(GtkWidget *widget, gdouble x, gdouble y);


static void clear_surface(void)
{
	cairo_t *cr = cairo_create(surface);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	cairo_destroy(cr);
}

/* Create a new surface of the appropriate size to store our scribbles */
static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	if (surface)
	{
		cairo_surface_destroy(surface);
	}

	surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
		CAIRO_CONTENT_COLOR,
		gtk_widget_get_allocated_width(widget),
		gtk_widget_get_allocated_height(widget));

	/* Initialize the surface to white */
	clear_surface();

	/* We've handled the configure event, no need for further processing. */
	return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_paint(cr);

	draw_brush(widget, i % 630, i / 630);
	i += 6;

	return FALSE;
}

/* Draw a rectangle on the surface at the given position */
static void draw_brush(GtkWidget *widget, gdouble x, gdouble y)
{
	/* Paint to the surface, where we store our state */
	cairo_t *cr = cairo_create(surface);

	cairo_rectangle(cr, x - 3, y - 3, 6, 6);
	cairo_fill(cr);

	cairo_destroy(cr);

	/* Now invalidate the affected region of the drawing area. */
	//gtk_widget_queue_draw_area(widget, x - 3, y - 3, 6, 6);
	gtk_widget_queue_draw(widget);
}

static void close_window(void)
{
	if (surface)
	{
		cairo_surface_destroy(surface);
	}
}

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

	g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);


	GtkWidget *drawing_area = gtk_drawing_area_new();
	/* set a minimum size */
	gtk_widget_set_size_request(drawing_area, 630, 420);
	gtk_container_add(GTK_CONTAINER(window), drawing_area);


	/* Signals used to handle the backing surface */
	g_signal_connect(drawing_area, "configure-event", G_CALLBACK(configure_event_cb), NULL);
	g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_cb), NULL);


	gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	const int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}
