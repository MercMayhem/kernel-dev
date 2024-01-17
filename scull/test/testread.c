#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd;
    char buffer[1024];
    ssize_t bytesRead;

    // Open the character device file
    fd = open("/dev/scull0", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the device file");
        return 1;
    }

    // Read data from the character device
    bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead == -1) {
        perror("Failed to read from the device");
        close(fd);
        return 1;
    }

    // Print the read data
    printf("Read from the device:\n%s\n", buffer);

    // Close the character device file
    close(fd);

    return 0;
}

