#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/scull0"

int main() {
    int fd;
    ssize_t ret;

    // Open the character device
    fd = open(DEVICE_FILE, O_WRONLY);

    if (fd == -1) {
        perror("Error opening the device file");
        return EXIT_FAILURE;
    }

    // Data to be written
    const char *data = "Hello, this is a test.";

    // Write data to the device
    ret = write(fd, data, strlen(data));

    if (ret == -1) {
        perror("Error writing to the device");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Data written successfully: %ld bytes\n", ret);

    // Close the device file
    close(fd);

    return EXIT_SUCCESS;
}
