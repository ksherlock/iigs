#pragma lint -1
#pragma optimize -1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gsos.h>
#include <resources.h>
#include <memory.h>


typedef struct IconHeader {
   Word iconType; // bit 15 = color (1) or b/w (0)
   Word iconSize; // number of bytes
   Word iconHeight; // height, in pixels
   Word iconWidth; // width, in pixels.
   // Byte iconImage[iconSize];
   // Byte iconMask[iconSize];
} IconHeader;


GSString255Ptr c2gs(const char *cp) {
	GSString255Ptr gs;
	int l = strlen(cp);
	gs = malloc(l + 3);
	if (gs) {
		gs->length = l;
		strcpy(gs->text, cp);
	}
	return gs;
}


void dump(const unsigned char *ptr, unsigned count) {

	static char hex[] = "0123456789abcdef";

	unsigned i,x;
	fputs("    $\"", stdout);

	for (i = 0; i < count; ++i) {
		x = ptr[i];
		fputc(hex[x >> 4], stdout);
		fputc(hex[x & 0x0f], stdout);
	}

	fputs("\"\n", stdout);
}

void one_icon(const char *name, ResID id) {

	Handle h;
	unsigned char *ptr;
	IconHeader *header;
	ResAttr attr;
	unsigned hh;
	unsigned ww;
	unsigned i;


	attr = GetResourceAttr(rIcon, id);
	h = LoadResource(rIcon, id);
	if (_toolErr) {
		fprintf(stderr, "%s: LoadResource: $%04x\n", name, _toolErr);

	}


	HLock(h);

	header = *(IconHeader **)h;

	hh = header->iconHeight;
	ww = header->iconWidth >> 1;

	printf("resource rIcon(%lu, $%04x) {\n", id, attr);
	printf("    $%04x, // type (%s)\n", header->iconType, header->iconType & 0x8000 ? "Color" : "B/W");
	printf("    %u, // height\n", header->iconHeight);
	printf("    %u, // width\n\n", header->iconWidth);

	ptr = ((unsigned char *)header) + sizeof(IconHeader);
	// print the icon

	for (i = 0; i < hh; ++i) {
		dump(ptr, ww);
		ptr += ww;
	}

	printf("    ,\n");
	// print the mask

	for (i = 0; i < hh; ++i) {
		dump(ptr, ww);
		ptr += ww;
	}
	printf("}\n\n");


	ReleaseResource(-1, rIcon, id);
	HUnlock(h);

}

void one_file(const char *name) {
	GSString255Ptr gname;
	unsigned rfd;
	unsigned depth;
	unsigned long ri;


	gname = c2gs(name);

	rfd = OpenResourceFile(0x8000 | readEnable, NULL, (Pointer)gname);
	if (_toolErr) {
		fprintf(stderr, "%s: OpenResourceFile: $%04x\n", name, _toolErr);
		free(gname);
		return;
	}

	depth = SetResourceFileDepth(1);

	for (ri = 1; ; ++ri) {
		ResID id = GetIndResource(rIcon, ri);
		if (_toolErr == resIndexRange) break;
		if (_toolErr) {
			fprintf(stderr, "%s: GetIndResource: $%04x\n", name, _toolErr);
			continue;
		}
		one_icon(name, id);
	}

	SetResourceFileDepth(depth);
	CloseResourceFile(rfd);
	free(gname);
}

int main(int argc, char **argv) {

	int i;
	ResourceStartUp(MMStartUp());
	for (i = 1; i < argc; ++i) {
		one_file(argv[i]);
	}
	ResourceShutDown();
	exit(0);
}