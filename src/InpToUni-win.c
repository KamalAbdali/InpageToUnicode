/* inp2unicode.c:   Convert from Inpage format to Unicode format*/
/* Kamal Abdali */

//gcc -o foo foo.c -mwindows

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
//typedef ULONG_PTR size_t; //if size_t and ssize_t produce errors
//typedef LONG_PTR ssize_t;

#define CHARSETSIZE (UCHAR_MAX+1)  //No. of unsigned charset (256)
#define INPFILESIZE 8388608  // 2**23 max inpage file size
#define OUTFILESIZE 8388608  // 2**23 max inpage file size
#define PATSIZE 4096         // max pattern size
#define SEP_LEN 2048         // max pattern size

#define INPAGEEOF 1
#define INPAGEDOCEND 2
#define INPAGESKIP 3
#define INPAGEREC 4

int findpat(unsigned char pat[], int len);
int nextrecordtype(void);
int readrecord(void);
void writeUnicode(void);
int writeall(void);
int tack(unsigned char ch);

long reclen;
HANDLE hIFile, hOFile;
unsigned char *inp_buf;
unsigned char *out_buf;
unsigned long inp_len, inp_pos, out_len, out_pos, dwWritten;

unsigned char zffff[5] = {0x00,0xff,0xff,0xff,0xff};
unsigned char dzzzzz[6] = {0x0d,0x00,0x00,0x00,0x00,0x00};
unsigned char separator[SEP_LEN]; 
unsigned char buf[65536];

unsigned char unicodebyte[256] = {

	/*          0    1    2    3    4    5    6    7  */
	/*          8    9    a    b    c    d    e    f  */
	/* 00 */ 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, // 07 
	/* 08 */ 0x55,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f, // 0f 
	/* 10 */ 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17, // 17 
	/* 18 */ 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f, // 1f 
	/* 20 */ 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, // 27 
	/* 28 */ 0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f, // 2f 
	/* 30 */ 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, // 37 
	/* 38 */ 0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f, // 3f 
	/* 40 */ 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, // 47 
	/* 48 */ 0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f, // 4f 
	/* 50 */ 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57, // 57 
	/* 58 */ 0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f, // 5f 
	/* 60 */ 0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67, // 67 
	/* 68 */ 0x68,0x69,0x6a,0x41,0x6c,0x6d,0x6e,0x6f, // 6f 
	/* 70 */ 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77, // 77 
	/* 78 */ 0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f, // 7f 
	/* 80 */ 0x80,0x27,0x28,0x7e,0x2a,0x79,0x2b,0x2c, // 87 
	/* 88 */ 0x86,0x2d,0x2e,0x2f,0x88,0x30,0x31,0x91, // 8f 
	/* 90 */ 0x32,0x98,0x33,0x34,0x35,0x36,0x37,0x38, // 97 
	/* 98 */ 0x39,0x3a,0x41,0x42,0xa9,0xaf,0x44,0x45, // 9f 
	/* a0 */ 0x46,0xba,0x48,0x26,0xcc,0xd2,0xc1,0xbe, // a7 
	/* a8 */ 0x4d,0x40,0x50,0x4e,0x4f,0x51,0x11,0xaf, // af 
	/* b0 */ 0x56,0xe1,0xb2,0x53,0x52,0x4c,0x24,0xa3, // b7 
	/* b8 */ 0x4a,0xc3,0xba,0xbb,0xbc,0x70,0x57,0x54, // bf 
	/* c0 */ 0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0x4b, // c7 
	/* c8 */ 0x22,0x23,0x25,0xcb,0x1f,0x1f,0x1f,0x14, // cf 
	/* d0 */ 0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67, // d7 
	/* d8 */ 0x68,0x69,0xda,0xdb,0xdc,0xdd,0x6a,0xdf, // df 
	/* e0 */ 0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0x13,0x12, // e7 
	/* e8 */ 0x6d,0xe9,0x1b,0xeb,0xec,0x0c,0x1f,0xef, // ef 
	/* f0 */ 0xc7,0x0d,0x0e,0xd4,0x19,0x40,0xf6,0x01, // f7 
	/* f8 */ 0x10,0x6b,0xfa,0xfb,0xfc,0xfd,0xfe,0xff  // ff 
	/*          0    1    2    3    4    5    6    7  */
	/*          8    9    a    b    c    d    e    f  */
	
};


