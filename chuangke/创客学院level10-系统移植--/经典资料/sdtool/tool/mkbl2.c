#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main (int argc, char *argv[])
{
	int		i;
	int		ret;
	FILE		*fp;
	char		*buf;
	int		buflen;
	unsigned int	checksum = 0;

	if (argc != 4) {
		printf("Usage: %s <source file> <destination file> <size>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	buflen = atoi(argv[3]);
	buf = (char *)calloc(1, buflen);
	if (buf == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	fp = fopen(argv[1], "rb");
	if(fp == NULL) {
		perror("open source file");
		free(buf);
		exit(EXIT_FAILURE);
	}

	ret = fread(buf, 1, 14 * 1024, fp);
	printf("read source file %d bytes\n", ret);

	fclose(fp);

	for (i = 0; i < (14 * 1024) - 4; i++)
		checksum += (unsigned char)(buf[i]);
	*(unsigned int *)(buf + i) = checksum;

	fp = fopen(argv[2], "wb");
	if (fp == NULL) {
		perror("open destination file");
		free(buf);
		exit(EXIT_FAILURE);
	}

	ret = fwrite(buf, buflen, 1, fp);
	if (ret != 1) {
		printf("destination file write error\n");
		free(buf);
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	free(buf);
	fclose(fp);

	exit(EXIT_SUCCESS);
}
