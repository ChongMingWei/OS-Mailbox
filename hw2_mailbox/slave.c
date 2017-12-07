#include "slave.h"

int computation(char *article,char *word);
int send_signal=0;
int receive_signal=0;
int main(int argc, char **argv)
{
	struct mail_t newmail,gotmail;
	int sysfd;
	int wordfreq = 0;
	sysfd = open("/sys/kernel/hw2/mailbox",O_RDWR);

	while(1) {
		receive_from_fd(sysfd,&gotmail);
		if(receive_signal)
			break;
	}

	/*	char *address = "/home/user/os/hw2_mailbox/test.txt";
		strcpy(gotmail.file_path,address);
		char *word = "apple";
		strcpy(gotmail.data.query_word,word);*/
	/*get the text*/
	char buffer[128] = {};
	char result[1024] = {};
	FILE *fp;
	if((fp = fopen(gotmail.file_path,"r")) == NULL) {
		perror("FAILED");
		fclose(fp);
		//exit(1);
	}
	while(fgets(buffer,128,fp)!=NULL) {
		strcat(result,buffer);
	}
	fclose(fp);
	/*get the freq of the query_word*/
	wordfreq = computation(result,gotmail.data.query_word);

	strcpy(newmail.file_path,gotmail.file_path);
	newmail.data.word_count = wordfreq;
	while(1) {
		send_to_fd(sysfd,&newmail);
		if(send_signal)
			break;
	}
}
int computation(char *article,char *word)
{
	int freq = 0;
	char *tmppt = NULL;

	tmppt = strstr(article,word);
	while(tmppt != NULL) {
		article = tmppt + strlen(word);
		tmppt = strstr(article,word);
		freq += 1;
	}
	return freq;
}
int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	send_signal = 1;
	int a = strlen(mail->file_path) + sizeof(mail->data.word_count);
	int ret_val = write(sysfs_fd,mail,a);
	if (ret_val == ERR_FULL) {
		printf("Write Failed\n");
	} else {
		printf("Write Success\n");
	}
}

int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{
	receive_signal = 1;
//	int a = strlen(mail->file_path) + strlen(mail->data.query_word);
	int ret_val = read(sysfs_fd,mail);
	if (ret_val == ERR_EMPTY) {
		printf("Read Failed\n");
	} else {
		printf("Read Success\n");
	}
}
