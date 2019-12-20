#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <winbase.h>

void seperateFilesFromFolders(WIN32_FIND_DATA[], WIN32_FIND_DATA[], WIN32_FIND_DATA[], char *);
void listFilesInDirectory(char[], WIN32_FIND_DATA[]);
void searchDirectory(char[], int, WIN32_FIND_DATA*, char[]); // 2nd arg (int) i a flag. Set to 0 to populate the 3rd arg with the immediate subdirs of the 1st arg,
													//  set to 1 to recurse through dir structure, starting at arg1.
int checkFileName(char *); // return 1 if input filename is of form *.ext
int cmpFileExt(char *, char *); // return 1 if the 2 input filenames have the same ext 
int cmp(char *, char *);


char filename[260] = "load.bat"; // default if nothing passed on command-line.
char folder1[260] = { '\0' };
char folder2[260] = { '\0' };
//char outputdir[260] = { '\0' };
char outputdir[] = "output";

int verbose = 0;

WIN32_FIND_DATA NullEntry;


char NullFileName[260] = { '\0' };
char prefix[260] = { '\0' };

char backslash[] = "\\";
char star[] = "*";
char cd[] = ".";
char bd[] = "..";

//typedef int BOOL;

unsigned int NUM_FOLDERS = 500;
unsigned int NUM_FILES   = 650;
BOOL flag = 0;

unsigned int iterations = 0;
int selectFolders()	{
	
	char * localPath = ".\\";
	WIN32_FIND_DATA* dirsi = (WIN32_FIND_DATA*)calloc(500, sizeof(WIN32_FIND_DATA));;
	searchDirectory(localPath, 0, dirsi, outputdir);
	
	for (unsigned int i = 0; i < 500; i++) {

		if (cmp(dirsi[i].cFileName, NullFileName))
			break;

		if (!cmp(dirsi[i].cFileName, cd) && !cmp(dirsi[i].cFileName, bd)) { // REMEMBER: DON'T RECURSIVELY SEARCH . OR .. !!!!!

			printf("%d: %s\n", i-1, dirsi[i].cFileName);
		}
	}
	
	int opt1 = -1;
	printf("Select Directory 1: ");
	scanf("%d", &opt1);
	
	int opt2 = -1;
	printf("Select Directory 2: ");
	scanf("%d", &opt2);
	
	if (opt1 == opt2)
		return 0;
	
	if ((opt1 < 1) || (opt2 < 1))	{
		
		printf("Cannot select an option less than one! Exiting...");
		return 0;
	}
	strcpy(folder1, dirsi[opt1+1].cFileName);
	strcpy(folder2, dirsi[opt2+1].cFileName);
	
	printf("\nFolder 1: %s\nFolder 2: %s\n", folder1, folder2);
	
	return 1;
	
}
int main(int argc, char **argv) {
	
	if(!selectFolders())
			return 0;	
	
	char* _outputdirA = (char *)calloc(260, sizeof(char));
	char* localPath = (char *)calloc(260, sizeof(char));

	strcpy(_outputdirA, cd);
	strcat(_outputdirA, backslash);
	strcat(_outputdirA, outputdir);
	//strcat(_outputdirA, backslash);

	strcpy(prefix, folder1);

	strcat(localPath, cd);
	strcat(localPath, backslash);
	strcat(localPath, prefix);
	strcat(localPath, backslash);

	WIN32_FIND_DATA* dirsi = (WIN32_FIND_DATA*)calloc(NUM_FOLDERS, sizeof(WIN32_FIND_DATA));;
	searchDirectory(localPath, 1, dirsi, _outputdirA);
	free(dirsi);
	
	// 2nd folder
	
	flag = 1;
	strcpy(prefix, folder2);
	strcpy(localPath, NullFileName);
	
	strcat(localPath, cd);
	strcat(localPath, backslash);
	strcat(localPath, prefix);
	strcat(localPath, backslash);

	WIN32_FIND_DATA* dirsi2 = (WIN32_FIND_DATA*)calloc(NUM_FOLDERS, sizeof(WIN32_FIND_DATA));;
	searchDirectory(localPath, 1, dirsi2, _outputdirA);
	free(dirsi2);
	
	free(_outputdirA);
	
	printf("COMPLETED. folder count = %d\n", iterations);

	return 0;
}


