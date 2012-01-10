/*
 * libusb example program demonstrating hotplug events
 * Copyright (C) 2011 Kevin Mehall <km@kevinmehall.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <sys/types.h>

#include <libusb.h>

static void device_added(libusb_device *dev, void *user_data)
{
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		fprintf(stderr, "failed to get device descriptor\n");
		return;
	}
	printf("%04x:%04x (bus %d, device %d) added\n",
		desc.idVendor, desc.idProduct,
		libusb_get_bus_number(dev), libusb_get_device_address(dev));
	
	// Note: this leaks the device, but we only get removal events for referenced devices
	struct libusb_device_handle *handle;
	r = libusb_open(dev, &handle);
	printf("Opened: %i\n", r);
}

static void device_removed(libusb_device *dev, void *user_data)
{
	printf("bus %d, device %d removed\n",
		libusb_get_bus_number(dev), libusb_get_device_address(dev));
}

int main(void)
{
	int r;

	r = libusb_init(NULL);
	if (r < 0)
		return r;
		
	libusb_set_debug(NULL, 3);

	libusb_register_hotplug_listeners(NULL, device_added, device_removed, 0);
	
	while(1) libusb_handle_events(NULL);

	libusb_exit(NULL);
	return 0;
}

