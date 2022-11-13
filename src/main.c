#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>    /* for getopt_long; POSIX standard getopt is in unistd.h */
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define INFO_OPTIONS 3

struct Info {
    char name[64];
    char value[256*PATH_MAX];
};

static struct Info info_table[INFO_OPTIONS] = {
    {"content-id", ""},
    {"files", ""},
    {"gp4-filename", "homebrew.gp4"},
};

static struct option long_options[] = {
/*   NAME                   ARGUMENT           FLAG  SHORTNAME */
    {info_table[0].name,    required_argument, NULL, 0},
    {info_table[1].name,    required_argument, NULL, 0},
    {NULL,                  0,                 NULL, 0}
};

static char *getDate(int longDate) {
    char *output = malloc(128);
    int hours, minutes, seconds, day, month, year;
    time_t now;

    time(&now);
    struct tm *local = localtime(&now);
 
    hours = local->tm_hour;         // get hours since midnight (0-23)
    minutes = local->tm_min;        // get minutes passed after the hour (0-59)
    seconds = local->tm_sec;        // get seconds passed after a minute (0-59)
 
    day = local->tm_mday;            // get day of month (1 to 31)
    month = local->tm_mon + 1;      // get month of year (0 to 11)
    year = local->tm_year + 1900;   // get year since 1900
    
    if (longDate)
        sprintf(output, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hours, minutes, seconds);
    else
        sprintf(output, "%d-%02d-%02d", year, month, day);

    return output;
}

// Expected parameter: long_date, content-id, short_date, files
static char *gp4Template = 
"<?xml version=\"1.0\"?>\n"
"<psproject xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" fmt=\"gp4\" version=\"1000\">\n"
"   <volume>\n"
"       <volume_type>pkg_ps4_app</volume_type>\n"
"       <volume_ts>%s</volume_ts>\n"
"       <package content_id=\"%s\" passcode=\"00000000000000000000000000000000\" storage_type=\"digital50\" app_type=\"full\" c_date=\"%s\" />\n"
"       <chunk_info chunk_count=\"1\" scenario_count=\"1\">\n"
"           <chunks>\n"
"               <chunk id=\"0\" layer_no=\"0\" label=\"Chunk #0\" />\n"
"           </chunks>\n"
"           <scenarios default_id=\"0\">\n"
"               <scenario id=\"0\" type=\"sp\" initial_chunk_count=\"1\" label=\"Scenario #0\">0</scenario>\n"
"           </scenarios>\n"
"       </chunk_info>\n"
"   </volume>\n"
"   <files img_no=\"0\">\n"
"%s"
"   </files>\n"
"   <rootdir>\n"
"       <dir targ_name=\"sce_sys\" />\n"
"       <dir targ_name=\"media\" />\n"
"       <dir targ_name=\"sce_module\" />\n"
"   </rootdir>\n"
"</psproject>";

static void printHelp() {
    printf("Usage: gp4-generator --content-id <content-d> --files <files separated by comma> [--gp4-filename <filename.gp4>]\n");
}

static char *generateStringFiles() {
    char *output = malloc(64*1024);
    char delim[] = ",";

    char *ptr = strtok(info_table[1].value, delim);

	while(ptr != NULL)
	{
		sprintf(output, "%s       <file targ_path=\"%s\" orig_path=\"%s\" />\n", output, ptr, ptr);
		ptr = strtok(NULL, delim);
	}

    return output;
}

static void generateGP4() {
    char output[128*1024] = {0};
    char *longDate = getDate(1);
    char *shortDate = getDate(0);
    char *files = generateStringFiles();

    sprintf(output, gp4Template, longDate, info_table[0].value, shortDate, files);

    FILE *file = fopen(info_table[2].value, "w");
    int results = fputs(output, file);
    if (results == EOF) {
        printf("Error generating GP4 file\n");
        exit(1);
    }
    fclose(file);

    free(longDate);
    free(shortDate);
    free(files);
}

static int checkInfoTable() {
    int res, i;

    res = 0;
    for (int i=0; i< INFO_OPTIONS; i++) {
        if (strlen(info_table[i].value) <= 0) {
            printf("Error: Missing %s value\n", info_table[i].name);
            res = -1;
        }
    }

    return res;
}

static int readParameters(int argc, char **argv) {
    int c;
    int digit_optind = 0;
    int aopt = 0, bopt = 0;
    char *copt = 0, *dopt = 0;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "0:",
                 long_options, &option_index)) != -1) {
        int this_option_optind = optind ? optind : 1;
        switch (c) {
        case 0:
            // printf ("option %s", long_options[option_index].name);
            if (optarg) {
                // printf (" with arg %s", optarg);
                strcpy(info_table[option_index].value, optarg);
            }
            // printf ("\n");
            break;
        default:
            break;
        }
    }

    return checkInfoTable();
}

int main(int argc, char **argv) {

    if (readParameters(argc, argv) >= 0)
        generateGP4();
    else
        printHelp();
    
    exit(0);
}
