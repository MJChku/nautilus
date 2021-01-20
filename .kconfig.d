deps_config := \
	src/net/Kconfig \
	src/fs/Kconfig \
	src/dev/Kconfig \
	Kconfig

.config include/autoconf.h: $(deps_config)

$(deps_config):
