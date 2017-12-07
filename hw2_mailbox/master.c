#include "master.h"
void get_request(char **argv,char **a,char **b,char **c,int i);
int get_filenum(struct mail_t *mail);
int send_signal = 0;
int receive_signal = 0;
int main(int argc, char **argv)
{
	char *word = NULL;
	char *path = NULL;
	char *slavenum = NULL;
	slavenum = "1";         //default = 1
	get_request(argv,&word,&path,&slavenum,1);
	get_request(argv,&word,&path,&slavenum,3);
	if(argc > 5)            //if set the # of slave
		get_request(argv,&word,&path,&slavenum,5);
	struct mail_t newmail;
	struct mail_t gotmail;
	/*write the mail*/
	strcpy(newmail.file_path,path);
	strcpy(newmail.data.query_word,word);

	int sysfd,txt_num;
	char *filearr = NULL;
	txt_num = get_filenum(&newmail);       //file number

	sysfd = open("/sys/kernel/hw2/mailbox",O_RDWR);
	while(1) {
		send_to_fd(sysfd,&newmail);
		if(send_signal)
			break;
	}
	/*after receive ,need to sum the count*/
	while(1) {
		receive_from_fd(sysfd,&gotmail);
		if(receive_signal)
			break;
	}
	printf("The total number of query word \"%s\" is %d",word,
	       gotmail.data.word_count);
}
void get_request(char **argv,char **a,char **b,char **c,int i)
{
	switch(argv[i][1]) {
	case 'q':
		*a = argv[i+1];
		break;
	case 'd':
		*b = argv[i+1];
		break;
	case 's':
		*c = argv[i+1];
		break;
	}
}
int get_filenum(struct mail_t *mail)
{
	int file_num;
	struct dirent *ent = NULL;
	DIR *pdir;
	if((pdir = opendir(mail->file_path)) == NULL) {
		file_num = 0;
		perror("Failed");
	} else {
		file_num = 0;
		while((ent = readdir(pdir)) != NULL)
			if(strlen(ent->d_name) > 2) {
				file_num++;
				//printf("name is %s\n",ent->d_name);
			}
	}
	closedir(pdir);
	return file_num;
}
int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	send_signal = 1;
	int b =  strlen(mail->file_path) + strlen(mail->data.query_word);
	int ret_val = write(sysfs_fd,mail,b);
	if (ret_val == ERR_FULL) {
		printf("Write Failed\n");
	} else {
		printf("Write Success\n");
	}
}


int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{
	receive_signal = 1;
	int ret_val = read(sysfs_fd,mail);
	if (ret_val == ERR_EMPTY) {
		printf("Read Failed\n");
	} else {
		printf("Read Success\n");
	}
}

