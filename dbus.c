#include <stdbool.h>
#include <stdio.h>
#include <gio/gio.h>


static void set_led_colour(GDBusConnection *conn, const char *ledpath, int r, int g, int b)
{
	GDBusProxy *proxy;
	GError *error = NULL;
	GVariant *ret;

	proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
			"org.freedesktop.ratbag1", ledpath, "org.freedesktop.ratbag1.Led",
			NULL, &error);

	if(error != NULL){
		printf("cheerlights: Error getting proxy for LED %s\n", ledpath);
		g_error_free(error);
		return;
	}

	ret = g_dbus_proxy_call_sync(proxy, "org.freedesktop.DBus.Properties.Set",
			g_variant_new("(ssv)", "org.freedesktop.ratbag1.Led", "Color",
				g_variant_new("(uuu)", r, g, b)), G_DBUS_CALL_FLAGS_NONE, -1,
			NULL, &error);

	if(error != NULL){
		printf("cheerlights-hid: Error setting LED colour.\n");
		g_error_free(error);
	}
	if(ret){
		g_variant_unref(ret);
	}
	g_object_unref(proxy);
}


static void iterate_profile_leds(GDBusConnection *conn, const char *profilepath, int r, int g, int b)
{
	GDBusProxy *proxy;
	GError *error = NULL;
	GVariant *variant;
	GVariantIter *leds;
	gchar *ledpath;

	proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
			"org.freedesktop.ratbag1", profilepath, "org.freedesktop.ratbag1.Profile", NULL, &error);

	if(error != NULL){
		printf("cheerlights-hid: Error getting proxy for device profile\n");
		g_error_free(error);
		return;
	}

	variant = g_dbus_proxy_get_cached_property(proxy, "Leds");
	if(variant){
		g_variant_get(variant, "ao", &leds);
		g_variant_unref(variant);

		while(g_variant_iter_loop(leds, "o", &ledpath)){
			set_led_colour(conn, ledpath, r, g, b);
		}
		g_variant_iter_free(leds);
	}else{
		printf("cheerlights-hid: Error enumerating LEDs.\n");
	}
	g_object_unref(proxy);
}


static void iterate_device_profiles(GDBusConnection *conn, const char *devicepath, int r, int g, int b)
{
	GDBusProxy *proxy;
	GError *error = NULL;
	GVariant *variant;
	GVariantIter *profiles;
	gchar *profilepath;

	proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
			"org.freedesktop.ratbag1", devicepath, "org.freedesktop.ratbag1.Device", NULL, &error);
	if(error != NULL){
		printf("cheerlights-hid: Error opening proxy for device profile.\n");
		g_error_free(error);
		return;
	}

	variant = g_dbus_proxy_get_cached_property(proxy, "Profiles");
	if(variant){
		g_variant_get(variant, "ao", &profiles);
		g_variant_unref(variant);

		while(g_variant_iter_loop(profiles, "o", &profilepath)){
			iterate_profile_leds(conn, profilepath, r, g, b);
			error = NULL;
			g_dbus_proxy_call_sync(proxy, "Commit", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
			if(error){
				printf("cheerlights-hid: Error committing changes to device (%s).\n", error->message);
				g_error_free(error);
			}
		}
		g_variant_iter_free(profiles);
	}else{
		printf("cheerlights-hid: Error enumerating profiles.\n");
	}
	g_object_unref(proxy);
}


void set_hid_led_colours(int r, int g, int b)
{
	GDBusConnection *conn;
	GDBusProxy *proxy;
	GError *error = NULL;
	GVariant *variant;
	GVariantIter *devices;
	gchar *devicepath;

	conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if(error != NULL){
		printf("cheerlights-hid: Error getting DBus bus.\n");
		g_error_free(error);
		return;
	}

	proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
			"org.freedesktop.ratbag1", "/org/freedesktop/ratbag1", "org.freedesktop.ratbag1.Manager", NULL, &error);
	if(error != NULL){
		printf("cheerlights-hid: Error opening proxy for device profile.\n");
		g_error_free(error);
		return;
	}

	variant = g_dbus_proxy_get_cached_property(proxy, "Devices");
	if(variant){
		g_variant_get(variant, "ao", &devices);
		g_variant_unref(variant);

		while(g_variant_iter_loop(devices, "o", &devicepath)){
			iterate_device_profiles(conn, devicepath, r, g, b);
		}
		g_variant_iter_free(devices);
	}else{
		printf("cheerlights-hid: Error opening device manager.\n");
	}
	g_object_unref(proxy);
	g_object_unref(conn);
}
