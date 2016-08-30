/*
 * replace ()
 *
 * from: bytes.com/topic/c/answers/223500-how-replace-substring-string-using-c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *replace(const char *s, const char *old, const char *new)
{
char *ret;
int i, count = 0;
size_t newlen = strlen(new);
size_t oldlen = strlen(old);

for (i = 0; s[i] != '\0'; i++) {
if (strstr(&s[i], old) == &s[i]) {
count++;
i += oldlen - 1;
}
}

ret = malloc(i + count * (newlen - oldlen));
if (ret == NULL)
exit(EXIT_FAILURE);

i = 0;
while (*s) {
if (strstr(s, old) == s) {
strcpy(&ret[i], new);
i += newlen;
s += oldlen;
} else
ret[i++] = *s++;
}
ret[i] = '\0';

return ret;
}

#ifdef UNIT_TEST
int main(void)
{
char mystr[] = "##this is##a examp#le";
char *newstr = NULL;

puts(mystr);
newstr = replace(mystr, "##", "****");
printf("%s\n", newstr);

free(newstr);
return 0;
}
#endif
