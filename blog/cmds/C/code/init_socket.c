__inline__ int unblock_socket(int sd) {
    int options;
    /*Unblock our socket to prevent recvfrom from blocking forever
      on certain target ports. */
    options = O_NONBLOCK | fcntl(sd, F_GETFL);
    fcntl(sd, F_SETFL, options);
    return 1;
}



__inline__ int block_socket(int sd) {
    int options;
    options = (~O_NONBLOCK) & fcntl(sd, F_GETFL);
    fcntl(sd, F_SETFL, options);
    return 1;
}


__inline__ void init_socket(int sd) {
    struct linger l;

    l.l_onoff = 1;
    l.l_linger = 0;

    if (setsockopt(sd, SOL_SOCKET, SO_LINGER,  &l, sizeof(struct linger)))
    {
        fprintf(stderr, "Problem setting socket SO_LINGER, errno: %d\n", errno);
        perror("setsockopt");
    }
}
