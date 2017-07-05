#include "header.h"
#include "video.h"

char (*FindVideos())[FILENAME_LEN]
{
	_finddata_t fd;
	int handle;
	int result = 1;
	int count = 1;
	int str_count = 0;
	static char video_name[][FILENAME_LEN] = {0,};

	handle = _findfirst(".\\*.avi",&fd);

	if(handle == 1)
	{
		printf("No files!!\n");
		return 0;
	}

	while(result != -1)
	{
		printf("(%d) ",count);
		str_count = 0;
		while(fd.name[str_count] != '\0')
		{
			printf("%c",fd.name[str_count]);
			//(*video_name+(count-1))[str_count] = fd.name[str_count];
			video_name[count-1][str_count] = fd.name[str_count];
			str_count++;
		}
		printf("\n");
		
		video_name[count-1][str_count] = '\0';

		//printf("(%d) %s\n", count,video_name[count-1][str_count]);
		result = _findnext(handle, &fd);
		count ++ ;


	}
	printf("\n");
	_findclose(handle);

	return video_name;
}