#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aewb_logger_receiver.h"
#include "aewb_logger_types.h"


aewb_logger_receiver_state_t *aewb_logger_create_receiver(const char *bind_ip, in_port_t port) {
    aewb_logger_receiver_state_t *p_state = malloc(sizeof(aewb_logger_receiver_state_t));
    memset(p_state, 0, sizeof(aewb_logger_receiver_state_t));

    // Creating socket file descriptor
    if ((p_state->sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return NULL;
    }

    p_state->bind_addr.sin_family = AF_INET;
    p_state->bind_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, bind_ip, &p_state->bind_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return NULL;
    }

    if (bind(p_state->sock_fd, (const struct sockaddr *)&p_state->bind_addr, sizeof(p_state->bind_addr)) < 0) {
        perror("bind failed");
        return NULL;
    }

    return p_state;
}

int32_t aewb_logger_recv_bytes(aewb_logger_receiver_state_t *p_state)
{
    memset(&p_state->buffer, 0U, sizeof(p_state->buffer));

    struct sockaddr src_addr;
    socklen_t src_addr_len;
    int32_t num_bytes_written = recvfrom(p_state->sock_fd, (int8_t *)&p_state->buffer, sizeof(p_state->buffer),
                 MSG_WAITALL, (struct sockaddr *)&src_addr,
                 &src_addr_len);

    if (num_bytes_written != sizeof(p_state->buffer)) {
        printf("aewb_logger_recv_bytes: error num_bytes_written!=sizeof(p_state->buffer), %d!=%lu\n",
                num_bytes_written, sizeof(p_state->buffer));
    return num_bytes_written;
}

int32_t aewb_logger_recv_log(aewb_logger_receiver_state_t *p_state)
{
    if (p_state == NULL) {
        return 0U;
    }

    return aewb_logger_recv_bytes(p_state);
}

void aewb_logger_destroy_receiver(aewb_logger_receiver_state_t *p_state) {
    if (p_state == NULL) {
        return;
    }

    close(p_state->sock_fd);
    free(p_state);
}