//------------------------------------------------------------//
//Boyer Moore Algorithm (standard)                            //
//------------------------------------------------------------//
/* This helper function checks, whether the last "portion" bytes
 * of "needle" (which is "nlen" bytes long) exist within the "needle"
 * at offset "offset" (counted from the end of the string),
 * and whether the character preceding "offset" is not a match.
 * Notice that the range being checked may reach beyond the
 * beginning of the string. Such range is ignored.
 * Code adapted from some public source whose link I can't find now.
 */
static int boyermoore_needlematch
    (unsigned char* needle, size_t nlen, size_t portion, size_t offset) {

    ssize_t virtual_begin = nlen-offset-portion;
    ssize_t ignore = 0;
    if (virtual_begin < 0) { 
        ignore = -virtual_begin; virtual_begin = 0; 
    }
    
    if (virtual_begin > 0 && 
           needle[virtual_begin-1] == needle[nlen-portion-1])
        return 0;

    return
        memcmp(needle + nlen - portion + ignore,
               needle + virtual_begin,
               portion - ignore) == 0;
}   

static size_t mymax(ssize_t a, ssize_t b) {return (a>b ? a : b);}

/* Returns a pointer to the first occurance of "needle"
 * within "haystack", or NULL if not found.
 */
unsigned char* BM
    (unsigned char* haystack, size_t hlen,
     unsigned char* needle,   size_t nlen) {

    size_t skip[nlen];
    ssize_t occ[CHARSETSIZE];
    size_t a, hpos;
    
    if (nlen > hlen || nlen <= 0 || !haystack || !needle) return NULL;

    /* Proprocess #1: init occ[]*/
    
    /* Initialize the table to default value */
    for (a=0; a<CHARSETSIZE; ++a) occ[a] = -1;
    
    /* Then populate it with the analysis of the needle */
    /* But ignoring the last letter */
    for (a=0; a<nlen-1; ++a) occ[needle[a]] = a;
    
    /* Preprocess #2: init skip[] */  
    /* Note: This step could be made a lot faster.
     * A simple implementation is shown here. */
    for (a=0; a<nlen; ++a) {
        size_t value = 0;
        while(value < nlen && 
                !boyermoore_needlematch(needle, nlen, a, value))
            ++value;
        skip[nlen-a-1] = value;
    }
    
    /* Search: */
    for (hpos=0; hpos <= hlen-nlen;) {
        size_t npos=nlen-1;
        while(needle[npos] == haystack[npos+hpos]) {
            if (npos == 0) 
                return haystack + hpos;
            --npos;
        }
        hpos += mymax(skip[npos], npos - occ[haystack[npos+hpos]]);
    }
    return NULL;
}
//-----------------------------------------------------------------------//

void initseparator(void) {

	int i;

	for (i=0; i<SEP_LEN; i++)
		separator[i]=0x00;
	for (i=1; i<0x15; i++)
		separator[4*i-4]=i;
	separator[0x50] = 0xFE;
	for (i=0x51; i<0x200; i++)
		separator[i] = 0xff;
}

int nextpat(unsigned char pat[], int len) {	

	int i,j; 

	if (inp_pos+len > inp_len)
		return 0;
	for (i=0, j=inp_pos; i<len; i++,j++) {
	 	if (pat[i] != inp_buf[j])
			return 0;
	}
	inp_pos = inp_pos+len;
	return 1;
}

int findpat(unsigned char pat[], int len) {	

	unsigned char *match_ptr;
	match_ptr = BM(inp_buf+inp_pos,inp_len-inp_pos,pat,len);
	if (match_ptr == NULL)
		return 0;
	else {
	 	inp_pos = match_ptr-inp_buf+len;
		return 1;
	}
}

int nextrecordtype(void) {	

	unsigned char c0,c1,c2,c3;
	
	if (inp_pos+4 > inp_len)
		return INPAGEEOF;  // 1
	else
 	   c0=inp_buf[inp_pos++];
	c1=inp_buf[inp_pos++];
	c2=inp_buf[inp_pos++];
	c3=inp_buf[inp_pos++];
	if (c0==0x00 && c1==0x00)
		return INPAGESKIP;  // 3
	if (c0==0xFF && c1==0xFF && c2==0xFF && c3==0xFF)
		return INPAGEDOCEND;  // 2
	reclen = 256*c1+c0;
	return INPAGEREC;  // 4
}

