#include <stdio.h> // to call printf 
#include <termios.h>
#include <unistd.h>

void main(){
	 struct termios raw;
	 tcgetattr(STDOUT_FILENO,&raw);
	 if(raw.c_iflag&IGNBRK) printf("IGNBRK\n");
	 if(raw.c_iflag&BRKINT) printf("BRKINT\n");
	 if(raw.c_iflag&IGNPAR) printf("IGNPAR\n");
	 if(raw.c_iflag&PARMRK) printf("PARMRK\n");
	 if(raw.c_iflag&INPCK) printf("INPCK\n");
	 if(raw.c_iflag&ISTRIP) printf("ISTRIP\n");
	 if(raw.c_iflag&INLCR) printf("INLCR\n");
	 if(raw.c_iflag&IGNCR) printf("IGNCR\n");
	 if(raw.c_iflag&ICRNL) printf("ICRNL\n");
//if(raw.c_iflag&IUCLC) printf("IUCLC\n");
if(raw.c_iflag&IXON ) printf("IXON \n");
if(raw.c_iflag&IXANY ) printf("IXANY \n");
if(raw.c_iflag&IXOFF ) printf("IXOFF \n");
if(raw.c_iflag&IMAXBEL ) printf("IMAXBEL \n");
if(raw.c_iflag&IUTF8 ) printf("IUTF8 \n");
if(raw.c_oflag&OPOST ) printf("OPOST \n");
//if(raw.c_oflag&OLCUC ) printf("OLCUC \n");
if(raw.c_oflag&ONLCR ) printf("ONLCR \n");
if(raw.c_oflag&OCRNL ) printf("OCRNL \n");
if(raw.c_oflag&ONOCR ) printf("ONOCR \n");
if(raw.c_oflag&ONLRET ) printf("ONLRET \n");
if(raw.c_oflag&OFILL ) printf("OFILL \n");
if(raw.c_oflag&OFDEL ) printf("OFDEL \n");
if(raw.c_oflag&NLDLY ) printf("NLDLY \n");
if(raw.c_oflag&CRDLY ) printf("CRDLY \n");
if(raw.c_oflag&TABDLY ) printf("TABDLY \n");
if(raw.c_oflag&BSDLY ) printf("BSDLY \n");
if(raw.c_oflag&VTDLY ) printf("VTDLY \n");
if(raw.c_oflag&FFDLY ) printf("FFDLY \n");
if(raw.c_cflag&CS5 ) printf("CS5 \n");
if(raw.c_cflag&CS6 ) printf("CS6 \n");
if(raw.c_cflag&CS7 ) printf("CS7 \n");
if(raw.c_cflag&CS8 ) printf("CS8 \n");
if(raw.c_cflag&CSTOPB ) printf("CSTOPB \n");
if(raw.c_cflag&CREAD ) printf("CREAD \n");
if(raw.c_cflag&PARENB ) printf("PARENB \n");
if(raw.c_cflag&PARODD ) printf("PARODD \n");
if(raw.c_cflag&HUPCL ) printf("HUPCL \n");
if(raw.c_cflag&CLOCAL ) printf("CLOCAL \n");
if(raw.c_lflag&ISIG ) printf("ISIG \n");
if(raw.c_lflag&ICANON ) printf("ICANON \n");
//if(raw.c_lflag&XCASE ) printf("XCASE \n");
if(raw.c_lflag&ECHO ) printf("ECHO \n");
if(raw.c_lflag&ECHOE ) printf("ECHOE \n");
if(raw.c_lflag&ECHOK ) printf("ECHOK \n");
if(raw.c_lflag&ECHONL ) printf("ECHONL \n");
if(raw.c_lflag&NOFLSH ) printf("NOFLSH \n");
if(raw.c_lflag&TOSTOP ) printf("TOSTOP \n");
if(raw.c_lflag&IEXTEN ) printf("IEXTEN \n");
 printf("VEOF : %d \n",raw.c_cc[VEOF]);
 printf("VEOL : %d \n",raw.c_cc[VEOL]);
 printf("VERASE : %d \n",raw.c_cc[VERASE]);
 printf("VINTR : %d \n",raw.c_cc[VINTR]);
 printf("VKILL : %d \n",raw.c_cc[VKILL]);
 printf("VMIN : %d \n",raw.c_cc[VMIN]);
 printf("VQUIT : %d \n",raw.c_cc[VQUIT]);
 printf("VSTART : %d \n",raw.c_cc[VSTART]);
 printf("VSTOP : %d \n",raw.c_cc[VSTOP]);
 printf("VSUSP : %d \n",raw.c_cc[VSUSP]);
 printf("VTIME : %d \n",raw.c_cc[VTIME]);
	return;	
}
