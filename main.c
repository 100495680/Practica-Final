#include <stdio.h>
#include <rpc/rpc.h>
#include "log_rpc.h"  // Include your header

int main(int argc, char *argv[])
{
    CLIENT *clnt;  // Pointer to the RPC client
    arg request_data;  // The argument structure you are sending
    void *result;

    // Initialize your argument structure (populate with relevant data)
    request_data.operation = "OP";
    request_data.user ="IGnacio";
    request_data.datetime ="10:30";

    // Create the RPC client connection to the server
    clnt = clnt_create("localhost", LOGRPC, LOGRPC_V1, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror("Error creating client handle");
        return 1;
    }

    // Call the RPC function
    result = log_operation_1(&request_data, clnt);
    if (result == NULL) {
        printf("RPC call failed\n");
    } else {
        printf("RPC call succeeded\n");
    }

    // Clean up the client connection
    clnt_destroy(clnt);
    return 0;
}