int readrecord(void) {

	size_t bytecount;

	if (inp_pos+reclen>inp_len || inp_buf[inp_pos+reclen-1]!=0x0d)
		return 1; //file read error
	else {
	 	memcpy(buf,inp_buf+inp_pos,reclen);
		inp_pos = inp_pos+reclen;
		return 0;
	}
}
/*
// Big endian byte sequence (if needed for another CPU architecture)
void writeUnicode(FILE *fout) {
 	unsigned char ch,ch1;
	int i;

	i=0;
   while (i < reclen-1) {
		ch = buf[i];
      if ((8 < ch && ch < 14) || (32 < ch && ch < 127)) {
         tack(0x00); tack(ch);
      }
      else if (ch == 0x04) {
         if (i > reclen-2)
            break;
         else {
            i++; ch1 = buf[i];
            switch (ch1) {
               case 0x09: case 0x0A: case 0x0B: case 0x0C:
                  case 0x0D: case 0x20:
                     tack(0x00); tack(ch1); break;
               case 0xDA: tack(0x00); tack(0x21); break;
               case 0xE1: tack(0x00); tack(0x29); break;
               case 0xE2: tack(0x00); tack(0x28); break;
               case 0xE9: tack(0x00); tack(0x3A); break;
               case 0xF6: tack(0xFD); tack(0xFA); break;
               case 0xFA: tack(0x00); tack(0x5D); break;
               case 0xFB: tack(0x00); tack(0x5B); break;
               case 0xFC: tack(0x00); tack(0x2E); break;
               case 0xFD: tack(0x20); tack(0x18); break;
               case 0xFE: tack(0x20); tack(0x19); break;
               default: tack(0x06); tack(unicodebyte[ch1]);
            }
         }
      }
		i++;
   }
   tack(0x00); tack(0x0D); 
	tack(0x00); tack(0x0A);
}
*/

// Little endian byte sequence
void writeUnicode(void) {	

	unsigned char ch,ch1;
	int i;

	i=0;
   while (i < reclen-1) {
		ch = buf[i];
      if ((0x08 < ch && ch < 0x0E) || (0x1F < ch && ch < 0xFF)) {  
			tack(ch); 
			tack(0x00); 
		}
      else if (ch == 0x04) {  
			if (i > reclen-2)
            break;
         else {  
				i++; ch1 = buf[i];
            switch (ch1) {  
					case 0x09: case 0x0A: case 0x0B: case 0x0C:
                  case 0x0D: case 0x20:
                     tack(ch1); tack(0x00); break;
               case 0x3A: tack(0x5E); tack(0x00); break;
               case 0xCB: tack(0xF2); tack(0xFD); break;
               case 0xDA: tack(0x21); tack(0x00); break;
               case 0xDB: tack(0x7D); tack(0x00); break;
               case 0xDC: tack(0x7B); tack(0x00); break;
               case 0xDD: tack(0x24); tack(0x00); break;
               case 0xDF: tack(0x2F); tack(0x00); break;
               case 0xE0: tack(0x26); tack(0x20); break;
               case 0xE1: tack(0x29); tack(0x00); break;
               case 0xE2: tack(0x28); tack(0x00); break;
               case 0xE3: tack(0x2A); tack(0x00); break;
               case 0xE4: tack(0x2B); tack(0x00); break;
               case 0xE9: tack(0x3A); tack(0x00); break;
               case 0xEB: tack(0xD7); tack(0x00); break;
               case 0xEC: tack(0x3D); tack(0x00); break;
               case 0xEF: tack(0xF7); tack(0x00); break;
               case 0xF5: tack(0x12); tack(0x22); break;
               case 0xF6: tack(0xFA); tack(0xFD); break;
               case 0xFA: tack(0x5D); tack(0x00); break;
               case 0xFB: tack(0x5B); tack(0x00); break;
               case 0xFC: tack(0x2E); tack(0x00); break;
               case 0xFD: tack(0x18); tack(0x20); break;
               case 0xFE: tack(0x19); tack(0x20); break;
               default: tack(unicodebyte[ch1]);tack(0x06); 
            }
         }
      }
		i++;
   }
   tack(0x0D); tack(0x00); 
	tack(0x0A); tack(0x00);
}

// append character to output buffer
int tack(unsigned char ch) {

	out_buf[out_pos] = ch; out_pos++;
}

