struct arg_log{
  string nombre<256>;
  string operacion<15>;
  string fecha_hora<20>;
};

struct arg_log_filename{
  string nombre<256>;
  string operacion<15>;
  string filename<256>;
  string fecha_hora<20>;
};

program TUPLAS {
  version TUPLASVER {
    int LOG(arg_log a) = 0;
    int LOG_FILENAME(arg_log_filename a) = 1;
  } = 1;
} = 5850;