--- make-freebsd.mk.orig	2016-09-03 14:18:11.671935000 +0000
+++ make-freebsd.mk	2016-09-03 14:18:27.316065000 +0000
@@ -18,6 +18,14 @@
 	DEFS+=-DZT_ENABLE_CLUSTER
 endif
+# Build with ZT_ENABLE_NETWORK_CONTROLLER=1 to build with the Sqlite network controller
+ifeq ($(ZT_ENABLE_NETWORK_CONTROLLER),1)
+        DEFS+=-DZT_ENABLE_NETWORK_CONTROLLER
+        INCLUDES+=-I/usr/local/include
+        LDFLAGS+=-L/usr/local/lib -lsqlite3
+        OBJS+=controller/SqliteNetworkController.o
+endif
+ 
 # "make debug" is a shortcut for this
 ifeq ($(ZT_DEBUG),1)
 	DEFS+=-DZT_TRACE
