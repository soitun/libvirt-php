/*
* libvirt-php.h: libvirt PHP bindings header file
*
* See COPYING for the license of this software
*
* Written by:
*   Radek Hladik <r.hladik@cybersales.cz>
*   Michal Novotny <minovotn@redhat.com>
*   David King
*   Jan-Paul van Burgsteden
*   Lyre <liyong@skybility.com> (or <4179e1@gmail.com>)
*   Daniel P. Berrange <berrange@redhat.com>
*   Tiziano Mueller <dev-zero@gentoo.org>
*   Yukihiro Kawada <warp.kawada@gmail.com>
*/

#ifndef PHP_LIBVIRT_H
#define PHP_LIBVIRT_H 1

#define DEBUG_SUPPORT

#ifdef DEBUG_SUPPORT
#define DEBUG_CORE
#define DEBUG_VNC
#endif

#define ARRAY_CARDINALITY(array)	(sizeof(array) / sizeof(array[0]))

/* Maximum number of authentication attempts */
#define VNC_MAX_AUTH_ATTEMPTS		10

/* Maximum size (in KiB) of log file when DEBUG_SUPPORT is enabled */
#define	DEFAULT_LOG_MAXSIZE		1024

/* Network constants */
#define	VIR_NETWORKS_ACTIVE		1
#define	VIR_NETWORKS_INACTIVE		2

/* Version constants */
#define	VIR_VERSION_BINDING		1
#define	VIR_VERSION_LIBVIRT		2

#ifdef COMPILE_DL_LIBVIRT
#include "php.h"
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_URL
#undef PACKAGE_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php_ini.h"
#include "standard/info.h"
#endif

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <dirent.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <inttypes.h>
#include <linux/tcp.h>

#ifdef __i386__
typedef uint32_t arch_uint;
#define UINTx PRIx32
#else
typedef uint64_t arch_uint;
#define UINTx PRIx64
#endif

typedef struct _resource_info {
	int type;
	virConnectPtr conn;
	arch_uint mem;
	int overwrite;
} resource_info;

ZEND_BEGIN_MODULE_GLOBALS(libvirt)
	char *last_error;
	zend_bool longlong_to_string_ini;
	char *iso_path_ini;
	char *image_path_ini;
	char *max_connections_ini;
	#ifdef DEBUG_SUPPORT
	int debug;
	#endif
	resource_info *binding_resources;
	int binding_resources_count;
ZEND_END_MODULE_GLOBALS(libvirt)

#ifdef ZTS
#define LIBVIRT_G(v) TSRMG(libvirt_globals_id, zend_libvirt_globals *, v)
#else
#define LIBVIRT_G(v) (libvirt_globals.v)
#endif

#define PHP_LIBVIRT_WORLD_VERSION "0.4.3"
#define PHP_LIBVIRT_WORLD_EXTNAME "libvirt"

/* Internal resource identifier objects */
#define INT_RESOURCE_CONNECTION		0x01
#define INT_RESOURCE_DOMAIN		0x02
#define INT_RESOURCE_NETWORK		0x04
#define INT_RESOURCE_NODEDEV		0x08
#define INT_RESOURCE_STORAGEPOOL	0x10
#define INT_RESOURCE_VOLUME		0x20
#define INT_RESOURCE_SNAPSHOT		0x40

/* Libvirt-php types */
typedef struct _php_libvirt_connection {
	virConnectPtr conn;
	long resource_id;
} php_libvirt_connection;

typedef struct _php_libvirt_domain {
	virDomainPtr domain;
	php_libvirt_connection* conn;
} php_libvirt_domain;

#if LIBVIR_VERSION_NUMBER>=8000
typedef struct _php_libvirt_snapshot {
	virDomainSnapshotPtr snapshot;
	php_libvirt_domain* domain;
} php_libvirt_snapshot;
#endif

typedef struct _php_libvirt_network {
	virNetworkPtr network;
	php_libvirt_connection* conn;
} php_libvirt_network;

typedef struct _php_libvirt_nodedev {
	virNodeDevicePtr device;
	php_libvirt_connection* conn;
} php_libvirt_nodedev;

typedef struct _php_libvirt_storagepool {
	virStoragePoolPtr pool;
	php_libvirt_connection* conn;
} php_libvirt_storagepool;

typedef struct _php_libvirt_volume {
	virStorageVolPtr volume;
	php_libvirt_connection* conn;
} php_libvirt_volume;

typedef struct _php_libvirt_cred_value {
	int count;
	int	type;
	char *result;
	unsigned int	resultlen;
} php_libvirt_cred_value;

/* Private definitions */
int vnc_refresh_screen(char *server, char *port, int scancode);
int vnc_send_keys(char *server, char *port, unsigned char *keys);
int vnc_send_pointer_event(char *server, char *port, int pos_x, int pos_y, int clicked, int release);

