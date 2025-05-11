#include <stdio.h>
#include "log_rpc.h"

void * log_operation_1_svc(arg *a, struct svc_req *rqstp) {
    static char *result = NULL;

    // Print in required format
    fflush(stdout);  // Important to flush output immediately

    return result;
}