int writeall() {

	WriteFile(hOFile, out_buf, out_pos, &dwWritten, NULL);
	GlobalFree(out_buf);
	CloseHandle(hOFile);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, 
                   LPSTR szCmdLine, int iCmdShow)
{  
   OPENFILENAME iFile, oFile; 
	unsigned long dwRead; //dwWritten global
	int t;
	unsigned char *match_posn;
   
   char iFileName[1000]="";
   char oFileName[1000]="";
       
   // Get input file name
   memset(&iFile,0,sizeof(iFile));   
   iFile.lStructSize = sizeof(iFile);
   iFile.lpstrFile = iFileName; 
   iFile.lpstrFile[0] = '\0'; // so GetOpenFileName is properly initialized
   iFile.nMaxFile = 1000;   
   iFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   iFile.lpstrDefExt - "inp";
   iFile.lpstrFilter = "InPage file to convert? (*.inp)\0*.inp\0\0";

   if (!GetOpenFileName(&iFile)) {
      MessageBox(NULL,"No Inpage File Selected! Quitting.","In2Uni Converter",MB_OK); 
      return 1; 
   }

	// read input file into a byte string
	hIFile = CreateFile(iFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, 0, NULL);
	if (hIFile != INVALID_HANDLE_VALUE) {
		inp_len = GetFileSize(hIFile, NULL);
      if (inp_len != INVALID_FILE_SIZE) {
			inp_buf = GlobalAlloc(GPTR, inp_len+1);
         if (inp_buf != NULL) {
				if (ReadFile(hIFile, inp_buf, inp_len, &dwRead, NULL)) {
					inp_buf[inp_len] = 0;
				}
			}
		}
		CloseHandle(hIFile);
	}

	// Get oupt file name
   memset(&oFile,0,sizeof(oFile));
   oFile.lStructSize = sizeof(oFile);
   oFile.lpstrFile = oFileName; 
   oFile.lpstrFile[0] = '\0'; // so GetSaveFileName is properly initialized
   oFile.nMaxFile = 1000;   
   oFile.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
   oFile.lpstrDefExt - "txt";
   oFile.lpstrFilter = "New Unicode text file? (*.txt)\0*.txt\0\0";

   //GetSaveFileName(&oFile);
   if (!GetSaveFileName(&oFile)) {
      MessageBox(NULL,"No Unicode File Selected! Quitting.","In2Uni Converter",MB_OK); 
      return 1; 
   }

	// write byte string into output file
	hOFile = CreateFile(oFileName, GENERIC_WRITE, 0, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, "Unicode File Creation Error", "In2Uni Coverter",MB_OK);
		exit(EXIT_FAILURE);
	}

	inp_pos = 0;

	if ((out_buf=
(unsigned char *) malloc(OUTFILESIZE*sizeof(unsigned char)))==NULL) {	

		MessageBox(NULL, "Output Buffer Creation Error", "In2Uni Coverter",MB_OK);
		exit(EXIT_FAILURE);
	}
	out_pos = 0;

	initseparator();
	while ((match_posn = BM(inp_buf,inp_len,separator,SEP_LEN)) != NULL) {	

		memcpy(match_posn,match_posn+SEP_LEN,inp_buf+inp_len-match_posn-SEP_LEN);
		inp_len = inp_len-SEP_LEN;
	}

   /* Byte order marker, little endian */
   tack(0xFF); tack(0xFE);
   
	do {	
		if (findpat(dzzzzz,6) == 0) {	
			MessageBox(NULL, "Corrupted InPage File", "In2Uni Coverter",MB_OK);
			writeall();
			return 1;
		}
		do {	
			t = nextrecordtype();
			if (t==INPAGEDOCEND || t==INPAGEEOF) { // 2 or 1
				writeall();
				return 0;
			}
			if (t==INPAGESKIP) {  // 3
				if (nextpat(zffff,5) == 0)
					break;
				else {	
					writeall();
					return 0;
				}
			}
			if (t==INPAGEREC) {  // 4
				readrecord();
				writeUnicode();
				continue;
			}
		} while(1);
	} while (1);
	writeall();

	if (hOFile != INVALID_HANDLE_VALUE) {
		WriteFile(hOFile, out_buf, out_pos, &dwWritten, NULL);
		GlobalFree(out_buf);
		CloseHandle(hOFile);
	}

   return 0;
}
