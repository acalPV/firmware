//
// Copyright 2014 Per Vices Corporation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "common.h"

static FILE* fout = NULL;
static FILE* dout = NULL;
FILE* configfile;


struct config get_config(char *filename){
        struct config configstruct;
        FILE *file = fopen (filename, "r");
        /*
        if (file == NULL){
        	//create file and write default values
        	printf("DEBUG - no config file. creating config file.\n");
        	fclose(file);
        	file = fopen(filename, "a");
        	fprintf(filename, "log file = /var/crimson/crimson.log\n");
        	fprintf(filename, "dump file = /var/crimson/dump.log\n");
        	fprintf(filename, "log level = INFO\n");
        	fclose(file);
        	fflush(file);
        	printf("DEBUG - cofig file created.\n");
        }*/

        char line[MAXBUF];
        int i = 0;

        while(fgets(line, sizeof(line), file) != NULL){
			char *cfline;
			cfline = strstr((char *)line,DELIM);
			cfline = cfline + strlen(DELIM);

			if (i == 0){
					memcpy(configstruct.LOGFILE,cfline,strlen(cfline));

			} else if (i == 1){
					memcpy(configstruct.DUMPFILE,cfline,strlen(cfline));

			} else if (i == 2){
					memcpy(configstruct.LOGLVL,cfline,strlen(cfline));

			}
			i++;
		} // End while
		fclose(file);
		fflush(file);
		printf("new log lvl: %s\n", configstruct.LOGLVL);
		return configstruct;
}

int PRINT( print_t priority, const char* format, ... ) {
	int ret = 0;
	va_list args;
	va_start(args, format);
	//char lvl;

	//struct config conf;
	//conf = get_config(CONFIG_FILE);

	// open the file
	if (!fout){
		fout = fopen(LOG_FILE, "a");
	}
	if (!dout){
		dout = fopen(DUMP_FILE, "a");
	}

	// get the time
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	// pre-pend the print message with time and info
	char newfmt[BUF_SIZE] = {0};
	switch (priority) {
		case ERROR:
			snprintf(newfmt, BUF_SIZE, "[%6ld.%03ld] ERROR: ", (long)ts.tv_sec, ts.tv_nsec / 1000000UL);
			break;
		case INFO:
			snprintf(newfmt, BUF_SIZE, "[%6ld.%03ld] INFO:  ", (long)ts.tv_sec, ts.tv_nsec / 1000000UL);
			break;
		case DEBUG:
			snprintf(newfmt, BUF_SIZE, "[%6ld.%03ld] DEBUG: ", (long)ts.tv_sec, ts.tv_nsec / 1000000UL);
			break;
		case VERBOSE:
			snprintf(newfmt, BUF_SIZE, "[%6ld.%03ld] VERB:  ", (long)ts.tv_sec, ts.tv_nsec / 1000000UL);
			break;
		case DUMP:
			snprintf(newfmt, BUF_SIZE, "[%6ld.%03ld] DUMP:  ", (long)ts.tv_sec, ts.tv_nsec / 1000000UL);
			break;
		default:
			snprintf(newfmt, BUF_SIZE, "[%6ld.%03ld] DFLT:  ", (long)ts.tv_sec, ts.tv_nsec / 1000000UL);
			break;
	}
	strcpy(newfmt + strlen(newfmt), format);
	/*
	//set different priorities based on the log level
	lvl = conf.LOGLVL;

	//log ERROR
	if (fout && strcmp(lvl, "ERROR") && priority == ERROR){
		ret = vfprintf(fout, newfmt, args ); // write to file
		ret = vprintf(newfmt, args); //write to stdout
	}
	//log INFO
	if (fout && strcmp(lvl, "INFO") && (priority == INFO || priority == ERROR)){
		ret = vfprintf(fout, newfmt, args );
		ret = vprintf(newfmt, args);
	}
	//log DEBUG
	if (fout && strcmp(lvl, "DEBUG") && (priority == INFO || priority == ERROR || priority == DEBUG)){
		ret = vfprintf(fout, newfmt, args );
		ret = vprintf(newfmt, args);
	}
	//log VERBOSE
	if (fout && strcmp(lvl, "VERBOSE") && (priority == INFO || priority == ERROR || priority == DEBUG || priority == VERBOSE)){
		ret = vfprintf(fout, newfmt, args );
		ret = vprintf(newfmt, args);
	}

	// ERROR, stderr
	if (priority == ERROR ) {
		ret = vfprintf(stderr, newfmt, args );
	}

	// DUMP, file
	if (priority == DUMP) {
		ret = vfprintf(dout, newfmt, args);
	}
	*/
	//priority sorting from old code
	//DUMP or DEBUG or INFO or ERROR, file
	if (fout && priority != VERBOSE){
		ret = vfprintf(fout, newfmt, args);
	}

	//INFO, stdout
	if (priority == INFO){
		ret = vprintf(newfmt, args);
	}

	// ERROR, stderr
	if (priority == ERROR ) {
		ret = vfprintf(stderr, newfmt, args );
	}

	//VERBOSE, stdout
	if (priority == VERBOSE){
		//ret = vprintf(newfmt, args);
	}

	// DUMP, file
	if (priority == DUMP) {
		ret = vfprintf(dout, newfmt, args);
	}

	// flush the file
	//fclose(fout);
	fflush(fout);
	//fclose(dout);
	fflush(dout);

	va_end(args);
	return ret;
}
