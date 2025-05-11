# Compiler and flags
CC = gcc
CFLAGS = -I/usr/include/tirpc
LDFLAGS = -ltirpc

# Files
RPC_X = log_rpc.x
RPC_SOURCES = log_rpc_xdr.c log_rpc_clnt.c log_rpc_svc.c
RPC_HEADERS = log_rpc.h
RPC_SERVER = log_rpc_server
RPC_IMPL = log_rpc_mod.c

SERVER = server
SERVER_SOURCES = servidor.c log_rpc_clnt.c log_rpc_xdr.c

# Default target
all: $(RPC_SERVER) $(SERVER)

# Generate RPC files
$(RPC_HEADERS) $(RPC_SOURCES): $(RPC_X)
	rpcgen $(RPC_X)

# Build RPC server
$(RPC_SERVER): log_rpc_svc.c log_rpc_xdr.c $(RPC_IMPL)
	$(CC) -o $@ log_rpc_svc.c log_rpc_xdr.c $(RPC_IMPL) $(CFLAGS) $(LDFLAGS)

# Build main server
$(SERVER): servidor.c log_rpc_clnt.c log_rpc_xdr.c
	$(CC) -o $@ $(SERVER_SOURCES) $(CFLAGS) $(LDFLAGS)

# Clean generated files
clean:
	rm -f $(RPC_SERVER) $(SERVER) log_rpc.h log_rpc_clnt.c log_rpc_svc.c log_rpc_xdr.c
