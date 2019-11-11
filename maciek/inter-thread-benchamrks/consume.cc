static int bytes_consumed = 0;

void consume(int * /*data*/, int size) { bytes_consumed += size; }

int get_bytes_consumed() { return bytes_consumed; }
