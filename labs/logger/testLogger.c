void infof(const char *format, ...);
void warnf(const char *format, ...);
void errorf(const char *format, ...);
void panicf(const char *format, ...);

int main() {
    char *infoMsg = "Hello World!";
    infof("INFO: %s\n", infoMsg);
    
    char *warningMsg = "This is a Warning";
    warnf("WARNING: %s\n", warningMsg);
    
    char *errorMsg = "This is an Error";
    errorf("ERROR: %s\n", errorMsg);
    
    char *panicMsg = "This is a panic message";
    panicf("CORE DUMPED: %s\n", panicMsg);
}
