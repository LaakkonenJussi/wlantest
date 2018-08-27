#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/wireless.h>

static bool test_wext(char *ifname)
{
	struct iwreq wrq = {0};
	int fd = -1, err = 0;

	if (!ifname)
		return false;

	fd = socket(PF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return false;

	strncpy(wrq.ifr_name, ifname, sizeof(wrq.ifr_name) - 1);

	err = ioctl(fd, SIOCGIWNAME, &wrq);
	
	close(fd);

	if (err < 0) {
		perror("ioctl() error");
		return false;
	}

	if (!strlen(wrq.u.name)) {
		printf("struct iwreq u.name not set for %s\n", ifname);
		return false;
	}

	printf("struct iwreq u.name = %s\n", wrq.u.name);

	return true;
}

int main(int argc, char** argv)
{
	bool ret = false;
	char *ifname = NULL;
	int len = 0;

	if (argc == 1) {
		len = strlen("wlan0\0")+1;
		ifname = (char*)calloc(1, sizeof(char) * len);
		strncpy(ifname, "wlan0", len);
	} else if (argc == 2) {
		len = strlen(argv[1]) + 1;
		ifname = (char*)calloc(1, sizeof(char) * len);
		strncpy(ifname, argv[1], len);
	}

	if (ifname) {
		printf("Checking ioctl() SIOCGIWNAME on %s\n", ifname);
		ret = test_wext(ifname);

		if (!ret)
			printf("%s is not a wireless interface\n", ifname);

		free(ifname);
	} else {
		printf("invalid arguments. Use %s <interface>\n", argv[0]);
	}

	return ret;
}

