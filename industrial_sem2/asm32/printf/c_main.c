int _printf(const char *fmt, ...);

int main()
{
    const char *fmt = "Example format string\n"
                      "Dec: %d\n"
                      "Bin: %b\n"
                      "Oct: %o\n"
                      "Hex: %x\n"
                      "Single char: %c\n"
                      "String: %s\n";
    int result = _printf(fmt, 300, 300, 300, 300, 'A', "test string");
    _printf("%d\n", result);

    return 0;
}
