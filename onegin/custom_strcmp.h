// свой strcmp для соответствия ТЗ
// Следите за тем, чтобы был windows-1251, на двухбайтной кодировке не работает
// (потратил прилично времени, чтобы это понять)
int custom_strcmp(const char *a, const char *b);

// перевернутый custom_strcmp
int custom_strcmp_rev(const char *a, const char *b);