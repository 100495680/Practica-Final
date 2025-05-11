struct arg {
    string user<256>;
    string operacion<256>; 
    string datetime<256>;
};


program LOGRPC {
    version LOGRPC_V1 {
        void LOG_OPERATION(arg) = 1;
    } = 1;
} = 0x10495680;