void searchDirectory(char path[], int r, WIN32_FIND_DATA * dirs, char _outputdir[]) {

	WIN32_FIND_DATA* files = (WIN32_FIND_DATA*)calloc(NUM_FILES, sizeof(WIN32_FIND_DATA));;
	//WIN32_FIND_DATA* dirsi = (WIN32_FIND_DATA*)calloc(NUM_FOLDERS, sizeof(WIN32_FIND_DATA));;
	
	char* localPath = (char*)calloc(260, sizeof(char));
	char* _outputdir2 = (char*)calloc(260, sizeof(char));
	
	strcpy(_outputdir2, _outputdir);
	
	char* temp = (char*)calloc(260, sizeof(char));;

	strcpy(localPath, path);
	strcpy(temp, localPath);
	strcat(temp, star);

	iterations++;
	
	WIN32_FIND_DATA* entries = (WIN32_FIND_DATA*)calloc(NUM_FOLDERS+NUM_FILES, sizeof(WIN32_FIND_DATA));

	listFilesInDirectory(temp, entries);
	seperateFilesFromFolders(entries, files, dirs, localPath);
	free(entries);
	

			
	if (!r) { // special case. No recurse.
	
		return;
	}
	
	for (unsigned int i = 0; i < NUM_FILES; i++) // Check for *.ext here
		if (cmp(files[i].cFileName, NullFileName))
			break;
		else	{

			//strcat(_outputdir2, backslash);
			//strcat(_outputdir2, files[i].cFileName);
			// if you modify _outputdir2 here, it won't work in dir loop!
			
			if (cmp(filename, files[i].cFileName))	{
				
				printf("File match found in: %s\n", localPath);
				//printf("Output path: %s\n", _outputdir2);
			}
			// output-localPath + filename
			
			char *out = (char *)calloc(260, sizeof(char));
			strcpy(out, _outputdir2);
			strcat(out, backslash);
			strcat(out, files[i].cFileName);
			
			char *in = (char *)calloc(260, sizeof(char));
			strcpy(in, localPath);
			strcat(in, files[i].cFileName);
			if (!flag)	{
				// copy to ouput\\..., no need to check for duplicates
				if (!CopyFile((LPCTSTR)in, (LPCTSTR)out, 1))
					printf("ERROR: bFailIfExists... %s\n", out);
				
			}
			else	{
				// copy to output\\..., deal with duplicates per policy
				if (!CopyFile((LPCTSTR)in, (LPCTSTR)out, 1))
					printf("ERROR: bFailIfExists (flag = 1)... %s\n", out);
			}
			
			free(out);
			free(in);
		}
		
	for (unsigned int i = 0; i < NUM_FOLDERS; i++) {
	
		if (cmp(dirs[i].cFileName, NullFileName))
			break;

		if (!cmp(dirs[i].cFileName, cd) && !cmp(dirs[i].cFileName, bd)) { // REMEMBER: DON'T RECURSIVELY SEARCH . OR .. !!!!!
		
			char* subdirectory = (char*)calloc(260, sizeof(char));
			char* localPath2 = (char*)calloc(260, sizeof(char));;

			strcpy(subdirectory, dirs[i].cFileName);
			strcpy(localPath2, localPath);
			strcat(localPath2, subdirectory);
			strcat(localPath2, backslash);

			
			char *out = (char *)calloc(260, sizeof(char));
			strcpy(out, _outputdir2);
			strcat(out, backslash);
			strcat(out, dirs[i].cFileName);
			
			SECURITY_ATTRIBUTES lpSecAtt;
			lpSecAtt.lpSecurityDescriptor = (LPVOID) NULL;
			lpSecAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
			lpSecAtt.bInheritHandle = FALSE;
			
			// create dir (dirs[i].cFileName), output-localPath + foldername
			if (!flag)	{
				
				if (!CreateDirectory((LPCTSTR)out, &lpSecAtt))
					printf("ERROR: CreateDirectory() failed... %s\n", out);
			}
			else	{
				
				// copy to output\\..., deal with duplicates per policy
				if (!CreateDirectory((LPCTSTR)out, &lpSecAtt))
					printf("ERROR: CreateDirectory() failed... %s\n", out);
			}

			free(out);

			WIN32_FIND_DATA* dirsi = (WIN32_FIND_DATA*)calloc(NUM_FOLDERS, sizeof(WIN32_FIND_DATA));;
			
			char* bu_outputdir2 = (char *)calloc(260, sizeof(char));
			strcpy(bu_outputdir2, _outputdir2);

			strcat(_outputdir2, backslash);
			strcat(_outputdir2, dirs[i].cFileName);

			//printf("\tlocalPath2 BEFORE = %s\n", localPath2);
			//printf("\tsubdirectory BEFORE = %s\n", subdirectory);
			//printf("_outputdir2 BEFORE = %s\n", _outputdir2);

			searchDirectory(localPath2, 1, dirsi, _outputdir2);

			strcpy(_outputdir2, bu_outputdir2);
			free(bu_outputdir2);


			//printf("_outputdir2 AFTER = %s\n", _outputdir2);

			free(dirsi);
			
			
			
			free(localPath2);
			free(subdirectory);
		}
	}
	
	
	free(localPath);
	free(files);

	//printf("temp = %s\n", temp);
	free(temp);
	free(_outputdir2);
}

