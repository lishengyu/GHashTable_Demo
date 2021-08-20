/*
 * This is a simple demo of GHashTable
 * See more information at https://docs.gtk.org/glib/struct.HashTable.html
 * compile method:
 * 		gcc main.c `pkg-config --libs --cflags glib-2.0` 
 */

#include <stdio.h>
#include <glib.h>

#define WHITESPACE "\n\r \t"
#define COMMENTS "#;\n"

void print_key_value(gpointer key, gpointer value, gpointer user_data);
void display_hash_table(GHashTable *table);
void free_key(gpointer key);
void free_value(gpointer value);
void notice(char *s);

void print_key_value(gpointer key, gpointer value, gpointer user_data)
{
	printf("key:[%s] value:[%s]\n", key, value);
}

void display_hash_table(GHashTable *table)
{
	printf("===================================\n");
	g_hash_table_foreach(table, print_key_value, NULL);
	printf("===================================\n");
}

void free_key(gpointer key)
{
	printf("[free key:%s]\n", key);
	free(key);
}

void free_value(gpointer value)
{
	printf("[free value:%s]\n", value);
	free(value);
}

void notice(char *s)
{
	printf("[%s][%d]:%s\n", __func__, __LINE__, s);
}

void printKey(gpointer data, gpointer user_data)
{
	printf("\t%s", data);
}

static char
*strip(char *s)
{
    char *e, *l = NULL;

    s += strspn(s, WHITESPACE);

    for (e = s; *e; e++) {
        if (!strchr(WHITESPACE, *e)) {
            l = e;
        }
    }

    if (l)
        *(l+1) = 0;
    else
        *s = 0;

    return s;
}

int parse_line(GHashTable *line_table, char *buf)
{
    char *key, *value;
    char *c;
    int hash_num;
    
    /* strspn:
     * buf中第一个不在WHITESPACE中的字符地址*/
    key = buf + strspn(buf, WHITESPACE);
    
    /* strpbrk:
     * key中第一次出现COMMENTS中任一字符的地址*/
    if ((c = strpbrk(key, COMMENTS)))
        *c = 0;
        
    /* strchr:
     * key中第一次出现‘=’字符的地址*/
    if (!(value = strchr(key, '='))) {
        printf("Missing '='\n");   
        return FALSE;
    }   
    
    *value = 0;
    value++; 
    
    key = strip(key);
    value = strip(value);
    
	char *str1, *str2;
	str1 = strdup(key);
	str2 = strdup(value);

    if (!g_hash_table_insert(line_table, str1, str2)) {
        return FALSE;
    }   
    
    return TRUE;
} 

int main(int argc, char *argv[])
{
	/* 定义GHashTable结构体，用户可以不用关心结构体内容，
	 * 对于结构体的操作都是通过提供的相应API进行操作；*/
	GHashTable *table1 = NULL;
    GHashTable *table2 = NULL;
	GList *lkey = NULL;
	GList *rvalue = NULL;
	gpointer *key = NULL;
	gpointer *value = NULL;
	FILE *fd;
	char buf[4096];

/*
 * 指定插入key-value并且关于key-value对的操作
 */
# if 1
	/* 新建GHashTable链表
	 */
	if ((table1 = g_hash_table_new(g_str_hash,g_str_equal)) == NULL) {
		return -1;
	}

	/* 插入key-value对
	 */
	g_hash_table_insert(table1, "1", "noe");
	g_hash_table_insert(table1, "2", "two");
	g_hash_table_insert(table1, "3", "three");
	g_hash_table_insert(table1, "4", "four");
	g_hash_table_insert(table1, "5", "five");
	g_hash_table_insert(table1, "6", "six");

	/* 调用g_hash_table_foreach打印当前hash表中所有的key-value对
	 */
	display_hash_table(table1);

	// 插入已有key，不同的value；
	g_hash_table_insert(table1, "1", "eleven");
	// 替换已有key的value
	g_hash_table_replace(table1, "2", "twelve");
	// 删除已有key
	g_hash_table_remove(table1, "3");
	
	display_hash_table(table1);

	// 获取keys
	lkey = g_hash_table_get_keys(table1);
	printf("Key List:[");
	g_list_foreach(lkey, printKey, "key");	
	printf("]\n");

	// 获取values
	rvalue = g_hash_table_get_values(table1);
	printf("Val List:[");
	g_list_foreach(rvalue, printKey, "value");	
	printf("]\n");

	// 通过key查找value，无法区分key不存在和key存在但是value为空的情况
	value = g_hash_table_lookup(table1, "4");
	if (value != NULL) {
		notice((char *) value);
	}

#endif


/*
 * 解析配置文件得到key-value对
 */
#if 1
	/* 新建GHashTable链表,并指定函数来释放key和value申请的内存;
	 */
	if ((table2 = g_hash_table_new_full(g_str_hash,g_str_equal,free_key,free_value)) == NULL) {
		return -1;
	}
	
	/* 解析配置文件，从配置文件中解析出key-value对；
	 */
	fd = fopen("/etc/udisks2/udisks2-control.conf", "r");
	while (!feof(fd)) {
		if (!fgets(buf, sizeof(buf), fd)) {
			if (feof(fd))
				break;
		}
		
		/* 逐行解析文件内容；
		 */
		if (parse_line(table2, buf) < 0) {
			return FALSE;
		}
	}

	fclose(fd);

	/* 打印当前hash表中的key-value对
	 */
	display_hash_table(table2);

	/* 调用g_hash_table_size函数获取key-value对的数量； 
	 */
	printf("[Size of hash table:%d]\n", g_hash_table_size(table2));
#endif	

	return 0;
}
