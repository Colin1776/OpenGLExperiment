u64 file_length(FILE* file)
{
    fseek(file, 0, SEEK_END);
    u64 tell = ftell(file);
    fseek(file, 0, SEEK_SET);
    return tell;
}

const char* load_file(const char* path)
{
    FILE* file = fopen(path, "rb");
    u64 file_size = file_length(file);
    char* data = (char*)malloc(file_size + 1);
    fread(data, 1, file_size, file);
    data[file_size] = 0;
    fclose(file);
    return data;
}