void listFilesInDirectory(char path[], WIN32_FIND_DATA entries[]) {

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(path, &data);      // DIRECTORY
	int i = 0;

	if (hFind != INVALID_HANDLE_VALUE) {

		do {
			entries[i++] = data;
		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
	
	/*
	for (int j = 0; j < i; j++)
		if (entries[j].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			//std::cout << "DIRECTORY";
		}
	*/
	
	entries[i] = NullEntry;
}

void seperateFilesFromFolders(WIN32_FIND_DATA entries[], WIN32_FIND_DATA  files[], WIN32_FIND_DATA  dirs[], char * pathname) {

	int i = 0;
	int di2 = 0;
	int fi2 = 0;
	while (!cmp(entries[i].cFileName, NullFileName)) {

		if (entries[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			dirs[di2++] = entries[i];

		else
			files[fi2++] = entries[i];

		i++;
	}

	if (fi2 > 649)
		printf("File count for %s = %d.\n", pathname, fi2);

	if (di2 > 499)
		printf("Directory count for %s = %d.\n", pathname, di2);
}

int checkFileName(char * fn)	{
	
	if ((fn[0] == '*') && (fn[1] == '.'))
		return 1;
	
	return 0;
}

int cmpFileExt(char *file1, char *file2)	{ 	// Not perfect. If file1's ext is longer than the full length of file2's filename, OOB access.
												// In that case there will be a 1/256 chance of a false negative.
	
	unsigned int len1 = strlen(file1) - 1;
	unsigned int len2 = strlen(file2) - 1;
	
	while (file1[len1] != '.')	{
	
		if (file1[len1] != file2[len2])
			return 0;
		
		if (file2[len2] == '.')
			return 0;
		
		if (len1 <= 0)
			return 0;
		
		len1--;
		len2--;
	}
	
	if (file2[len2] != '.')
		return 0;
	
	return 1;
}

inline int cmp(char *a, char *b) { // returns true (1) if the 2 c-strings match, as it should...

	if (strcmp(a, b) == 0)
		return 1;

	return 0;
}