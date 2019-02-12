#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h>

#define CPU_FREQ               (3000)
#define MAX_FUNCTION_NUMBER    (128)
#define MAX_FUNCTION_NAME_SIZE (128)

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

typedef struct {
    char func[MAX_FUNCTION_NAME_SIZE];
    int cnt;
    uint64_t cpu_cycle;
} statistic_time_t;

typedef struct {
    int num; 
    statistic_time_t st[MAX_FUNCTION_NUMBER];
} record_time_t;

#ifdef PROFILE

static record_time_t s_stat_time;

static void
add_time_record(const char *func, uint64_t time)
{
    int i;
    for (i = 0; i < s_stat_time.num; i++) {
        if (strcmp(s_stat_time.st[i].func, func) == 0)
            break;
    } 
    assert(i < MAX_FUNCTION_NAME_SIZE);
    /* function not exist now */
    statistic_time_t *st;
    st = &s_stat_time.st[i]; 
    if (i == s_stat_time.num) {
        s_stat_time.num += 1;
        strcpy(st->func, func);
    } 
    st->cnt += 1;
    st->cpu_cycle += time;
}

static int
time_sort_method(const void *t1, const void *t2)
{
    statistic_time_t *ta = (statistic_time_t *)t1;
    statistic_time_t *tb = (statistic_time_t *)t2;
    if (ta->cpu_cycle != tb->cpu_cycle) {
        return ta->cpu_cycle > tb->cpu_cycle;
    }
    return ta->cnt > tb->cnt;
}

static void
stat_time_record(const char *file_name)
{
    int i;
//    char write_file[256];
    
//    struct stat sb;

    //if (stat("logs", &sb) != 0 || !S_ISDIR(sb.st_mode)) {
    //    mkdir("logs", S_IRUSR | S_IWUSR | S_IXUSR);
    //}
    //sprintf(write_file, "logs/%s", file_name);
    //
    // TODO: replace this 

    FILE *f = fopen(file_name, "a+");
    if (!f) {
        fprintf(stderr, "can't open file %s\n", file_name);
        return;
    }
    qsort(s_stat_time.st, s_stat_time.num, sizeof(statistic_time_t), time_sort_method);
    statistic_time_t *data;
    uint64_t total_time = 0;
    fprintf(f, "%-40s%20s%20s\n", "Function", "Count", "TotalTime(us)");
    for (i = 0; i < s_stat_time.num; i++) {
        data = &s_stat_time.st[i];
        fprintf(f, "%-40s%20d%20lu us\n", data->func, data->cnt, data->cpu_cycle / CPU_FREQ);
        total_time += data->cpu_cycle;
    }
    fprintf(f, "Total time consumption is %lu us\n\n", total_time / CPU_FREQ);
    memset(&s_stat_time, 0, sizeof(s_stat_time));
    fclose(f);
}

#endif