int set_logfile(char *filename, long maxsize);
char *get_datetime(void);
#ifdef DEBUG_SUPPORT
int gdebug;
#endif

#define PHP_LIBVIRT_CONNECTION_RES_NAME "Libvirt connection"
#define PHP_LIBVIRT_DOMAIN_RES_NAME "Libvirt domain"
#define PHP_LIBVIRT_STORAGEPOOL_RES_NAME "Libvirt storagepool"
#define PHP_LIBVIRT_VOLUME_RES_NAME "Libvirt volume"
#define PHP_LIBVIRT_NETWORK_RES_NAME "Libvirt virtual network"
#define PHP_LIBVIRT_NODEDEV_RES_NAME "Libvirt node device"
#if LIBVIR_VERSION_NUMBER>=8000
#define PHP_LIBVIRT_SNAPSHOT_RES_NAME "Libvirt domain snapshot"
#endif

PHP_MINIT_FUNCTION(libvirt);
PHP_MSHUTDOWN_FUNCTION(libvirt);
PHP_RINIT_FUNCTION(libvirt);
PHP_RSHUTDOWN_FUNCTION(libvirt);
PHP_MINFO_FUNCTION(libvirt);

/* Common functions */
PHP_FUNCTION(libvirt_get_last_error);
/* Connect functions */
PHP_FUNCTION(libvirt_connect);
PHP_FUNCTION(libvirt_connect_get_uri);
PHP_FUNCTION(libvirt_connect_get_hostname);
PHP_FUNCTION(libvirt_connect_get_hypervisor);
PHP_FUNCTION(libvirt_connect_get_capabilities);
PHP_FUNCTION(libvirt_connect_get_maxvcpus);
PHP_FUNCTION(libvirt_connect_get_sysinfo);
PHP_FUNCTION(libvirt_connect_get_encrypted);
PHP_FUNCTION(libvirt_connect_get_secure);
PHP_FUNCTION(libvirt_connect_get_information);
/* Node functions */
PHP_FUNCTION(libvirt_node_get_info);
/* Domain functions */
PHP_FUNCTION(libvirt_domain_get_counts);
PHP_FUNCTION(libvirt_domain_lookup_by_name);
PHP_FUNCTION(libvirt_domain_get_xml_desc);
PHP_FUNCTION(libvirt_domain_get_screenshot);
PHP_FUNCTION(libvirt_domain_get_screen_dimensions);
PHP_FUNCTION(libvirt_domain_change_vcpus);
PHP_FUNCTION(libvirt_domain_change_memory);
PHP_FUNCTION(libvirt_domain_change_boot_devices);
PHP_FUNCTION(libvirt_domain_disk_add);
PHP_FUNCTION(libvirt_domain_disk_remove);
PHP_FUNCTION(libvirt_domain_nic_add);
PHP_FUNCTION(libvirt_domain_nic_remove);
PHP_FUNCTION(libvirt_domain_get_info);
PHP_FUNCTION(libvirt_domain_get_uuid);
PHP_FUNCTION(libvirt_domain_get_uuid_string);
PHP_FUNCTION(libvirt_domain_get_name);
PHP_FUNCTION(libvirt_domain_get_id);
PHP_FUNCTION(libvirt_domain_lookup_by_id);
PHP_FUNCTION(libvirt_domain_lookup_by_uuid);
PHP_FUNCTION(libvirt_domain_lookup_by_uuid_string);
PHP_FUNCTION(libvirt_domain_destroy);
PHP_FUNCTION(libvirt_domain_create);
PHP_FUNCTION(libvirt_domain_resume);
PHP_FUNCTION(libvirt_domain_core_dump);
PHP_FUNCTION(libvirt_domain_shutdown);
PHP_FUNCTION(libvirt_domain_suspend);
PHP_FUNCTION(libvirt_domain_undefine);
PHP_FUNCTION(libvirt_domain_reboot);
PHP_FUNCTION(libvirt_domain_define_xml);
PHP_FUNCTION(libvirt_domain_create_xml);
PHP_FUNCTION(libvirt_domain_memory_peek);
PHP_FUNCTION(libvirt_domain_memory_stats);
PHP_FUNCTION(libvirt_domain_update_device);
PHP_FUNCTION(libvirt_domain_block_stats);
PHP_FUNCTION(libvirt_domain_interface_stats);
PHP_FUNCTION(libvirt_domain_get_connect);
PHP_FUNCTION(libvirt_domain_migrate);
PHP_FUNCTION(libvirt_domain_get_job_info);
PHP_FUNCTION(libvirt_domain_xml_xpath);
PHP_FUNCTION(libvirt_domain_get_block_info);
PHP_FUNCTION(libvirt_domain_get_network_info);
PHP_FUNCTION(libvirt_domain_migrate_to_uri);
PHP_FUNCTION(libvirt_domain_get_autostart);
PHP_FUNCTION(libvirt_domain_set_autostart);
PHP_FUNCTION(libvirt_domain_is_active);
PHP_FUNCTION(libvirt_domain_get_next_dev_ids);
PHP_FUNCTION(libvirt_domain_send_keys);
PHP_FUNCTION(libvirt_domain_send_pointer_event);
/* Domain snapshot functions */
PHP_FUNCTION(libvirt_domain_has_current_snapshot);
PHP_FUNCTION(libvirt_domain_snapshot_create);
PHP_FUNCTION(libvirt_domain_snapshot_lookup_by_name);
PHP_FUNCTION(libvirt_domain_snapshot_get_xml);
PHP_FUNCTION(libvirt_domain_snapshot_revert);
PHP_FUNCTION(libvirt_domain_snapshot_delete);
/* Storage pool and storage volume functions */
PHP_FUNCTION(libvirt_storagepool_lookup_by_name);
PHP_FUNCTION(libvirt_storagepool_lookup_by_volume);
PHP_FUNCTION(libvirt_storagepool_list_volumes);
PHP_FUNCTION(libvirt_storagepool_get_info);
PHP_FUNCTION(libvirt_storagevolume_lookup_by_name);
PHP_FUNCTION(libvirt_storagevolume_lookup_by_path);
PHP_FUNCTION(libvirt_storagevolume_get_name);
PHP_FUNCTION(libvirt_storagevolume_get_path);
PHP_FUNCTION(libvirt_storagevolume_get_info);
PHP_FUNCTION(libvirt_storagevolume_get_xml_desc);
PHP_FUNCTION(libvirt_storagevolume_create_xml);
PHP_FUNCTION(libvirt_storagevolume_create_xml_from);
PHP_FUNCTION(libvirt_storagevolume_delete);
PHP_FUNCTION(libvirt_storagepool_get_uuid_string);
PHP_FUNCTION(libvirt_storagepool_get_name);
PHP_FUNCTION(libvirt_storagepool_lookup_by_uuid_string);
PHP_FUNCTION(libvirt_storagepool_get_xml_desc);
PHP_FUNCTION(libvirt_storagepool_define_xml);
PHP_FUNCTION(libvirt_storagepool_undefine);
PHP_FUNCTION(libvirt_storagepool_create);
PHP_FUNCTION(libvirt_storagepool_destroy);
PHP_FUNCTION(libvirt_storagepool_is_active);
PHP_FUNCTION(libvirt_storagepool_get_volume_count);
PHP_FUNCTION(libvirt_storagepool_refresh);
PHP_FUNCTION(libvirt_storagepool_set_autostart);
PHP_FUNCTION(libvirt_storagepool_get_autostart);
PHP_FUNCTION(libvirt_storagepool_build);
/* Network functions */
PHP_FUNCTION(libvirt_network_define_xml);
PHP_FUNCTION(libvirt_network_undefine);
PHP_FUNCTION(libvirt_network_get);
PHP_FUNCTION(libvirt_network_get_xml_desc);
PHP_FUNCTION(libvirt_network_get_bridge);
PHP_FUNCTION(libvirt_network_get_information);
PHP_FUNCTION(libvirt_network_get_active);
PHP_FUNCTION(libvirt_network_set_active);
/* Nodedev functions */
PHP_FUNCTION(libvirt_nodedev_get);
PHP_FUNCTION(libvirt_nodedev_capabilities);
PHP_FUNCTION(libvirt_nodedev_get_xml_desc);
PHP_FUNCTION(libvirt_nodedev_get_information);
/* Listing functions */
PHP_FUNCTION(libvirt_list_nodedevs);
PHP_FUNCTION(libvirt_list_networks);
PHP_FUNCTION(libvirt_list_domains);
PHP_FUNCTION(libvirt_list_domain_snapshots);
PHP_FUNCTION(libvirt_list_domain_resources);
PHP_FUNCTION(libvirt_list_active_domains);
PHP_FUNCTION(libvirt_list_active_domain_ids);
PHP_FUNCTION(libvirt_list_inactive_domains);
PHP_FUNCTION(libvirt_list_storagepools);
PHP_FUNCTION(libvirt_list_active_storagepools);
PHP_FUNCTION(libvirt_list_inactive_storagepools);
/* Common functions */
PHP_FUNCTION(libvirt_version);
PHP_FUNCTION(libvirt_check_version);
PHP_FUNCTION(libvirt_has_feature);
PHP_FUNCTION(libvirt_get_iso_images);
PHP_FUNCTION(libvirt_image_create);
PHP_FUNCTION(libvirt_image_remove);
/* Debugging functions */
PHP_FUNCTION(libvirt_logfile_set);
PHP_FUNCTION(libvirt_print_binding_resources);

extern zend_module_entry libvirt_module_entry;
#define phpext_libvirt_ptr &libvirt_module_entry

#endif
